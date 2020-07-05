#ifndef MATH_H
#define MATH_H

#include <math.h>

namespace Math
{
    const float PI = 3.14159265359f;
    const float div180 = 1.0f / 180.0f;

    int Limit(int x, int min, int max)
    {
        return (x < min) ? min : ((x > max) ? max : x);
    }

    float Interpolate(float x1, float x2, float t)
    {
        return x1 + (x2 - x1) * t;
    }

    float Deg2Rad(float deg)
    {
        return deg * div180 / PI;
    }
} // namespace Math

#endif

// member access into incomplete type
#include "Matrix.h"
#include "Vector.h"
