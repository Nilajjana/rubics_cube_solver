#pragma once
#include <queue>
#include "../inputproc/cubie.hpp"
class Bfs
{ 
    public:
        uint16_t sliceMove[495][18];
        uint8_t slicePermMove[24][18];
        int comoves(int crdnt , Cubieste& cb,int mv);
        int slicemoves(int mv,Cubieste& cb,int slice);
        int eomoves(int crdnt , Cubieste& cb,int mv);
        int cpmoves(int crdnt, Cubieste& cb, int mv);
        int slicemoves2(int mv,Cubieste& cb,int slice);
        void bfstwstsls(std::vector<uint8_t>& twistSliceTable);
        void bfsflpsls(std::vector<uint8_t>& flipSliceTable);
        void bfscpsls(std::vector<uint8_t>& cpSliceTable);
};

