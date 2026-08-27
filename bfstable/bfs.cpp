
#include <queue>
#include "../inputproc/cubie.hpp"
#include "encoder.hpp"
#include "../rubicsmove/cubemove.hpp"
#include "bfs.hpp"
#include <iostream>

int Bfs::comoves(int crdnt, Cubieste& cb, int mv)
{
    Encoder ec;
    int cdco = crdnt / 495;
    ec.decodeCO(cdco, cb.co);
    cb = Moves::applyMove(cb, mv);
    return ec.encodeCO(cb.co);
}

int Bfs::slicemoves(int mv, Cubieste& cb, int slice)
{
    Encoder ec;
    for (int i = 0; i < 12; i++)
    {
        cb.ep[i] = i;
        cb.eo[i] = 0;
    }
    ec.decodeSlice(slice, cb.ep);
    cb = Moves::applyMove(cb, mv);
    return ec.encodeSlice(cb.ep);
}

int Bfs::slicemoves2(int mv, Cubieste& cb, int slice)
{
    Encoder ec;

    for (int i = 0; i < 12; i++)
    {
        cb.ep[i] = i;
        cb.eo[i] = 0;
    }

    ec.decodeLehmer4(slice, &cb.ep[8]);
    for (int i = 0; i < 4; i++)
        cb.ep[8 + i] += 8;   // decodeLehmer4 outputs ranks 0..3; shift back to cubie IDs 8..11

    cb = Moves::applyMove(cb, mv);
    return ec.lehmer4(&cb.ep[8]);
}

int Bfs::eomoves(int crdnt, Cubieste& cb, int mv)
{
    Encoder ec;
    int cdco = crdnt / 495;
    ec.decodeEO(cdco, cb.eo);
    cb = Moves::applyMove(cb, mv);
    return ec.encodeEO(cb.eo);
}
int Bfs::cpmoves(int crdnt, Cubieste& cb, int mv)
{
    Encoder ec;
    int cdcp = crdnt / 24;
    ec.decodeLehmer8(cdcp, cb.cp);
    cb = Moves::applyMove(cb, mv);
    return ec.lehmer8coder(cb.cp);
}
int Bfs::epmoves(int crdnt, Cubieste& cb, int mv)
{
    Encoder ec;
    int cdep = crdnt / 24;
    ec.decodeLehmer8(cdep, &cb.ep[0]);
    cb = Moves::applyMove(cb, mv);
    return ec.lehmer8coder(cb.ep);
}

void Bfs::bfstwstsls(std::vector<uint8_t>& twistSliceTable)
{
    Cubieste cb;
    for(int i=0;i<8;i++)
    {
        cb.cp[i]=i;
        cb.co[i]=0;
    }
    for(int i=0;i<12;i++)
    {
        cb.ep[i]=i;
        cb.eo[i]=0;
    }
    Encoder ec;
    for(int slice=0;slice<495;slice++)
    {
        for(int i=0;i<18;i++)
        {
            sliceMove[slice][i]=slicemoves(i,cb,slice);
        }
    }

    for(int i=0;i<8;i++)
    {
        cb.cp[i]=i;
        cb.co[i]=0;
    }
    for(int i=0;i<12;i++)
    {
        cb.ep[i]=i;
        cb.eo[i]=0;
    }
    uint16_t cdco=ec.encodeCO(cb.co);
    uint16_t cdsl=ec.encodeSlice(cb.ep);
    int crdnt=cdco*495+cdsl;
    std::queue<int> qu;
    qu.push(crdnt);
    twistSliceTable[crdnt]=0;
    int size=0;
    int front;
    int i=0;
    int coor=0;
    uint8_t level=0;
    while(!qu.empty())
    {
        size=qu.size();
        while(size--)
        {
            front=qu.front();
            qu.pop();
            for(i=0;i<18;i++)
            {
                coor=comoves(front,cb,i);
                coor=coor*495+sliceMove[front%495][i];
                if(twistSliceTable[coor]==255)
                {
                    twistSliceTable[coor]=level+1;
                    qu.push(coor);
                }
            }
        }
        level++;
    }
    size_t unvisited = 0;

    for (uint8_t v : twistSliceTable)
    {
        if (v == 255)
            unvisited++;
    }

    std::cout << "Unvisited states: " << unvisited << '\n';
    std::cout << "Visited states: "
          << twistSliceTable.size() - unvisited << '\n';
    std::cout<<"the max level in twist slice is "<<(int)(level-1)<<" which ended at "<<twistSliceTable.size()<<"\n";
}

void Bfs::bfsflpsls(std::vector<uint8_t>& flipSliceTable)
{
    Cubieste cb;
    for(int i=0;i<8;i++)
    {
        cb.cp[i]=i;
        cb.co[i]=0;
    }
    for(int i=0;i<12;i++)
    {
        cb.ep[i]=i;
        cb.eo[i]=0;
    }
    Encoder ec;
    uint16_t cdeo=ec.encodeEO(cb.eo);
    uint16_t cdsl=ec.encodeSlice(cb.ep);
    int crdnt=cdeo*495+cdsl;
    std::queue<int> qu;
    qu.push(crdnt);
    flipSliceTable[crdnt]=0;
    int size=0;
    int front;
    int i=0;
    int coor=0;
    uint8_t level=0;
    while(!qu.empty())
    {
        size=qu.size();
        while(size--)
        {
            front=qu.front();
            qu.pop();
            for(i=0;i<18;i++)
            {
                coor=eomoves(front,cb,i);
                coor=coor*495+sliceMove[front%495][i];
                if(flipSliceTable[coor]==255)
                {
                    flipSliceTable[coor]=level+1;
                    qu.push(coor);
                }
            }
        }
        level++;
    }

    size_t unvisited = 0;

    for (uint8_t v : flipSliceTable)
    {
        if (v == 255)
            unvisited++;
    }

    std::cout << "Unvisited states: " << unvisited << '\n';
    std::cout << "Visited states: "
          << flipSliceTable.size() - unvisited << '\n';
    std::cout<<"the max level in flip slice is "<<(int)(level-1)<<" which ended at "<<flipSliceTable.size()<<"\n";
}
void Bfs::bfscpsls(std::vector<uint8_t>& cpSliceTable)
{
    constexpr int kPhase2Moves[10] = { 0, 1, 2, 4, 7, 10, 13, 15, 16, 17 }; // U, U2, U', F2, B2, L2, R2, D, D2, D'
    Cubieste cb;
    for(int i=0;i<8;i++)
    {
        cb.cp[i]=i;
        cb.co[i]=0;
    }
    for(int i=0;i<12;i++)
    {
        cb.ep[i]=i;
        cb.eo[i]=0;
    }
    Encoder ec;
    for(int slice=0;slice<24;slice++)
    {
        for(int i:kPhase2Moves)
        {
            slicePermMove[slice][i]=slicemoves2(i,cb,slice);
        }
    }

    for(int i=0;i<8;i++)
    {
        cb.cp[i]=i;
        cb.co[i]=0;
    }
    for(int i=0;i<12;i++)
    {
        cb.ep[i]=i;
        cb.eo[i]=0;
    }
    uint16_t cdcp=ec.lehmer8coder(cb.cp);
    uint16_t cdsl2=ec.lehmer4(&cb.ep[8]);
    int crdnt=cdcp*24+cdsl2;
    std::queue<int> qu;
    qu.push(crdnt);
    cpSliceTable[crdnt]=0;
    int size=0;
    int front;
    int i=0;
    int coor=0;
    uint8_t level=0;
    while(!qu.empty())
    {
        size=qu.size();
        while(size--)
        {
            front=qu.front();
            qu.pop();
            for(int i:kPhase2Moves)
            {
                coor=cpmoves(front,cb,i);
                coor=coor*24+slicePermMove[front%24][i];
                if(cpSliceTable[coor]==255)
                {
                    cpSliceTable[coor]=level+1;
                    qu.push(coor);
                }
            }
        }
        level++;
    }
    size_t unvisited = 0;

    for (uint8_t v : cpSliceTable)
    {
        if (v == 255)
            unvisited++;
    }

    std::cout << "Unvisited states: " << unvisited << '\n';
    std::cout << "Visited states: "
          << cpSliceTable.size() - unvisited << '\n';
    std::cout<<"the max level cp slice is "<<(int)(level-1)<<" which ended at "<<cpSliceTable.size()<<"\n";
}

void Bfs::bfsepsls(std::vector<uint8_t>& udEdgeSliceTable)
{
    Cubieste cb;
    for(int i=0;i<8;i++)
    {
        cb.cp[i]=i;
        cb.co[i]=0;
    }
    for(int i=0;i<12;i++)
    {
        cb.ep[i]=i;
        cb.eo[i]=0;
    }
    Encoder ec;
    constexpr int kPhase2Moves[10] = { 0, 1, 2, 4, 7, 10, 13, 15, 16, 17 }; // U, U2, U', F2, B2, L2, R2, D, D2, D'
    uint16_t cdep=ec.lehmer8coder(cb.ep);
    uint16_t cdsl2=ec.lehmer4(&cb.ep[8]);
    int crdnt=cdep*24+cdsl2;
    std::queue<int> qu;
    qu.push(crdnt);
    udEdgeSliceTable[crdnt]=0;
    int size=0;
    int front;
    int i=0;
    int coor=0;
    uint8_t level=0;
    while(!qu.empty())
    {
        size=qu.size();
        while(size--)
        {
            front=qu.front();
            qu.pop();
            for(int i:kPhase2Moves)
            {
                coor=epmoves(front,cb,i);
                coor=coor*24+slicePermMove[front%24][i];
                if(udEdgeSliceTable[coor]==255)
                {
                    udEdgeSliceTable[coor]=level+1;
                    qu.push(coor);
                }
            }
        }
        level++;
    }
    size_t unvisited = 0;

    for (uint8_t v : udEdgeSliceTable)
    {
        if (v == 255)
            unvisited++;
    }

    std::cout << "Unvisited states: " << unvisited << '\n';
    std::cout << "Visited states: "
          << udEdgeSliceTable.size() - unvisited << '\n';
    std::cout<<"the max level ud edge p slice is "<<(int)(level-1)<<" which ended at "<<udEdgeSliceTable.size()<<"\n";
}
