#ifndef Object_H
#define Object_H

#include "Common.h"
#include "Transform.h"

class Object
{
public:
    float *data;
    int dataLength;
    int verCount;
    int *triangles;
    int triCount;
    Transform transform;
    Vertex *mesh;

    Object() {}

    void Init(
        float *_data,
        int _dataLength,
        int _verCount,
        int *_triangles,
        int _triCount,
        Transform *_transform)
    {
        data = _data;
        dataLength = _dataLength;
        verCount = _verCount;
        triangles = _triangles;
        triCount = _triCount;
        transform = *_transform;
        mesh = new Vertex[_verCount];
        std::cout << "object.transform add = " << &transform << std ::endl;

        int step = dataLength / verCount;
        int p;
        for (int i = 0; i < verCount; i++)
        {
            p = i * step;
            Vector v = {data[p], data[p + 1], data[p + 2], 1.0f}; // w 后面再算
            Color c = {data[p + 3], data[p + 4], data[p + 5], 1.0f};
            Texcoord tc = {data[p + 6], data[p + 7]};
            mesh[i] = Vertex(v, c, tc, 1.0f);
        }

        // for (int i = 0; i < verCount; i++)
        // {
        //     Vertex v = mesh[i];
        //     v.pos.Print();
        // }
    }
};

#endif