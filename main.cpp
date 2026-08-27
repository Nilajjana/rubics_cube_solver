#include "inputproc/cube.hpp"
#include "inputproc/stk_to_cub.hpp"
#include <bits/stdc++.h>
#include "inputproc/cubie.hpp"
#include "bfstable/heuristictable.hpp"
#include "bfstable/encoder.hpp"
//#include "kociemba/kociembaph1.hpp"
//#include "kociemba/kociembaph2.hpp"

using namespace std;

class tsmltoseg
{
    public:
    array<char, 9> input()
    {
        array<char, 9> face;

        for(int i = 0; i < 9; i++)
            cin >> face[i];

        std::cout<<"input of face success";
        return face;
    }
    Cube usrinp()
    {
        Cube cub;
        std::cout<<"enter the sticker on the white side\n";
        cub.U=input();
        std::cout<<"enter the sticker on the green side\n";
        cub.F=input();
        std::cout<<"enter the sticker on the yellow side\n";
        cub.D=input();
        std::cout<<"enter the sticker on the red side\n";
        cub.R=input();
        std::cout<<"enter the sticker on the orange side\n";
        cub.L=input();
        std::cout<<"enter the sticker on the blue side\n";
        cub.B=input(); 
        std::cout<<"input success\n";
        return cub;
    }
};


int main()
{
    Cube cub;
    tsmltoseg ob;
    cout<<R"(|************|
             |*U1**U2**U3*|
             |************|
             |*U4**Wh**U6*|
             |************|
             |*U7**U8**U9*|
             |************|
 ************|************|************|************
 *L1**L2**L3*|*F1**F2**F3*|*R1**R2**R3*|*B1**B2**B3*
 ************|************|************|************
 *L4**Or**L6*|*F4**Gr**F6*|*R4**Re**R6*|*B4**Bl**B6*
 ************|************|************|************
 *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*
 ************|************|************|************
             |************|
             |*D1**D2**D3*|
             |************|
             |*D4**Ye**D6*|
             |************|
             |*D7**D8**D9*|
             |************|)"<<"\n";
    cout<<"here Wh= white(U5), Gr=green(F5), Ye=yellow(Y5), Re=red(R5), Bl=blue(B5), Or=orange(O5)\n";
    cub=ob.usrinp();
    inpproc ip;
    Cubieste cb;
    cb=ip.cornerinfer(cub,cb);
    cb=ip.edgeinfer(cub,cb);
    std::cout <<"the cp and co are";
    for(int i=0;i<8;i++)
    {
        std::cout<<static_cast<int>(cb.cp[i])<<"   "<<static_cast<int>(cb.co[i])<<"\n";
    }
    std::cout<< "the ep and eo are";
    for(int i=0;i<12;i++)
    {
        std::cout<<static_cast<int>(cb.ep[i])<<"   "<<static_cast<int>(cb.eo[i])<<"\n";
    }
    int vald1=ip.inpvald(cb.cp,8);
    int vald2=ip.inpvald(cb.ep,12);
    uint8_t sum=0;
    for(int i=0;i<8;i++)
    {
        sum+=cb.co[i];
    }
    int cosas=sum%3;
    sum=0;
    for(int i=0;i<12;i++)
    {
        sum+=cb.eo[i];
    }
    int eosas=sum%2;
    
    if(vald1!=vald2||cosas!=0||eosas!=0)
    {
        std::cout<<"the input is wrong pls enter correct input";
        return 0;
    }

    Heuristictable ht;
    ht.load_or_generatetable();
    //KociembaPhase1 kp1;
    //kp1.kociembaPhase1(cb);
    //KociembaPhase2 kp2;
    //kp2.kociembaPhase2(cb); 
    
    return 0;
}