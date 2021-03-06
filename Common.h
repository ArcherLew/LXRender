#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

#ifndef COMMON_H
#define COMMON_H

inline float _Float2Pixel(float f)
{
    return (int)(f + 0.5f);
}

typedef unsigned int UINT32;
typedef unsigned char UCHAR;

typedef struct Color
{
    float r;
    float g;
    float b;
    float a;

    Color() {}

    Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

    Color &operator+=(Color &c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    Color &operator-=(Color &c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    Color &operator*=(float m)
    {
        r *= m;
        g *= m;
        b *= m;
        a *= m;
        return *this;
    }

    Color operator*(float m)
    {
        Color c;
        c.r = this->r * m;
        c.g = this->g * m;
        c.b = this->b * m;
        c.a = this->a * m;
        return c;
    }

    Color operator/(float m)
    {
        Color c;
        c.r = this->r / m;
        c.g = this->g / m;
        c.b = this->b / m;
        c.a = this->a / m;
        return c;
    }

    UINT32 ToInt32()
    {
        int R = (int)(r * 255.0f);
        int G = (int)(g * 255.0f);
        int B = (int)(b * 255.0f);
        R = Math::Limit(R, 0, 255);
        G = Math::Limit(G, 0, 255);
        B = Math::Limit(B, 0, 255);
        return (R << 16) | (G << 8) | (B);
    }

    static void Interpolate(Color *c, Color *c1, Color *c2, float t)
    {
        c->r = Math::Interpolate(c1->r, c2->r, t);
        c->g = Math::Interpolate(c1->g, c2->g, t);
        c->b = Math::Interpolate(c1->b, c2->b, t);
        c->a = Math::Interpolate(c1->a, c2->a, t);
    }

    static UINT32 Mul(UINT32 cc, float f)
    {
        UCHAR a = cc >> 24;
        UCHAR r = ((cc & 0x00ff0000) >> 16) * f;
        UCHAR g = ((cc & 0x0000ff00) >> 8) * f;
        UCHAR b = (cc & 0x000000ff) * f;
        UINT32 c = a << 24 | r << 16 | g << 8 | b;
        return c;
    }

} Color;

typedef struct Texcoord
{
    float u, v;

    Texcoord() {}

    Texcoord(float _u, float _v) : u(_u), v(_v) {}

    Texcoord &operator+=(Texcoord &tc)
    {
        u += tc.u;
        v += tc.v;
        return *this;
    }

    Texcoord &operator-=(Texcoord &tc)
    {
        u -= tc.u;
        v -= tc.v;
        return *this;
    }

    Texcoord &operator*=(float m)
    {
        u *= m;
        v *= m;
        return *this;
    }

    Texcoord operator*(float m)
    {
        Texcoord tc;
        tc.u = this->u * m;
        tc.v = this->v * m;
        return tc;
    }

    Texcoord operator/(float m)
    {
        Texcoord tc;
        tc.u = this->u / m;
        tc.v = this->v / m;
        return tc;
    }

    static void Interpolate(Texcoord *tc, Texcoord *tc1, Texcoord *tc2, float t)
    {
        tc->u = Math::Interpolate(tc1->u, tc2->u, t);
        tc->v = Math::Interpolate(tc1->v, tc2->v, t);
    }
} Texcoord;

typedef struct Vertex
{
    Vector pos;
    Vector wPos;
    Vector normal;
    Color color;
    Texcoord tc;
    float rhw;

    // Vector wPos;

    Vertex(){};

    Vertex(Vector p, Color c, float r) : pos(p), color(c), tc({0.0f, 0.0f}), rhw(r) {}

    Vertex(Vector p, Color c, Texcoord t, float r) : pos(p), color(c), tc(t), rhw(r) {}

    void Add(Vertex *v)
    {
        pos += v->pos;
        wPos += v->wPos;
        normal += v->normal;
        color += v->color;
        tc += v->tc;
        rhw += v->rhw;

        // wPos += v->wPos;
    }

    void Sub(Vertex *v)
    {
        pos -= v->pos;
        wPos -= v->wPos;
        normal -= v->normal;
        color -= v->color;
        tc -= v->tc;
        rhw -= v->rhw;

        // wPos -= v->wPos;
    }

    void Div(float d)
    {
        float inv = 1 / d;
        pos *= inv;
        wPos *= inv;
        normal *= inv;
        color *= inv;
        tc *= inv;
        rhw *= inv;

        // wPos *= inv;
    }

    void ApplyRhw()
    {
        rhw = 1.0f / pos.w;

        tc *= rhw;
        color *= rhw;
        wPos *= rhw;
        // normal *= rhw;
    }

    void RevertRhw(Vertex *v)
    {
        v->color = color / rhw;
        v->tc = tc / rhw;
        v->wPos = wPos / rhw;
        v->normal = normal;
    }

    static void Interpolate(Vertex *v, Vertex *v1, Vertex *v2, float t)
    {
        Vector::Interpolate(&v->pos, &v1->pos, &v2->pos, t);
        Vector::Interpolate(&v->wPos, &v1->wPos, &v2->wPos, t);
        Color::Interpolate(&v->color, &v1->color, &v2->color, t);
        Texcoord::Interpolate(&v->tc, &v1->tc, &v2->tc, t);
        v->normal = v1->normal;
        v->rhw = Math::Interpolate(v1->rhw, v2->rhw, t);
    }
} Vertex;

// 光栅线
typedef struct Scanline
{
    // 光栅线的左顶点(屏幕像素坐标)
    Vertex v;
    // 光删线的绘制顶点(位置，颜色，uv)递增步长
    Vertex step;
    // 光删线左顶点x坐标
    int x;
    // 光删线y坐标
    int y;
    // 光删线宽度
    int w;

    Scanline() {}

    Scanline(Vertex _v, Vertex s, int _x, int _y, int _w) : v(_v), step(s), x(_x), y(_y), w(_w) {}

    void Init(Vertex v1, Vertex v2, int _y)
    {
        y = _y;
        x = _Float2Pixel(v1.pos.x);
        if (v1.pos.x == v2.pos.x)
            w = 0;
        else
            w = _Float2Pixel(v2.pos.x) - x;

        v = v1;
        step = v2;
        step.Sub(&v1);
        step.Div(w);
    }
} Scanline;

// 梯形的左右边
// @v: 代表edge上的某一个插值点，用于绘制梯形中的当前光栅线时，该光删线在edge上的端点
// @v1,v2: edge的两个端点
typedef struct TrapEdge
{
    Vertex v, v1, v2;
} TrapEdge;

// 用于光栅化绘制的梯形（1个三角形可以拆成1个或2个梯形）
// @top,bottom: y of the trap's top & bottom
// @left,right: edge of the trap's left & right
typedef struct Trapezoid
{
    float top, bottom;
    TrapEdge left, right;

    void SetEdge(const Vertex *lv1, const Vertex *lv2, const Vertex *rv1, const Vertex *rv2)
    {
        left.v1 = *lv1;
        left.v2 = *lv2;
        right.v1 = *rv1;
        right.v2 = *rv2;
    }

    void SetTopBottom(float t, float b)
    {
        top = t;
        bottom = b;
    }

    // 更新三角形的两条边上的当前插值顶点
    void UpdateEdgeInterpVertex(float y)
    {
        // 计算插值
        float lt = (y - left.v1.pos.y) / (left.v2.pos.y - left.v1.pos.y);
        float rt = (y - right.v1.pos.y) / (right.v2.pos.y - right.v1.pos.y);

        // if (lt > 1.0f || rt > 1.0f || rt < 0.0f || lt < 0.0f)
        // {
        //     int i = 9;
        // }

        // 根据差值计算当前光栅两头的顶点
        Vertex::Interpolate(&left.v, &left.v1, &left.v2, lt);
        Vertex::Interpolate(&right.v, &right.v1, &right.v2, rt);
    }

} Trapezoid;

#endif