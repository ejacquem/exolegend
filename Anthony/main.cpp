#include "search.hpp"
#include "gladiator.h"
#include <cmath>
#include <set>
#include "utils.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#undef abs
#include "Vector2D.hpp"
#define ROBOT_CHECK_RADIUS 1.0
#define ANGLE_WATCH_ERROR radians(5)
#define ANGLE_ATTACK_ERROR radians(35)
enum State
{
    CHILL,
    FLEE
};
enum EnemyState
{
    LOOKING,
    UNAWARE,
    CHARGING,
    ATTACKING,
    NONE
};
uint8_t allyId;
std::vector<uint8_t> robots;


enum ForceDirection
{
	BACKWARD,
	FORWARD,
	DEFAUT
};

MazeSquare *cell = NULL;
Gladiator *gladiator;
int shrink = 0;
State state = CHILL;

bool first = true;
float turnSpeed = 0.1;
float angleError = radians(10);
std::vector<MazeSquare *> bestPath;

auto start = std::chrono::high_resolution_clock::now();
auto timeBefore = std::chrono::high_resolution_clock::now();
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

float mix(float a, float b, float t){
	return a * (1 - t) + b * t;
}

// turn the robot at a direction accounting for the initial speed
void smoothLookAt(float rad){
	float currentAngle = gladiatorAngle();
	float diff = angleDifference(currentAngle, rad);
	if (abs(diff) < angleError)
		return stop();
	turnSpeed = clamp(diff * 0.3f, -0.3f, 0.3f);
	
	float speedR = mix(turnSpeed, gladiator->control->getWheelSpeed(WheelAxis::RIGHT), 0.8);
	float speedL = mix(-turnSpeed, gladiator->control->getWheelSpeed(WheelAxis::LEFT), 0.8);

	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, speedR);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, -speedL);
}

// cap speed to 1 but keep ratio
void capSpeed(double *right, double *left) {
	double max = std::max(abs(*right), abs(*left));
	*right /= max;
	*left /= max;
}

float kw = 1.2; // weight of the angular speed
float kv = 1.0f; // weight of the linear speed
float wlimit = 0.8f; // angular speed limit
float vlimit = 0.5; // linear speed limit
float posError = 0.07; // position error

bool go_to(Position dest, Position pos, bool bailOut = false, ForceDirection forceDir = DEFAUT)
{
    double consvl, consvr;
    double dx = dest.x - pos.x; // dest position from current position
    double dy = dest.y - pos.y;
    double d = sqrt(dx * dx + dy * dy);

	double rho = atan2(dy, dx); // angle toward dest
	double da = reductionAngle(rho - pos.a); // angle between dest and current position

	bool moveBackward = false;
	if (forceDir == BACKWARD || forceDir == DEFAUT) {
		double rhoBack = reductionAngle(rho + M_PI); // opposite direction
		double daBack = reductionAngle(rhoBack - pos.a); // angle to opposite

		if (forceDir == BACKWARD || abs(daBack) < abs(da)) { // Less rotation needed?
			da = daBack;
			rho = rhoBack;
			moveBackward = true;
		}
	}

	if (abs(da) > (angleError * 1)) {
		lookAt(rho);
	}
    else if (d > posError)
    {
		double consw = kw * da;
        double consv = kv * d * cos(da);
        consw = abs(consw) > wlimit ? (consw > 0 ? 1 : -1) * wlimit : consw;
        consv = abs(consv) > vlimit ? (consv > 0 ? 1 : -1) * vlimit : consv;

		float angularSpeed = gladiator->robot->getRobotRadius() * consw;
		
		// Reverse motion if moving backward
		if (moveBackward) {
			consv = -consv;
			// angularSpeed = -angularSpeed;
		}

        consvl = consv - angularSpeed; // GFA 3.6.2
        consvr = consv + angularSpeed; // GFA 3.6.2

		if (bailOut){
			capSpeed(&consvr, &consvl);
		}

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

bool isPlayerOutOfMap(){
	Position pos = gladiator->robot->getData().position;
	float mazeSize = gladiator->maze->getCurrentMazeSize();
	float shrinkSize = mazeSize / 6;
	float mazeTotalSize = gladiator->maze->getSize() - (remaining  <= 2) * shrinkSize;
	float limitMin = (mazeTotalSize - mazeSize) / 2;
	float limitMax = (mazeTotalSize - mazeSize) / 2 + mazeSize;
	return pos.x > limitMax || pos.x < limitMin || pos.y > limitMax || pos.y < limitMin;
}

bool isPlayerInDanger(){
	MazeSquare *currentSquare = gladiator->maze->getNearestSquare();
	if (currentSquare == nullptr)
		return true;
	return currentSquare->danger > 5;
}
int minCellToBomb = 2;
bool goToEmptyCell = false;

uint8_t getClosestEnemy()
{
    Position pos = gladiator->robot->getData().position;
    uint8_t closest = 100;
    float minDist = 1000;
    for (uint8_t id : robots)
    {
        RobotData enemy = gladiator->game->getOtherRobotData(id);
        if (enemy.teamId == gladiator->robot->getData().teamId)
            continue;
        if (enemy.lifes <= 0)
            continue;
        Position enemyPos = enemy.position;
        float dist = Vector2D::distance(Vector2D(pos.x, pos.y), Vector2D(enemyPos.x, enemyPos.y)).getMagnitude();
        if (dist < minDist && dist <= ROBOT_CHECK_RADIUS)
        {
            minDist = dist;
            closest = id;
        }
    }
    return closest;
}

EnemyState getEnemyState(uint8_t robot)
{
    Position pos = gladiator->robot->getData().position;
    RobotData enemy = gladiator->game->getOtherRobotData(robot);
    Position enemyPos = enemy.position;
    float dist = Vector2D::distance(Vector2D(pos.x, pos.y), Vector2D(enemyPos.x, enemyPos.y)).getMagnitude();

    float angleToEnemy = atan2(enemyPos.y - pos.y, enemyPos.x - pos.x);
    float angleDiff = abs(angleDifference(enemyPos.a, -angleToEnemy));

        if (dist < 0.5)
        {
            if (angleDiff < ANGLE_ATTACK_ERROR)
                return ATTACKING;
            else
                return UNAWARE;
        }
        else
        {
            if (angleDiff < ANGLE_WATCH_ERROR)
            {
                if (enemy.vl >= 0.5 && enemy.vr >= 0.5)
                {
                    return CHARGING;
                }
                else
                {
                    return LOOKING;
                }
            }
            else return UNAWARE;
        }
        return NONE;
}


void ChargeTheWeaks(RobotData enemy)
{
    gladiator->log("Charging the weaks");
    Position pos = gladiator->robot->getData().position;
    Position enemyPos = enemy.position;
    go_to(enemyPos, pos, true, FORWARD);
}

void reset()
{
    bestPath.clear();
    cell = NULL;
    gladiator->log("Reset");
    start = std::chrono::high_resolution_clock::now();
	first = false;
}

int getTimeRemaining() {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    return 20 - elapsed;
}

void setup()
{
    gladiator = new Gladiator();
    gladiator->game->onReset(&reset);
}

void loop()
{
	if(first)
	{
		RobotList initialRobots = gladiator->game->getPlayingRobotsId();
		robots.clear();
		for (int i = 0; i < 4; i++)
		{
			if (initialRobots.ids[i] != gladiator->robot->getData().id)
			{
				if (gladiator->game->getOtherRobotData(initialRobots.ids[i]).teamId != gladiator->robot->getData().teamId)
				{
					robots.push_back(initialRobots.ids[i]);
				}
				else
					allyId = initialRobots.ids[i];
			}
	 
		}
		first = false;
	}
	
    if (gladiator->game->isStarted())
    {
		if ((isPlayerOutOfMap() || isPlayerInDanger()) && shrink < 4)
		{
			gladiator->log("Fleeing");
			bestPath.clear();
			cell = NULL;
			go_to({1.5, 1.5, 0}, gladiator->robot->getData().position, true, FORWARD);
			delay(100);
			return;
		}
		else
		{
			uint8_t closest = getClosestEnemy();
            if (closest != 100)
            {
                EnemyState enemyState = getEnemyState(closest);
                if (enemyState != NONE)
                {
                    switch (enemyState)
                    {
                    case LOOKING:
                        gladiator->log("Robot watching us");
                        break;
                    case UNAWARE:
                        gladiator->log("Robot unaware");
                        ChargeTheWeaks(gladiator->game->getOtherRobotData(closest));
                        cell = NULL;
                        bestPath.clear();
                        delay(10);
                        return;
                        break;
                    case CHARGING:
                        ChargeTheWeaks(gladiator->game->getOtherRobotData(closest));
                        cell = NULL;
                        bestPath.clear();
                        delay(10);
                        return;
                        break;
                    case ATTACKING:
                        ChargeTheWeaks(gladiator->game->getOtherRobotData(closest));
                        cell = NULL;
                        bestPath.clear();
                        delay(10);
                        return;
                        break;
                    default:
                        break;
                    }
                }
            }
		
        	remaining = getTimeRemaining();
			if(shrink < 3)
			{
				minCellToBomb = 1;
			}
			int bombCount = gladiator->weapon->getBombCount();
			if (bombCount > 0 && Utils::checkWall(gladiator) > minCellToBomb)
			{
				gladiator->weapon->dropBombs(bombCount);
				goToEmptyCell = false;
			}

			if (bestPath.empty())
			{
				gladiator->log("No path, searching");
				bestPath = search(gladiator, remaining < 0 ? 1 : remaining, shrink);
				gladiator->log("Path found");
				if (bestPath.empty())
				{
					gladiator->log("No path found after search, going center");
					go_to({1.5, 1.5, 0}, gladiator->robot->getData().position);
				}
			}
			else if (!bestPath.empty())
			{
				if (cell == NULL)
				{
					cell = bestPath.back();
				}
				else
				{
					Position goal = {Utils::PosIntToFloat(cell->i), Utils::PosIntToFloat(cell->j), 0};
					Position start = gladiator->robot->getData().position;
					if (go_to(goal, start))
					{
						gladiator->log("Reached Cell");
						bestPath.pop_back();
						cell = NULL;
					}
				}
			}
			if(bombCount > 2 && !goToEmptyCell)
			{
				goToEmptyCell = true;
				cell = NULL;
				bestPath.clear();
			}
			if (remaining <= 0)
			{
				start = std::chrono::high_resolution_clock::now();
				gladiator->log("Area shrinked !");
				shrink++;
			}
		}
    	
	}
	delay(10); // boucle à 100Hz
}