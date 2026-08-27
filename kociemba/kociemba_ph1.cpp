#include "../inputproc/cubie.hpp"
#include "kociemba_ph1.hpp"
#include "../bfstable/bfs.hpp"
#include "../bfstable/encoder.hpp"
#include <algorithm>
#include "DFS.hpp"
#include "../bfstable/heuristictable.hpp"
#include <iostream>

// add near the top of kociemba_ph1.cpp, outside any function
namespace solution_sets
{
    std::vector<int> solution1;
    std::vector<int> solution2;
}


namespace
{
    int twist_sls_crdnt(Cubieste& cb)
    {
        Encoder ec;
        int cdco=ec.encodeCO(cb.co);
        int cdsl=ec.encodeSlice(cb.ep);
        int crdnt=cdco*495+cdsl;
        return crdnt;
    }
    int flip_sls_crdnt(Cubieste& cb)
    {
        Encoder ec;
        int cdeo=ec.encodeEO(cb.eo);
        int cdsl=ec.encodeSlice(cb.ep);
        int crdnt=cdeo*495+cdsl;
        return crdnt;
    }
}

int KociembaPhase1::cost_f_n1(Cubieste cb)
{
    int crdnt1=Heuristic::twistSliceTable[twist_sls_crdnt(cb)];
    int crdnt2=Heuristic::flipSliceTable[flip_sls_crdnt(cb)];
    return std::max(crdnt1,crdnt2);
}

bool KociembaPhase1::kociembaG1_sb_grp_chkr(Cubieste cb)
{
    for(int i=0;i<8;i++)
    {
        if(cb.co[i]!=0)
        {
            return false;
        }
    }
    for(int i=0;i<12;i++)
    {
        if(cb.eo[i]!=0)
            return false;
    }
    for(int i=0;i<8;i++)
    {
        if(cb.ep[i]>=8)
            return false;
    }
    return true;
}
bool KociembaPhase1::soln_chkr(Cubieste cb)
{
    for(int i=0;i<8;i++)
    {
        if(cb.cp[i]!=i||cb.co[i]!=0)
        return false;
    }
    for(int i=0;i<12;i++)
    {
        if(cb.ep[i]!=i||cb.eo[i]!=0)
        return false;
    }
    return true;
}

bool KociembaPhase1::kociembaPhase1(Cubieste& cb)
{
    int h=cost_f_n1(cb);
    Dfs df;
    while (true)
    {
        int result=df.dFs1(cb,0,h,-1);
        std::cout<<"one level is explored in phase 1 and now going to level "<<result<<"\n";
        if(result==-1)
        {
            return true;
        }
        h=result;
    }
    return false;
}