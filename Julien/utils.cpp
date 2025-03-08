#include "utils.hpp"

float Utils::PosIntToFloat(int pos)
{
    return ((pos * 0.25) + 0.125);
}

enum dir{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

dir getDir(MazeSquare *a, MazeSquare *b) {
    if(a->i == b->i)
        return a->j < b->j ? DOWN : UP;
    else
        return a->i < b->i ? LEFT : RIGHT;
}

void Utils::getStraightLine(std::vector<MazeSquare *> *path, MazeSquare *start) {
    std::vector<MazeSquare *>::iterator it = path->end() - 1;
    dir currentDir = getDir(start, *it);

    while (it != path->begin()) {
        if (currentDir == getDir(*it, *(it - 1)))
            path->pop_back();
        else
            break;
        it--;
    }
}