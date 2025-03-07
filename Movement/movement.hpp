#ifndef MOVEMENT_H
#define MOVEMENT_H  

#include "gladiator.h"

class Movement
{
    public:
    Movement();
    void go_to(Position cons, Position pos, Gladiator *gladiator);
    float kw ;
    float kv ;
    float wlimit ;
    float vlimit ;
    float erreurPos ;
    private:
    double reductionAngle(double x);

};

#endif