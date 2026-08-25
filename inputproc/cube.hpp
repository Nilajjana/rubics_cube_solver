#ifndef CUBE_H
#define CUBE_H
#include <array>
struct Cube
{
    std::array<char, 9> U;
    std::array<char, 9> F;
    std::array<char, 9> D;
    std::array<char, 9> R;
    std::array<char, 9> L;
    std::array<char, 9> B;
};
#endif