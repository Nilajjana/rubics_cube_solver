#ifndef DFS2
#define DFS2
#include "../inputproc/cubie.hpp"
#include <vector>

namespace moves2
{
    constexpr int kPhase2Moves[10] = { 0, 1, 2, 4, 7, 10, 13, 15, 16, 17 };   // U, U2, U', F2, B2, L2, R2, D, D2, D'
}

class Dfs2
{
    public:
        bool kociembaPhase2(Cubieste cb);
        int dFs2(const Cubieste& cb,int g,int bound,int lastmv);
        int cost_f_n2(Cubieste cb);
};

#endif 