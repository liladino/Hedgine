#!/usr/bin/env python3
"""
Train a 768 -> Hx2 -> 1 perspective NNUE on data written by convert.py.

The network
-----------
Input: the 768 piece-square features (piece * 64 + square, engine piece order),
seen from both sides:
  white view: the features as stored
  black view: colours swapped, board mirrored vertically
              (piece + 6) % 12, square ^ 56

Feature transformer (the "accumulator"), shared by both views:
  acc = bias + sum of the weight rows of the active features      (H values)
That sum is exactly what nn.EmbeddingBag(mode="sum") computes, and exactly
what the engine will do: start from the bias, add one row per piece.

Output layer:
  h   = clamp([acc_us, acc_them], 0, 1)     us = side to move   (2H values)
  out = h . w_out + b_out
  eval (centipawns, side to move's POV) = out * SCALE

Training target
---------------
For each position (converted to the side to move's POV):
  target = LAMBDA * sigmoid(score / SCALE) + (1 - LAMBDA) * result
  loss   = (sigmoid(out) - target)^2
So the net learns a win probability, and out * SCALE maps it back to cp.
LAMBDA = 1 trains on the search scores only, 0 on the game results only.

Outputs (with -o nnue/data/net):
  net.pt          PyTorch checkpoint (saved every epoch)
  net.bin         weights for the engine, format below
  net.golden.txt  "FEN | expected cp" lines the C inference must reproduce

net.bin format, little endian:
  char[4]  "HNNU"
  uint32   version (1)
  uint32   H
  uint32   SCALE
  float32  ft_weight[768][H]   row f = weights of feature f
  float32  ft_bias[H]
  float32  out_weight[2H]      first H: side to move's accumulator, then the other
  float32  out_bias

Usage:
  python train.py nnue/data/train.bin -o nnue/data/net --epochs 20
"""

import argparse
import math
import struct
import time

import chess
import numpy as np
import torch
import torch.nn as nn

from convert import PAD, RECORD, features

NUM_FEATURES = 768

GOLDEN_FENS = [
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "rnbqkb1r/pp3ppp/3pp3/4P3/3P4/2P2N2/P4PPP/R1BQKB1R b KQkq - 0 8",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "6k1/5ppp/8/8/8/8/5PPP/3R2K1 b - - 0 1",
]


def make_black_lut():
    """Maps a white-view feature index to the black-view index. PAD stays PAD."""
    lut = np.full(NUM_FEATURES + 1, PAD, dtype=np.int64)
    for f in range(NUM_FEATURES):
        piece, sq = divmod(f, 64)
        lut[f] = ((piece + 6) % 12) * 64 + (sq ^ 56)
    return torch.from_numpy(lut)


BLACK_LUT = make_black_lut()


class Net(nn.Module):
    def __init__(self, hidden):
        super().__init__()
        self.hidden = hidden
        # one weight row per feature, plus a zero row for PAD that is left out of the sum
        self.ft = nn.EmbeddingBag(NUM_FEATURES + 1, hidden, mode="sum", padding_idx=PAD)
        self.ft_bias = nn.Parameter(torch.zeros(hidden))
        self.out = nn.Linear(2 * hidden, 1)
        with torch.no_grad():
            # small init: ~30 pieces summed should land around the 0..1 clamp range
            self.ft.weight.uniform_(-0.15, 0.15)
            self.ft.weight[PAD].zero_()

    def forward(self, white_idx, black_idx, stm):
        acc_white = self.ft(white_idx) + self.ft_bias
        acc_black = self.ft(black_idx) + self.ft_bias
        black_to_move = stm.unsqueeze(1)
        us = torch.where(black_to_move, acc_black, acc_white)
        them = torch.where(black_to_move, acc_white, acc_black)
        h = torch.clamp(torch.cat([us, them], dim=1), 0.0, 1.0)
        return self.out(h).squeeze(1)


def to_tensors(rec):
    """numpy records -> model inputs and side-to-move POV labels."""
    white_idx = torch.from_numpy(rec["feat"].astype(np.int64))
    black_idx = BLACK_LUT[white_idx]
    stm = torch.from_numpy(rec["stm"].astype(np.bool_))
    score = torch.from_numpy(rec["score"].astype(np.float32))
    result = torch.from_numpy(rec["result"].astype(np.float32)) / 2
    score = torch.where(stm, -score, score)
    result = torch.where(stm, 1 - result, result)
    return white_idx, black_idx, stm, score, result


def batch_loss(model, rec, scale, lam):
    white_idx, black_idx, stm, score, result = to_tensors(rec)
    out = model(white_idx, black_idx, stm)
    target = lam * torch.sigmoid(score / scale) + (1 - lam) * result
    return ((torch.sigmoid(out) - target) ** 2).mean()


def evaluate(model, data, batch, scale, lam):
    model.eval()
    total, n = 0.0, 0
    with torch.no_grad():
        for i in range(0, len(data), batch):
            rec = data[i:i + batch]
            total += batch_loss(model, rec, scale, lam).item() * len(rec)
            n += len(rec)
    model.train()
    return total / max(n, 1)


def eval_fen(model, fen, scale):
    """Centipawns from the side to move's POV, computed like the engine will."""
    board = chess.Board(fen)
    white_idx = torch.from_numpy(features(board).astype(np.int64)).unsqueeze(0)
    black_idx = BLACK_LUT[white_idx]
    stm = torch.tensor([board.turn == chess.BLACK])
    with torch.no_grad():
        return model(white_idx, black_idx, stm).item() * scale


def export(model, path, scale):
    h = model.hidden
    with open(path, "wb") as f:
        f.write(struct.pack("<4sIII", b"HNNU", 1, h, scale))
        for t in (model.ft.weight[:NUM_FEATURES], model.ft_bias, model.out.weight, model.out.bias):
            f.write(t.detach().numpy().astype("<f4").tobytes())


def write_golden(model, path, scale):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        for fen in GOLDEN_FENS:
            f.write(f"{fen} | {eval_fen(model, fen, scale):.4f}\n")
    # perspective sanity check: colour-mirrored positions must get the same score
    worst = max(abs(eval_fen(model, fen, scale) - eval_fen(model, chess.Board(fen).mirror().fen(), scale))
                for fen in GOLDEN_FENS)
    print(f"mirror check: max difference {worst:.6f} cp (should be ~0)")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("data", help=".bin file written by convert.py")
    ap.add_argument("-o", "--out", required=True, help="output path without extension, e.g. nnue/data/net")
    ap.add_argument("--hidden", type=int, default=128)
    ap.add_argument("--epochs", type=int, default=20)
    ap.add_argument("--batch", type=int, default=16384)
    ap.add_argument("--lr", type=float, default=1e-3)
    ap.add_argument("--lam", type=float, default=0.75, help="weight of the search score vs the game result")
    ap.add_argument("--scale", type=int, default=400, help="cp = net output * scale")
    ap.add_argument("--clip", type=float, default=1.98, help="weight clipping, keeps later quantization simple")
    ap.add_argument("--val", type=float, default=0.02, help="fraction held out for validation")
    ap.add_argument("--max-positions", type=int, default=0,
                    help="train on at most this many positions (0 = all). The validation set stays "
                         "the same, so runs with different limits are comparable (learning curve)")
    ap.add_argument("--seed", type=int, default=1)
    args = ap.parse_args()

    torch.manual_seed(args.seed)
    rng = np.random.default_rng(args.seed)

    data = np.memmap(args.data, dtype=RECORD, mode="r")
    n_val = max(1, int(len(data) * args.val))
    train, val = data[:-n_val], data[-n_val:]  # the converter already shuffled the file
    if args.max_positions > 0:
        train = train[:args.max_positions]  # a random subset, since the file is shuffled
    batches_per_epoch = math.ceil(len(train) / args.batch)
    print(f"{len(train)} training / {len(val)} validation positions, {batches_per_epoch} batches per epoch")

    model = Net(args.hidden)
    opt = torch.optim.Adam(model.parameters(), lr=args.lr)
    # learning rate decays smoothly to 1% of the start over the run
    sched = torch.optim.lr_scheduler.CosineAnnealingLR(
        opt, T_max=args.epochs * batches_per_epoch, eta_min=args.lr * 0.01)

    print(f"epoch {0:3}  val loss {evaluate(model, val, args.batch, args.scale, args.lam):.6f}  (untrained)")
    t0 = time.time()
    for epoch in range(1, args.epochs + 1):
        total = 0.0
        for b in rng.permutation(batches_per_epoch):
            rec = train[b * args.batch:(b + 1) * args.batch]
            loss = batch_loss(model, rec, args.scale, args.lam)
            opt.zero_grad()
            loss.backward()
            opt.step()
            sched.step()
            with torch.no_grad():
                for p in model.parameters():
                    p.clamp_(-args.clip, args.clip)
            total += loss.item() * len(rec)
        val_loss = evaluate(model, val, args.batch, args.scale, args.lam)
        print(f"epoch {epoch:3}  train loss {total / len(train):.6f}  val loss {val_loss:.6f}  "
              f"startpos {eval_fen(model, GOLDEN_FENS[0], args.scale):+.0f} cp  {time.time() - t0:.0f}s")
        torch.save({"model": model.state_dict(), "args": vars(args), "epoch": epoch}, args.out + ".pt")

    export(model, args.out + ".bin", args.scale)
    write_golden(model, args.out + ".golden.txt", args.scale)
    print(f"wrote {args.out}.pt, {args.out}.bin, {args.out}.golden.txt")


if __name__ == "__main__":
    main()
