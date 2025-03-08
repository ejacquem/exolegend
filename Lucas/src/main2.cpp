#include "gladiator.h"
Gladiator* gladiator;

float turnSpeed = 0.1;
float angleError = radians(5);
unsigned long startTime = 0;
unsigned long currentTime = 0; // time in ms since the beginning of the match
unsigned long timeUntilShrink = 0; // time in ms since the beginning of the match

void reset();
void setup() {
    //instanciation de l'objet gladiator
    gladiator = new Gladiator();
    //enregistrement de la fonction de reset qui s'éxecute à chaque fois avant qu'une partie commence
    gladiator->game->onReset(&reset);
}

void reset() {
	startTime = millis();
    //fonction de reset:
    //initialisation de toutes vos variables avant le début d'un match
}

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

// Position getNearestBomb(){
// 	const MazeSquare *currentSquare = gladiator->maze->getNearestSquare();
// 	Position nearestBomb = {currentSquare->i, currentSquare->j, 0};
// 	float nearestBombDistance = 1000;

// 	for (int i = 0; i < 12; i++) {
// 		for (int j = 0; j < 12; j++) {
// 			MazeSquare *square = gladiator->maze->getSquare(i, j);
// 			if (square->coin.value > 0 && 
// 				square->danger == 0 &&
// 				square->possession != gladiator->robot->getData().teamId) {
// 				Position temp = {i, j, 0};
// 				float dist = sqrt(pow(temp.x - currentSquare->i, 2) + pow(temp.y - currentSquare->j, 2));
// 				if (dist < nearestBombDistance) {
// 					nearestBombDistance = dist;
// 					nearestBomb = temp;
// 				}
// 			}
// 		}
// 	}

// 	return nearestBomb;
// }

// MazeSquare *nearestBomb(MazeSquare *bombA, MazeSquare *bombB){

// }

MazeSquare *getNearestBomb(){
	MazeSquare *nearestBomb = gladiator->maze->getNearestSquare();
	float nearestBombDistance = 1000;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			MazeSquare *square = gladiator->maze->getSquare(i, j);
			if (square == nullptr)
				continue;
			if (square->coin.value > 0 && 
				square->danger == 0 &&
				square->possession != gladiator->robot->getData().teamId) {
				MazeSquare * temp = square;
				float bombPosx = (temp->i + 0.5) * gladiator->maze->getSquareSize();
				float bombPosy = (temp->j + 0.5) * gladiator->maze->getSquareSize();
				float dist = sqrt(pow(bombPosx - gladiatorPosition().x, 2) + pow(bombPosy - gladiatorPosition().y, 2));
				if (dist < nearestBombDistance) {
					nearestBombDistance = dist;
					nearestBomb = temp;
				}
			}
		}
	}

	return nearestBomb;
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

bool isPlayerOutOfMap(){
	Position pos = gladiator->robot->getData().position;
	float mazeSize = gladiator->maze->getCurrentMazeSize();
	float shrinkSize = mazeSize / 6;
	float mazeTotalSize = gladiator->maze->getSize() - (timeUntilShrink  < 10) * shrinkSize;
	float limitMin = (mazeTotalSize - mazeSize) / 2;
	float limitMax = (mazeTotalSize - mazeSize) / 2 + mazeSize;
	return pos.x > limitMax || pos.x < limitMin || pos.y > limitMax || pos.y < limitMin;
}

bool isPlayerInDanger(){
	MazeSquare *currentSquare = gladiator->maze->getNearestSquare();
	if (currentSquare == nullptr)
		return true;
	return currentSquare->danger > 3;
}

int runToSafetyTimer = 0;

static int deltaTime = 0;
static unsigned long lastTime = 0;

void loop() {
	
	if(gladiator->game->isStarted()) { //tester si un match à déjà commencer
		currentTime = millis() - startTime;
		timeUntilShrink = 20000 - (currentTime % 20000);
		deltaTime = millis() - lastTime;
		lastTime = millis();
		runToSafetyTimer -= deltaTime;
		static Position destination = {6, 6, 0};

		if (runToSafetyTimer > 0)
		{
			gladiator->log("Running to safety");
			go_to(destination, gladiatorPosition());
			delay(10);
			return;
		}
		
		gladiator->log("Game Started");
		// if(((currentTime / 1000) / 4) % 2 == 0) {
		// 	lookAt(1.57);
		// }
		// else {
		// 	lookAt(-1.57);
		// }
		// gladiator->log("angle: %f", gladiator->robot->getData().position.a);
		// gladiator->log("millis: %lu", millis());
		gladiator->log("currentTime: %ld", currentTime);
		gladiator->log("timeUntilShrink: %ld", timeUntilShrink);
		if(MazeSquare *square = gladiator->maze->getNearestSquare()) {
			gladiator->log("gladiator square i: %d, j: %d", square->i, square->j);
		}
		else
		{
			gladiator->log("gladiator square is null");
		}
		// gladiator->log("startTime: %ld", startTime);

		if (isPlayerOutOfMap()) {
			runToSafetyTimer = 5000;
			destination = {6, 6, 0};

			// gladiator->log("player in Danger");
			// gladiator->log("danger: %d", gladiator->maze->getNearestSquare()->danger);

			destination.x = (destination.x + 0.5) * gladiator->maze->getSquareSize();
			destination.y = (destination.y + 0.5) * gladiator->maze->getSquareSize();

			// gladiator->log("player Out of map");
			// gladiator->log("x: %f, y: %f", gladiator->robot->getData().position.x, gladiator->robot->getData().position.y);
			// gladiator->log("mazeSize: %f", gladiator->maze->getCurrentMazeSize());
			// gladiator->log("mazeTotalSize: %f", gladiator->maze->getSize());
			// gladiator->log("destination x: %f, y: %f", destination.x, destination.y);
			
		}
		else
		{
			MazeSquare *nearestBombSquare = getNearestBomb();
			if (nearestBombSquare)
			{
				Position nearestBomb = {0};
				nearestBomb.x = (nearestBombSquare->i + 0.5) * gladiator->maze->getSquareSize();
				nearestBomb.y = (nearestBombSquare->j + 0.5) * gladiator->maze->getSquareSize();
				destination = nearestBomb;
			}
			//drop a bomb
		}
		go_to(destination, gladiatorPosition());
		if (gladiator->weapon->getBombCount() > 0)
			gladiator->weapon->dropBombs(1);

		//Lorsque le jeu commencera le robot ira en ligne droite
		delay(10);
    }
    //La consigne en vitesse est forcée à 0 lorsque aucun match n'a débuté.
}
