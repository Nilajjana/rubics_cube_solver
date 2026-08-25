#pragma once

#include "../inputproc/cubie.hpp"

class Moves
{
public:

    static Cubieste U(const Cubieste& cube);
    static Cubieste U_PRIME(const Cubieste& cube);
    static Cubieste U2(const Cubieste& cube);

    static Cubieste D(const Cubieste& cube);
    static Cubieste D_PRIME(const Cubieste& cube);
    static Cubieste D2(const Cubieste& cube);

    static Cubieste L(const Cubieste& cube);
    static Cubieste L_PRIME(const Cubieste& cube);
    static Cubieste L2(const Cubieste& cube);

    static Cubieste R(const Cubieste& cube);
    static Cubieste R_PRIME(const Cubieste& cube);
    static Cubieste R2(const Cubieste& cube);

    static Cubieste F(const Cubieste& cube);
    static Cubieste F_PRIME(const Cubieste& cube);
    static Cubieste F2(const Cubieste& cube);

    static Cubieste B(const Cubieste& cube);
    static Cubieste B_PRIME(const Cubieste& cube);
    static Cubieste B2(const Cubieste& cube);

    static Cubieste applyMove(const Cubieste& cube, int mv);
};