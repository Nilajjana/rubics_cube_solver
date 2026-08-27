#include "../inputproc/cubie.hpp"
#include "kociembaph1.hpp"
#include "../bfstable/bfs.hpp"
#include "../bfstable/encoder.hpp"
#include <algorithm>
//IDA_Phase1(cube):
//
//    bound = heuristicPhase1(cube)
//
//    repeat:
//
//        result = DFS_Phase1(
//                    cube,
//                    g = 0,
//                    bound
//                 )
//
//        if result == FOUND:
//            return solution
//
//        bound = bound + 1

namespace
{
    int twist_sls_crdnt(Cubieste cb)
    {
        Encoder ec;
        uint16_t cdco=ec.encodeCO(cb.co);
        uint16_t cdsl=ec.encodeSlice(cb.ep);
        int crdnt=cdco*495+cdsl;
    }
    int flip_sls_crdnt(Cubieste cb)
    {
        Encoder ec;
        uint16_t cdeo=ec.encodeEO(cb.eo);
        uint16_t cdsl=ec.encodeSlice(cb.ep);
        int crdnt=cdeo*495+cdsl;
    }
}

uint8_t KociembaPhase1::cost_f_n(Cubieste cb)
{
    int crdnt1=twist_sls_crdnt(cb);
    int crdnt2=flip_sls_crdnt(cb);
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
        if(cb.ep[i]>8)
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

Cubieste KociembaPhase1::kociembaPhase1(Cubieste cb)
{
    uint8_t h=cost_f_n(cb);
    while (true)
    {
        
    }
}