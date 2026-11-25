"""
Input feature sets for train.py.

Every feature set is computed from the same stored training data: the list of
768-style piece-square indices (piece * 64 + square, padded with PAD) that
convert.py writes. So any data converted for the "psq" net can train any
feature set here, nothing has to be reconverted.

A feature set turns that list into (index, value) pairs, one list per view:

    accumulator = ft_bias + sum of value * ft_weight[index]

which is nn.EmbeddingBag(mode="sum") with per_sample_weights, and exactly what
the engine does: start from the bias, add value * row for each active input.

The black view is always derived the same way: first map every piece to the
black view of the 768 space (swap colours, mirror the board vertically, see
BLACK_LUT), then apply the feature set as if it were white's view.

Feature sets
------------
psq    the original 768 inputs, value always 1 (unchanged, file magic "HNNU")

blind  the "blind" net, file magic "HNNB". Inputs are grouped, any group can be
       switched off with --features. Indices are contiguous, the enabled groups
       follow each other in this order ("us" = the colour whose view it is,
       for the white view that's white):

       occ     128  occupancy: 64 squares of our pieces, then 64 of theirs
                    (all piece types, kings included), value 1
       counts   10  number of our Q, R, B, N, P then theirs, value
                    count / divisor, divisors 1, 2, 2, 2, 8 (so a normal
                    starting army gives 1 everywhere; promotions can go above)
       kings    32  one-hot king rank and file: our king's rank (8), our king's
                    file (8), their king's rank (8), their king's file (8),
                    value 1. Ranks counted from the view's own back rank.

       All three: 170 inputs. --features counts alone is a learned material
       eval, a useful baseline for the ablation ladder.
"""

import struct

import numpy as np
import torch

from convert import PAD

NUM_PSQ = 768  # size of the stored feature space


def make_black_lut():
    """Maps a white-view feature index to the black-view index. PAD stays PAD."""
    lut = np.full(NUM_PSQ + 1, PAD, dtype=np.int64)
    for f in range(NUM_PSQ):
        piece, sq = divmod(f, 64)
        lut[f] = ((piece + 6) % 12) * 64 + (sq ^ 56)
    return torch.from_numpy(lut)


BLACK_LUT = make_black_lut()


class PsqFeatures:
    """The original 768 piece-square inputs."""

    name = "psq"
    num_inputs = NUM_PSQ
    pad = PAD

    def views(self, feat):
        """feat: (B, 32) int64 stored indices -> (white_idx, white_val, black_idx, black_val)."""
        return feat, None, BLACK_LUT[feat], None

    def header(self, hidden, scale):
        return struct.pack("<4sIII", b"HNNU", 1, hidden, scale)

    def describe(self):
        return "psq (768 inputs)"


# engine piecenums: king, queen, rook, bishop, knight, pawn; black is +6
KING = 0
COUNT_DIVISORS = {1: 1.0, 2: 2.0, 3: 2.0, 4: 2.0, 5: 8.0}  # Q R B N P

BLIND_GROUPS = {"occ": (1, 128), "counts": (2, 10), "kings": (4, 32)}  # name: (header bit, inputs)
BLIND_ORDER = ("occ", "counts", "kings")


class BlindFeatures:
    """Occupancy + piece counts + one-hot king rank/file, each group optional."""

    name = "blind"
    PER_PIECE = 3  # at most 3 inputs per piece: occupancy + (count or king rank + king file)

    def __init__(self, groups):
        groups = set(groups)
        unknown = groups - set(BLIND_GROUPS)
        if unknown or not groups:
            raise ValueError(f"blind feature groups must be a non-empty subset of {', '.join(BLIND_ORDER)}")
        self.groups = [g for g in BLIND_ORDER if g in groups]

        self.base = {}
        n = 0
        for g in self.groups:
            self.base[g] = n
            n += BLIND_GROUPS[g][1]
        self.num_inputs = n
        self.pad = n  # EmbeddingBag padding row, left out of the sum
        self.mask = sum(BLIND_GROUPS[g][0] for g in self.groups)

        # LUT: stored (white-view) index -> PER_PIECE (input, value) pairs
        idx = np.full((NUM_PSQ + 1, self.PER_PIECE), self.pad, dtype=np.int64)
        val = np.zeros((NUM_PSQ + 1, self.PER_PIECE), dtype=np.float32)
        for f in range(NUM_PSQ):
            piece, sq = divmod(f, 64)
            colour, ptype = divmod(piece, 6)  # colour 0 = us (white in white's view)
            pairs = []
            if "occ" in self.base:
                pairs.append((self.base["occ"] + colour * 64 + sq, 1.0))
            if ptype == KING:
                if "kings" in self.base:
                    rank, file = divmod(sq, 8)
                    b = self.base["kings"] + colour * 16
                    pairs.append((b + rank, 1.0))
                    pairs.append((b + 8 + file, 1.0))
            elif "counts" in self.base:
                pairs.append((self.base["counts"] + colour * 5 + (ptype - 1), 1.0 / COUNT_DIVISORS[ptype]))
            for k, (i, v) in enumerate(pairs):
                idx[f, k] = i
                val[f, k] = v
        self.lut_idx = torch.from_numpy(idx)
        self.lut_val = torch.from_numpy(val)

    def _map(self, feat):
        b = feat.shape[0]
        return self.lut_idx[feat].reshape(b, -1), self.lut_val[feat].reshape(b, -1)

    def views(self, feat):
        white_idx, white_val = self._map(feat)
        black_idx, black_val = self._map(BLACK_LUT[feat])
        return white_idx, white_val, black_idx, black_val

    def header(self, hidden, scale):
        return struct.pack("<4sIIIII", b"HNNB", 1, hidden, scale, self.mask, self.num_inputs)

    def describe(self):
        return f"blind ({'+'.join(self.groups)}, {self.num_inputs} inputs)"


def make_feature_set(arch, features=None):
    if arch == "psq":
        if features:
            raise ValueError("--features only applies to --arch blind")
        return PsqFeatures()
    if arch == "blind":
        return BlindFeatures(features.split(",") if features else BLIND_ORDER)
    raise ValueError(f"unknown arch {arch}")
