#ifndef CUBE_INPUT_HPP
#define CUBE_INPUT_HPP

#include <array>

class CubeInput
{
public:
    CubeInput();

    // Launch the interactive cube editor.
    // Returns the six faces in physical position order:
    // U, F, D, R, L, B
    std::array<std::array<char, 9>, 6> run();

private:
    std::array<std::array<char, 9>, 6> faces;

    // Current cursor position.
    int currentFace;
    int currentRow;
    int currentCol;

    // Draw the entire cube.
    void draw();

    // Handle one keyboard input.
    bool handleInput(int key);

    // Change the current sticker to the next colour.
    void nextColour();

    // Reset the cube to its initial state.
    void reset();

    // Convert face/row/column into screen coordinates.
    void getScreenPosition(
        int face,
        int row,
        int col,
        int& y,
        int& x
    ) const;
};

#endif