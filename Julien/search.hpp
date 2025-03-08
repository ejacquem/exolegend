#ifndef SEARCH_H
#define SEARCH_H
#include "gladiator.h"
#include <array>
#include <vector>
#include <algorithm>

// class Search 
// {
//     public:
//         Search()
//         {
//         }
//         static MazeSquare* search(Gladiator* gladiator, MazeSquare* square , MazeSquare* previousSquare);
//         static bool CheckWall(MazeSquare* square, std::vector<MazeSquare*> &availableSquares);
//         static void CheckAllWall(MazeSquare* square, std::vector<MazeSquare*> &availableSquares);
// };

uint8_t teamId ;
#define MAX_MOVES 4
#define WALL_MALUS 15
#define BORDER_MALUS 50
#define DEAD_END_MALUS 4
#define DANGER_MULT 10
#define COIN_MULT 10
#define ROBOT_PROXITIMTY_RADIUS 3
#define ROBOT_PROXIMITY_DANGER 10

int const size = 12;
int shrinked = 0;
int remaining = 0;

int getScore(MazeSquare* square, bool isGoingThroughWall) {
    int score = 0;
    if (square->i == shrinked || square->i == size - shrinked - 1 || square->j == shrinked || square->j == size - shrinked - 1)
        score -= remaining < 5 ? BORDER_MALUS / remaining : 0;
    int i = 0;
    if (!square->northSquare)
        i++;
    if (!square->southSquare)
        i++;
    if (!square->westSquare)
        i++;
    if (!square->eastSquare)
        i++;
    score -= i == 3 ? DEAD_END_MALUS : 0;
    score -= square->danger * DANGER_MULT;
    score += square->coin.value * COIN_MULT;
    score -= isGoingThroughWall ? WALL_MALUS : 0;
    score += 100;
    return score;
}

bool isOutside(MazeSquare* square) {
    int min = shrinked;
    int max = size - shrinked - 1;
    return square->i < min || square->i >= max || square->j < min || square->j >= max;
}

struct Possibilty {
    std::array<MazeSquare*, MAX_MOVES> squares {}; 
    int score = 0;
};

Possibilty bestPossibility {};

void explore(MazeSquare* square, int currentMove, Possibilty lastPossibility, Gladiator* gladiator, bool wall = false) {
    //gladiator->log("Score at begining of explore : %d", lastPossibility.score);
    lastPossibility.score += getScore(square, wall);
    lastPossibility.squares[currentMove] = square;
    //gladiator->log("explore i = %d j =%d score=%d currentMove=%d", square->i, square->j, lastPossibility.score, currentMove);

    if (lastPossibility.score > bestPossibility.score) {
       // gladiator->log("New bestPossibility old score was %d now %d", bestPossibility.score, lastPossibility.score);
        bestPossibility = lastPossibility;
    }
    else
    {
       // gladiator->log("Not the bestPossibility current score %d", lastPossibility.score);
    }

    MazeSquare* last = nullptr;
    if (currentMove > 0)
        last = lastPossibility.squares[currentMove - 1];
    currentMove++;
    if (currentMove < MAX_MOVES) {
       // gladiator->log("Exploring");
        if (square->northSquare != nullptr && square->northSquare != last) {
            explore(square->northSquare, currentMove, lastPossibility, gladiator);
        }
        else if (square->northSquare == nullptr && square->northSquare != last && !isOutside(square)) {
            MazeSquare* nSquare = gladiator->maze->getSquare(square->i, square->j + 1);
            if (nSquare)
                explore(nSquare, currentMove, lastPossibility, gladiator, true);
        }
        if (square->southSquare != nullptr && square->southSquare != last) {
            explore(square->southSquare, currentMove, lastPossibility, gladiator);
        }
        else if (square->southSquare == nullptr && square->southSquare != last && !isOutside(square)) {
            MazeSquare* nSquare = gladiator->maze->getSquare(square->i, square->j - 1);
            if (nSquare)
                explore(nSquare, currentMove, lastPossibility, gladiator, true);
        }
        if (square->eastSquare != nullptr && square->eastSquare != last) {
            explore(square->eastSquare, currentMove, lastPossibility, gladiator);
        }
        else if (square->eastSquare == nullptr && square->eastSquare != last && !isOutside(square)) {
            MazeSquare* nSquare = gladiator->maze->getSquare(square->i + 1, square->j);
            if (nSquare)
                explore(nSquare, currentMove, lastPossibility, gladiator, true);
        }
        if (square->westSquare != nullptr && square->westSquare != last) {
            explore(square->westSquare, currentMove, lastPossibility, gladiator);
        }
        else if (square->westSquare == nullptr && square->westSquare != last && !isOutside(square)) {
            MazeSquare* nSquare = gladiator->maze->getSquare(square->i - 1, square->j);
            if (nSquare)
                explore(nSquare, currentMove, lastPossibility, gladiator, true);
        }
    }
   // gladiator->log("Done");
}

std::vector<MazeSquare *> search(Gladiator* gladiator, int _remaining, int _shrink) {
    bestPossibility = {};
    bestPossibility.score = -5000;
    remaining = _remaining;
    shrinked = _shrink;
    MazeSquare* a = gladiator->maze->getNearestSquare();
    std::vector<MazeSquare *> path;
    int currentMove = 0;
    Possibilty defaultPos {};
    defaultPos.score = 0;
    gladiator->log("Exploring");
    explore(a, currentMove, defaultPos, gladiator);
    gladiator->log("Finished");

    if (std::empty(bestPossibility.squares)) {
        gladiator->log("No bestPossibility");
        return {};
    }
    else
    {
        gladiator->log("Score of the best : %d", bestPossibility.score);
        gladiator->log("Size of the best : %lld", bestPossibility.squares.size());

        for (std::size_t i = 0 ; i < bestPossibility.squares.size(); i++) {
            if (bestPossibility.squares[i] == nullptr)
                break;
            path.push_back(bestPossibility.squares[i]);
            gladiator->log("bestPossibility i = %d j = %d", bestPossibility.squares[i]->i, bestPossibility.squares[i]->j);
        }
    }
    gladiator->log("Returning");
    std::reverse(path.begin(), path.end());
    return path;
}
    
#endif