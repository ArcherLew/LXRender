#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

typedef struct Matrix
{
    // 因为矩阵乘法习惯写成从右往左，如 TRS
    // 则 T×R 写成行列式的形式为:
    //                   R[1]x R[2]x R[3]x
    //                   R[1]y R[2]y R[3]y
    //                   R[1]z R[2]z R[3]z
    // T[1]x T[2]x T[3]x
    // T[1]y T[2]y T[3]y
    // T[1]z T[2]z T[3]z
    // 所以用 i 表示列, j 表示行; 一列即代表矩阵的一个基
    float data[4][4];

    Matrix()
    {
    }

    // identity
    void SetIdentity()
    {
        data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0f;
        data[0][1] = data[0][2] = data[0][3] = 0.0f;
        data[1][0] = data[1][2] = data[1][3] = 0.0f;
        data[2][0] = data[2][1] = data[2][3] = 0.0f;
        data[3][0] = data[3][1] = data[3][2] = 0.0f;
    }

    // identity
    void SetZero()
    {
        data[0][1] = data[0][1] = data[0][2] = data[0][3] = 0.0f;
        data[1][1] = data[1][1] = data[1][2] = data[1][3] = 0.0f;
        data[2][1] = data[2][1] = data[2][2] = data[2][3] = 0.0f;
        data[3][1] = data[3][1] = data[3][2] = data[3][3] = 0.0f;
    }

    // m = this * m2 <=> m = m2 × this，对 this 做 m2 的变换
    // 之所以要这么定义是因为乘法操作符优先级是从左往右
    Matrix operator*(Matrix &m2)
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
    static void Mul(Matrix *m, Matrix *m1, Matrix *m2)
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
    static void Add(Matrix *m, Matrix *m1, Matrix *m2)
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m->data[i][j] = m1->data[i][j] + m2->data[i][j];
    }

    // m = m1 - m2
    static void Sub(Matrix *m, Matrix *m1, Matrix *m2)
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m->data[i][j] = m1->data[i][j] - m2->data[i][j];
    }

    void Print()
    {
        std::cout << data[0][0] << data[1][0] << data[2][0] << data[3][0] << std::endl;
        std::cout << data[0][1] << data[1][1] << data[2][1] << data[3][1] << std::endl;
        std::cout << data[0][2] << data[1][2] << data[2][2] << data[3][2] << std::endl;
        std::cout << data[0][3] << data[1][3] << data[2][3] << data[3][3] << std::endl;
    }
} Matrix;

#endif