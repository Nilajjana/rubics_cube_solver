#include "inputproc/cube.hpp"
#include "inputproc/stk_to_cub.hpp"
#include <bits/stdc++.h>
#include "inputproc/cubie.hpp"
#include "bfstable/heuristictable.hpp"
#include "bfstable/encoder.hpp"
#include "kociemba/kociemba_ph1.hpp"
#include "rubicsmove/cubemove.hpp"
#include <ncurses.h>
#include "inputproc/cube_inp.hpp"
#include <optional>

using namespace std;

class tsmltoseg
{
public:


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
    std::optional<Cube> usrinp()
    {
        Cube cub;
        CubeInput editor;

        auto inputFaces = editor.run();
        if (editor.wasCancelled())
        {
            cout << "\nInput cancelled by user (ESC). Exiting.\n";
            exit(0);
        }
        auto Uraw = inputFaces[0];
        auto Fraw = inputFaces[1];
        auto Draw = inputFaces[2];
        auto Rraw = inputFaces[3];
        auto Lraw = inputFaces[4];
        auto Braw = inputFaces[5];

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
                    cout << "\nWARNING: Two face centres have the same colour: "
                         << centres[i] << "\n";

                    cout << "The six centre colours must all be different.\n";
                    return std::nullopt;
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
        bool unknownColour = false;

        auto convertFace =
        [&](const array<char, 9>& raw)
        {
            array<char, 9> converted{};

            for (int i = 0; i < 9; i++)
            {
                auto it = colourToFace.find(raw[i]);

                if (it == colourToFace.end())
                {
                    cout << "\nWARNING: Unknown colour '"
                         << raw[i] << "' found in input.\n";

                    unknownColour = true;
                    return converted;
                }

                converted[i] = it->second;
            }

            return converted;
        };


        /*
            Now convert all six physical faces.
        */
        cub.U = convertFace(Uraw);
        if (unknownColour) return std::nullopt;
        cub.F = convertFace(Fraw);
        if (unknownColour) return std::nullopt;
        cub.D = convertFace(Draw);
        if (unknownColour) return std::nullopt;
        cub.R = convertFace(Rraw);
        if (unknownColour) return std::nullopt;
        cub.L = convertFace(Lraw);
        if (unknownColour) return std::nullopt;
        cub.B = convertFace(Braw);
        if (unknownColour) return std::nullopt;


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

        bool countBad = false;

        for (char c : centres)
        {
            if (count[(unsigned char)c] != 9)
            {
                cout << "\nWARNING: Colour '" << c
                     << "' occurs "
                     << count[(unsigned char)c]
                     << " times instead of 9.\n";

                countBad = true;
            }
        }

        if (countBad)
            return std::nullopt;


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
    inpproc ip;
    Cubieste cb;

    while (true)
    {
        auto result = ob.usrinp();

        if (!result)
        {
            cout << "\nPress any key to re-enter the cube "
                    "(or ESC on the input screen to quit).\n";
            cout.flush();
            std::cin.get();
            continue;
        }

        cub = *result;
        cb = Cubieste{};

        bool decodeFailed = false;

        try
        {
            cb = ip.cornerinfer(cub, cb);
            cb = ip.edgeinfer(cub, cb);
        }
        catch (const std::exception& e)
        {
            cout << "\nWARNING: could not interpret the cube from your input:\n"
                 << "  " << e.what() << "\n";
            decodeFailed = true;
        }

        if (decodeFailed)
        {
            cout << "\nPress any key to re-enter the cube "
                    "(or ESC on the input screen to quit).\n";
            cout.flush();
            std::cin.get();
            continue;
        }

        int vald1 = ip.inpvald(cb.cp, 8);
        int vald2 = ip.inpvald(cb.ep, 12);

        int sum = 0;
        for (int i = 0; i < 8; i++)
            sum += cb.co[i];
        int cosas = sum % 3;

        sum = 0;
        for (int i = 0; i < 12; i++)
            sum += cb.eo[i];
        int eosas = sum % 2;

        bool bad = (vald1 != vald2) || (cosas != 0) || (eosas != 0);

        if (!bad)
            break;   // valid input — proceed to solving below

        if (vald1 != vald2)
            cout << "there are cycles in ur input\n";
        if (eosas != 0)
            cout << "the edge orientations are wrong\n";
        if (cosas != 0)
            cout << "the corner orientations are wrong\n";

        cout << "\nWARNING: the input is invalid. Press any key to re-enter the cube "
                "(or ESC on the input screen to quit).\n";
        cout.flush();
        std::cin.get();
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