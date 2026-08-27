#ifndef KOCIEMBA1
#define KOCIEMBA1
#include "../inputproc/cubie.hpp"
#include <vector>

namespace solution_sets
{
    extern std::vector<int> solution1;
    extern std::vector<int> solution2;
}

class KociembaPhase1
{
    public:
        bool kociembaG1_sb_grp_chkr(Cubieste cb);
        bool kociembaPhase1(Cubieste& cb);
        bool soln_chkr(Cubieste cb);
        int cost_f_n1(Cubieste cb);
};

#endif