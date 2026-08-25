#include "inputproc/cube.hpp"
#include "inputproc/stk_to_cub.hpp"
#include <bits/stdc++.h>
#include "inputproc/cubie.hpp"
#include "bfstable/heuristictable.hpp"
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
        std::cout<<"enter the sticker colour on the side which has center white as W,G,Y,R,O,B and white must be up wards with green front and red right and so on fro the respective centers\n";
        cub.U=input();
        std::cout<<"enter the sticker colour on the side which has center green as W,G,Y,R,O,B and green must be up wards with yellow front and red right and so on for the respective centers\n";
        cub.F=input();
        std::cout<<"enter the sticker colour on the side which has center yello as W,G,Y,R,O,B and yello must be up wards with blue front and red right and so on for the respective centers\n";
        cub.D=input();
        std::cout<<"enter the sticker colour on the side which has center red as W,G,Y,R,O,B and red must be up wards with yello front and blue right and so on for the respective centers\n";
        cub.R=input();
        std::cout<<"enter the sticker colour on the side which has center orange as W,G,Y,R,O,B and orange must be up wards with yello front and right green and so on for the respective centers\n";
        cub.L=input();
        std::cout<<"enter the sticker colour on the side which has center blue as W,G,Y,R,O,B and blue must be up wards with yello front and orange right and so on for the respective centers\n";
        cub.B=input(); 
        std::cout<<"input success\n";
        return cub;
    }
};
int main()
{
    Cube cub;
    tsmltoseg ob;
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
    int vald2=ip.inpvald(cb.ep,12);\
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
    
    if(vald1!=vald2||cosas==eosas||cosas==0)
    {
        std::cout<<"the input is wrong pls enter correct input";
        return 0;
    }
    heuristictable ht;
    ht.load_or_generatetable();
    //ht.load_or_generatetable();
    return 0;
}