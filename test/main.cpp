#include gladiator.h
Gladiator* gladiator;

float turnSpeed = 0.1;
float angleError = 0.1;
unsigned long startTime = 0;
unsigned long currentTime = 0; // time in ms since the beginning of the match

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

void turnLeft() {
	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, turnSpeed);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, -turnSpeed);
}
void turnRight() {
	gladiator->control->setWheelSpeed(WheelAxis::RIGHT, -turnSpeed);
	gladiator->control->setWheelSpeed(WheelAxis::LEFT, turnSpeed);
}

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
	turnSpeed = clamp(diff, -0.5f, 0.5f);

	turn(turnSpeed);
}

float radToDeg(float rad) {
	return rad * 180 / PI;
}

float degToRad(float deg) {
	return deg * PI / 180;
}

void loop() {
	currentTime = millis() - startTime;
    if(gladiator->game->isStarted()) { //tester si un match à déjà commencer
		if(((currentTime / 1000) / 2) % 2 == 0) {
			lookAt(1.57);
		}
		else {
			lookAt(-1.57);
		}
		// gladiator->log(angle: %f, gladiator->robot->getData().position.a);
		// gladiator->log(millis: %lu, millis());
		// gladiator->log(currentTime: %ld, currentTime);
		// gladiator->log(startTime: %ld, startTime);
		//Lorsque le jeu commencera le robot ira en ligne droite
		delay(10);
    }
    //La consigne en vitesse est forcée à 0 lorsque aucun match n'a débuté.
}
