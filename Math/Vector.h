#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

typedef struct Vector
{
    float x, y, z, w;

    Vector() {}

    Vector(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    Vector &operator+=(const Vector &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector &operator-=(const Vector &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector &operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    // v' = v * m 相当于 v' = mv，其实是矩阵在左
    // 之所以要这么定义是因为乘法操作符优先级是从左往右
    Vector operator*(Matrix &m)
    {
        Vector v;
        v.x = this->x * m.data[0][0] + this->y * m.data[1][0] + this->z * m.data[2][0] + this->w * m.data[3][0];
        v.y = this->x * m.data[0][1] + this->y * m.data[1][1] + this->z * m.data[2][1] + this->w * m.data[3][1];
        v.z = this->x * m.data[0][2] + this->y * m.data[1][2] + this->z * m.data[2][2] + this->w * m.data[3][2];
        v.w = this->x * m.data[0][3] + this->y * m.data[1][3] + this->z * m.data[2][3] + this->w * m.data[3][3];
        return v;
    }

    Vector operator*(float f)
    {
        Vector v;
        v.x = this->x * f;
        v.y = this->y * f;
        v.z = this->z * f;
        return v;
    }

    float Length()
    {
        float sq = x * x + y * y + z * z;
        return (float)sqrt(sq);
    }

    void Normalize()
    {
        float length = Length();
        if (length != 0.0f)
        {
            float inv = 1.0f / length;
            x *= inv;
            y *= inv;
            z *= inv;
        }
    }

    // Vector operator-(const Vector &v1, const Vector &v2)
    // {
    //     Vector v;
    //     v.x = v1.x - v2.x;
    //     v.y = v1.y - v2.y;
    //     v.z = v1.z - v2.z;
    //     v.w = 1.0f;
    //     return v;
    // }
    void Print()
    {
        std::cout << x << "," << y << "," << z << "," << w << std::endl;
    }

    static void Sub(Vector *v, const Vector *v1, const Vector *v2)
    {
        v->x = v1->x - v2->x;
        v->y = v1->y - v2->y;
        v->z = v1->z - v2->z;
    }

    static float Dot(const Vector *v1, const Vector *v2)
    {
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
    }

    static void Cross(Vector *v, const Vector *v1, const Vector *v2)
    {
        v->x = v1->y * v2->z - v1->z * v2->y;
        v->y = v1->z * v2->x - v1->x * v2->z;
        v->z = v1->x * v2->y - v1->y * v2->x;
        v->w = 1.0f;
    }

    static void Interpolate(Vector *v, Vector *v1, Vector *v2, float t)
    {
        v->x = Math::Interpolate(v1->x, v2->x, t);
        v->y = Math::Interpolate(v1->y, v2->y, t);
        v->z = Math::Interpolate(v1->z, v2->z, t);
        v->w = 1.0f;
    }
} Vector;

typedef Vector Point;

#endif