#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include <iostream>

class Transform
{
public:
    Matrix matModel, matTranslate, matRotation, matScale;
    Matrix matCurRot;

    bool needUpdateTRS = false;

    Transform()
    {
        matModel.SetIdentity();
        matTranslate.SetIdentity();
        matRotation.SetIdentity();
        matScale.SetIdentity();
    }

    // translate
    void SetTranslate(float x, float y, float z, bool updateTRS = true)
    {
        matTranslate.SetIdentity();
        matTranslate.data[3][0] = x;
        matTranslate.data[3][1] = y;
        matTranslate.data[3][2] = z;
        needUpdateTRS = true;
    }

    void SetScale(float x, float y, float z, bool updateTRS = true)
    {
        matScale.SetIdentity();
        matScale.data[0][0] = x;
        matScale.data[1][1] = y;
        matScale.data[2][2] = z;

        needUpdateTRS = true;
    }

    // rotate
    // 四元数构造
    void SetRotate(float x, float y, float z, float theta, bool updateTRS = true)
    {
        Matrix::GetRotateMatrix(&matRotation, x, y, z, theta);
        needUpdateTRS = true;
    }

    // 基于当前状态，绕世界空间某个轴旋转
    void DoRotateX(float dx)
    {
        Matrix::GetRotateMatrix(&matCurRot, 1.0f, 0.0f, 0.0f, dx);
        matRotation = matRotation * matCurRot;
        needUpdateTRS = true;
    }

    void DoRotateY(float dy)
    {
        Matrix::GetRotateMatrix(&matCurRot, 0.0f, 1.0f, 0.0f, dy);
        matRotation = matRotation * matCurRot;
        needUpdateTRS = true;
    }

    void DoRotateZ(float dz)
    {
        Matrix::GetRotateMatrix(&matCurRot, 0.0f, 0.0f, 1.0f, dz);
        matRotation = matRotation * matCurRot;
        needUpdateTRS = true;
    }

    void SetTRS()
    {
        if (needUpdateTRS)
            matModel = matScale * matRotation * matTranslate;
    }
};

#endif