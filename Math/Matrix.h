// #ifndef MATRIX_H
// #define MATRIX_H
#pragma once

// #include "Math.h"

class Matrix
{
public:
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

    void SetIdentity();

    void SetZero();

    // m = this * m2 <=> m = m2 × this，对 this 做 m2 的变换
    // 之所以要这么定义是因为乘法操作符优先级是从左往右
    Matrix operator*(Matrix &m2);

    // m = m2 × m1，对 m1 做 m2 的变换
    static void Mul(Matrix *m, Matrix *m1, Matrix *m2);

    // m = m1 + m2
    static void Add(Matrix *m, Matrix *m1, Matrix *m2);

    // m = m1 - m2
    static void Sub(Matrix *m, Matrix *m1, Matrix *m2);

    static void GetRotateMatrix(Matrix *m, float x, float y, float z, float theta);

    void Print();
};
