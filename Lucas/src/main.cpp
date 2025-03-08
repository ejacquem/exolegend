// #include "gladiator.h"
// #include <cmath>
// // #include "movement.hpp"
// #include <set>
// // #include "utils.hpp"
// #include <iostream>
// #include <vector>
// #include <queue>
// #include <map>
// #include <unordered_set>
// // #include "BFS.hpp"
// #include <algorithm>
// #include <chrono>
// // #include "navigation.hpp"
// // #include "MazeCell.hpp"
// #include "Vector2Int.hpp"
// #undef abs

// int size = 12;
// int shrinked = 0;
// std::vector<MazeSquare*> foundPath;
// MazeSquare *cell = NULL;
// Gladiator *gladiator;
// bool firstIter = true;
// auto startTime = std::chrono::steady_clock::now();

// enum Direction
// {
//     NORTH,
//     SOUTH,
//     EAST,
//     WEST
// };
// Direction getRandomDirection(Direction lastDirection, bool forceForward)
// {
//     // if (forceForward)
//     // {
//     //     int random = std::rand() % 10;
//     //     if (random < 3)
//     //         return lastDirection;
//     // }
//     int random = std::rand() % 4;
//     switch (random)
//     {
//     case 0:
//         return NORTH;
//     case 1:
//         return SOUTH;
//     case 2:
//         return EAST;
//     case 3:
//         return WEST;
//     }
// 	gladiator->log("Nothing returned here 1");
// 	return EAST;
// }

// Vector2Int getNextCell(Direction direction, Vector2Int pos)
// {
//     switch (direction)
//     {
//     case NORTH:
//         return Vector2Int(pos.x, pos.y + 1);
//     case SOUTH:
//         return Vector2Int(pos.x, pos.y - 1);
//     case EAST:
//         return Vector2Int(pos.x + 1, pos.y);
//     case WEST:
//         return Vector2Int(pos.x - 1, pos.y);
//     }
// 	gladiator->log("Nothing returned here 2");
// 	return Vector2Int(pos.x + 1, pos.y);
// }

// bool isOutOfBounds(Vector2Int pos)
// {
//     return (pos.x < 0 || pos.x >= 12 || pos.y < 0 || pos.y >= 12);
// }

// bool goingThroughWall(Direction direction, MazeSquare *square)
// {
//     switch (direction)
//     {
//     case NORTH:
//         return !square->northSquare;
//     case SOUTH:
//         return !square->southSquare;
//     case EAST:
//         return !square->eastSquare;
//     case WEST:
//         return !square->westSquare;
//     }
// 	gladiator->log("Nothing returned here 3");
// 	return false;
// }

// int scoreOfCell(MazeSquare cell, bool isGoingThroughWall)
// {
//     int minIndex = 0;
//     int maxIndex = 12;
//     int score = 0;

//     if (cell.i == minIndex || cell.i == maxIndex || cell.j == minIndex || cell.j == maxIndex)
//         score -= 1;
//     int i = 0;
//     if (!cell.northSquare)
//         i++;
//     if (!cell.southSquare)
//        i++;
//     if (!cell.westSquare)
//         i++;
//     if (!cell.eastSquare)
//         i++;
//     score -= i == 3 ? 4 : 0;
//     score -= cell.danger * 2;
//     score += cell.coin.value * 10;
//     score -= isGoingThroughWall ? 8 : 0;
//     return score;
// }

// std::vector<Vector2Int> findPathToGo(Gladiator *gladiator)
// {
//     int maxTries = 1000;
//     int depth = 10;

//     MazeSquare *fromSquare = gladiator->maze->getNearestSquare();
//     Vector2Int startPos(fromSquare->i, fromSquare->j);
//     std::vector<Vector2Int> bestPath(depth);
//     Direction lastDirection = EAST;
//     int maxScore = -5000;
//     std::vector<Vector2Int> currentPath(depth);

//     for (int i = 0; i < maxTries; i++)
//     {
//         Vector2Int pos = startPos;
//         int scoreOfSim = 0;

//         for (int j = 0; j < depth; j++)
//         {
//             Direction nextDirection = getRandomDirection(lastDirection, j != 0);
//             Vector2Int nextPosition = getNextCell(nextDirection, pos);
//             if (isOutOfBounds(nextPosition))
//             {
//                 j--;
//                 gladiator->log("Out of bounds");
//                 continue;
//             }
//             int x = nextPosition.x;
//             int y = nextPosition.y;

//             MazeSquare *nextMazeSquare = gladiator->maze->getSquare(x, y);
// 			if (nextMazeSquare == NULL)
// 			{
// 				gladiator->log("Next maze square is null");
// 				continue;
// 			}

//             bool crossWall = goingThroughWall(nextDirection, nextMazeSquare);
//             int score = scoreOfCell(*nextMazeSquare, crossWall);

//             pos = nextPosition;
//             lastDirection = nextDirection;
//             scoreOfSim += score;
//             currentPath[j] = nextPosition;
//             gladiator->log("J %d", j);
//         }
//         if (maxScore < scoreOfSim)
//         {
//             gladiator->log("New best path score: %d, old one was %d", scoreOfSim, maxScore);
//             maxScore = scoreOfSim;
//             bestPath = currentPath;
//         }
//         gladiator->log ("Next iter  i %d", i);
//     }
//     gladiator->log("Best path score: %d", maxScore);
//     gladiator->log("Best path size: %u", bestPath.size());
//     return bestPath;
// }

// void reset()
// {
//     std::vector<Vector2Int> path = findPathToGo(gladiator);
//     for (size_t i = 0; i < path.size(); i++)
//     {
//         gladiator->log("Path %u: x %d, y %d", i, path[i].x, path[i].y);
//     }
// }

// void setup()
// {
//     gladiator = new Gladiator();
//     gladiator->game->onReset(&reset);
// }

// void loop()
// {
//     if (gladiator->game->isStarted())
//     {

//     }
//     delay(10); // boucle à 100Hz
// }
