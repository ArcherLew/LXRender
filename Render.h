#include <stdlib.h>

#include "Util.h"

// #ifndef GLM_H
// #define GLM_H
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #endif

//=====================================================================
// device
//=====================================================================
typedef struct
{
    int width;             // 窗口宽度
    int height;            // 窗口高度
    IUINT32 **framebuffer; // 像素缓存：framebuffer[y] 代表第 y行
    IUINT32 background;    // 背景色
} device_t;

class Render
{

private:
    device_t device;

public:
    Render() {}

    Render(int width, int height, void *fb)
    {
        device.width = width;
        device.height = height;
        device.background = 0x000000;

        int need = width * height * 4; // 颜色缓冲区字节数，每个像素四种颜色 rgba, 每种颜色 0~255
        char *ptr = (char *)malloc(need);
        device.framebuffer = (IUINT32 **)ptr;

        char *framebuf = (char *)fb;

        for (int j = 0; j < height; j++)
        {
            device.framebuffer[j] = (IUINT32 *)(framebuf + width * 4 * j);
        }
    }

    void Clear()
    {
        int y, x, height = device.height;

        // clear frame-buffer
        for (y = 0; y < device.height; y++)
        {
            IUINT32 *dst = device.framebuffer[y];
            // IUINT32 cc = (height - 1 - y) * 230 / (height - 1); // lx?: 230 ?
            // cc = (cc << 16) | (cc << 8) | cc;
            // if (mode == 0)
            IUINT32 cc = device.background;
            for (x = device.width; x > 0; dst++, x--) // why dst++ x--
                dst[0] = cc;
        }
    }

    void DrawPixel(int x, int y, Color *color)
    {
        IUINT32 cc = Util.Color2Int32(color);
    }
};