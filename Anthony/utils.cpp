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

int Utils::checkWall(Gladiator *gladiator)
{
    int count = 0;
    MazeSquare *myPos = gladiator->maze->getNearestSquare();
    int teamId = gladiator->robot->getData().teamId; 
    if(myPos->eastSquare != nullptr && myPos->eastSquare->possession != teamId)
    {
        count ++;
        if( myPos->eastSquare->possession != 0)
        {
            count ++;
        }
    }
    if(myPos->westSquare != nullptr  && myPos->westSquare->possession != teamId)
    {
        count ++;
        if( myPos->westSquare->possession != 0)
        {
            count ++;
        }
    }
    if(myPos->northSquare != nullptr && myPos->northSquare->possession != teamId)
    {
        count ++;
        if( myPos->northSquare->possession != 0)
        {
            count ++;
        }
    }
    if(myPos->southSquare != nullptr && myPos->southSquare->possession != teamId)
    {
        count ++;
        if( myPos->southSquare->possession != 0)
        {
            count ++;
        }
    }
    return count;
}
