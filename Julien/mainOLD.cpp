/*#include "gladiator.h"
#include <cmath>
#include "movement.hpp"
#include <set>
#include "utils.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_set>
#include "BFS.hpp"
#include <algorithm>
#include <chrono>
#include "MazeCell.hpp"
#include "Vector2Int.hpp"
#undef abs

int size = 12;
int shrinked = 0;
std::vector<MazeSquare*> foundPath;
MazeSquare *cell = NULL;
Gladiator *gladiator;
bool firstIter = true;
auto startTime = std::chrono::steady_clock::now();


float turnSpeed = 0.1;
float angleError = radians(5);






float gladiatorAngle() {
	return gladiator->robot->getData().position.a;
}

Position gladiatorPosition() {
	return gladiator->robot->getData().position;
}

double reductionAngle(double x)
{
    x = fmod(x + PI, 2 * PI);  // Step 1: Wrap angle within [-2π, 2π]
    if (x < 0)
        x += 2 * PI;  // Step 2: Ensure the result is within [0, 2π]
    return x - PI;    // Step 3: Shift to range [-π, π]
}

double angleDifference(double a, double b)
{
    return reductionAngle(b - a);
}

template <typename T>
T clamp(T value, T minValue, T maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

// void turnLeft() {
// 	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, turnSpeed);
// 	gladiator->control->setWheelSpeed(WheelAxis::LEFT, -turnSpeed);
// }
// void turnRight() {
// 	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, -turnSpeed);
// 	gladiator->control->setWheelSpeed(WheelAxis::LEFT, turnSpeed);
// }

void turn(float speed)
{
	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, speed);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, -speed);
}

void moveforward(float moveSpeed) {
	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, moveSpeed);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, moveSpeed);
}

void moveBackward(float moveSpeed) {
	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, -moveSpeed);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, -moveSpeed);
}

void stop() {
	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, 0);
}

void lookAt(float rad) {
	//cap the angle between 0 and 2PI
	rad = reductionAngle(rad);
	float currentAngle = gladiatorAngle();
	float diff = angleDifference(currentAngle, rad);
	if (abs(diff) < angleError)
		return stop();
	turnSpeed = clamp(diff * 0.3f, -0.3f, 0.3f);

	turn(turnSpeed);
}

float kw = 1.2; // weight of the angular speed
float kv = 1.f; // weight of the linear speed
float wlimit = 0.8f; // angular speed limit
float vlimit = 0.56; // linear speed limit
float posError = 0.07; // position error

bool go_to(Position dest, Position pos)
{
    double consvl, consvr;
    double dx = dest.x - pos.x; // dest position from current position
    double dy = dest.y - pos.y;
    double d = sqrt(dx * dx + dy * dy);

	double rho = atan2(dy, dx); // angle toward dest
	double da = reductionAngle(rho - pos.a); // angle between dest and current position

	if (abs(da) > (angleError * 1.5)) {
		lookAt(rho);
	}
    else if (d > posError)
    {
		double consw = kw * da;
        double consv = kv * d * cos(da);
        consw = abs(consw) > wlimit ? (consw > 0 ? 1 : -1) * wlimit : consw;
        consv = abs(consv) > vlimit ? (consv > 0 ? 1 : -1) * vlimit : consv;

		float angularSpeed = gladiator->robot->getRobotRadius() * consw;

        consvl = consv - angularSpeed; // GFA 3.6.2
        consvr = consv + angularSpeed; // GFA 3.6.2
		gladiator->control->setWheelSpeed(WheelAxis::RIGHT, consvr, false); // GFA 3.2.1
		gladiator->control->setWheelSpeed(WheelAxis::LEFT, consvl, false);  // GFA 3.2.1
    }
    else
    {
		stop();
		return true;
    }
	return false;
}

enum Direction
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};
Direction getRandomDirection(Direction lastDirection, bool forceForward)
{
    // if (forceForward)
    // {
    //     int random = std::rand() % 10;
    //     if (random < 3)
    //         return lastDirection;
    // }
    int random = std::rand() % 4;
    switch (random)
    {
    case 0:
        return NORTH;
    case 1:
        return SOUTH;
    case 2:
        return EAST;
    case 3:
        return WEST;
    }
	gladiator->log("Nothing returned here 1");
	return EAST;
}

/*Vector2Int getNextCell(Direction direction, Vector2Int pos)
{
    switch (direction)
    {
    case NORTH:
        return Vector2Int(pos.x, pos.y + 1);
    case SOUTH:
        return Vector2Int(pos.x, pos.y - 1);
    case EAST:
        return Vector2Int(pos.x + 1, pos.y);
    case WEST:
        return Vector2Int(pos.x - 1, pos.y);
    }
	gladiator->log("Nothing returned here 2");
	return Vector2Int(pos.x + 1, pos.y);
}

bool isOutOfBounds(Vector2Int pos)
{
    return (pos.x < 0 || pos.x >= 12 || pos.y < 0 || pos.y >= 12);
}

bool goingThroughWall(Direction direction, MazeSquare *square)
{
    switch (direction)
    {
    case NORTH:
        return !square->northSquare;
    case SOUTH:
        return !square->southSquare;
    case EAST:
        return !square->eastSquare;
    case WEST:
        return !square->westSquare;
    }
	gladiator->log("Nothing returned here 3");
	return false;
}*/

/*int scoreOfCell(MazeSquare cell, bool isGoingThroughWall)
{
    int minIndex = 0;
    int maxIndex = 12;
    int score = 0;

    if (cell.i == minIndex || cell.i == maxIndex || cell.j == minIndex || cell.j == maxIndex)
        score -= 1;
    int i = 0;
    if (!cell.northSquare)
        i++;
    if (!cell.southSquare)
       i++;
    if (!cell.westSquare)
        i++;
    if (!cell.eastSquare)
        i++;
    score -= i == 3 ? 4 : 0;
    score -= cell.danger * 2;
    score += cell.coin.value * 10;
    score -= isGoingThroughWall ? 8 : 0;
    return score;
}

MazeSquare* getNextSquare(Direction direction, MazeSquare* square)
{
    switch (direction)
    {
    case NORTH:
        return square->northSquare;
    case SOUTH:
        return square->southSquare;
    case EAST:
        return square->eastSquare;
    case WEST:
        return square->westSquare;
    }
    gladiator->log("Nothing returned here 2");
    return nullptr;
}

bool isOutOfBounds(MazeSquare* square)
{
    return (square == nullptr);
}

bool goingThroughWall(Direction direction, MazeSquare *square)
{
    switch (direction)
    {
    case NORTH:
        return !square->northSquare;
    case SOUTH:
        return !square->southSquare;
    case EAST:
        return !square->eastSquare;
    case WEST:
        return !square->westSquare;
    }
    gladiator->log("Nothing returned here 3");
    return false;
}

std::vector<MazeSquare *> findPathToGo(Gladiator *gladiator)
{
    int maxTries = 50;
    int depth = 6;

    MazeSquare *fromSquare = gladiator->maze->getNearestSquare();
    std::vector<MazeSquare *> bestPath(depth);
    Direction lastDirection = EAST;
    int maxScore = -5000;
    std::vector<MazeSquare *> currentPath(depth);
    std::unordered_set<MazeSquare *> visited;
    int y = 0;
    
    for (int i = 0; i < maxTries; i++)
    {
        MazeSquare *pos = fromSquare;
        int scoreOfSim = 0;
        visited.clear();

        y = 4;
        for (int j = 0; j < depth; j++)
        {
            Direction nextDirection = getRandomDirection(lastDirection, j != 0);
            MazeSquare *nextSquare = getNextSquare(nextDirection, pos);
            if (visited.find(nextSquare) != visited.end())
            {
                gladiator->log("Already visited");
                j--;
                y++;
                break;
            }
            if (isOutOfBounds(nextSquare))
            {
                j--;
                gladiator->log("Out of bounds");
                y++;    
                break;
            }
            visited.insert(nextSquare);

            bool crossWall = goingThroughWall(nextDirection, pos);
            int score = scoreOfCell(*nextSquare, crossWall);

            pos = nextSquare;
            lastDirection = nextDirection;
            scoreOfSim += score;
            currentPath[j] = nextSquare;
        }
        if (maxScore < scoreOfSim)
        {
            gladiator->log("New best path score: %d, old one was %d", scoreOfSim, maxScore);
            maxScore = scoreOfSim;
            bestPath = currentPath;
        }
    }
    gladiator->log("Best path score: %d", maxScore);
    gladiator->log("Best path size: %u", bestPath.size());
    return bestPath;
}

/*std::vector<MazeSquare *> findPathToGo(Gladiator *gladiator)
{
    int maxTries = 100;
    int depth = 10;

    MazeSquare *fromSquare = gladiator->maze->getNearestSquare();
    Vector2Int startPos(fromSquare->i, fromSquare->j);
    std::vector<MazeSquare *> bestPath(depth);
    Direction lastDirection = EAST;
    int maxScore = -5000;
    std::vector<MazeSquare *> currentPath(depth);
    std::unordered_set<MazeSquare *> visited;
    int y = 0;

    for (int i = 0; i < maxTries; i++)
    {
        MazeSquare *pos = fromSquare;
        int scoreOfSim = 0;
        visited.clear();

        y = 0;
        for (int j = 0; j < depth; j++)
        {
            Direction nextDirection = getRandomDirection(lastDirection, j != 0);
            Vector2Int nextPosition = getNextCell(nextDirection, Vector2Int(pos->i, pos->j));
            if (visited.find(nextPosition) != visited.end())
            {
                gladiator->log("Already visited");
                j--;
                y++;
                if (y > 10)
                    break;
                continue;
            }
            if (isOutOfBounds(nextPosition))
            {
                j--;
                gladiator->log("Out of bounds");
                y++;
                if (y > 10)
                    break;
                continue;
            }
            visited.insert(nextPosition);
            int x = nextPosition.x;
            int y = nextPosition.y;

            MazeSquare *nextMazeSquare = gladiator->maze->getSquare(x, y);
			if (nextMazeSquare == NULL)
			{
				gladiator->log("Next maze square is null");
				continue;
			}

            bool crossWall = goingThroughWall(nextDirection, nextMazeSquare);
            int score = scoreOfCell(*nextMazeSquare, crossWall);

            pos = nextPosition;
            lastDirection = nextDirection;
            scoreOfSim += score;
            currentPath[j] = nextPosition;
            gladiator->log("J %d", j);
        }
        if (maxScore < scoreOfSim)
        {
            gladiator->log("New best path score: %d, old one was %d", scoreOfSim, maxScore);
            maxScore = scoreOfSim;
            bestPath = currentPath;
        }
        gladiator->log ("Next iter  i %d", i);
    }
    gladiator->log("Best path score: %d", maxScore);
    gladiator->log("Best path size: %u", bestPath.size());
    return bestPath;
}*/

/*void reset()
{
    foundPath = findPathToGo(gladiator);
    for (int i = 0; i < foundPath.size(); i++)
        gladiator->log("Path %llu: x %d, y %d", i, foundPath[i]->i, foundPath[i]->j);
}


void setup()
{
    gladiator = new Gladiator();
    gladiator->game->onReset(&reset);
}

void loop()
{
    if (gladiator->game->isStarted())
    {
        if (foundPath.size() > 0)
        {
            if (cell == NULL)
            {
                cell  = foundPath.back();
                gladiator->log("Cell: x %d, y %d", cell->i, cell->j);
            }
            else
            {
                Navigation nav(gladiator);
                Position position = {Utils::PosIntToFloat(cell->i), Utils::PosIntToFloat(cell->i) ,0};
                if (go_to(gladiator->robot->getData().position, position))
                {
                    gladiator->log("Reached cell: x %d, y %d", cell->i, cell->j);
                    foundPath.pop_back();
                    cell = NULL;
                }
            }
        }
    }
    delay(10); // boucle à 100Hz
}*/
