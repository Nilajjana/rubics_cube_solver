# Rubik's Cube Solver

A C++ implementation of a **Rubik's Cube solver based on Herbert Kociemba's Two-Phase Algorithm**.

The program accepts a scrambled cube as sticker-color input, converts it into a cubie-level representation, validates the cube configuration, and searches for a solution using **IDA*** guided by precomputed pruning tables.

---

## Table of Contents

* [Overview](#overview)
* [How the Solver Works](#how-the-solver-works)
* [Project Structure](#project-structure)
* [Coordinates and Pruning Tables](#coordinates-and-pruning-tables)
* [Building](#building)
* [Usage](#usage)
* [Input and Output](#input-and-output)

  * [Cube Input Layout](#cube-input-layout)
  * [Face Input](#face-input)
  * [Centre-Color Detection](#centre-color-detection)
  * [Sticker-to-Cubie Conversion](#sticker-to-cubie-conversion)
  * [Pruning Table Generation](#pruning-table-generation)
  * [Search Output](#search-output)
  * [Move Notation](#move-notation)
* [Complete Solving Pipeline](#complete-solving-pipeline)
* [Known Issues / TODO](#known-issues--todo)

---

## Overview

The solver follows the classic **Kociemba Two-Phase Algorithm**.

### Phase 1

Phase 1 reduces the cube from the general group **G0** into the subgroup **G1**.

The resulting cube must have:

* All corners correctly oriented.
* All edges correctly oriented.
* The four E-slice edges (`FL`, `FR`, `BL`, `BR`) remaining within the E-slice.

Phase 1 allows all **18 face turns**.

### Phase 2

Phase 2 solves the cube completely from **G1**.

Only moves that keep the cube inside G1 are allowed:

```text
U  U2  U'
D  D2  D'
L2 R2 F2 B2
```

Both phases use **IDA*** (Iterative Deepening A*) search.

The search is guided by heuristic values stored in pruning tables. These tables are loaded from disk when available and generated automatically using **Breadth-First Search (BFS)** when they are missing.

---

## How the Solver Works

The overall process is:

```text
Sticker-color input
        ↓
Centre-color detection
        ↓
Sticker → Facelet representation
        ↓
Facelet → Cubie representation
        ↓
Cube validity checks
        ↓
Load / generate pruning tables
        ↓
Phase 1 IDA*
        ↓
Phase 2 IDA*
        ↓
Solution move sequence
```

---

## Project Structure

The project is divided into several logical modules:

```text
.
├── main.cpp
│   └── Program entry point and user input
│
├── inputproc/
│   ├── cube.hpp
│   │   └── Sticker-based cube representation
│   │
│   ├── cubie.hpp
│   │   └── Cubie-based representation (cp, co, ep, eo)
│   │
│   └── stk_to_cub.hpp/.cpp
│       └── Sticker → cubie conversion,
│           corner/edge orientation and
│           permutation validation
│
├── rubicsmove/
│   └── cubemove.hpp/.cpp
│       └── Cube move engine implementing
│           the 18 face turns
│
├── bfstable/
│   ├── encoder.hpp/.cpp
│   │   └── Coordinate encoding/decoding
│   │
│   ├── bfs.hpp/.cpp
│   │   └── BFS-based pruning table generation
│   │
│   └── heuristictable.hpp/.cpp
│       └── Pruning table management,
│           generation, loading and saving
│
└── kociemba/
    ├── DFS.hpp/.cpp
    │   └── Phase 1 IDA* search
    │
    ├── DFS2.hpp/.cpp
    │   └── Phase 2 IDA* search
    │
    └── kociemba_ph1.hpp/.cpp
        └── Phase 1 driver, G1 subgroup
            check and heuristic/cost functions
```

---

## Coordinates and Pruning Tables

The solver uses four coordinate-based pruning tables.

| Table              | Coordinate                                       |         Size | Used in |
| ------------------ | ------------------------------------------------ | -----------: | ------- |
| `twistSliceTable`  | Corner orientation (CO) × slice-edge combination | `2187 × 495` | Phase 1 |
| `flipSliceTable`   | Edge orientation (EO) × slice-edge combination   | `2048 × 495` | Phase 1 |
| `cpSliceTable`     | Corner permutation (CP) × slice-edge permutation | `40320 × 24` | Phase 2 |
| `udEdgeSliceTable` | UD-edge permutation × slice-edge permutation     | `40320 × 24` | Phase 2 |

Each table stores the minimum number of moves required to reach the solved state for a particular coordinate.

The tables are generated using BFS:

```text
Bfs::bfstwstsls()
Bfs::bfsflpsls()
Bfs::bfscpsls()
Bfs::bfsepsls()
```

The resulting values are reused as admissible heuristics during IDA* search.

### Table Files

The generated tables are stored as raw binary files containing one `uint8_t` value per entry:

```text
tables/
├── twist_slice.bin
├── flip_slice.bin
├── cp_slice.bin
└── udedge_slice.bin
```

On startup, the solver checks whether all four files exist.

If they do:

```text
Loading pruning tables...
```

Otherwise:

```text
Pruning tables not found.
Generating tables...
```

The generated tables are then saved to the `tables/` directory and reused on subsequent runs.

> **Note:** Table generation can take noticeable time and memory on the first run. The twist and flip tables alone contain more than one million entries each. Once generated, the tables only need to be built once.

---

## Building

This project currently does **not include a CMake or Makefile build system**.

It can be compiled directly using any compiler supporting **C++17**.

### Using g++

From the project root:

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

Then run:

```bash
./cube_solver
```

---

## Usage

Run the compiled program:

```bash
./cube_solver
```

The program displays a cube net and asks you to enter the stickers for each face.

The faces are entered in this order:

```text
U F D R L B
```

Each face requires **9 sticker colors**.

---

# Input and Output

## Cube Input Layout

When the program starts, it displays the following cube net:

```text
             |************|
             |*U1**U2**U3*|
             |************|
             |*U4**U5**U6*|
             |************|
             |*U7**U8**U9*|
             |************|
 ************|************|************|************
 *L1**L2**L3*|*F1**F2**F3*|*R1**R2**R3*|*B1**B2**B3*
 ************|************|************|************
 *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*
 ************|************|************|************
 *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*
 ************|************|************|************
             |************|
             |*D1**D2**D3*|
             |************|
             |*D4**D5**D6*|
             |************|
             |*D7**D8**D9*|
             |************|
```

---

## Face Input

The six faces are entered in this order:

```text
U F D R L B
```

Each sticker is represented by a capital-letter color symbol:

| Letter | Color  |
| ------ | ------ |
| `W`    | White  |
| `Y`    | Yellow |
| `R`    | Red    |
| `B`    | Blue   |
| `O`    | Orange |
| `G`    | Green  |

Each face can be entered as three rows of three characters.

Example:

```text
Enter U face:

BRG
ROB
GOR
```

The same process is repeated for the remaining faces:

```text
Enter F face:
YGG
GYY
YRB

Enter D face:
RBY
ORW
ROO

Enter R face:
YWW
RGW
OGB

Enter L face:
RWO
BBY
WBB

Enter B face:
OGW
OWY
WYG
```

---

## Centre-Color Detection

The center sticker of each face determines the relationship between the physical cube colors and the solver's internal face notation.

For example:

```text
Detected centre mapping:
U = O
F = Y
D = R
R = G
L = B
B = W
```

This allows the solver to work with different physical color orientations rather than assuming that a specific color is always assigned to a specific face.

---

## Sticker-to-Cubie Conversion

After reading the sticker input, the solver converts the sticker representation into its internal **cubie representation**.

It first prints the converted facelets in `UFDRLB` notation:

```text
Converted cube:
U: LDRDULRUD
F: FRRRFFFDL
D: DLFUDBDUU
R: FBBDRBURL
L: DBULLFBLL
B: URBUBFBFR

Input successfully converted to UFDRLB notation.
```

The solver then identifies all corners and edges and calculates their:

* Permutation (`cp` / `ep`)
* Orientation (`co` / `eo`)

### Corner Example

```text
Corner UFR: DRF -> cp = 4, co = 0
Corner UFL: RFU -> cp = 0, co = 2
Corner ULB: LDB -> cp = 6, co = 1
Corner UBR: RUB -> cp = 3, co = 1
Corner DFR: FLU -> cp = 1, co = 2
Corner DFL: DFL -> cp = 5, co = 0
Corner DLB: DBR -> cp = 7, co = 0
Corner DRB: ULB -> cp = 2, co = 0
```

The corner orientation constraint is also checked:

```text
Corner orientation sum
= 0 + 2 + 1 + 1 + 2 + 0 + 0 + 0
= 6 mod 3
= 0
```

### Edge Example

The twelve edges are processed similarly:

```text
Edge UF: UR -> ep = 3, eo = 0
Edge UL: DB -> ep = 6, eo = 0
Edge UB: DR -> ep = 7, eo = 0
Edge UR: LB -> ep = 9, eo = 0
Edge DF: LD -> ep = 5, eo = 1
Edge DL: UL -> ep = 1, eo = 0
Edge DB: UF -> ep = 0, eo = 0
Edge DR: BR -> ep = 10, eo = 0
Edge FL: RF -> ep = 11, eo = 0
Edge LB: LF -> ep = 8, eo = 1
Edge BR: UB -> ep = 2, eo = 0
Edge RF: DF -> ep = 4, eo = 0
```

This conversion stage allows the solver to detect invalid cube configurations before starting the search.

---

## Pruning Table Generation

If the pruning tables do not exist, they are generated automatically.

The program prints:

```text
Pruning tables not found.
Generating tables...
```

The four tables are generated using BFS.

Example output:

```text
Unvisited states: 0
Visited states: 1082565
the max level in twist slice is 9 which ended at 1082565

Unvisited states: 0
Visited states: 1013760
the max level in flip slice is 9 which ended at 1013760

Unvisited states: 0
Visited states: 967680
the max level cp slice is 14 which ended at 967680

Unvisited states: 0
Visited states: 967680
the max level ud edge p slice is 12 which ended at 967680

Tables generated and saved.
```

The resulting files are stored in:

```text
tables/
```

On subsequent executions, they are loaded from disk instead of being regenerated.

---

## Search Output

Once the pruning tables are available, the solver performs the two-phase IDA* search.

### Phase 1

The program reports the search bounds explored during Phase 1:

```text
One level is explored in phase 1.
Bound: 7 | Next bound: 8 | Time: 0 ms

One level is explored in phase 1.
Bound: 8 | Next bound: 9 | Time: 0 ms
```

### Phase 2

Phase 2 similarly reports each completed search bound:

```text
Phase 2 bound 8 completed | next bound = 9 | time = 5.527e-06 seconds
Phase 2 bound 9 completed | next bound = 10 | time = 4.488e-06 seconds
Phase 2 bound 10 completed | next bound = 11 | time = 3.967e-05 seconds
Phase 2 bound 11 completed | next bound = 12 | time = 0.000269844 seconds
Phase 2 bound 12 completed | next bound = 13 | time = 0.00237412 seconds
Phase 2 bound 13 completed | next bound = -1 | time = 0.00528594 seconds
```

---

## Solution Output

After finding a solution, the solver prints the moves produced by each phase:

```text
The solution steps in first phase are:-
L B2 U' F U F L F R

In the second phase:-
L2 U' B2 R2 F2 U L2 U' L2 U2 F2 D F2

Total steps needed are 22
```

The complete solution is:

```text
L B2 U' F U F L F R L2 U' B2 R2 F2 U L2 U' L2 U2 F2 D F2
```

---

## Move Notation

The solver uses standard Rubik's Cube move notation.

| Notation                     | Meaning                    |
| ---------------------------- | -------------------------- |
| `U`, `D`, `L`, `R`, `F`, `B` | 90° clockwise face turn    |
| `'`                          | 90° counter-clockwise turn |
| `2`                          | 180° turn                  |

For example:

```text
R
```

means a clockwise right-face turn.

```text
R'
```

means a counter-clockwise right-face turn.

```text
R2
```

means a 180° right-face turn.

---

## Complete Solving Pipeline

A complete execution follows this pipeline:

```text
┌─────────────────────────┐
│ Sticker-color input     │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Centre-color detection  │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Sticker → facelet       │
│ conversion              │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Facelet → cubie         │
│ conversion              │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Cube validity checks    │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Load / generate         │
│ pruning tables          │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Phase 1 IDA*            │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Phase 2 IDA*            │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Solution move sequence  │
└─────────────────────────┘
```

The project therefore provides a complete command-line pipeline:

**Physical cube sticker colors → cubie representation → heuristic search → solving sequence**

---

## Known Issues / TODO

The solver currently:

* Runs successfully.
* Accepts and validates cube configurations.
* Generates and loads pruning tables.
* Performs both phases of the Kociemba search.
* Produces a complete solution sequence.

However, there is currently an issue when the generated solution is physically applied to the scrambled cube.

After applying the complete solution, **four edges remain flipped**:

```text
BL
BD
RB
RF
```

This suggests that there is still an issue somewhere in the:

* Edge orientation calculation (`eo`)
* Edge permutation/orientation transformations
* Move engine
* Solution application pipeline

The next major debugging task is therefore to investigate the edge-orientation calculation and/or the edge permutation/orientation transformations.

---
