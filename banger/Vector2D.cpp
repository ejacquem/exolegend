#include "Vector2D.hpp"

Vector2D::Vector2D() {
}

Vector2D::Vector2D(float x, float y) {
    this->x = x;
    this->y = y;
}

Vector2D Vector2D::operator+(const Vector2D& vector) {
    return Vector2D(this->x + vector.x, this->y + vector.y);
}

Vector2D Vector2D::operator-(const Vector2D& vector)
{
    return Vector2D(this->x - vector.x, this->y - vector.y);
}

Vector2D Vector2D::operator*(const float& f) {
    return Vector2D(this->x * f, this->y * f);
}

Vector2D Vector2D::operator/(const float& f) {
    return Vector2D(this->x / f, this->y / f);
}

Vector2D Vector2D::operator=(const Vector2D& vector) {
    this->x = vector.x;
    this->y = vector.y;
    return *this;
}

Vector2D Vector2D::operator+=(const Vector2D& vector) {
    this->x += vector.x;
    this->y += vector.y;
    return *this;
}

Vector2D Vector2D::operator-=(const Vector2D& vector) {
    this->x -= vector.x;
    this->y -= vector.y;
    return *this;
}

Vector2D Vector2D::operator/=(const float& f) {
    this->x /= f;
    this->y /= f;
    return *this;
}

Vector2D Vector2D::operator*=(const float& f) {
    this->x *= f;
    this->y *= f;
    return *this;
}

bool Vector2D::operator==(const Vector2D& vector) {
    return this->x == vector.x && this->y == vector.y;
}

bool Vector2D::operator!=(const Vector2D& vector) {
    return this->x != vector.x || this->y != vector.y;
}

void Vector2D::normalize() {
    float magnitude = getMagnitude();
    this->x /= magnitude;
    this->y /= magnitude;
}

float Vector2D::getAngle() {
    return atan2(this->y, this->x);
}

float Vector2D::getMagnitude() {
    //return sqrt(pow(this->x, 2) + pow(this->y, 2));
    return sqrt(x*x + y*y);
}

Vector2D Vector2D::zero() {
    return Vector2D(0, 0);
}

Vector2D Vector2D::distance(const Vector2D& v1, const Vector2D& v2) {
    return Vector2D(v2.x - v1.x, v2.y - v1.y);
}