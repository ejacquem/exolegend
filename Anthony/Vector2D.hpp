#pragma once
#include <iostream>
#include <math.h>

class Vector2D {
    private:
        Vector2D();
    public:
        Vector2D(float x, float y);
        float x, y;
        Vector2D operator+(const Vector2D& v);
        Vector2D operator-(const Vector2D& v);
        Vector2D operator*(const float& f);
        Vector2D operator/(const float& f);
        Vector2D operator=(const Vector2D& v);
        Vector2D operator+=(const Vector2D& v);
        Vector2D operator-=(const Vector2D& v);
        Vector2D operator*=(const float& f);
        Vector2D operator/=(const float& f);
        bool operator==(const Vector2D& v);
        bool operator!=(const Vector2D& v);
        void normalize();
        float getAngle();
        float getMagnitude();

        static Vector2D zero();
        static Vector2D distance(const Vector2D& v1, const Vector2D& v2);     
};