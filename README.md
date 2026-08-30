# Rubik's Cube Solver (Kociemba Two-Phase Algorithm)

A C++ implementation of a Rubik's Cube solver based on Herbert Kociemba's
two-phase algorithm. The program takes a scrambled cube as sticker-color
input, converts it into a cubie-level representation, and searches for a
solution using IDA* search guided by precomputed pruning (heuristic) tables.

## Overview

The solver works in two phases, following the classic Kociemba approach:

- **Phase 1** — Reduce the cube from the general group **G0** into the
  subgroup **G1**, where all corners are correctly oriented, all edges are
  correctly oriented, and the four E-slice edges (FL, FR, BL, BR) stay
  within the E-slice. This phase uses all 18 face turns.
- **Phase 2** — Solve the cube completely from **G1**, using only the moves
  that keep the cube inside G1: `U, U2, U', D, D2, D', L2, R2, F2, B2`.

Both phases are searched with **IDA\*** (iterative-deepening A*), where the
heuristic is read from pruning tables that are either loaded from disk or
generated via breadth-first search (BFS) on first run.

## Project Structure

The code is organized into logical modules (paths as referenced by the
`#include` directives in `main.cpp`):

```
.
├── main.cpp                  Entry point: reads user input, runs the solver
│
├── inputproc/                 Sticker → cubie conversion
│   ├── cube.hpp                Sticker-based Cube representation (6 faces × 9 stickers)
│   ├── cubie.hpp                Cubie-based Cubieste representation (cp, co, ep, eo)
│   ├── stk_to_cub.hpp/.cpp      Maps sticker colors to corner/edge permutation
│   │                            and orientation (cp, co, ep, eo), plus parity
│   │                            validation (inpvald)
│
├── rubicsmove/                Move engine
│   ├── cubemove.hpp/.cpp        Applies the 18 face turns (U, U', U2, ... D')
│   │                            directly to a Cubieste
│
├── bfstable/                  Coordinate encoding + pruning table generation
│   ├── encoder.hpp/.cpp         Encodes/decodes cube state into compact
│   │                            coordinates (CO, EO, slice, Lehmer codes for
│   │                            corner/edge permutations)
│   ├── bfs.hpp/.cpp              Breadth-first search over each coordinate
│   │                            space to build the pruning tables
│   ├── heuristictable.hpp/.cpp   Owns the pruning tables, generates them via
│   │                            Bfs, and loads/saves them to disk (tables/)
│
└── kociemba/                  Search
    ├── DFS.hpp/.cpp              Phase 1 IDA* depth-first search
    ├── DFS2.hpp/.cpp             Phase 2 IDA* depth-first search
    └── kociemba_ph1.hpp/.cpp     Phase 1 driver, G1 subgroup check, and the
                                 cost/heuristic function for phase 1
```

## Coordinates and Pruning Tables

The solver represents cube state with four coordinates, each with its own
pruning table combined with the E-slice edge permutation:

| Table | Coordinate | Size | Used in |
|---|---|---|---|
| `twistSliceTable` | Corner orientation (CO) × Slice edge combination | 2187 × 495 | Phase 1 |
| `flipSliceTable` | Edge orientation (EO) × Slice edge combination | 2048 × 495 | Phase 1 |
| `cpSliceTable` | Corner permutation (CP) × Slice edge permutation | 40320 × 24 | Phase 2 |
| `udEdgeSliceTable` | UD edge permutation × Slice edge permutation | 40320 × 24 | Phase 2 |

Each table stores, for every coordinate, the minimum number of moves needed
to reach the solved state for that coordinate — computed once via BFS
(`Bfs::bfstwstsls`, `bfsflpsls`, `bfscpsls`, `bfsepsls`) and reused as an
admissible heuristic during IDA* search (`cost_f_n1`, `cost_f_n2`).

Tables are stored under `tables/` as raw binary (`uint8_t` per entry):

- `tables/twist_slice.bin`
- `tables/flip_slice.bin`
- `tables/cp_slice.bin`
- `tables/udedge_slice.bin`

On startup, `Heuristictable::load_or_generatetable()` checks whether all
four files exist. If they do, it loads them; otherwise it generates them
from scratch and writes them out, so subsequent runs start instantly.

> Note: table generation can take noticeable time and memory on first run
> (the twist/flip tables alone have over a million entries each). Once
> generated, they only need to be built once.

## Building

No build system (CMake/Makefile) is included in this snapshot; the project
can be compiled directly with any C++17-capable compiler, keeping the
directory layout above intact so the relative `#include` paths resolve.
Example with g++:

```bash
g++ -std=c++17 -O2 \
    main.cpp \
    inputproc/stk_to_cub.cpp \
    rubicsmove/cubemove.cpp \
    bfstable/encoder.cpp \
    bfstable/bfs.cpp \
    bfstable/heuristictable.cpp \
    kociemba/kociemba_ph1.cpp \
    kociemba/DFS.cpp \
    kociemba/DFS2.cpp \
    -o cube_solver
```

## Usage

Run the compiled binary and follow the prompts. You'll be asked to enter
the 9 stickers of each face (U, F, D, R, L, B in that order), typed as
color letters:

| Letter | Color |
|---|---|
| `W` | White |
| `G` | Green |
| `Y` | Yellow |
| `R` | Red |
| `O` | Orange |
| `B` | Blue |

The program expects the standard orientation: white up / green front for
the U and F faces, and each subsequent face entered with the color
matching its own center facing up, as described in the on-screen prompts.

```
./cube_solver
```

The program will:

1. Parse your sticker input into a cubie representation (`cornerinfer`,
   `edgeinfer`).
2. Validate the input (permutation parity, corner/edge orientation parity)
   and reject invalid scrambles.
3. Load or generate the pruning tables.
4. Run phase 1 and phase 2 IDA* search, printing the bound explored at each
   iteration.
5. Print the full move sequence (phase 1 + phase 2) in standard cube
   notation, e.g. `U F2 R' ... | D2 L2 ...`.

## Known Issues / TODO

- the code runs fine and solves every thing but even at the end 9 edges remin
  flipped after implementing the solving sequence on the scrambled cube the 
  edges are nmely UF, FL, FR, BD, UL, DL, DR.


