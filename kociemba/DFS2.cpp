
#include "../inputproc/cubie.hpp"
#include <vector>
#include "DFS2.hpp"
#include "kociemba_ph1.hpp"
#include "../bfstable/bfs.hpp"
#include "../bfstable/encoder.hpp"
#include "../bfstable/heuristictable.hpp"
#include "../rubicsmove/cubemove.hpp"
#include "DFS.hpp"
#include <iostream>

namespace
{
    int cp_sls_crdnt(Cubieste cb)
    {
        Encoder ec;
        int cdcp=ec.lehmer8coder(cb.cp);
        int cdsl2=ec.lehmer4(&cb.ep[8]);
        int crdnt=cdcp*24+cdsl2;
        return crdnt;
    }
    int udEdge_sls_crdnt(Cubieste cb)
    {
        Encoder ec;
        int cdep=ec.lehmer8coder(cb.ep);
        int cdsl2=ec.lehmer4(&cb.ep[8]);
        int crdnt=cdep*24+cdsl2;
        return crdnt;
    }
}

int Dfs2::cost_f_n2(Cubieste cb)
{
    int crdnt1=Heuristic::cpSliceTable[cp_sls_crdnt(cb)];
    int crdnt2=Heuristic::udEdgeSliceTable[udEdge_sls_crdnt(cb)];
    return std::max(crdnt1,crdnt2);
}

bool Dfs2::kociembaPhase2(Cubieste cb)
{
    int h=cost_f_n2(cb);
    while (true)
    {
        int result=dFs2(cb,0,h,-1);
        std::cout<<"one level is explored in phase 2 \n";
        if(result==-1)
        {
            return true;
        }
        h=result;
    }
    return false;
}

int Dfs2::dFs2(const Cubieste& cb,int g,int bound,int lastmv)
{
    Dfs2 kp2;
    KociembaPhase1 kp1;
    int h=kp2.cost_f_n2(cb);
    int f=g+h;
    if(f>bound)
        return f;
    if(kp1.soln_chkr(cb))
    {
        return -1;
    }
    Cubieste next;
    int minin=255;
    Moves mov;
    std::cout<<"going to explore level "<<g<<"\n";
    for(int i:moves2::kPhase2Moves)
    {
        if(redundancy_chk::redun_dnt(lastmv,i))
            continue;

        next=cb;
        next=mov.applyMove(next,i);
        solution_sets::solution2.push_back(i);
        int result=dFs2(next,g+1,bound,i);
        if(result==-1)
            return -1;
        
        minin=std::min(minin,result);
        solution_sets::solution2.pop_back();
    }
    return minin;
}
