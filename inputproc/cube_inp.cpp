#include "cube_inp.hpp"

#include <ncurses.h>

#include <array>
#include <cstdlib>
#include <string>

CubeInput::CubeInput()
{
    reset();
}


void CubeInput::reset()
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

    faces = {};

    /*
        Give every sticker an initial colour.

        The centres are initialized to their conventional
        colours so that the cube starts as a solved cube.

            U = W
            F = R
            D = Y
            R = B
            L = G
            B = O
    */

    const std::array<char, 6> centreColours =
    {
        'W',   // U
        'R',   // F
        'Y',   // D
        'B',   // R
        'G',   // L
        'O'    // B
    };

    for (int face = 0; face < 6; face++)
    {
        for (int i = 0; i < 9; i++)
        {
            faces[face][i] = centreColours[face];
        }
    }

    currentFace = 0;
    currentRow = 0;
    currentCol = 0;
}


void CubeInput::getScreenPosition(
    int face,
    int row,
    int col,
    int& y,
    int& x
) const
{
    /*
        Each sticker occupies 4 terminal columns.

        The cube net is:

                       U

                L      F      R      B

                       D
    */

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


void CubeInput::draw()
{
    clear();

    mvprintw(
        0,
        0,
        "RUBIK'S CUBE INPUT"
    );

    mvprintw(
        1,
        0,
        "Arrow Keys: Move    SPACE: Change colour    R: Reset    ENTER: Confirm"
    );

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

        getScreenPosition(face, 0, 0, y, x);

        mvprintw(y - 2, x, "%s", faceNames[face]);
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

                char colour = faces[face][row * 3 + col];

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

                mvprintw(
                    y,
                    x,
                    "[%c]",
                    colour
                );

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
        "Current: %s%d%d",
        faceNames[currentFace],
        currentRow + 1,
        currentCol + 1
    );

    mvprintw(
        28,
        0,
        "Colours: W  Y  R  B  O  G"
    );

    mvprintw(
        29,
        0,
        "Press ENTER when the cube is completely entered."
    );
    refresh();
}


void CubeInput::nextColour()
{
    /*
        Colour cycle:

            W → Y → R → B → O → G → W
    */

    static const std::array<char, 6> colours =
    {
        'W',
        'Y',
        'R',
        'B',
        'O',
        'G'
    };

    char& current =
        faces[currentFace][currentRow * 3 + currentCol];

    for (std::size_t i = 0; i < colours.size(); i++)
    {
        if (current == colours[i])
        {
            current = colours[(i + 1) % colours.size()];
            return;
        }
    }

    /*
        If somehow the current sticker contains an invalid
        character, reset it to the first colour.
    */

    current = colours[0];
}


bool CubeInput::handleInput(int key)
{
    switch (key)
    {
        case KEY_UP:

            if (currentRow > 0)
            {
                currentRow--;
            }
            else
            {
                /*
                    Move from the top of a face to the face above it.

                    F -> U
                    D -> F
                */

                if (currentFace == 1)       // F -> U
                {
                    currentFace = 0;
                    currentRow = 2;
                }
                else if (currentFace == 2)  // D -> F
                {
                    currentFace = 1;
                    currentRow = 2;
                }
            }

            return true;


        case KEY_DOWN:

            if (currentRow < 2)
            {
                currentRow++;
            }
            else
            {
                /*
                    Move from the bottom of a face to the face below it.

                    U -> F
                    F -> D
                */

                if (currentFace == 0)       // U -> F
                {
                    currentFace = 1;
                    currentRow = 0;
                }
                else if (currentFace == 1)  // F -> D
                {
                    currentFace = 2;
                    currentRow = 0;
                }
            }

            return true;


        case KEY_LEFT:

            if (currentCol > 0)
            {
                currentCol--;
            }
            else
            {
                /*
                    Move between the side faces.

                    F -> L
                    R -> F
                    B -> R
                    L -> B

                    This makes the horizontal navigation wrap
                    around the four side faces.
                */

                if (currentFace == 1)       // F -> L
                {
                    currentFace = 4;
                    currentCol = 2;
                }
                else if (currentFace == 3)  // R -> F
                {
                    currentFace = 1;
                    currentCol = 2;
                }
                else if (currentFace == 5)  // B -> R
                {
                    currentFace = 3;
                    currentCol = 2;
                }
                else if (currentFace == 4)  // L -> B
                {
                    currentFace = 5;
                    currentCol = 2;
                }
            }

            return true;


        case KEY_RIGHT:

            if (currentCol < 2)
            {
                currentCol++;
            }
            else
            {
                /*
                    Move between the side faces.

                    L -> F
                    F -> R
                    R -> B
                    B -> L
                */

                if (currentFace == 4)       // L -> F
                {
                    currentFace = 1;
                    currentCol = 0;
                }
                else if (currentFace == 1)  // F -> R
                {
                    currentFace = 3;
                    currentCol = 0;
                }
                else if (currentFace == 3)  // R -> B
                {
                    currentFace = 5;
                    currentCol = 0;
                }
                else if (currentFace == 5)  // B -> L
                {
                    currentFace = 4;
                    currentCol = 0;
                }
            }

            return true;


        case ' ':

            nextColour();
            return true;


        case 'r':
        case 'R':

            reset();
            return true;


        case '\n':
        case KEY_ENTER:

            /*
                Finish editing the entire cube.
            */

            return false;


        default:

            return true;
    }
}

std::array<std::array<char, 9>, 6> CubeInput::run()
{
    /*
        Initialize ncurses.
    */

    initscr();

    /*
        Don't wait for Enter after every key.
    */

    cbreak();

    /*
        We don't want typed characters automatically
        appearing on the screen.
    */

    noecho();

    /*
        Enable arrow keys.
    */

    keypad(stdscr, TRUE);

    /*
        Don't block waiting for a character.
        We DO want blocking input here, so getch()
        waits until the user presses something.
    */

    curs_set(0);

    /*
        Main editor loop.
    */

    bool running = true;

    while (running)
    {
        draw();

        int key = getch();

        running = handleInput(key);
    }

    /*
        Restore the normal terminal before returning.
    */

    endwin();

    return faces;
}