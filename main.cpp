#include "inputproc/cube.hpp"
#include "inputproc/stk_to_cub.hpp"
#include <bits/stdc++.h>
#include "inputproc/cubie.hpp"
#include "bfstable/heuristictable.hpp"
#include "bfstable/encoder.hpp"
#include "kociemba/kociemba_ph1.hpp"
#include "rubicsmove/cubemove.hpp"

using namespace std;

class tsmltoseg
{
public:

    // Read exactly 9 colour characters for one physical face
    array<char, 9> input()
    {
        array<char, 9> face;

        for (int i = 0; i < 9; i++)
            cin >> face[i];

        return face;
    }

    /*
        Convert arbitrary colour letters into the solver's
        internal face labels U F D R L B.

        The physical face positions are already known from
        the net:

            U F D R L B

        But the COLOUR belonging to each face can vary.

        Therefore:
            U5 -> U
            F5 -> F
            D5 -> D
            R5 -> R
            L5 -> L
            B5 -> B

        and every other sticker having that colour gets
        converted to the corresponding face label.
    */
    Cube usrinp()
    {
        Cube cub;

        cout << "Enter all facelets using COLOUR letters.\n";
        cout << "Enter faces in physical position order: U F D R L B\n";
        cout << "Each face requires 9 stickers.\n\n";

        cout << "Enter U face with capital letter colour symbol like W=white Y=yellow R=red B=blue O=orange G=green:\n";
        auto Uraw = input();

        cout << "Enter F face:\n";
        auto Fraw = input();

        cout << "Enter D face:\n";
        auto Draw = input();

        cout << "Enter R face:\n";
        auto Rraw = input();

        cout << "Enter L face:\n";
        auto Lraw = input();

        cout << "Enter B face:\n";
        auto Braw = input();


        /*
            Determine the colour of each physical face from
            its centre sticker.
        */
        char Ucolour = Uraw[4];
        char Fcolour = Fraw[4];
        char Dcolour = Draw[4];
        char Rcolour = Rraw[4];
        char Lcolour = Lraw[4];
        char Bcolour = Braw[4];


        /*
            Validate that all six centres are different.
        */
        array<char, 6> centres =
        {
            Ucolour,
            Fcolour,
            Dcolour,
            Rcolour,
            Lcolour,
            Bcolour
        };

        for (int i = 0; i < 6; i++)
        {
            for (int j = i + 1; j < 6; j++)
            {
                if (centres[i] == centres[j])
                {
                    cerr << "\nERROR: Two face centres have the same colour: "
                         << centres[i] << "\n";

                    cerr << "The six centre colours must all be different.\n";
                    exit(1);
                }
            }
        }


        /*
            Build:

                colour -> solver face

            Example:

                Ucentre = G
                Fcentre = R
                Dcentre = B
                Rcentre = Y
                Lcentre = O
                Bcentre = W

            gives:

                G -> U
                R -> F
                B -> D
                Y -> R
                O -> L
                W -> B
        */
        unordered_map<char, char> colourToFace;

        colourToFace[Ucolour] = 'U';
        colourToFace[Fcolour] = 'F';
        colourToFace[Dcolour] = 'D';
        colourToFace[Rcolour] = 'R';
        colourToFace[Lcolour] = 'L';
        colourToFace[Bcolour] = 'B';


        /*
            Convert one raw face from colours into
            U/F/D/R/L/B facelet labels.
        */
        auto convertFace =
        [&](const array<char, 9>& raw)
        {
            array<char, 9> converted;

            for (int i = 0; i < 9; i++)
            {
                auto it = colourToFace.find(raw[i]);

                if (it == colourToFace.end())
                {
                    cerr << "\nERROR: Unknown colour '"
                         << raw[i] << "' found in input.\n";

                    exit(1);
                }

                converted[i] = it->second;
            }

            return converted;
        };


        /*
            Now convert all six physical faces.
        */
        cub.U = convertFace(Uraw);
        cub.F = convertFace(Fraw);
        cub.D = convertFace(Draw);
        cub.R = convertFace(Rraw);
        cub.L = convertFace(Lraw);
        cub.B = convertFace(Braw);


        /*
            Check that each colour occurs exactly 9 times.
            This catches many bad inputs before cornerinfer().
        */
        array<int, 256> count{};

        for (char c : Uraw) count[(unsigned char)c]++;
        for (char c : Fraw) count[(unsigned char)c]++;
        for (char c : Draw) count[(unsigned char)c]++;
        for (char c : Rraw) count[(unsigned char)c]++;
        for (char c : Lraw) count[(unsigned char)c]++;
        for (char c : Braw) count[(unsigned char)c]++;

        for (char c : centres)
        {
            if (count[(unsigned char)c] != 9)
            {
                cerr << "\nERROR: Colour '" << c
                     << "' occurs "
                     << count[(unsigned char)c]
                     << " times instead of 9.\n";

                exit(1);
            }
        }


        /*
            Display the automatically determined mapping.
        */
        cout << "\nDetected centre mapping:\n";

        cout << "U = " << Ucolour << "\n";
        cout << "F = " << Fcolour << "\n";
        cout << "D = " << Dcolour << "\n";
        cout << "R = " << Rcolour << "\n";
        cout << "L = " << Lcolour << "\n";
        cout << "B = " << Bcolour << "\n";

        cout << "\nConverted cube:\n";

        cout << "U: ";
        for (char c : cub.U) cout << c;
        cout << "\n";

        cout << "F: ";
        for (char c : cub.F) cout << c;
        cout << "\n";

        cout << "D: ";
        for (char c : cub.D) cout << c;
        cout << "\n";

        cout << "R: ";
        for (char c : cub.R) cout << c;
        cout << "\n";

        cout << "L: ";
        for (char c : cub.L) cout << c;
        cout << "\n";

        cout << "B: ";
        for (char c : cub.B) cout << c;
        cout << "\n";

        cout << "\nInput successfully converted to UFDRLB notation.\n";

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
    cout<<R"(             |************|
             |*U1**U2**U3*|
             |************|
             |*U4**U5**U6*|
             |************|
             |*U7**U8**U9*|
             |************|
 ************|************|************|************
 *L1**L2**L3*|*F1**F2**F3*|*R1**R2**R3*|*B1**B2**B3*
 ************|************|************|************
 *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*
 ************|************|************|************
 *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*
 ************|************|************|************
             |************|
             |*D1**D2**D3*|
             |************|
             |*D4**D5**D6*|
             |************|
             |*D7**D8**D9*|
             |************|)"<<"\n";
    cub=ob.usrinp();
    inpproc ip;
    Cubieste cb;
    cb=ip.cornerinfer(cub,cb);
    cb=ip.edgeinfer(cub,cb);

    int vald1=ip.inpvald(cb.cp,8);
    int vald2=ip.inpvald(cb.ep,12);
    int sum=0;
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
    
    if(vald1!=vald2)
    {
        cout<<"there are cycles in ur input\n"; 
    }
    if(eosas!=0)
    {
        cout<<"the edge orientations are wrong\n";
    }
    if(cosas!=0)
    {
        cout<<"the corner orientations are wrong\n";
    }
    if(vald1!=vald2||cosas!=0||eosas!=0)
    {
        std::cout<<"the input is wrong pls enter correct input\n";
        return 0;
    }
    KociembaPhase1 kp1;
    if(kp1.soln_chkr(cb))
    {
        cout<<"it is already solved migger what do u want are u dumb??\n";
        return 0;
    }
    Heuristictable ht;
    ht.load_or_generatetable();
    int solution=0;
    if(kp1.kociembaPhase1(cb))
    {
        solution=1;
    }
    else
    {
        return 0;
    }
    string solutionstr1=ob.movesToNotation(solution_sets::solution1);
    string solutionstr2=ob.movesToNotation(solution_sets::solution2);
    if(solution==1)
    {
        cout<<"the solution steps in first phase are:- "<<solutionstr1<<"  in the second phase:- "<<solutionstr2<<"\n";
        int steps=solution_sets::solution2.size()+solution_sets::solution1.size();
        cout<<"total steps needed are "<<steps<<"\n";
    }

    return 0;
}

//still there is abi error in co calculatio and icannot get rid of it i am leaving it at this will return later and until then buy 