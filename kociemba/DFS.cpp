//DFS_Phase1(cube, g, bound):
//
//    h = heuristicPhase1(cube)
//
//    if g + h > bound:
//        return FAIL
//
//    if cube is in G1:
//        start Phase 2
//        return result
//
//    for each Phase-1 move:
//
//        apply move
//
//        if DFS_Phase1(cube, g + 1, bound):
//            return FOUND
//
//        undo move
//
//    return FAIL

#include "../inputproc/cubie.hpp"
#include <vector>
#include "DFS.hpp"