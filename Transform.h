#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <iostream>
#include "Math/Math.h"

class Transform
{
public:
    Matrix matModel, matTranslate, matRotation, matScale;

    Transform()
    {
        matModel.SetIdentity(true);
        matTranslate.SetIdentity();
        matRotation.SetIdentity();
        matScale.SetIdentity();
    }

    // translate
    void SetTranslate(float x, float y, float z, bool updateTRS = true)
    {
        matTranslate.SetIdentity();
        matTranslate.data[0][3] = x;
        matTranslate.data[1][3] = y;
        matTranslate.data[2][3] = z;
        if (updateTRS)
            SetTRS();
    }

    void SetScale(float x, float y, float z, bool updateTRS = true)
    {
        matScale.SetIdentity();
        matScale.data[0][0] = x;
        matScale.data[1][1] = y;
        matScale.data[2][2] = z;
        if (updateTRS)
            SetTRS();
    }

    // rotate
    // 四元数构造
    void SetRotate(float x, float y, float z, float theta, bool updateTRS = true)
    {
        float qsin = (float)sin(theta * 0.5f);
        float qcos = (float)cos(theta * 0.5f);
        Vector vec = {x, y, z, 1.0f};
        float w = qcos;
        vec.Normalize();
        x = vec.x * qsin;
        y = vec.y * qsin;
        z = vec.z * qsin;
        matRotation.data[0][0] = 1 - 2 * y * y - 2 * z * z;
        matRotation.data[0][1] = 2 * x * y - 2 * w * z;
        matRotation.data[0][2] = 2 * x * z + 2 * w * y;
        matRotation.data[1][0] = 2 * x * y + 2 * w * z;
        matRotation.data[1][1] = 1 - 2 * x * x - 2 * z * z;
        matRotation.data[1][2] = 2 * y * z - 2 * w * x;
        matRotation.data[2][0] = 2 * x * z - 2 * w * y;
        matRotation.data[2][1] = 2 * y * z + 2 * w * x;
        matRotation.data[2][2] = 1 - 2 * x * x - 2 * y * y;
        matRotation.data[3][0] = matRotation.data[3][1] = matRotation.data[3][2] = 0.0f;
        matRotation.data[0][3] = matRotation.data[1][3] = matRotation.data[2][3] = 0.0f;
        matRotation.data[3][3] = 1.0f;

        if (updateTRS)
            SetTRS();
    }

    void SetTRS()
    {
        matModel = matScale * matRotation * matTranslate;
    }
};

#endif