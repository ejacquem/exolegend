#pragma once

#include <cmath>
#undef abs

class Vector2Int {
    public:
        Vector2Int();
        Vector2Int(int x, int y);
        Vector2Int operator-(const Vector2Int &other) const;
        Vector2Int operator+(const Vector2Int &other) const;

        bool operator==(const Vector2Int &other) const;
        bool operator!=(const Vector2Int &other) const;
        int x;
        int y;
};