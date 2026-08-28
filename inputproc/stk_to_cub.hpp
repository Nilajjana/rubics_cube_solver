#ifndef STICKER_HPP
#define STICKER_HPP
#include <array>
#include "cube.hpp"
#include "cubie.hpp"

// Fixed physical colour scheme for a SOLVED cube:
//   White = Up, Green = Front, Yellow = Down,
//   Red = Right, Orange = Left, Blue = Back.
//
// This is now the ONLY place the colour scheme lives (see colourToFace()
// in stk_to_cub.cpp). The old cornerTable/edgeTable colour tables have
// been removed: they were a second, independent source of truth that
// could drift from the position tables and were matched with an
// unordered set comparison, which is what let mirrored/impossible
// corners through undetected.

class inpproc
{
    public:
        Cubieste cornerinfer(Cube& cube, Cubieste cb);
        Cubieste edgeinfer(Cube& cube, Cubieste cb);
    public:
        bool inpvald(const uint8_t* p, int n);
};
#endif