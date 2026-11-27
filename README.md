# Hedgine
A small UCI compatible chess engine

```
                                     #    #
 .  .     .                       #   ##  # #   ##  ##
 |__| _  _| _ . _  _       # #  #       ##   ##      ####  #  #
 |  |(/.(_](_]|[ )(/.  #  #   ##                         ## ## ##
           ._|        # ###                                      #
                     #   #                                        ###  ##
                  ####                           ||                  ##  #
                   #                            ====                     ##
                   #                       ____  ||  ____                  #
                  ###   ##                / __ \ /\ / __ \                ## 
                 #   ##     ##           | /  \ |  | /  \ |               #
     ###       #    ###        #         | \   \ \/ /   / |                ##
   #    #####      #   #        #         \ \__/ || \__/ /                ##
   #     #         #   #         #        |______________|                  #
    #####           ###          #         \____________/                # #
        #                       ##                                  ##### # 
         #        # #           #                                   #
          ###      #           #                        ###    # ###
             ####          ###  ###   #    ##### #   ##  #  ###
                                     ##    ###     ##
```

## Build

From `hedgineUCI/`:

```
make mingwall                 # Windows (MinGW), links ws2_32
make all                      # other platforms
make opt                      # -O3 -march=native
make debug                    # -g3 -DDEBUG, extra runtime checks
```

Options, combinable with any target:

| option            | effect                                                   |
|-------------------|----------------------------------------------------------|
| `NNUE=1`          | evaluate with the NNUE instead of the hand-crafted eval, incremental accumulator included |
| `NNUE_NET=<path>` | the net embedded at build time, default `../nnue/data/full_psq.bin`. Psq (`HNNU`) and blind (`HNNB`) nets both work |
| `DEBUG=1`         | debug build. With NNUE it compares the incremental accumulator with a fresh one at every evaluation |

Example (blind net with occupancy only):

```
make mingwall NNUE=1 NNUE_NET=../nnue/data/blind_occ.bin
```

The engine prints the net it loaded at startup, blind nets with their feature
groups.

## Run

```
./hedgine
```

The engine speaks UCI on stdin / stdout. A manual session:

```
uci
ucinewgame
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves d2d4 d7d5 c2c4 e7e6
go movetime 2000
quit
```

## Tests

Tests are selected with the first command line argument (exact match, case sensitive):

```
./hedgine <test> [arguments]
```

Every test exits when it's done, whether it passed or not. Results are printed
(look for `passed`, `failed` and `fail, quitting`); the exit code is always 0.
Any other first argument, or none, starts the engine normally.

| command | what it checks | build |
|---------------------------------------|---|---|
| `testAll`                             | runs `testMakeMove`, `testMovePicker`, `testPseudo`, `test` (perft), the eval symmetry check, `testFifty`; NNUE builds also `testNNUEInc` with the embedded net instead of the symmetry check. Prints `fail, quitting` on the first failure | any |
| `test`                                | perft node counts on the standard positions | any |
| `testMakeMove`                        | makeMove / undoMove restore the board exactly | any |
| `testPseudo`                          | the pseudo-legality validator against the move generator (perft positions, depth 3) | any |
| `testMovePicker`                      | perft through the move picker matches the move generator (Kiwipete, position 4) | any |
| `testFifty`                           | fifty move counter through `position ... moves ...` | any |
| `testNNUE <net.bin> <net.golden.txt>` | whether the C inference matches PyTorch on the positions `train.py` wrote into the golden file (tolerance 0.05 cp) | `NNUE=1` |
| `testNNUEInc [net.bin]`               | 1000 random walks: the incremental accumulator equals a fresh one after every push and pop. Uses the embedded net if no file is given | `NNUE=1` |
| `testSearch [depth]`                  | search speed: time, nodes and knps per position at the given depth (default 6) | any |
| `testLasker`, `testEnglish`           | 15 second search on a fixed position (Lasker-Reichhelm, an English opening), for eyeballing and profiling | any |

Example:

```
./hedgine testNNUE ../nnue/data/blind_occ.bin ../nnue/data/blind_occ.golden.txt
```

### Checking a new net

1. `python nnue/train.py ...` writes `<out>.bin` and `<out>.golden.txt`, and
   prints a mirror check (a position and its colour-flipped copy must get the
   same score, so the difference should be ~0).
2. `testNNUE <out>.bin <out>.golden.txt`: the engine reads the net the same way
   as Python does.
3. `testNNUEInc <out>.bin`: the incremental updates stay exact with this net.

## Profiling

```
make profiling 
./hedgine testLasker 
gprof hedgine > report.txt 
```
