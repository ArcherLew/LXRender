#ifndef LX_MATH_H
#define LX_MATH_H
// #pragma once

#include <math.h>

namespace Math
{
    extern const float PI;
    extern const float div180;

    int Limit(int x, int min, int max);

    float Interpolate(float x1, float x2, float t);

    float Deg2Rad(float deg);
} // namespace Math

#endif
