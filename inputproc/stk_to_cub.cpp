#include <array>
#include "stk_to_cub.hpp"
#include "cube.hpp"
#include "cubie.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
enum Face
{
    U,F,D,R,L,B
};
struct Stickerpos//contains face and index number of that sticker form the iput 
{
    Face face;
    int index;
};
struct Corner//contains the total corner table for the input to get converted
{
    Stickerpos sticker[3];
};
const Corner corner[8] =
{
    // UFR
    {{{U, 8}, {F, 2}, {R,0}}},

    // UFL
    {{{U, 6}, {F, 0}, {L, 2}}},

    // ULB
    {{{U, 0}, {L, 0}, {B, 2}}},

    // UBR
    {{{U, 2}, {B, 0}, {R, 2}}},

    // DFR
    {{{D, 2}, {F, 8}, {R, 6}}},

    // DFL
    {{{D, 0}, {F, 6}, {L, 8}}},

    // DLB
    {{{D, 6}, {L, 6}, {B, 8}}},

    // DRB
    {{{D, 8}, {R, 8}, {B, 6}}}
};
char getsticker(Cube& cube,Stickerpos p)
{
    switch(p.face)
    {
        case U:return cube.U[p.index];
        case F:return cube.F[p.index];
        case D:return cube.D[p.index];
        case R:return cube.R[p.index];
        case L:return cube.L[p.index];
        case B:return cube.B[p.index];
    }
    std::cout<<"getsticker success\n";
    return '?';
}
std::array<char,3> getcolourcon(Cube cube,int a)
{
    std::cout<<"getcolourcon success\n";
    return {getsticker(cube,corner[a].sticker[0]),
            getsticker(cube,corner[a].sticker[1]),
            getsticker(cube,corner[a].sticker[2])};
}
bool samecon(const std::array<char,3> a,const std::array<char,3> b)
{
    std::cout<<"samecon success\n";
    return ((a[0]==b[0]||a[0]==b[1]||a[0]==b[2])
            &&(a[1]==b[0]||a[1]==b[1]||a[1]==b[2])
            &&(a[2]==b[0]||a[2]==b[1]||a[2]==b[2]));
}

int getCO(const std::array<char, 3>& colors, int position)
{
    for (int i = 0; i < 3; i++)
    {
        if (colors[i] == 'W' || colors[i] == 'Y')
            return i;   // trust the corner[] table's declared rotational order
    }
    return -1;
}

int getcp(std::array<char,3> colour)
{
    for(int i=0;i<8;i++)
    {
        if(samecon(colour,cornerTable[i].colors))
        {
            std::cout<<"getcp success\n";
            return i;
        }
    }
    return -1;
}
struct Edge
{
    Stickerpos sticker[2];
};
const Edge edge[12]
{
    // UF
    {{{U, 7}, {F, 1}}},

    // UL
    {{{U, 3}, {L, 1}}},

    // UB
    {{{U, 1}, {B, 1}}},

    // UR
    {{{U, 5}, {R, 1}}},

    // DF
    {{{D, 1}, {F, 7}}},

    // DL
    {{{D, 3}, {L, 7}}},

    // DB
    {{{D, 7}, {B, 7}}},

    // DR
    {{{D, 5}, {R, 7}}},

    // FL
    {{{F, 3}, {L, 5}}},

    // LB
    {{{L, 3}, {B, 5}}},

    // BR
    {{{B, 3}, {R, 5}}},

    // RF
    {{{R, 3}, {F, 5}}}
};
bool sameedg(const std::array<char,2> a,const std::array<char,2> b)
{
    std::cout<<"sameedg success \n";
    return ((a[0]==b[0]||a[0]==b[1])&&(a[1]==b[0]||a[1]==b[1]));
}
std::array<char,2> getcolouredg(Cube cube,int a)
{
    std::cout<<"getcolouredg success\n";
    return {getsticker(cube,edge[a].sticker[0]),
            getsticker(cube,edge[a].sticker[1]),};
}
int getep(std::array<char,2> colour)
{
    for(int i=0;i<12;i++)
    {
        if(sameedg(colour,edgeTable[i].colors))
        {
            std::cout<<"getep success\n";
            return i;
        }
    }
    return -1;
}
bool isUD(char c)
{
    return c == 'W' || c == 'Y';
}

bool isFB(char c)
{
    return c == 'G' || c == 'B';
}
int geteo(std::array<char,2> colour,int position)
{
    Face face1,face2;
    face1=edge[position].sticker[0].face;
    face2=edge[position].sticker[1].face;
    if (isUD(colour[0])){
        std::cout<<"succesfull eo finding";
        return (face1 == U || face1 == D) ? 0 : 1;
    }
    if (isUD(colour[1])){
        std::cout<<"succesfull eo finding";
        return (face2 == U || face2 == D) ? 0 : 1;
    }
    // No U/D color => this is one of the middle-layer edges.
    // Its F/B color determines orientation.

    if (isFB(colour[0])){
        std::cout<<"succesfull eo finding";
        return (face1 == F || face1 == B) ? 0 : 1;
    }
    if (isFB(colour[1])){
        std::cout<<"succesfull eo finding";
        return (face2 == F || face2 == B) ? 0 : 1;
    }
    return -1; // impossible
}
Cubieste inpproc::cornerinfer(Cube& cube,Cubieste cst)
{
    for(int i=0;i<8;i++)
    {
        auto colour=getcolourcon(cube,i);
        cst.cp[i]=getcp(colour);
        cst.co[i]=getCO(colour,i);
    }
    std::cout<<"cornerinfer success\n";
    return cst;
}
Cubieste inpproc::edgeinfer(Cube& cube,Cubieste cst)
{
    for(int i=0;i<12;i++)
    {
        auto colour=getcolouredg(cube,i);
        cst.ep[i]=getep(colour);
        cst.eo[i]=geteo(colour,i);
    }
    std::cout<<"edgeinfer success\n";
    return cst;
}
bool inpproc::inpvald(const uint8_t* p,int n)
{
    std::vector<bool> visited(n,false);
    int j=0,cycles=0;
    for(int i=0;i<n;i++)
    {
        if(!visited[i])
        {
            cycles++;
            j=i;
            while(!visited[j])
            {
                visited[j]=true;
                j=p[j];
            }
        }
    }
    return (n-cycles)%2;
}
