# Reversi – Full Game with AI Options

## Overview

This academic project presents a full implementation of the board game Reversi, written in C and developed by **Zandicedev** at the University of Bordeaux.

## Game modes
The project provides multiple game modes:

1. **Human vs Human**  
   Default mode (tactic level = 0)

2. **Human vs AI – Easy Level**  
   Tactic level = 1 → plays against a Random AI

3. **Human vs AI – Hard Level**  
   Tactic level = 2 → plays against a Minimax AI with Alpha-Beta pruning

The project focuses on performance, modularity, clean architecture, and optimized AI search.

---

## Features

### Game Engine
- Standard Reversi rules
- Legal move generation
- Automatic piece flipping
- Score calculation
- Game-over detection

### Efficient Board Representation
- Bitboard implementation
- Fast move computation
- Optimized board updates

### Artificial Intelligence
- Minimax algorithm
- Alpha-Beta pruning optimization
- Depth-limited search
- Heuristic board evaluation
- Random AI option

### Undo / Redo
- Move history tracking
- Undo moves (`u`)
- Redo moves (`r`)

### File Parser
- Load board states from file
- Input validation
- Error handling
- Save and replay games

### Testing
- Dedicated test files
- Board validation tests
- Extra test suite for edge cases

---

## Project Structure

```bash
Reversi
├── Makefile
├── README.md
├── include
│   ├── board.h
│   └── player.h
├── src
│   ├── Makefile
│   ├── board.c
│   ├── player.c
│   ├── reversi.c
│   └── reversi.h
├── game.txt
├── .gitignore
└── tests
    ├── Makefile
    ├── board_extra_tests.c
    ├── board_parser
    │   ├── board-0x0.fail
    │   └── ...
    └── board_tests.c
```
## Compilation

Compile the project:

```bash
make
```

## Usage
Display help:
```bash
./reversi -h
```

### Run the default game (Human vs Human):

```bash
./reversi
```

### Load a saved game:
```bash
./reversi game.txt
```


### Enable verbose mode (display moves and board updates):

```bash
./reversi -v
```

## AI Options
You can configure each player using:
- bX → Black player
- wX → White player

Where X is:
- 0 = Human
- 1 = Random AI
- 2 = Minimax AI (with Alpha-Beta)

### Examples
Random (Black) vs Minimax (White) with verbose mode:
```bash
./reversi -v -b1 -w2
```
Minimax (Black) vs Human (White):
```bash
./reversi -v -b2
```
By default:
- White player = Human
- Black player = Human (unless specified)
To use undo when you plau tappe u et pour redo r


### Controls During Game: 
To use undo, press `u`.  
To redo, press `r`.

## AI Details
### Minimax

The AI explores possible moves recursively to select the optimal move based on a heuristic evaluation.

### Alpha-Beta Pruning
Reduces the number of explored branches, significantly improving performance.

### Heuristic Evaluation
Board evaluation considers:
- Piece difference
- Positional weights
- Strategic positions (corners and edges)
- Game progression

### Testing
Run board tests:
```bash
cd tests
make
./board_tests
```

Run extra tests:
```bash
cd tests
gcc -Wall board_extra_tests.c -o board_extra_tests
./board_extra_tests
```
### Technical Highlights:
- Bit-level optimizations (bitboards)
- Modular architecture
- Recursive AI search
- Structured testing

## References

Reversi rules:
- https://www.mathsisfun.com/games/reversi.html
- https://en.wikipedia.org/wiki/Reversi