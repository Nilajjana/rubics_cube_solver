#include "../inputproc/cubie.hpp"
#include "cubemove.hpp"


// ============================================================
// U
// ============================================================

Cubieste Moves::U(const Cubieste& cube)
{
    Cubieste result = cube;

    // Corners
    result.cp[0] = cube.cp[3];
    result.cp[1] = cube.cp[0];
    result.cp[2] = cube.cp[1];
    result.cp[3] = cube.cp[2];

    result.co[0] = cube.co[3];
    result.co[1] = cube.co[0];
    result.co[2] = cube.co[1];
    result.co[3] = cube.co[2];

    // Edges
    result.ep[0] = cube.ep[3];
    result.ep[1] = cube.ep[0];
    result.ep[2] = cube.ep[1];
    result.ep[3] = cube.ep[2];

    result.eo[0] = cube.eo[3];
    result.eo[1] = cube.eo[0];
    result.eo[2] = cube.eo[1];
    result.eo[3] = cube.eo[2];

    return result;
}


// ============================================================
// U'
// ============================================================

Cubieste Moves::U_PRIME(const Cubieste& cube)
{
    return U2(U(cube));
}


// ============================================================
// U2
// ============================================================

Cubieste Moves::U2(const Cubieste& cube)
{
    return U(U(cube));
}


// ============================================================
// D
// ============================================================

Cubieste Moves::D(const Cubieste& cube)
{
    Cubieste result = cube;

    // Corners
    result.cp[4] = cube.cp[5];
    result.cp[5] = cube.cp[6];
    result.cp[6] = cube.cp[7];
    result.cp[7] = cube.cp[4];

    result.co[4] = cube.co[5];
    result.co[5] = cube.co[6];
    result.co[6] = cube.co[7];
    result.co[7] = cube.co[4];

    // Edges
    result.ep[4] = cube.ep[5];
    result.ep[5] = cube.ep[6];
    result.ep[6] = cube.ep[7];
    result.ep[7] = cube.ep[4];

    result.eo[4] = cube.eo[5];
    result.eo[5] = cube.eo[6];
    result.eo[6] = cube.eo[7];
    result.eo[7] = cube.eo[4];

    return result;
}


// ============================================================
// D'
// ============================================================

Cubieste Moves::D_PRIME(const Cubieste& cube)
{
    return D2(D(cube));
}


// ============================================================
// D2
// ============================================================

Cubieste Moves::D2(const Cubieste& cube)
{
    return D(D(cube));
}


// ============================================================
// R
// ============================================================

Cubieste Moves::R(const Cubieste& cube)
{
    Cubieste result = cube;

    // Corners
    result.cp[0] = cube.cp[4];
    result.cp[3] = cube.cp[0];
    result.cp[7] = cube.cp[3];
    result.cp[4] = cube.cp[7];

    // Corner orientation
    result.co[0] = (cube.co[4] + 2) % 3;
    result.co[3] = (cube.co[0] + 1) % 3;
    result.co[7] = (cube.co[3] + 2) % 3;
    result.co[4] = (cube.co[7] + 1) % 3;

    // Edges
    //
    // Your indices:
    // UR = 3
    // DR = 7
    // BR = 10
    // RF = 11

    result.ep[3]  = cube.ep[11]; // UR <- RF
    result.ep[7]  = cube.ep[10]; // DR <- BR
    result.ep[10] = cube.ep[7];  // BR <- DR
    result.ep[11] = cube.ep[3];  // RF <- UR

    // R does not flip edge orientation
    result.eo[3]  = cube.eo[11];
    result.eo[7]  = cube.eo[10];
    result.eo[10] = cube.eo[7];
    result.eo[11] = cube.eo[3];

    return result;
}


// ============================================================
// R'
// ============================================================

Cubieste Moves::R_PRIME(const Cubieste& cube)
{
    return R2(R(cube));
}


// ============================================================
// R2
// ============================================================

Cubieste Moves::R2(const Cubieste& cube)
{
    return R(R(cube));
}


// ============================================================
// L
// ============================================================

Cubieste Moves::L(const Cubieste& cube)
{
    Cubieste result = cube;

    // Corners
    result.cp[1] = cube.cp[2];
    result.cp[2] = cube.cp[6];
    result.cp[5] = cube.cp[1];
    result.cp[6] = cube.cp[5];

    // Corner orientation
    result.co[1] = (cube.co[2] + 1) % 3;
    result.co[2] = (cube.co[6] + 2) % 3;
    result.co[5] = (cube.co[1] + 2) % 3;
    result.co[6] = (cube.co[5] + 1) % 3;

    // Edges
    result.ep[1] = cube.ep[9]; // UL <- LB
    result.ep[5] = cube.ep[8]; // DL <- FL
    result.ep[8] = cube.ep[1]; // FL <- UL
    result.ep[9] = cube.ep[5]; // LB <- DL

    result.eo[1] = cube.eo[9];
    result.eo[5] = cube.eo[8];
    result.eo[8] = cube.eo[1];
    result.eo[9] = cube.eo[5];

    return result;
}


// ============================================================
// L'
// ============================================================

Cubieste Moves::L_PRIME(const Cubieste& cube)
{
    return L2(L(cube));
}


// ============================================================
// L2
// ============================================================

Cubieste Moves::L2(const Cubieste& cube)
{
    return L(L(cube));
}


// ============================================================
// F
// ============================================================

Cubieste Moves::F(const Cubieste& cube)
{
    Cubieste result = cube;

    // Corners
    result.cp[0] = cube.cp[1];
    result.cp[1] = cube.cp[5];
    result.cp[4] = cube.cp[0];
    result.cp[5] = cube.cp[4];

    // Corner orientation
    result.co[0] = (cube.co[1] + 1) % 3;
    result.co[1] = (cube.co[5] + 2) % 3;
    result.co[4] = (cube.co[0] + 2) % 3;
    result.co[5] = (cube.co[4] + 1) % 3;

    // Edges
    result.ep[0]  = cube.ep[11]; // UF <- RF
    result.ep[4]  = cube.ep[8];  // DF <- FL
    result.ep[8]  = cube.ep[4];  // FL <- DF
    result.ep[11] = cube.ep[0];  // RF <- UF

    // F flips these four edges
    result.eo[0]  = cube.eo[11] ^ 1;
    result.eo[4]  = cube.eo[8]  ^ 1;
    result.eo[8]  = cube.eo[4]  ^ 1;
    result.eo[11] = cube.eo[0]  ^ 1;

    return result;
}


// ============================================================
// F'
// ============================================================

Cubieste Moves::F_PRIME(const Cubieste& cube)
{
    return F2(F(cube));
}


// ============================================================
// F2
// ============================================================

Cubieste Moves::F2(const Cubieste& cube)
{
    return F(F(cube));
}


// ============================================================
// B
// ============================================================

Cubieste Moves::B(const Cubieste& cube)
{
    Cubieste result = cube;

    // Corners
    result.cp[2] = cube.cp[3];
    result.cp[3] = cube.cp[7];
    result.cp[6] = cube.cp[2];
    result.cp[7] = cube.cp[6];

    // Corner orientation
    result.co[2] = (cube.co[3] + 1) % 3;
    result.co[3] = (cube.co[7] + 2) % 3;
    result.co[6] = (cube.co[2] + 2) % 3;
    result.co[7] = (cube.co[6] + 1) % 3;

    // Edges
    result.ep[2]  = cube.ep[10]; // UB <- BR
    result.ep[6]  = cube.ep[9];  // DB <- LB
    result.ep[9]  = cube.ep[2];  // LB <- UB
    result.ep[10] = cube.ep[6];  // BR <- DB

    // B flips these four edges
    result.eo[2]  = cube.eo[10] ^ 1;
    result.eo[6]  = cube.eo[9]  ^ 1;
    result.eo[9]  = cube.eo[2]  ^ 1;
    result.eo[10] = cube.eo[6]  ^ 1;

    return result;
}


// ============================================================
// B'
// ============================================================

Cubieste Moves::B_PRIME(const Cubieste& cube)
{
    return B2(B(cube));
}


// ============================================================
// B2
// ============================================================

Cubieste Moves::B2(const Cubieste& cube)
{
    return B(B(cube));
}

Cubieste Moves::applyMove(const Cubieste& cube, int mv)
{
    switch(mv)
    {
        case 0:  return U(cube);
        case 1:  return U2(cube);
        case 2:  return U_PRIME(cube);

        case 3:  return F(cube);
        case 4:  return F2(cube);
        case 5:  return F_PRIME(cube);

        case 6:  return B(cube);
        case 7:  return B2(cube);
        case 8:  return B_PRIME(cube);

        case 9:  return L(cube);
        case 10: return L2(cube);
        case 11: return L_PRIME(cube);

        case 12: return R(cube);
        case 13: return R2(cube);
        case 14: return R_PRIME(cube);

        case 15: return D(cube);
        case 16: return D2(cube);
        case 17: return D_PRIME(cube);
    }
    return cube; // invalid index, should never happen
}