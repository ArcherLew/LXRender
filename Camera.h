#ifndef CAMERA_H
#define CAMERA_H

#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

class Camera
{
public:
    Matrix matViewProj, matView, matProject;
    Vector position, front;
    float yaw, pitch;
    float width, height, aspect, fov, zNear, zFar;

    bool needUpdateView = false;
    bool needUpdateProjection = false;

    Vector up = {0, 1, 0, 1};
    const float speed = 0.1f;
    const float sensitivity = 1.0f;

    Camera()
    {
        matViewProj.SetIdentity();
        matView.SetIdentity();
        matProject.SetIdentity();
    }

    void Init(Vector _position, float _yaw, float _pitch, float _fov, float _width, float _height, float _near, float _far)
    {
        position = _position;
        yaw = _yaw;
        pitch = _pitch;
        width = _width;
        height = _height;
        aspect = width / height;
        fov = _fov;
        zNear = _near;
        zFar = _far;

        Update(true);
    }

    Vector GetFront()
    {
        front.x = sin(Math::Deg2Rad(yaw)) * cos(Math::Deg2Rad(pitch));
        front.y = sin(Math::Deg2Rad(pitch));
        front.z = cos(Math::Deg2Rad(yaw)) * cos(Math::Deg2Rad(pitch));
        front.w = 1.0f;
        front.Normalize();
        return front;
    }

    // view
    // world -> view
    // = [ view -> world ]^-1
    // =
    // | xAxis 0 |   | 1 0 0 -camPos.x |   | xAxis -camPos*xAxis |
    // | yAxis 0 | × | 0 1 0 -camPos.y | = | yAxis -camPos*yAxis |
    // | zAxis 0 |   | 0 0 1 -camPos.z |   | zAxis -camPos*zAxis |
    // | 0 0 0 1 |   | 0 0 0     1     |   | 0 0 0        1      |
    void UpdateView(bool forceUpdate = false)
    {
        if (!forceUpdate && !needUpdateView)
            return;
        else
            needUpdateView = false;

        Vector xAxis, yAxis, zAxis;
        zAxis = GetFront();
        zAxis.Normalize();
        Vector::Cross(&xAxis, &up, &zAxis);
        xAxis.Normalize();
        Vector::Cross(&yAxis, &zAxis, &xAxis);

        matView.data[0][0] = xAxis.x;
        matView.data[1][0] = xAxis.y;
        matView.data[2][0] = xAxis.z;
        matView.data[3][0] = -Vector::Dot(&position, &xAxis);

        matView.data[0][1] = yAxis.x;
        matView.data[1][1] = yAxis.y;
        matView.data[2][1] = yAxis.z;
        matView.data[3][1] = -Vector::Dot(&position, &yAxis);

        matView.data[0][2] = zAxis.x;
        matView.data[1][2] = zAxis.y;
        matView.data[2][2] = zAxis.z;
        matView.data[3][2] = -Vector::Dot(&position, &zAxis);

        matView.data[0][3] = 0.0f;
        matView.data[1][3] = 0.0f;
        matView.data[2][3] = 0.0f;
        matView.data[3][3] = 1.0f;

        Matrix::Mul(&matViewProj, &matView, &matProject);
    }

    // perspective fov
    void UpdateProjection(bool forceUpdate = false)
    {
        if (!forceUpdate && !needUpdateProjection)
            return;
        else
            needUpdateProjection = false;

        float fax = 1.0f / (float)tan(fov * 0.5f);
        matProject.SetZero();
        matProject.data[0][0] = (float)(fax / aspect);
        matProject.data[1][1] = (float)(fax);
        matProject.data[2][2] = zFar / (zFar - zNear);
        matProject.data[3][2] = -zNear * zFar / (zFar - zNear);
        matProject.data[2][3] = 1;

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

    void MoveYawPitch(float xOffset, float yOffset)
    {
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        needUpdateView = true;
    }

    void MoveX(float dx)
    {
        position.x += dx;
        needUpdateView = true;
    }

    void MoveY(float dy)
    {
        position.y += dy;
        needUpdateView = true;
    }

    void MoveZ(float dz)
    {
        position.z += dz;
        needUpdateView = true;
    }

    void Update(bool forceUpdate = false)
    {
        UpdateView(forceUpdate);
        UpdateProjection(forceUpdate);
    }
};

#endif