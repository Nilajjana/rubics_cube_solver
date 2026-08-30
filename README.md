## Input and Output

The solver uses an **interactive terminal-based input/output interface**.

### Input

When the program starts, it first displays a net of the cube showing the positions of all 54 facelets:

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

The program then asks for the six faces in the following order:

```text
U F D R L B
```

Each face requires **9 sticker colors**, entered as capital-letter color symbols:

| Letter | Color  |
| ------ | ------ |
| `W`    | White  |
| `Y`    | Yellow |
| `R`    | Red    |
| `B`    | Blue   |
| `O`    | Orange |
| `G`    | Green  |

The nine stickers can be entered as three rows of three characters. For example:

```text
Enter U face with capital letter colour symbol like W=white Y=yellow R=red B=blue O=orange G=green:
BRG
ROB
GOR
```

The same process is repeated for the remaining five faces:

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

### Centre-Color Detection

The center sticker of each face is used to determine the relationship between the input colors and the solver's internal face notation.

For example, the program may report:

```text
Detected centre mapping:
U = O
F = Y
D = R
R = G
L = B
B = W
```

This allows the solver to work with arbitrary physical color orientations rather than assuming that a particular color is always on a particular face.

### Sticker-to-Cubie Conversion

After reading the input, the solver converts the sticker representation into its internal **cubie representation**.

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

The program then identifies every corner and edge, calculating its permutation and orientation.

Example corner output:

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

It also validates the corner orientation constraint:

```text
Corner orientation sum = 0 + 2 + 1 + 1 + 2 + 0 + 0 + 0 = 6 mod 3 = 0
```

The twelve edges are processed in the same way:

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

### Pruning Table Generation

If the pruning tables do not already exist, the program generates them automatically:

```text
Pruning tables not found.
Generating tables...
```

The four tables are generated using BFS:

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

The generated tables are saved in the `tables/` directory. On subsequent executions, they are loaded from disk instead of being regenerated.

### Search Output

Once the tables are available, the solver performs the two-phase IDA* search.

During Phase 1, the program reports the search bounds:

```text
One level is explored in phase 1. Bound: 7 | Next bound: 8 | Time: 0 ms
One level is explored in phase 1. Bound: 8 | Next bound: 9 | Time: 0 ms
```

Phase 2 similarly reports each completed search bound:

```text
Phase 2 bound 8 completed | next bound = 9 | time = 5.527e-06 seconds
Phase 2 bound 9 completed | next bound = 10 | time = 4.488e-06 seconds
Phase 2 bound 10 completed | next bound = 11 | time = 3.967e-05 seconds
Phase 2 bound 11 completed | next bound = 12 | time = 0.000269844 seconds
Phase 2 bound 12 completed | next bound = 13 | time = 0.00237412 seconds
Phase 2 bound 13 completed | next bound = -1 | time = 0.00528594 seconds
```

After a solution is found, the solver prints the moves produced by each phase:

```text
the solution steps in first phase are:-
L B2 U' F U F L F R

in the second phase:-
L2 U' B2 R2 F2 U L2 U' L2 U2 F2 D F2

total steps needed are 22
```

The complete solution is therefore:

```text
L B2 U' F U F L F R L2 U' B2 R2 F2 U L2 U' L2 U2 F2 D F2
```

The notation follows standard Rubik's Cube move notation:

| Notation                     | Meaning                    |
| ---------------------------- | -------------------------- |
| `U`, `D`, `L`, `R`, `F`, `B` | 90° clockwise face turn    |
| `'`                          | 90° counter-clockwise turn |
| `2`                          | 180° turn                  |

For example:

```text
R
```

means a clockwise right-face turn,

```text
R'
```

means a counter-clockwise right-face turn, and

```text
R2
```

means a 180° right-face turn.

### Complete Example

A typical complete execution therefore follows this pipeline:

```text
Sticker-color input
       ↓
Centre-color detection
       ↓
Sticker → facelet conversion
       ↓
Facelet → cubie conversion
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

This makes the program a complete command-line pipeline from **physical cube sticker colors → cubie representation → heuristic search → solving sequence**.

## Known Issues / TODO

* The solver currently runs successfully and produces a complete solution sequence.
* However, when the generated solution is physically applied to the scrambled cube, **four edges remain flipped** at the end:

  * BL
  * BD
  * RB
  * RF
* This indicates that there is still an issue somewhere in the **edge orientation / move transformation / solution application pipeline**, despite the cube passing the current input validation and the search producing a solution.
* Investigating the edge-orientation calculation (`eo`) and/or edge permutation/orientation transformations in the move engine is the next major debugging task.

