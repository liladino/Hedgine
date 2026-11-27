#!/usr/bin/env python3
"""
Convert PGN exports into NNUE training data.

Input: PGN file(s), games written one after the other. Every engine
move carries the search result as a comment: {<centipawns>/<depth>}, with the
score from White's point of view. Moves without such a comment (book moves,
random opening plies, mate scores) are not used as training positions, but they
are still played on the board.

The score belongs to the position before the move (that is where the engine
searched), and the move itself is the engine's best move.

Filtering, per scored position:
  - side to move in check                -> skipped
  - best move is a capture or promotion  -> skipped (not a quiet position)
  - |score| > --max-score                -> skipped
  - duplicate positions (same Zobrist hash) are removed, the first one is kept

Output:
  <out>.bin  numpy records (dtype RECORD below), shuffled unless --no-shuffle.
             Load with np.memmap(path, dtype=RECORD, mode='r').
  <out>.txt  (with --txt) "FEN | score | result" lines, White's POV, result
             1.0 / 0.5 / 0.0. Deduplicated but not shuffled. This is the text
             format bullet's tools can read, and it is handy for eyeballing.

Feature index of a piece (White's perspective):
  piece * 64 + square
  piece:  the engine's piecenums (consts.h): wking=0 ... wpawn=5, bking=6 ... bpawn=11
  square: a1 = 0, b1 = 1, ..., h8 = 63 (same as the engine's bitboards)
Black's perspective is derived from it in the trainer: swap the colour
((piece + 6) % 12) and mirror the square vertically (square ^ 56).

Usage:
  python convert.py games1.pgn games2.pgn -o data/train --txt
"""

import argparse
import os
import re
import sys
import time
from collections import Counter
from contextlib import nullcontext
from multiprocessing import Pool

import chess
import chess.polyglot
import numpy as np

MAX_PIECES = 32
PAD = 768  # unused feature slots hold this value

RECORD = np.dtype([
    ("feat", "<u2", (MAX_PIECES,)),  # feature indices, White's perspective, padded with PAD
    ("stm", "u1"),                   # side to move: 0 = white, 1 = black
    ("result", "u1"),                # game result, White's POV: 0 = loss, 1 = draw, 2 = win
    ("score", "<i2"),                # search score in centipawns, White's POV
])

# python-chess piece type -> engine piecenums (consts.h); black pieces are +6
ENGINE_PIECE = {
    chess.KING: 0,
    chess.QUEEN: 1,
    chess.ROOK: 2,
    chess.BISHOP: 3,
    chess.KNIGHT: 4,
    chess.PAWN: 5,
}

RESULTS = {"1-0": 2, "1/2-1/2": 1, "0-1": 0}
HEADER_RE = re.compile(r'\[(\w+)\s+"(.*)"\]')
TOKEN_RE = re.compile(r"\{[^}]*\}|[^\s{}]+")
MOVENUM_RE = re.compile(r"^\d+\.+")
SCORE_RE = re.compile(r"^\s*([+-]?\d+)\s*/\s*(\d+)")


def features(board):
    out = np.full(MAX_PIECES, PAD, dtype=np.uint16)
    n = 0
    for color, offset in ((chess.WHITE, 0), (chess.BLACK, 6)):
        for ptype, piece in ENGINE_PIECE.items():
            for sq in chess.scan_forward(board.pieces_mask(ptype, color)):
                out[n] = (piece + offset) * 64 + sq
                n += 1
    return out


def read_games(paths):
    """Yield the text of each game. A header line after movetext starts a new game."""
    for path in paths:
        with open(path, encoding="utf-8", errors="replace") as f:
            buf, seen_moves = [], False
            for line in f:
                s = line.strip()
                if s.startswith("["):
                    if seen_moves:
                        yield "".join(buf)
                        buf, seen_moves = [], False
                    buf.append(line)
                elif s:
                    seen_moves = True
                    buf.append(line)
            if seen_moves:
                yield "".join(buf)


def process_game(job):
    text, max_score, want_txt = job
    stats = Counter(games=1)
    empty = (np.empty(0, RECORD), np.empty(0, np.uint64), [], stats)

    headers = dict(HEADER_RE.findall(text))
    result = RESULTS.get(headers.get("Result"))
    if result is None:
        stats["games_no_result"] += 1
        return empty
    try:
        board = chess.Board(headers["FEN"]) if "FEN" in headers else chess.Board()
    except ValueError:
        stats["games_bad_fen"] += 1
        return empty

    # [san, comment] pairs
    moves = []
    movetext = " ".join(l for l in text.splitlines() if not l.lstrip().startswith("["))
    for tok in TOKEN_RE.findall(movetext):
        if tok[0] == "{":
            if moves:
                moves[-1][1] = tok[1:-1]
            continue
        tok = MOVENUM_RE.sub("", tok)
        if not tok or tok in RESULTS or tok == "*" or tok[0] == "$":
            continue
        moves.append([tok, None])

    recs, hashes, lines = [], [], []
    for san, comment in moves:
        try:
            move = board.parse_san(san)
        except ValueError:
            stats["games_bad_move"] += 1  # keep what was collected before the bad move
            break
        m = SCORE_RE.match(comment) if comment else None
        if m is None:
            stats["unscored"] += 1
        else:
            score = int(m.group(1))
            if board.is_check():
                stats["skip_in_check"] += 1
            elif board.is_capture(move) or move.promotion:
                stats["skip_tactical"] += 1
            elif abs(score) > max_score:
                stats["skip_big_score"] += 1
            else:
                recs.append((features(board), int(board.turn == chess.BLACK), result, score))
                hashes.append(chess.polyglot.zobrist_hash(board))
                if want_txt:
                    lines.append(f"{board.fen()} | {score} | {result / 2:.1f}\n")
        board.push(move)

    stats["positions"] += len(recs)
    return np.array(recs, dtype=RECORD), np.array(hashes, dtype=np.uint64), lines, stats


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("pgn", nargs="+", help="PGN files exported by HedgineGUI")
    ap.add_argument("-o", "--out", required=True, help="output path without extension, e.g. data/train")
    ap.add_argument("--workers", type=int, default=os.cpu_count())
    ap.add_argument("--max-score", type=int, default=4000, help="drop positions with |score| above this (cp)")
    ap.add_argument("--txt", action="store_true", help="also write a FEN | score | result text file")
    ap.add_argument("--no-dedup", action="store_true")
    ap.add_argument("--no-shuffle", action="store_true")
    ap.add_argument("--seed", type=int, default=1)
    args = ap.parse_args()

    out_dir = os.path.dirname(args.out)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
    raw_bin, raw_txt = args.out + ".raw.bin", args.out + ".raw.txt"
    final_bin, final_txt = args.out + ".bin", args.out + ".txt"

    # pass 1: parse games in parallel, append records to a raw file in game order
    t0 = time.time()
    stats, hash_parts, n = Counter(), [], 0
    jobs = ((g, args.max_score, args.txt) for g in read_games(args.pgn))
    txt_ctx = open(raw_txt, "w", encoding="utf-8", newline="\n") if args.txt else nullcontext()
    with open(raw_bin, "wb") as fb, txt_ctx as ft, Pool(args.workers) as pool:
        for recs, hashes, lines, st in pool.imap(process_game, jobs, chunksize=64):
            stats.update(st)
            if len(recs):
                recs.tofile(fb)
                hash_parts.append(hashes)
                n += len(recs)
            if ft is not None and lines:
                ft.writelines(lines)
            if stats["games"] % 2000 == 0:
                print(f"\r{stats['games']} games, {n} positions, {time.time() - t0:.0f}s",
                      end="", file=sys.stderr, flush=True)
    print(file=sys.stderr)

    # pass 2: dedup and shuffle
    hashes = np.concatenate(hash_parts) if hash_parts else np.empty(0, np.uint64)
    if args.no_dedup:
        keep = np.arange(n)
    else:
        keep = np.sort(np.unique(hashes, return_index=True)[1])
    order = keep if args.no_shuffle else np.random.default_rng(args.seed).permutation(keep)

    raw = np.memmap(raw_bin, dtype=RECORD, mode="r", shape=(n,)) if n else np.empty(0, RECORD)
    chunk = 1 << 20
    with open(final_bin, "wb") as f:
        for i in range(0, len(order), chunk):
            idx = order[i:i + chunk]
            s = np.argsort(idx)  # read in file order, write in shuffled order
            part = np.empty(len(idx), RECORD)
            part[s] = raw[idx[s]]
            part.tofile(f)

    # sanity check: if scores are White's POV, they should rise with White's result
    mean_by_result = {}
    for r, name in ((2, "1-0"), (1, "1/2"), (0, "0-1")):
        sel = raw["score"][keep][raw["result"][keep] == r] if n else np.empty(0)
        mean_by_result[name] = (len(sel), float(sel.mean()) if len(sel) else float("nan"))
    del raw
    os.remove(raw_bin)

    if args.txt:
        mask = np.zeros(n, bool)
        mask[keep] = True
        with open(raw_txt, encoding="utf-8") as fin, open(final_txt, "w", encoding="utf-8", newline="\n") as fout:
            for i, line in enumerate(fin):
                if mask[i]:
                    fout.write(line)
        os.remove(raw_txt)

    print(f"games:              {stats['games']}")
    for k in ("games_no_result", "games_bad_fen", "games_bad_move"):
        if stats[k]:
            print(f"  {k + ':':18}{stats[k]}")
    print(f"unscored moves:     {stats['unscored']}")
    print(f"skipped in check:   {stats['skip_in_check']}")
    print(f"skipped tactical:   {stats['skip_tactical']}")
    print(f"skipped big score:  {stats['skip_big_score']}")
    print(f"duplicates removed: {n - len(keep)}")
    print(f"positions written:  {len(keep)} -> {final_bin}" + (f", {final_txt}" if args.txt else ""))
    print("mean score by result (should be +, ~0, -):")
    for name, (cnt, mean) in mean_by_result.items():
        print(f"  {name:4} {cnt:10} positions, mean {mean:+.0f} cp")
    print(f"time: {time.time() - t0:.1f}s")


if __name__ == "__main__":
    main()
