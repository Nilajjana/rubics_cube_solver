#include "cube_inp.hpp"

#include <ncurses.h>

#include <array>


CubeInput::CubeInput()
{
    /*
        Face order:

            0 = U
            1 = F
            2 = D
            3 = R
            4 = L
            5 = B
    */

    /*
        Start with all stickers blank.

        The user will enter every sticker manually.
    */

    for (auto& face : faces)
    {
        face.fill(' ');
    }

    /*
        Start cursor at U1.
    */

    currentFace = 0;
    currentRow = 0;
    currentCol = 0;
}


/*
    Convert a face/row/column position into
    terminal coordinates.

    Cube layout:

                       U

                L      F      R      B

                       D
*/

void CubeInput::getScreenPosition(
    int face,
    int row,
    int col,
    int& y,
    int& x
) const
{
    constexpr int stickerWidth = 4;
    constexpr int stickerHeight = 2;

    constexpr int topY = 3;
    constexpr int middleY = 11;

    constexpr int leftX = 4;
    constexpr int faceGap = 16;

    int faceX = 0;
    int faceY = 0;

    switch (face)
    {
        case 0: // U

            faceX = leftX + faceGap;
            faceY = topY;

            break;


        case 1: // F

            faceX = leftX + faceGap;
            faceY = middleY;

            break;


        case 2: // D

            faceX = leftX + faceGap;
            faceY = middleY + 10;

            break;


        case 3: // R

            faceX = leftX + (faceGap * 2);
            faceY = middleY;

            break;


        case 4: // L

            faceX = leftX;
            faceY = middleY;

            break;


        case 5: // B

            faceX = leftX + (faceGap * 3);
            faceY = middleY;

            break;
    }

    y = faceY + (row * stickerHeight);
    x = faceX + (col * stickerWidth);
}


/*
    Draw the entire cube.
*/

void CubeInput::draw()
{
    clear();

    /*
        Title.
    */

    mvprintw(
        0,
        0,
        "RUBIK'S CUBE INPUT"
    );


    /*
        Controls.
    */

    mvprintw(
        1,
        0,
        "Arrow Keys: Move    W/Y/R/B/O/G: Set colour    ENTER: Confirm"
    );


    /*
        Face names.

            0 = U
            1 = F
            2 = D
            3 = R
            4 = L
            5 = B
    */

    const char* faceNames[6] =
    {
        "U",
        "F",
        "D",
        "R",
        "L",
        "B"
    };


    /*
        Draw face labels.
    */

    for (int face = 0; face < 6; face++)
    {
        int y;
        int x;

        getScreenPosition(
            face,
            0,
            0,
            y,
            x
        );

        mvprintw(
            y - 2,
            x,
            "%s",
            faceNames[face]
        );
    }


    /*
        Draw all 54 stickers.
    */

    for (int face = 0; face < 6; face++)
    {
        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                int y;
                int x;

                getScreenPosition(
                    face,
                    row,
                    col,
                    y,
                    x
                );


                /*
                    Get the sticker colour.
                */

                char colour =
                    faces[face][row * 3 + col];


                /*
                    Highlight the currently selected sticker.
                */

                if (
                    face == currentFace &&
                    row == currentRow &&
                    col == currentCol
                )
                {
                    attron(A_REVERSE);
                }


                /*
                    Display blank stickers as [ ].
                */

                if (colour == ' ')
                {
                    mvprintw(
                        y,
                        x,
                        "[ ]"
                    );
                }
                else
                {
                    mvprintw(
                        y,
                        x,
                        "[%c]",
                        colour
                    );
                }


                /*
                    Remove highlight.
                */

                if (
                    face == currentFace &&
                    row == currentRow &&
                    col == currentCol
                )
                {
                    attroff(A_REVERSE);
                }
            }
        }
    }


    /*
        Display current position.
    */

    mvprintw(
        27,
        0,
        "Current: %s [%d,%d]",
        faceNames[currentFace],
        currentRow + 1,
        currentCol + 1
    );


    /*
        Valid colours.
    */

    mvprintw(
        28,
        0,
        "Valid colours: W  Y  R  B  O  G"
    );


    /*
        Explanation.
    */

    mvprintw(
        29,
        0,
        "Lowercase letters are automatically converted to uppercase."
    );


    mvprintw(
        30,
        0,
        "Press ENTER when the cube is completely entered."
    );


    refresh();
}


/*
    Handle keyboard input.

    Valid colour keys:

        W / w
        Y / y
        R / r
        B / b
        O / o
        G / g

    Everything else is ignored except:

        Arrow keys
        ENTER
*/


bool CubeInput::handleInput(int key)
{
    switch (key)
    {
        /*
            UP
        */
        case 27:
            cancelled = true;
            return false;

        case KEY_UP:

            if (currentRow > 0)
            {
                currentRow--;
            }
            else
            {
                /*
                    Move between horizontal faces.

                    F -> U
                    D -> F
                */

                if (currentFace == 1)
                {
                    currentFace = 0;
                    currentRow = 2;
                }
                else if (currentFace == 2)
                {
                    currentFace = 1;
                    currentRow = 2;
                }
            }

            return true;


        /*
            DOWN
        */

        case KEY_DOWN:

            if (currentRow < 2)
            {
                currentRow++;
            }
            else
            {
                /*
                    Move between horizontal faces.

                    U -> F
                    F -> D
                */

                if (currentFace == 0)
                {
                    currentFace = 1;
                    currentRow = 0;
                }
                else if (currentFace == 1)
                {
                    currentFace = 2;
                    currentRow = 0;
                }
            }

            return true;


        /*
            LEFT
        */

        case KEY_LEFT:

            if (currentCol > 0)
            {
                currentCol--;
            }
            else
            {
                /*
                    Move around the four side faces.

                    F -> L
                    R -> F
                    B -> R
                    L -> B
                */

                if (currentFace == 1)
                {
                    currentFace = 4;
                    currentCol = 2;
                }
                else if (currentFace == 3)
                {
                    currentFace = 1;
                    currentCol = 2;
                }
                else if (currentFace == 5)
                {
                    currentFace = 3;
                    currentCol = 2;
                }
                else if (currentFace == 4)
                {
                    currentFace = 5;
                    currentCol = 2;
                }
            }

            return true;


        /*
            RIGHT
        */

        case KEY_RIGHT:

            if (currentCol < 2)
            {
                currentCol++;
            }
            else
            {
                /*
                    Move around the four side faces.

                    L -> F
                    F -> R
                    R -> B
                    B -> L
                */

                if (currentFace == 4)
                {
                    currentFace = 1;
                    currentCol = 0;
                }
                else if (currentFace == 1)
                {
                    currentFace = 3;
                    currentCol = 0;
                }
                else if (currentFace == 3)
                {
                    currentFace = 5;
                    currentCol = 0;
                }
                else if (currentFace == 5)
                {
                    currentFace = 4;
                    currentCol = 0;
                }
            }

            return true;


        /*
            WHITE
        */

        case 'w':
        case 'W':

            faces[currentFace]
                 [currentRow * 3 + currentCol] = 'W';

            return true;


        /*
            YELLOW
        */

        case 'y':
        case 'Y':

            faces[currentFace]
                 [currentRow * 3 + currentCol] = 'Y';

            return true;


        /*
            RED
        */

        case 'r':
        case 'R':

            faces[currentFace]
                 [currentRow * 3 + currentCol] = 'R';

            return true;


        /*
            BLUE
        */

        case 'b':
        case 'B':

            faces[currentFace]
                 [currentRow * 3 + currentCol] = 'B';

            return true;


        /*
            ORANGE
        */

        case 'o':
        case 'O':

            faces[currentFace]
                 [currentRow * 3 + currentCol] = 'O';

            return true;


        /*
            GREEN
        */

        case 'g':
        case 'G':

            faces[currentFace]
                 [currentRow * 3 + currentCol] = 'G';

            return true;


        /*
            ENTER

            Finish the input.
        */

        case '\n':
        case KEY_ENTER:

            return false;


        /*
            EVERYTHING ELSE

            Ignore it.
        */

        default:

            return true;
    }
}


/*
    Start the interactive editor.
*/

std::array<std::array<char, 9>, 6> CubeInput::run()
{
    /*
        Start ncurses.
    */

    initscr();


    /*
        Read keys immediately without requiring ENTER.
    */

    cbreak();


    /*
        Don't echo typed characters.
    */

    noecho();


    /*
        Enable arrow-key processing.
    */

    keypad(stdscr, TRUE);


    /*
        Hide the normal terminal cursor.
    */

    curs_set(0);


    /*
        Main input loop.
    */

    bool running = true;

    while (running)
    {
        draw();

        int key = getch();

        running = handleInput(key);
    }


    /*
        Restore normal terminal mode.
    */

    endwin();


    /*
        Return all six faces.

            [0] = U
            [1] = F
            [2] = D
            [3] = R
            [4] = L
            [5] = B
    */

    return faces;
}