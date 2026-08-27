#ifndef KOCIEMBA2
#define KOCIEMBA2

#include "../inputproc/cubie.hpp"

class KociembaPhase2
{
    private:
        bool solved_chker(Cubieste cb);
    public:
        Cubieste kociembaPhase2(Cubieste cb);
};

#endif