// #include <math.h>
#include "Math.h"

const float Math::PI = 3.14159265359f;
const float Math::div180 = 1.0f / 180.0f;

int Math::Limit(int x, int min, int max)
{
    return (x < min) ? min : ((x > max) ? max : x);
}

float Math::Limit(float x, float min, float max)
{
    return (x < min) ? min : ((x > max) ? max : x);
}

float Math::Interpolate(float x1, float x2, float t)
{
    return x1 + (x2 - x1) * t;
}

float Math::Deg2Rad(float deg)
{
    return deg * div180 / PI;
}
