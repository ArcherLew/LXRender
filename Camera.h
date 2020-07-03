#ifndef CAMERA_H
#define CAMERA_H

#include "Math/Math.h"

class Camera
{
public:
    Matrix matViewProj, matView, matProject;
    float width, height, aspect;

    Camera()
    {
        matViewProj.SetIdentity();
        matView.SetIdentity();
        matProject.SetIdentity();
    }

    void Init(
        Vector *camPos,
        Vector *lookAt,
        Vector *up,
        float fov,
        float w,
        float h,
        float zn,
        float zf)
    {
        width = w;
        height = h;
        aspect = w / h;
        SetView(camPos, lookAt, up, false);
        SetProjection(fov, aspect, zn, zf);
    }
    // view
    // world -> view
    // = [ view -> world ]^-1
    // =
    // | xAxis 0 |   | 1 0 0 -camPos.x |   | xAxis -camPos*xAxis |
    // | yAxis 0 | × | 0 1 0 -camPos.y | = | yAxis -camPos*yAxis |
    // | zAxis 0 |   | 0 0 1 -camPos.z |   | zAxis -camPos*zAxis |
    // | 0 0 0 1 |   | 0 0 0     1     |   | 0 0 0        1      |
    void SetView(
        const Vector *camPos,
        const Vector *lookAt,
        const Vector *up,
        bool updateVP = true)
    {
        Vector xAxis, yAxis, zAxis;
        zAxis = *lookAt;
        zAxis.Normalize();
        Vector::Cross(&xAxis, up, &zAxis);
        xAxis.Normalize();
        Vector::Cross(&yAxis, &zAxis, &xAxis);

        matView.data[0][0] = xAxis.x;
        matView.data[1][0] = xAxis.y;
        matView.data[2][0] = xAxis.z;
        matView.data[3][0] = -Vector::Dot(camPos, &xAxis);

        matView.data[0][1] = yAxis.x;
        matView.data[1][1] = yAxis.y;
        matView.data[2][1] = yAxis.z;
        matView.data[3][1] = -Vector::Dot(camPos, &yAxis);

        matView.data[0][2] = zAxis.x;
        matView.data[1][2] = zAxis.y;
        matView.data[2][2] = zAxis.z;
        matView.data[3][2] = -Vector::Dot(camPos, &zAxis);

        matView.data[0][3] = 0.0f;
        matView.data[1][3] = 0.0f;
        matView.data[2][3] = 0.0f;
        matView.data[3][3] = 1.0f;

        if (updateVP)
            Matrix::Mul(&matViewProj, &matView, &matProject);
    }

    // perspective fov
    void SetProjection(
        float fov,
        float aspect,
        float zn,
        float zf,
        bool updateVP = true)
    {
        float fax = 1.0f / (float)tan(fov * 0.5f);
        matProject.SetZero();
        matProject.data[0][0] = (float)(fax / aspect);
        matProject.data[1][1] = (float)(fax);
        matProject.data[2][2] = zf / (zf - zn);
        matProject.data[3][2] = -zn * zf / (zf - zn);
        matProject.data[2][3] = 1;

        if (updateVP)
            Matrix::Mul(&matViewProj, &matView, &matProject);
    }

    // proj -> ndc -> screen
    Vector GetScreenPos(const Vector *projPos)
    {
        Vector scrPos;
        float rhw = 1.0f / projPos->w;
        scrPos.x = (projPos->x * rhw + 1.0f) * width * 0.5f;
        scrPos.y = (1.0f - projPos->y * rhw) * height * 0.5f;
        scrPos.z = projPos->z * rhw;
        scrPos.w = projPos->w;
        return scrPos;
    }
};

#endif