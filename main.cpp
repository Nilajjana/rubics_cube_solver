#include "inputproc/cube.hpp"
#include "inputproc/stk_to_cub.hpp"
#include <bits/stdc++.h>
#include "inputproc/cubie.hpp"
#include "bfstable/heuristictable.hpp"
#include "bfstable/encoder.hpp"
#include "kociemba/kociemba_ph1.hpp"

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
    std::string movesToNotation(const std::vector<int>& moves)
    {
        static const std::string notation[18] =
        {
            "U",  "U2",  "U'",
            "F",  "F2", "F'",
            "B",  "B2", "B'",
            "L",  "L2", "L'",
            "R",  "R2", "R'",
            "D",  "D2", "D'"
        };
    
        std::string result;
    
        for (int mv : moves)
        {
            if (mv < 0 || mv >= 18)
                continue;   // or throw an error
        
            if (!result.empty())
                result += ' ';
        
            result += notation[mv];
        }
    
        return result;
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
    cout<<"here Wh= white(U5), Gr=green(F5), Ye=yellow(Y5), Re=red(R5), Bl=blue(B5), Or=orange(O5)\n"<<"\n";
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
    
    if(vald1!=vald2||(cosas==eosas&&eosas==0))
    {
        std::cout<<"the input is wrong pls enter correct input\n";
        return 0;
    }

    KociembaPhase1 kp1;
    if(kp1.soln_chkr(cb))
    {
        cout<<"it is already solved nigger what do u want are u dumb??\n";
        return 0;
    }
    Heuristictable ht;
    ht.load_or_generatetable();
    string solutionstr1=ob.movesToNotation(solution_sets::solution1);
    string solutionstr2=ob.movesToNotation(solution_sets::solution2);
    if(kp1.kociembaPhase1(cb))
    {
        cout<<"the solution steps in first phase are:- "<<solutionstr1<<"in the second phase:- "<<solutionstr2;
    }
    
    return 0;
}