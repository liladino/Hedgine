# Hedgine
A small UCI compatible chess engine

```
                                     #    #
 .  .     .                       #   ##  # #   ##  ##
 |__| _  _| _ . _  _       # #  #       ##   ##      ####  #  #
 |  |(/.(_](_]|[ )(/#  #  #   ##                         ## ## ##
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


run
```
./hedgine
```


test
```
./hedgine test quit
uci
ucinewgame
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves d2d4 a7a5 b1c3 d7d5 e2e4 d5e4 c3e4 f7f5 e4g3 e7e6 c2c3 c7c5 f1b5 b8c6 g1f3 h7h5 f3e5 c5d4 e5c6 b7c6 b5c6 c8d7 c6a8 d8a8 d1d4 h5h4 g3e2 a8g2 h1f1 g2h2 e2f4 h8h6 c1e3 a5a4 e1c1 d7c6 d4d8 e8f7 d8c7 g8e7 d1d8 e6e5 c7e5 c6e8 f1d1 h6f6 d8e8 f7e8 d1d8 e8d8 f4e6 f6e6 e5h2 e7g6 e3g5 d8d7 f2f4 e6e1 c1d2 e1a1 a2a3 a1a2 d2c2 f8a3 h2d2 d7c6 d2g2 c6d6 g2d2 d6c6 d2d8 a2b2 c2d1 b2b1 d1e2 b1b6 d8e8 c6c5 e8c8 b6c6 c8f5 c5c4 f5e4 c4b5 e2d3 c6d6 d3c2 d6c6 f4f5 g6f8 g5h4 g7g6 f5f6 a3c5 e4d3 b5b6 f6f7 a4a3 h4d8 b6a7 d3d5 a7b7 d8g5 f8e6 g5h6 b7c7 d5e5 c7b7 c2b3 c5e7 h6e3 g6g5 e5b5 b7c7 b5a5 c7d7 a5a7 c6c7 a7a4 c7c6 a4a8 d7c7 a8e8 e7f8 e3g5 c6b6 b3a2 b6b2 a2a1 b2e2 g5e7 f8g7 e7b4 e2e4 e8e7 c7b6 a1a2 e4e2 a2a3 g7h6 e7f6 h6c1 a3b3 b6c7 b4a5 c7d7 f6f5 e2b2 b3c4 c1h6 a5b4 b2b4 c3b4 d7e7 b4b5
go movetime 2000
```

profiling
```
make profiling 
./hedgine testLasker quit
gprof hedgine > report.txt 
```