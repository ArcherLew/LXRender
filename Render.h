#define STB_IMAGE_IMPLEMENTATION

#include <stdlib.h>

#include "Camera.h"
#include "Common.h"
#include "Math/Math.h"
#include "Scene.h"
#include "stb_image.h"

// #ifndef GLM_H
// #define GLM_H
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #endif

//=====================================================================
// device
//=====================================================================
typedef struct Device
{
    int width;  // 窗口宽度
    int height; // 窗口高度

    UINT32 **framebuffer; // 像素缓存：framebuffer[y] 代表第 y行
    float **zbuffer;      // 深度缓存：zbuffer[y] 为第 y行指针

    int render_state;  // 渲染状态
    UINT32 background; // 背景色
    UINT32 foreground; // 线框颜色

    UINT32 **texture; // 纹理：同样是每行索引
    int texWidth;     // 纹理宽度
    int texHeight;    // 纹理高度
    int nrChannels;   // 纹理通道
    float maxU;       // 纹理最大宽度：tex_width - 1
    float maxV;       // 纹理最大高度：tex_height - 1

    Device() {}
    Device(int _width, int _height, void *fb)
    {
        // init device
        width = _width;
        height = _height;
        background = 0x000000;

        // framebuffer: width * height * int : 颜色缓冲区字节数，每个像素四种颜色 rgba, 每种颜色 0~255
        // zbuffer : width * height * float : 深度缓冲
        // todo: framebuffer was memset by fb
        int bufSize = sizeof(void *) * (height * 2) + width * height * 8;

        // ptr 类型是 char * , 所以 += 的长度增量单位就是 char
        char *ptr = (char *)malloc(bufSize);
        framebuffer = (UINT32 **)ptr;
        ptr += sizeof(void *) * height;

        zbuffer = (float **)ptr;
        ptr += sizeof(void *) * height;

        // framebuffer, zbuffer 的行指针
        char *framebuf, *zbuf;
        framebuf = (char *)ptr;
        zbuf = (char *)ptr + width * height * 4;

        if (fb != NULL)
            framebuf = (char *)fb;

        for (int j = 0; j < height; j++)
        {
            framebuffer[j] = (UINT32 *)(framebuf + width * 4 * j);
            zbuffer[j] = (float *)(zbuf + width * 4 * j);
        }
        ptr += width * height * 8;

        // // todo: ???
        // texture = (UINT32 **)ptr;
        // ptr += sizeof(void *) * 1024;
        texture = NULL;
    }

    void LoadTexture(const char *texPath)
    {
        // stbi_set_flip_vertically_on_load(true); // d3d 应该不需要反转
        unsigned char *texData = stbi_load(texPath, &texWidth, &texHeight, &nrChannels, 0);
        if (NULL != texData)
        {
            std::cout << "load texture success : " << texPath << std::endl;
            std::cout << "tex size : " << width << " x " << height << std::endl;
            std::cout << "channels : " << nrChannels << std::endl;
            std::cout << "data size : " << sizeof(texData) << std::endl;

            // 定位 texData
            char *ptrData = (char *)texData;

            // for (int i = 0; i < 128; i++)
            // {
            //     ptrData++;
            //     std::cout
            //         << i << " -> " << *ptrData << std::endl;
            // }

            // 定位 texture v
            char *ptr = (char *)malloc(texWidth * texHeight * 4 + sizeof(void *) * texHeight);
            texture = (UINT32 **)ptr;
            ptr += sizeof(void *) * texHeight;
            UINT32 c;

            for (int j = 0; j < texHeight; j++)
            {
                texture[j] = (UINT32 *)(ptr + texWidth * 4 * j);

                for (int i = 0; i < texWidth; i++)
                {
                    if (nrChannels == 3)
                    {
                        UCHAR c0 = ptrData[0];
                        UCHAR c1 = ptrData[1];
                        UCHAR c2 = ptrData[2];
                        c = 0xff << 24 | ptrData[0] << 16 | ptrData[1] << 8 | ptrData[2];
                    }
                    else if (nrChannels == 4)
                    {
                        c = ptrData[0] << 24 | ptrData[3] << 16 | ptrData[2] << 8 | ptrData[1];
                    }
                    if (c < 0)
                        texture[j][i] = 0;
                    texture[j][i] = c;

                    ptrData += nrChannels;
                }
            }

            if (nrChannels < 3 || nrChannels > 4)
                std::cout << "NrChannels cannot be decode!" << std::endl;

            stbi_image_free(texData);
        }
        else
        {
            if (NULL == texData)
                std::cout << "Failed to load texture!" << std::endl;
        }
    }
} Device;

class Render
{
public:
    Device device;
    Camera camera;
    Matrix matMVP;

    Render() {}

    Render(int width, int height, void *fb)
    {
        device = Device(width, height, fb);

        // init camera
        Vector camPos = {0.0f, 0.0f, -1.0f, 1.0f};
        float fov = Math::PI * 0.5f;
        camera.Init(camPos, 0.0f, 0.0f, fov, (float)width, (float)height, 1.0f, 500.0f);
    }

    void Clear()
    {
        int y, x, height = device.height;

        // clear frame-buffer
        for (y = 0; y < device.height; y++)
        {
            UINT32 *dst = device.framebuffer[y];
            // UINT32 cc = (height - 1 - y) * 230 / (height - 1); // lx?: 230 ?
            // cc = (cc << 16) | (cc << 8) | cc;
            // if (mode == 0)
            UINT32 cc = device.background;
            for (x = device.width; x > 0; dst++, x--) // why dst++ x--
                dst[0] = cc;
        }
        for (y = 0; y < device.height; y++)
        {
            float *dst = device.zbuffer[y];
            for (x = device.width; x > 0; dst++, x--)
                dst[0] = 0.0f;
        }
    }

    UINT32 ReadTexture(float u, float v)
    {
        // todo: look if need -1
        int x = (device.texWidth - 1) * u;
        int y = (device.texHeight - 1) * v;
        int cc = device.texture[y][x];
        return cc;
    }

    // 绘制像素点
    void DrawPixel(int x, int y, UINT32 c)
    {
        device.framebuffer[y][x] = c;
    }

    // 绘制线条
    void DrawLine(int x1, int y1, int x2, int y2, UINT32 c)
    {
        int x, y, rem = 0;

        if (x1 == x2 && y1 == y2)
            DrawPixel(x1, y1, c);
        else if (x1 == x2 && y1 != y2)
        {
            int inc = (y1 <= y2) ? 1 : -1;
            for (y = y1; y != y2; y += inc)
                DrawPixel(x1, y, c);
        }
        else if (x1 != x2 && y1 == y2)
        {
            int inc = (x1 <= x2) ? 1 : -1;
            for (x = x1; x != x2; x += inc)
                DrawPixel(x, y1, c);
        }
        else if (x1 != x2 && y1 != y2)
        {
            // 算法思想参照此文：https://www.cnblogs.com/dongry/p/10716608.html
            int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
            int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
            if (dx >= dy)
            {
                // 转换到 x1 < x2 的情形来处理
                if (x2 < x1)
                    x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
                for (x = x1, y = y1; x <= x2; x++)
                {
                    DrawPixel(x, y, c);
                    // bresenham 算法的优化，化小数位整数来操作：
                    // 假设每格增加的高度为_dy, 则 dx/dy 等于 1/_dy，
                    // 所以可以用 rem+n*dy>dx 来判定 x+n 后，y 是否+1,rem 是之前剩下的 x
                    rem += dy;
                    if (rem >= dx)
                    {
                        rem -= dx;
                        y += (y2 >= y1) ? 1 : -1;
                        DrawPixel(x, y, c);
                    }
                    DrawPixel(x2, y2, c);
                }
            }
            else
            {
                if (y2 < y1)
                    x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
                for (x = x1, y = y1; y <= y2; y++)
                {
                    DrawPixel(x, y, c);
                    rem += dx;
                    if (rem >= dy)
                    {
                        rem -= dy;
                        x += (x2 >= x1) ? 1 : -1;
                        DrawPixel(x, y, c);
                    }
                }
                DrawPixel(x2, y2, c);
            }
        }
    }

    // 绘制光栅线
    void DrawScanLine(Scanline *scanline)
    {
        UINT32 *framebuffer = device.framebuffer[scanline->y];
        float *zbuffer = device.zbuffer[scanline->y];
        int x = scanline->x;
        int w = scanline->w;
        int width = device.width;

        Vertex v;

        for (int i = 0; x < width; x++, i++)
        {
            if (x >= 0 && i < w)
            {
                float rhw = scanline->v.rhw;
                float z = zbuffer[x];
                if (rhw >= z)
                {
                    float w = 1.0f / rhw;
                    zbuffer[x] = rhw;

                    scanline->v.RevertRhw(&v);

                    UINT32 cc = ReadTexture(v.tc.u, v.tc.v);
                    // UINT32 cc = v.color.ToInt32();
                    framebuffer[x] = cc;
                }
            }
            scanline->v.Add(&scanline->step);
        }
    }

    // 绘制顶底水平三角形(在屏幕空间)
    void DrawTriangle2D(Trapezoid *trap)
    {
        Scanline scanline;
        int top = _Float2Pixel(trap->top);
        int bottom = _Float2Pixel(trap->bottom);

        // 逐行光栅化
        for (int j = top; j < bottom; j++)
        {
            if (j >= 0 && j < device.height)
            {
                trap->UpdateEdgeInterpVertex((float)j + 0.5f);
                scanline.Init(trap->left.v, trap->right.v, j);
                DrawScanLine(&scanline);
            }
        }
    }

    // 绘制顶底非水平三角形(在屏幕空间)
    // @v1,v2,v3: 屏幕像素坐标
    void DrawTriangle2D(const Vertex *v1, const Vertex *v2, const Vertex *v3)
    {
        // TODO: Wire frame mode
        Trapezoid traps[2];
        int n;

        n = SplitTriangle(traps, v1, v2, v3);

        // printf("%d", n);
        // return;

        for (int i = 0; i < n; i++)
            DrawTriangle2D(&traps[i]);
    }

    // 根据三角形生成 0-2 个梯形，并且返回合法梯形的数量
    // @return: 拆分的梯形数量
    int SplitTriangle(Trapezoid *trap, const Vertex *p1,
                      const Vertex *p2, const Vertex *p3)
    {
        const Vertex *p;
        float k, x;

        // 统一为 p3 > p2 > p1 的情况
        if (p1->pos.y > p2->pos.y)
            p = p1, p1 = p2, p2 = p;
        if (p1->pos.y > p3->pos.y)
            p = p1, p1 = p3, p3 = p;
        if (p2->pos.y > p3->pos.y)
            p = p2, p2 = p3, p3 = p;

        // 排除三点共线
        if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y)
            return 0;
        if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x)
            return 0;

        // 倒三角形
        if (p1->pos.y == p2->pos.y)
        {
            if (p1->pos.x > p2->pos.x)
                p = p1, p1 = p2, p2 = p;
            trap[0].SetTopBottom(p1->pos.y, p3->pos.y);
            trap[0].SetEdge(p1, p3, p2, p3);
            return (trap[0].top < trap[0].bottom) ? 1 : 0;
        }

        // 正三角形
        if (p2->pos.y == p3->pos.y)
        {
            if (p2->pos.x > p3->pos.x)
                p = p2, p2 = p3, p3 = p;
            trap[0].SetTopBottom(p1->pos.y, p3->pos.y);
            trap[0].SetEdge(p1, p2, p1, p3);
            return (trap[0].top < trap[0].bottom) ? 1 : 0;
        }

        // 正三角形+倒三角形
        trap[0].SetTopBottom(p1->pos.y, p2->pos.y);
        trap[1].SetTopBottom(p2->pos.y, p3->pos.y);

        k = (p3->pos.y - p1->pos.y) / (p2->pos.y - p1->pos.y);
        x = p1->pos.x + (p2->pos.x - p1->pos.x) * k;

        if (x <= p3->pos.x)
        {
            trap[0].SetEdge(p1, p2, p1, p3);
            trap[1].SetEdge(p2, p3, p1, p3);
        }
        else
        {
            trap[0].SetEdge(p1, p3, p1, p2);
            trap[1].SetEdge(p1, p3, p2, p3);
        }

        return 2;
    }

    void DrawTriangle(Vertex *v1, Vertex *v2, Vertex *v3, Matrix matMVP)
    {
        Point projPos1 = v1->pos * matMVP;
        Point projPos2 = v2->pos * matMVP;
        Point projPos3 = v3->pos * matMVP;

        // todo: check cvv

        v1->pos = camera.GetScreenPos(&projPos1);
        v2->pos = camera.GetScreenPos(&projPos2);
        v3->pos = camera.GetScreenPos(&projPos3);

        // 世界/观察空间中呈线性分布的位置，进行投影变换后，会变成非线性分布;
        //
        // 由于 w(=-z) 表示了到摄像机的距离
        // 可以假设在观察空间中，在z值不同(垂直于视线)的平面上，有两条相同长度的线段，
        //    两条线段的两个顶点的uv/color差都相等，显然pos差(长度))也相等,
        //    那么这两条线段投影到屏幕空间的长度应该是和z值成反比的,
        //    所以两条线段两个顶点之间的uv/color/pos差在屏幕空间里的投影做插值时候，
        //    uv/color/pos对于屏幕空间距离的微分值也和z成反比
        // 那么假设一个线段的两个顶点z值不同，则该线段投影并变换到到屏幕空间中，
        //    其uv/color/pos对于屏幕空间距离的微分处处不相等·且和每处的z成反比
        // 所以这里先把 uv/color/pos 这些在世界/观察空间中线性分布的值除w(即-z)，
        //    然后再在屏幕空间里做插值，插值操作完成后再乘回w(即-z)，还原回原本正确的值

        v1->ApplyRhw();
        v2->ApplyRhw();
        v3->ApplyRhw();

        DrawTriangle2D(v1, v2, v3);
    }

    void DrawObject(Object *obj)
    {
        Matrix matMVP = obj->transform.matModel * camera.matViewProj;
        // Matrix::Mul(&matMVP, &obj->transform.matModel, &camera.matViewProj);
        int t1, t2, t3;

        for (int t = 0; t < obj->triCount; t++)
        {
            t1 = obj->triangles[t * 3];
            t2 = obj->triangles[t * 3 + 1];
            t3 = obj->triangles[t * 3 + 2];

            Vertex v1 = obj->mesh[t1];
            Vertex v2 = obj->mesh[t2];
            Vertex v3 = obj->mesh[t3];

            DrawTriangle(&v1, &v2, &v3, matMVP);
        }
    }
};
