# Rubik's Cube Solver

A C++ implementation of a **Rubik's Cube solver based on Herbert Kociemba's Two-Phase Algorithm**.

The program presents an interactive, on-screen cube net that the user fills in with sticker colors, converts that input into a cubie-level representation, validates the cube configuration, and searches for a solution using **IDA*** guided by precomputed pruning tables.

---

## Table of Contents

* [Overview](#overview)
* [How the Solver Works](#how-the-solver-works)
* [Project Structure](#project-structure)
* [Coordinates and Pruning Tables](#coordinates-and-pruning-tables)
* [Building](#building)
* [Usage](#usage)
* [Input and Output](#input-and-output)

  * [Interactive Cube Editor](#interactive-cube-editor)
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
Interactive sticker-color input (ncurses)
        ↓
Centre-color detection & validation
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
│   └── Program entry point, centre-color mapping,
│       and top-level validation
│
├── inputproc/
│   ├── cube.hpp
│   │   └── Sticker-based cube representation
│   │
│   ├── cubie.hpp
│   │   └── Cubie-based representation (cp, co, ep, eo)
│   │
│   ├── cube_inp.hpp/.cpp
│   │   └── Interactive ncurses cube-net editor:
│   │       renders the net, handles arrow-key
│   │       navigation and colour entry, and
│   │       returns the six completed faces
│   │
│   └── stk_to_cub.hpp/.cpp
│       └── Sticker → cubie conversion. Builds a
│           self-consistent colour scheme from the
│           six centre stickers, then identifies each
│           corner/edge and its orientation, and
│           validates permutation parity
│
├── rubicsmove/
│   └── cubemove.hpp/.cpp
│       └── Cube move engine implementing
│           the 18 face turns
│
├── bfstable/
│   ├── encoder.hpp/.cpp
│   │   └── Coordinate encoding/decoding, including
│   │       bounds-checked Lehmer-code decoding
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

### Coordinate Decoding Safety

`Encoder::decodeLehmer8` and `Encoder::decodeLehmer4` (used to invert the corner-permutation and slice-permutation coordinates) now validate their input before decoding:

* A null output pointer throws `std::invalid_argument`.
* An index outside the valid range (`[0, 8!)` or `[0, 4!)`) throws `std::out_of_range`.
* Internally, the shared `decodePermutation<N>` helper also defensively clamps each digit to the remaining candidate count, so a corrupted or truncated coordinate can never read past the end of its working array.

This makes the coordinate layer fail loudly on malformed input instead of silently producing an undefined permutation.

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

It can be compiled directly using any compiler supporting **C++17**. Because the cube-input editor (`inputproc/cube_inp.cpp`) uses **ncurses** for the interactive terminal UI, `ncurses` must be installed and linked.

### Installing ncurses

```bash
# Debian / Ubuntu
sudo apt install libncurses-dev

# macOS (Homebrew)
brew install ncurses
```

### Using g++

From the project root:

```bash
g++ -std=c++17 -O2 \
    main.cpp \
    inputproc/stk_to_cub.cpp \
    inputproc/cube_inp.cpp \
    rubicsmove/cubemove.cpp \
    bfstable/encoder.cpp \
    bfstable/bfs.cpp \
    bfstable/heuristictable.cpp \
    kociemba/kociemba_ph1.cpp \
    kociemba/DFS.cpp \
    kociemba/DFS2.cpp \
    -lncurses \
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

An interactive, full-screen cube net opens in the terminal. Navigate it with the arrow keys and fill in every sticker's color, then press **ENTER** to hand the completed cube off to the solver.

---

# Input and Output

## Interactive Cube Editor

Input is no longer typed row-by-row. Instead, `CubeInput` (in `inputproc/cube_inp.cpp`) draws the full cube net with `ncurses` and lets you move a highlighted cursor around it:

```text
RUBIK'S CUBE INPUT
Arrow Keys: Move    U/Y/R/B/O/G: Set colour    ENTER: Confirm

                    [ ] [ ] [ ]

                    [ ] [ ] [ ]

                    [ ] [ ] [ ]

    L               F               R               B

    [ ] [ ] [ ]     [ ] [ ] [ ]     [ ] [ ] [ ]     [ ] [ ] [ ]

    [ ] [ ] [ ]     [ ] [ ] [ ]     [ ] [ ] [ ]     [ ] [ ] [ ]

    [ ] [ ] [ ]     [ ] [ ] [ ]     [ ] [ ] [ ]     [ ] [ ] [ ]



                    D

                    [ ] [ ] [ ]

                    [ ] [ ] [ ]

                    [ ] [ ] [ ]

Current: U [1,1]
Valid colours: W  Y  R  B  O  G
Lowercase letters are automatically converted to uppercase.
Press ENTER when the cube is completely entered.
```

Controls:

| Input                  | Effect                                                                 |
| ----------------------- | ----------------------------------------------------------------------- |
| Arrow keys             | Move the highlighted cursor between stickers, wrapping between faces (e.g. off the right edge of `L` moves onto `F`, off the bottom of `U` moves onto `F`, and so on) |
| `W` `Y` `R` `B` `O` `G` | Set the color of the currently highlighted sticker (lowercase is upper-cased automatically) |
| `ENTER`                 | Finish input and hand the completed net to the solver                 |

The currently selected sticker is shown in reverse video, and the status line at the bottom (`Current: U [1,1]`) always reports which face/row/column the cursor is on. Every sticker starts blank (`[ ]`) so it's obvious at a glance which ones still need a color before pressing `ENTER`.

---

## Centre-Color Detection

The centre sticker of each face determines the relationship between the physical cube's colors and the solver's internal face notation (`U F D R L B`). Centre stickers never move relative to one another, so whatever color sits at a face's centre position *is* that face's color for this particular cube — nothing is hardcoded to a fixed scheme like "white = Up".

This mapping is validated in two places:

1. **`main.cpp` (`tsmltoseg::usrinp`)** — after the editor returns, it checks that all six centre colors are distinct and that every centre color occurs exactly 9 times across the whole cube, exiting with an error message before any solving work begins if either check fails. It then builds a `colour → face` map from the six centres and converts every sticker on every face into `U`/`F`/`D`/`R`/`L`/`B` notation.
2. **`stk_to_cub.cpp` (`ColourScheme` / `buildColourScheme`)** — independently builds the same kind of colour-to-face mapping from the (already face-labelled) cube's centres, throwing `std::invalid_argument` if two centres share a color or if a sticker's color never appears as any centre.

Example output:

```text
Detected centre mapping:
U = O
F = Y
D = R
R = G
L = B
B = W
```

This allows the solver to work with any physical color orientation rather than assuming a specific color always maps to a specific face.

---

## Sticker-to-Cubie Conversion

After the interactive input is complete and converted to `UFDRLB` notation, the solver converts the sticker representation into its internal **cubie representation**.

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

### Corner Identification and Orientation

Each physical corner position is checked against the three stickers actually observed there. `decodeCornerAt` looks for a **cyclic rotation** match against every possible corner identity's "home" sticker triple — a real corner can only be twisted in place, never mirrored, so only rotation matches are accepted. Anything else (an impossible or mistyped sticker combination) is rejected with a thrown `std::runtime_error` instead of being silently forced into some identity.

Once the identity (`cp`) is found, orientation (`co`) is derived from which physical slot the piece's U/D-facing sticker landed in. Because the two "chirality groups" of corner positions map their R/L and F/B side-stickers to `co = 1` vs `co = 2` in opposite ways, this mapping is looked up per-position (`isGroupA[]`) rather than assumed to be uniform across all eight corners — this is what previously allowed some mirrored/impossible corners to slip through undetected.

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

### Edge Identification and Orientation

The twelve edges are processed similarly: each position's two observed stickers are compared to every edge identity's home pair in both orders — a match in the original order means `eo = 0`, a match in swapped order means `eo = 1`. A position that matches neither order for any identity throws `std::runtime_error`.

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

This conversion stage lets the solver reject invalid cube configurations before starting the search, rather than searching against a malformed cube state.

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
│ Interactive sticker      │
│ input (ncurses)          │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Centre-color detection  │
│ & validation             │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Sticker → facelet       │
│ conversion              │
└────────────┬────────────┘
             ↓
┌─────────────────────────┐
│ Facelet → cubie         │
│ conversion (chirality-  │
│ aware, rejects mirrors) │
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

The project therefore provides a complete interactive pipeline:

**Physical cube sticker colors → cubie representation → heuristic search → solving sequence**

---

## Known Issues / TODO

The solver currently:

* Runs successfully with the new interactive ncurses cube editor.
* Accepts and validates cube configurations, including centre-color duplication/coverage checks and chirality-aware corner/edge identification that rejects impossible or mirrored input.
* Generates and loads all four pruning tables (Phase 1 and Phase 2).
* Performs both phases of the Kociemba search with bounds-checked coordinate decoding.
* Produces a complete solution sequence.

Open items:

* **Re-verify the previously reported edge-flip bug.** An earlier version of this README noted that after applying the generated solution to a physical cube, four edges (`BL`, `BD`, `RB`, `RF`) remained flipped. The sticker-to-cubie decoding in `stk_to_cub.cpp` has since been substantially rewritten — corner identity/orientation now uses proper cyclic-rotation matching with per-position chirality handling instead of the old unordered-set comparison that could let mirrored/impossible corners through undetected. This class of bug is a plausible explanation for the original symptom, but it hasn't been explicitly re-tested against a physical cube yet, so it should be re-verified before being considered closed.
* **Inappropriate language in `main.cpp`.** The "already solved" message contains an offensive slur and should be replaced with a normal message (e.g. `"The cube is already solved."`).
* **Leftover debug `std::cout` statements in `stk_to_cub.cpp`.** `decodeCornerAt` and `decodeEdgeAt` unconditionally print every corner/edge decode to stdout. This is useful for the documented example output above, but should probably be gated behind a `--verbose` flag rather than always firing.
* **Stale TODO comment in `heuristictable.cpp`.** `generateTables()` still has a leftover `// TODO: bf cpslice / bf udedge_slice` comment even though both calls (`bf.bfscpsls(...)`, `bf.bfsepsls(...)`) are already implemented directly above it. The comment should be removed.
* **No automated tests.** There is currently no test suite covering the encoder/decoder round-trips, move engine, or search correctness.
* **No build system file.** The project still relies on a manual `g++` command; a `CMakeLists.txt` or `Makefile` (now also needs to link `ncurses`) would make builds more reproducible.

---
