#ifndef SEARCH_H
#define SEARCH_H
#include "gladiator.h"
#include <array>
#include <vector>
#include <algorithm>


uint8_t teamId ;
#define MAX_MOVES 5
#define WALL_MALUS 15
#define BORDER_MALUS 250
#define DEAD_END_MALUS 0
#define DANGER_MULT 30
#define COIN_MULT 10
#define ROBOT_PROXITIMTY_RADIUS 3
#define ROBOT_PROXIMITY_DANGER 10


int const size = 12;
int shrinked = 0;
int remaining = 0;
Gladiator* _gladiator;

int getScore(MazeSquare* square, bool isGoingThroughWall, bool needToPutBomb) {
    int score = 0;
   // _gladiator->log("Calculating score for square at (%d, %d)", square->i, square->j);
    if (square->i == shrinked || square->i == size - shrinked - 1 || square->j == shrinked || square->j == size - shrinked - 1)
    {
       // _gladiator->log("On a border, minusing %d to score", remaining < 5 ? BORDER_MALUS / remaining : 0);
        score -= remaining < 5 ? BORDER_MALUS / remaining : 0;
    }
    int i = 0;
    if (!square->northSquare) i++;
    if (!square->southSquare) i++;
    if (!square->westSquare) i++;
    if (!square->eastSquare) i++;
    if (i == 3) {
        score -= DEAD_END_MALUS;
      //  _gladiator->log("Is a dead end, minusing %d to score", DEAD_END_MALUS);
    }

    int dangerPenalty = square->danger * DANGER_MULT;
    score -= dangerPenalty;
   // _gladiator->log("Danger level %d, minusing %d to score", square->danger, dangerPenalty);
   int coinBonus = square->coin.value * COIN_MULT; 
   if(needToPutBomb)
    {
        score -= coinBonus;
    }
    else
    {
        score += coinBonus;
    }


   // _gladiator->log("Coin value %d, adding %d to score", square->coin.value, coinBonus);

    if (isGoingThroughWall) {
        score -= WALL_MALUS;
      //  _gladiator->log("Going through a wall, minusing %d to score", WALL_MALUS);
    }

    score += 50;
    //_gladiator->log("Adding base score of 100");

   // _gladiator->log("Final score for square at (%d, %d) is %d", square->i, square->j, score);
    return score;
}

bool isOutside(MazeSquare* square) {
    int min = shrinked;
    int max = size - shrinked - 1;
    return square->i < min || square->i >= max || square->j < min || square->j >= max;
}

struct Possibilty {
    std::array<MazeSquare*, MAX_MOVES> squares {}; 
    std::array<int, MAX_MOVES> scores {};
    int score = 0;

};

Possibilty bestPossibility {};

void explore(MazeSquare* square, int currentMove, Possibilty lastPossibility, Gladiator* gladiator, bool wall = false) {
    //gladiator->log("Score at begining of explore : %d", lastPossibility.score);
    int squareScore = getScore(square, wall, gladiator->weapon->getBombCount() > 2);
    lastPossibility.score += squareScore;
    lastPossibility.squares[currentMove] = square;
    lastPossibility.scores[currentMove] = squareScore;
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

void shrinkPossibility() {
    int currentMove = MAX_MOVES - 1;
    while (currentMove > 0) {
        if (bestPossibility.scores[currentMove] < 0) {
            bestPossibility.score -= bestPossibility.scores[currentMove];
            bestPossibility.squares[currentMove] = nullptr;
            bestPossibility.scores[currentMove] = 0;
            currentMove--;
        } else {
            break;
        }
    }
}

std::vector<MazeSquare *> search(Gladiator* gladiator, int _remaining, int _shrink) {
    bestPossibility = {};
    bestPossibility.score = -5000;
    remaining = _remaining;
    shrinked = _shrink;
    _gladiator = gladiator;
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

        //shrinkPossibility();
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