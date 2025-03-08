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

MazeSquare *cell = NULL;
Gladiator *gladiator;
int shrink = 0;

bool first = true;
float turnSpeed = 0.1;
float angleError = radians(5);
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

void reset()
{
    bestPath.clear();
    cell = NULL;
    gladiator->log("Reset");
    start = std::chrono::high_resolution_clock::now();
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
    if (gladiator->game->isStarted())
    {
        int remaining = getTimeRemaining();
        if (remaining < 2)
        {
            bestPath.clear();
            cell = NULL;
        }

        int bombCount = gladiator->weapon->getBombCount();
        if (bombCount > 0)
            gladiator->weapon->dropBombs(bombCount);

        if (bestPath.empty())
        {
            gladiator->log("No path, searching");
            bestPath = search(gladiator, remaining, shrink);
            gladiator->log("Path found");
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
        if (remaining <= 0)
        {
            start = std::chrono::high_resolution_clock::now();
            gladiator->log("Area shrinked !");
            shrink++;
        }
    }
    delay(10); // boucle à 100Hz
}
