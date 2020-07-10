#include <iostream>

#include "Matrix.h"
#include "Vector.h"

// identity
void Matrix::SetIdentity()
{
    data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0f;
    data[0][1] = data[0][2] = data[0][3] = 0.0f;
    data[1][0] = data[1][2] = data[1][3] = 0.0f;
    data[2][0] = data[2][1] = data[2][3] = 0.0f;
    data[3][0] = data[3][1] = data[3][2] = 0.0f;
}

// identity
void Matrix::SetZero()
{
    data[0][1] = data[0][1] = data[0][2] = data[0][3] = 0.0f;
    data[1][1] = data[1][1] = data[1][2] = data[1][3] = 0.0f;
    data[2][1] = data[2][1] = data[2][2] = data[2][3] = 0.0f;
    data[3][1] = data[3][1] = data[3][2] = data[3][3] = 0.0f;
}

// m = this * m2 <=> m = m2 × this，对 this 做 m2 的变换
// 之所以要这么定义是因为乘法操作符优先级是从左往右
Matrix Matrix::operator*(Matrix &m2)
{
    Matrix m;
    // 按照习惯: m的i列j行 = m2的第j行 × this的第i列
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            m.data[i][j] = 0;
            for (int k = 0; k < 4; k++)
                m.data[i][j] += this->data[i][k] * m2.data[k][j];
        }
    return m;
}

// m = m2 × m1，对 m1 做 m2 的变换
void Matrix::Mul(Matrix *m, Matrix *m1, Matrix *m2)
{
    // 按照习惯: m的i列j行 = m2的第j行 × m1的第i列
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            m->data[i][j] = 0;
            for (int k = 0; k < 4; k++)
                m->data[i][j] += m1->data[i][k] * m2->data[k][j];
        }
}
// m = m1 + m2
void Matrix::Add(Matrix *m, Matrix *m1, Matrix *m2)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m->data[i][j] = m1->data[i][j] + m2->data[i][j];
}

// m = m1 - m2
void Matrix::Sub(Matrix *m, Matrix *m1, Matrix *m2)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m->data[i][j] = m1->data[i][j] - m2->data[i][j];
}

void Matrix::GetRotateMatrix(Matrix *m, float x, float y, float z, float theta)
{
    float qsin = (float)sin(theta * 0.5f);
    float qcos = (float)cos(theta * 0.5f);
    Vector vec = {x, y, z, 1.0f};
    float w = qcos;
    vec.Normalize();
    x = vec.x * qsin;
    y = vec.y * qsin;
    z = vec.z * qsin;
    m->data[0][0] = 1 - 2 * y * y - 2 * z * z;
    m->data[0][1] = 2 * x * y - 2 * w * z;
    m->data[0][2] = 2 * x * z + 2 * w * y;
    m->data[1][0] = 2 * x * y + 2 * w * z;
    m->data[1][1] = 1 - 2 * x * x - 2 * z * z;
    m->data[1][2] = 2 * y * z - 2 * w * x;
    m->data[2][0] = 2 * x * z - 2 * w * y;
    m->data[2][1] = 2 * y * z + 2 * w * x;
    m->data[2][2] = 1 - 2 * x * x - 2 * y * y;
    m->data[3][0] = m->data[3][1] = m->data[3][2] = 0.0f;
    m->data[0][3] = m->data[1][3] = m->data[2][3] = 0.0f;
    m->data[3][3] = 1.0f;
}

void Matrix::Print()
{
    std::cout << data[0][0] << data[1][0] << data[2][0] << data[3][0] << std::endl;
    std::cout << data[0][1] << data[1][1] << data[2][1] << data[3][1] << std::endl;
    std::cout << data[0][2] << data[1][2] << data[2][2] << data[3][2] << std::endl;
    std::cout << data[0][3] << data[1][3] << data[2][3] << data[3][3] << std::endl;
}