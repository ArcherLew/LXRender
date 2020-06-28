#ifndef MATH_H
#define MATH_H

namespace Math
{
    int Limit(int x, int min, int max)
    {
        return (x < min) ? min : ((x > max) ? max : x);
    }

    float Interpolate(float x1, float x2, float t)
    {
        return x1 + (x2 - x1) * t;
    }
} // namespace

typedef struct Vector
{
    float x, y, z, w;

    Vector() {}

    Vector(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    void Add(Vector *v)
    {
        x += v->x;
        y += v->y;
        z += v->z;
    }

    void Sub(Vector *v)
    {
        x -= v->x;
        y -= v->y;
        z -= v->z;
    }

    void Mul(float m)
    {
        x *= m;
        y *= m;
        z *= m;
    }

    void Div(float d)
    {
        Mul(1 / d);
    }

    static void Interpolate(Vector *v, Vector *v1, Vector *v2, float t)
    {
        v->x = Math::Interpolate(v1->x, v2->x, t);
        v->y = Math::Interpolate(v1->y, v2->y, t);
        v->z = Math::Interpolate(v1->z, v2->z, t);
        // todo: w ?
    }

} Vector;

#endif