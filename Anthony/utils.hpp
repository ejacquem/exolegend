#pragma once
#include "gladiator.h"
#include <vector>

class Utils
{
    public:
        static float PosIntToFloat(int pos);
        static void getStraightLine(std::vector<MazeSquare *> *path, MazeSquare *start);
        static int checkWall(Gladiator *gladiator);
};