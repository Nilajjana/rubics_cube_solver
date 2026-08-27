#ifndef DFS
#define DFS
#include "../inputproc/cubie.hpp"
#include <vector>

namespace redundancy_chk
{
    inline bool redun_dnt(int lastmv,int mv)
    {
        if(lastmv/3==mv/3)
            return true;
        return false;
    }
}

class Dfs
{
    public:
        int dFs1(const Cubieste& cb,int g,int bound,int lastmv);
};

#endif 