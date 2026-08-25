#ifndef STICKER_HPP
#define STICKER_HPP
#include <array>
#include "cube.hpp"
#include "cubie.hpp"

struct CornerInfo
{
    const char* name;
    std::array<char, 3> colors;
};
struct EdgeInfo
{
    const char* name;
    std::array<char, 2> colors;
};

const CornerInfo cornerTable[8] =
{
    {"UFR", {'W', 'G', 'R'}},
    {"UFL", {'W', 'G', 'O'}},
    {"ULB", {'W', 'O', 'B'}},
    {"UBR", {'W', 'B', 'R'}},

    {"DFR", {'Y', 'G', 'R'}},
    {"DFL", {'Y', 'G', 'O'}},
    {"DLB", {'Y', 'O', 'B'}},
    {"DRB", {'Y', 'R', 'B'}}
};

const EdgeInfo edgeTable[12] =
{
    {"UF", {'W', 'G'}},
    {"UL", {'W', 'O'}},
    {"UB", {'W', 'B'}},
    {"UR", {'W', 'R'}},

    {"DF", {'Y', 'G'}},
    {"DL", {'Y', 'O'}},
    {"DB", {'Y', 'B'}},
    {"DR", {'Y', 'R'}},

    {"FL", {'G', 'O'}},
    {"LB", {'O', 'B'}},
    {"BR", {'B', 'R'}},
    {"RF", {'R', 'G'}}
};

class inpproc
{
    public:
        Cubieste cornerinfer(Cube& cube,Cubieste cb);
        Cubieste edgeinfer(Cube& cube,Cubieste cb);
    public:
        bool inpvald(const uint8_t* p,int n);
};
#endif