#include "Vector2Int.hpp"

Vector2Int::Vector2Int() : x(0.), y(0.) {}

Vector2Int::Vector2Int(int _x, int _y) : x(_x), y(_y) {}



Vector2Int Vector2Int::operator-(const Vector2Int &other) const { return {x - other.x, y - other.y}; }
Vector2Int Vector2Int::operator+(const Vector2Int &other) const { return {x + other.x, y + other.y}; }

bool Vector2Int::operator==(const Vector2Int &other) const { return std::abs(x - other.x) < 1e-5 && std::abs(y - other.y) < 1e-5; }
bool Vector2Int::operator!=(const Vector2Int &other) const { return !(*this == other); }
	