#ifndef KOCIEMBA1
#define KOCIEMBA1
#include "../inputproc/cubie.hpp"



class KociembaPhase1
{
    private:
        bool kociembaG1_sb_grp_chkr(Cubieste cb);
        uint8_t cost_f_n(Cubieste cb);
    public:
        Cubieste kociembaPhase1(Cubieste cb);
        bool soln_chkr(Cubieste cb);
};

#endif