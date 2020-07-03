#ifndef MATH_H
#define MATH_H

#include <math.h>

namespace Math
{
    const float PI = 3.1415926f;

    int Limit(int x, int min, int max)
    {
        return (x < min) ? min : ((x > max) ? max : x);
    }

    float Interpolate(float x1, float x2, float t)
    {
        return x1 + (x2 - x1) * t;
    }
} // namespace Math

#endif

// member access into incomplete type
#include "Matrix.h"
#include "Vector.h"
