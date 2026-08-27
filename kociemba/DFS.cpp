
#include "../inputproc/cubie.hpp"
#include <vector>
#include "DFS.hpp"
#include "kociemba_ph1.hpp"
#include "DFS2.hpp"
#include "../rubicsmove/cubemove.hpp"
#include <algorithm>
#include <iostream>


int Dfs::dFs1(const Cubieste& cb,int g,int bound,int lastmv)
{
    KociembaPhase1 kp1;
    int h=kp1.cost_f_n1(cb);
    int f=g+h;
    if(f>bound)
        return f;
    if(kp1.kociembaG1_sb_grp_chkr(cb))
    {
        Dfs2 df2;
        if(df2.kociembaPhase2(cb))
        {
            return -1;
        }
    }
    Cubieste next;
    int minin=255;
    Moves mov;
    std::cout<<"going to explore  level "<<g<<"\n";
    for(int i=0;i<18;i++)
    {
        if(redundancy_chk::redun_dnt(lastmv,i))
            continue;

        next=cb;
        next=mov.applyMove(next,i);
        solution_sets::solution1.push_back(i);
        int result=dFs1(next,g+1,bound,i);
        if(result==-1)
            return -1;
        
        minin=std::min(minin,result);
        solution_sets::solution1.pop_back();
    }
    return minin;
}