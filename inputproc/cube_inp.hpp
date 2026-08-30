#ifndef CUBE_INPUT_HPP
#define CUBE_INPUT_HPP

#include <array>          // <-- add this back

class CubeInput
{
public:
    CubeInput();

    std::array<std::array<char, 9>, 6> run();

    bool wasCancelled() const { return cancelled; }

private:
    std::array<std::array<char, 9>, 6> faces;

    int currentFace;
    int currentRow;
    int currentCol;

    bool cancelled = false;

    void draw();

    bool handleInput(int key);

    void getScreenPosition(
        int face,
        int row,
        int col,
        int& y,
        int& x
    ) const;
};

#endif