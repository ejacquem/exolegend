#ifndef SEARCH_H
#define SEARCH_H
#include "gladiator.h"
#include <array>
#include <vector>
#include <algorithm>

#define MAX_MOVES 3
struct Possibilty {
    std::array<MazeSquare*, MAX_MOVES> squares {}; 
    int score = 0;
};

std::vector<Possibilty> possibilities  {};
void findBestRoad(std::vector<Possibilty> &possibilities)
{
    std::sort(possibilities.begin(), possibilities.end(), [](const Possibilty a, const Possibilty b )
    {
        return a.score > b.score;
    });
}

void explore(MazeSquare* square, int currentMove, Possibilty lastPossibility, Gladiator* gladiator) 
{
   
    lastPossibility.score += square->coin.value;
    lastPossibility.squares[currentMove] = square;
    gladiator->log("exlore i = %d j =%d score=%d currentMove=%d", square->i , square->j, lastPossibility.score, currentMove);
    possibilities.push_back(lastPossibility);

    MazeSquare* last = nullptr;
    if (currentMove > 0)
        last = lastPossibility.squares[currentMove - 1];
    currentMove++;
    if (currentMove < MAX_MOVES) {
        if (square->northSquare != nullptr && square->northSquare != last)
            explore(square->northSquare, currentMove, lastPossibility, gladiator);
        if (square->southSquare != nullptr  && square->southSquare != last)
            explore(square->southSquare, currentMove, lastPossibility,gladiator);
        if (square->eastSquare != nullptr && square->eastSquare != last)
            explore(square->eastSquare, currentMove, lastPossibility,gladiator);
        if (square->westSquare != nullptr && square->westSquare != last)
            explore(square->westSquare, currentMove, lastPossibility,gladiator);
    }
}

std::array<MazeSquare*, MAX_MOVES> search(Gladiator* gladiator) 
{
    MazeSquare* myPos = gladiator->maze->getNearestSquare();
    int currentMove = 0;
    Possibilty defaultPos {};
    explore(myPos, currentMove, defaultPos, gladiator);
    findBestRoad(possibilities);
    return possibilities.front().squares;
}


    
#endif
