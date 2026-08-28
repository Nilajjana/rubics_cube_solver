#include <array>
#include "stk_to_cub.hpp"
#include "cube.hpp"
#include "cubie.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

enum Face
{
    U, F, D, R, L, B
};

struct Stickerpos
{
    Face face;
    int index;
};

struct Corner
{
    Stickerpos sticker[3];
};

// Fixed physical position -> facelet index table.
// Order matches cp[] indices 0..7: UFR, UFL, ULB, UBR, DFR, DFL, DLB, DRB.
// sticker[0] of every entry is always the U/D-facing facelet of that
// corner position (unchanged from the original table).
const Corner corner[8] =
{
    {{{U, 8}, {F, 2}, {R, 0}}},   // UFR
    {{{U, 6}, {F, 0}, {L, 2}}},   // UFL
    {{{U, 0}, {L, 0}, {B, 2}}},   // ULB
    {{{U, 2}, {B, 0}, {R, 2}}},   // UBR
    {{{D, 2}, {F, 8}, {R, 6}}},   // DFR
    {{{D, 0}, {F, 6}, {L, 8}}},   // DFL
    {{{D, 6}, {L, 6}, {B, 8}}},   // DLB
    {{{D, 8}, {R, 8}, {B, 6}}}    // DRB
};

static const char* CORNER_NAMES[8] =
{
    "UFR", "UFL", "ULB", "UBR", "DFR", "DFL", "DLB", "DRB"
};

struct Edge
{
    Stickerpos sticker[2];
};

// Order matches ep[] indices 0..11: UF,UL,UB,UR,DF,DL,DB,DR,FL,LB,BR,RF.
const Edge edge[12] =
{
    {{{U, 7}, {F, 1}}},   // UF
    {{{U, 3}, {L, 1}}},   // UL
    {{{U, 1}, {B, 1}}},   // UB
    {{{U, 5}, {R, 1}}},   // UR
    {{{D, 1}, {F, 7}}},   // DF
    {{{D, 3}, {L, 7}}},   // DL
    {{{D, 7}, {B, 7}}},   // DB
    {{{D, 5}, {R, 7}}},   // DR
    {{{F, 3}, {L, 5}}},   // FL
    {{{L, 3}, {B, 5}}},   // LB
    {{{B, 3}, {R, 5}}},   // BR
    {{{R, 3}, {F, 5}}}    // RF
};

static const char* EDGE_NAMES[12] =
{
    "UF", "UL", "UB", "UR", "DF", "DL", "DB", "DR", "FL", "LB", "BR", "RF"
};

static char getsticker(Cube& cube, Stickerpos p)
{
    switch (p.face)
    {
        case U: return cube.U[p.index];
        case F: return cube.F[p.index];
        case D: return cube.D[p.index];
        case R: return cube.R[p.index];
        case L: return cube.L[p.index];
        case B: return cube.B[p.index];
    }
    return '?';
}

// The colour scheme is no longer hardcoded. Centre stickers never move
// relative to each other, so whatever colour sits at the centre of a
// face position IS that face's colour on this particular cube -- read
// it straight from the input instead of assuming white=Up etc. This
// makes the decoder work regardless of which way the cube happens to
// be held/photographed, and for any (self-consistent) sticker set, not
// just the standard WCA colours.
struct ColourScheme
{
    char toFace[256];   // toFace[(unsigned char)colour] = face letter, or 0 if unset

    char lookup(char colour) const
    {
        char f = toFace[(unsigned char)colour];
        if (f == 0)
            throw std::invalid_argument(
                std::string("Sticker colour '") + colour +
                "' never appears as a face centre, so it can't be "
                "mapped to a face. Check the input.");
        return f;
    }
};

// index 4 of a 3x3 face (row-major) is always the centre sticker.
static ColourScheme buildColourScheme(Cube& cube)
{
    ColourScheme scheme{};
    for (auto& c : scheme.toFace)
        c = 0;

    const struct { char face; char colour; } centres[6] =
    {
        { 'U', cube.U[4] },
        { 'F', cube.F[4] },
        { 'D', cube.D[4] },
        { 'R', cube.R[4] },
        { 'L', cube.L[4] },
        { 'B', cube.B[4] },
    };

    for (const auto& c : centres)
    {
        unsigned char key = (unsigned char)c.colour;
        if (scheme.toFace[key] != 0)
            throw std::invalid_argument(
                std::string("Two face centres share the colour '") +
                c.colour + "' -- a cube can't have two centres the same "
                "colour. Check the input.");
        scheme.toFace[key] = c.face;
    }

    return scheme;
}

// "Home" letters for corner position j: what should be read at that
// position's three sticker slots on a solved cube. Derived directly
// from the position table above, so there is only one table to keep
// in sync, not two.
static std::array<char, 3> homeCornerLetters(int j)
{
    static const char faceChar[6] = { 'U', 'F', 'D', 'R', 'L', 'B' };
    return {
        faceChar[corner[j].sticker[0].face],
        faceChar[corner[j].sticker[1].face],
        faceChar[corner[j].sticker[2].face]
    };
}

static std::array<char, 2> homeEdgeLetters(int j)
{
    static const char faceChar[6] = { 'U', 'F', 'D', 'R', 'L', 'B' };
    return {
        faceChar[edge[j].sticker[0].face],
        faceChar[edge[j].sticker[1].face]
    };
}

// Reads the actual stickers at physical position i and finds which
// corner identity sits there and at what orientation, by checking
// every CYCLIC ROTATION of every home triple. A corner can physically
// only be twisted, never mirrored, so only a rotation match is
// accepted -- anything else (impossible / mistyped input) is rejected
// with an error instead of being silently forced to some identity.
static void decodeCornerAt(Cube& cube, const ColourScheme& scheme, int i,
                            uint8_t& cpOut, uint8_t& coOut)
{
    std::array<char, 3> actual = {
        scheme.lookup(getsticker(cube, corner[i].sticker[0])),
        scheme.lookup(getsticker(cube, corner[i].sticker[1])),
        scheme.lookup(getsticker(cube, corner[i].sticker[2]))
    };

    for (int j = 0; j < 8; ++j)
    {
        auto home = homeCornerLetters(j);

        for (int r = 0; r < 3; ++r)
        {
            if (actual[0] == home[(0 + r) % 3] &&
                actual[1] == home[(1 + r) % 3] &&
                actual[2] == home[(2 + r) % 3])
            {
                cpOut = static_cast<uint8_t>(j);
                // r is how far the home triple had to rotate to reach
                // the observed stickers; co is defined (matching the
                // original getCO behaviour) as the slot index within
                // the observed triple where the U/D-facing letter
                // ended up, i.e. (3 - r) % 3.
                coOut = static_cast<uint8_t>((3 - r) % 3);

                std::cout << "Corner " << CORNER_NAMES[i] << ": "
                          << actual[0] << actual[1] << actual[2]
                          << " -> cp = " << j << ", co = " << (int)coOut
                          << '\n';
                return;
            }
        }
    }

    throw std::runtime_error(
        std::string("ERROR: corner ") + CORNER_NAMES[i] +
        " (stickers " + actual[0] + actual[1] + actual[2] +
        ") does not match any valid corner. This means either a typo "
        "in the input or a physically impossible (mirrored) cube.");
}

// Same idea for edges. An edge has only two stickers, so the only two
// possibilities are "same order as home" (eo = 0) or "swapped" (eo = 1);
// there is no mirrored/rotated case to worry about.
static void decodeEdgeAt(Cube& cube, const ColourScheme& scheme, int i,
                          uint8_t& epOut, uint8_t& eoOut)
{
    std::array<char, 2> actual = {
        scheme.lookup(getsticker(cube, edge[i].sticker[0])),
        scheme.lookup(getsticker(cube, edge[i].sticker[1]))
    };

    for (int j = 0; j < 12; ++j)
    {
        auto home = homeEdgeLetters(j);

        if (actual[0] == home[0] && actual[1] == home[1])
        {
            epOut = static_cast<uint8_t>(j);
            eoOut = 0;
            std::cout << "Edge " << EDGE_NAMES[i] << ": "
                      << actual[0] << actual[1]
                      << " -> ep = " << j << ", eo = 0\n";
            return;
        }
        if (actual[0] == home[1] && actual[1] == home[0])
        {
            epOut = static_cast<uint8_t>(j);
            eoOut = 1;
            std::cout << "Edge " << EDGE_NAMES[i] << ": "
                      << actual[0] << actual[1]
                      << " -> ep = " << j << ", eo = 1\n";
            return;
        }
    }

    throw std::runtime_error(
        std::string("ERROR: edge ") + EDGE_NAMES[i] +
        " (stickers " + actual[0] + actual[1] +
        ") does not match any known edge. Check your input.");
}

Cubieste inpproc::cornerinfer(Cube& cube, Cubieste cst)
{
    ColourScheme scheme = buildColourScheme(cube);

    for (int i = 0; i < 8; i++)
    {
        uint8_t cp, co;
        decodeCornerAt(cube, scheme, i, cp, co);
        cst.cp[i] = cp;
        cst.co[i] = co;
    }
    return cst;
}

Cubieste inpproc::edgeinfer(Cube& cube, Cubieste cst)
{
    ColourScheme scheme = buildColourScheme(cube);

    for (int i = 0; i < 12; i++)
    {
        uint8_t ep, eo;
        decodeEdgeAt(cube, scheme, i, ep, eo);
        cst.ep[i] = ep;
        cst.eo[i] = eo;
    }
    return cst;
}

bool inpproc::inpvald(const uint8_t* p, int n)
{
    std::vector<bool> visited(n, false);
    int j = 0, cycles = 0;
    for (int i = 0; i < n; i++)
    {
        if (!visited[i])
        {
            cycles++;
            j = i;
            while (!visited[j])
            {
                visited[j] = true;
                j = p[j];
            }
        }
    }
    return (n - cycles) % 2;
}