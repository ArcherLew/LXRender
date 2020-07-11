// #ifndef VECTOR_H
// #define VECTOR_H
#pragma once

#include "Math.h"
#include "Matrix.h"

class Vector
{
public:
    float x, y, z, w;

    Vector() {}

    Vector(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    Vector &operator+=(const Vector &v);

    Vector &operator-=(const Vector &v);

    Vector &operator*=(float f);

    // v' = v * m 相当于 v' = mv，其实是矩阵在左
    // 之所以要这么定义是因为乘法操作符优先级是从左往右
    Vector operator*(Matrix &m);

    Vector operator*(float f);

    Vector operator-(Vector &v2);

    float Length();

    void Normalize();

    void Print();

    static void Sub(Vector *v, const Vector *v1, const Vector *v2);

    static float Dot(const Vector *v1, const Vector *v2);

    static void Cross(Vector *v, Vector *v1, Vector *v2);

    static void Interpolate(Vector *v, Vector *v1, Vector *v2, float t);
};

// #endif