
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

#include <iostream>

#include "Render.h"

//=====================================================================
// Win32 窗口及图形绘制：为 device 提供一个 DibSection 的 FB
//=====================================================================
int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];             // 当前键盘按下状态
static HWND screen_handle = NULL; // 主窗口 HWND
static HDC screen_dc = NULL;      // 配套的 HDC
static HBITMAP screen_hb = NULL;  // DIB
static HBITMAP screen_ob = NULL;  // 老的 BITMAP
unsigned char *screen_fb = NULL;  // frame buffer
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);
int screen_close(void);
void screen_dispatch(void);
void screen_update(void);

// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

bool firstMouse = true;
int curMouseX, curMouseY, preMouseX, preMouseY;

// 查找相关资料了解到 #pragma comment (lib, xxxx) 这种是VS的用法。
// gcc是不支持这种用法的，gcc是用参数 -lxxx 来引用lib库的，于是在命令行中执行如下语句
// #ifdef _MSC_VER
// #pragma comment(lib, "gdi32.lib")
// #pragma comment(lib, "user32.lib")
// #endif

int screen_init(int w, int h, const TCHAR *title)
{
    WNDCLASS wc = {CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
                   NULL, NULL, NULL, NULL, _T("SCREEN3.1415926")};
    BITMAPINFO bi = {{sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
                      w * h * 4, 0, 0, 0, 0}};
    RECT rect = {0, 0, w, h};
    int wx, wy, sx, sy;
    LPVOID ptr;
    HDC hDC;

    screen_close();

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc))
        return -1;

    screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,
                                 WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    if (screen_handle == NULL)
        return -2;

    screen_exit = 0;
    hDC = GetDC(screen_handle);
    screen_dc = CreateCompatibleDC(hDC);
    ReleaseDC(screen_handle, hDC);

    screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
    if (screen_hb == NULL)
        return -3;

    screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
    screen_fb = (unsigned char *)ptr;
    screen_w = w;
    screen_h = h;
    screen_pitch = w * 4;

    AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
    wx = rect.right - rect.left;
    wy = rect.bottom - rect.top;
    sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
    sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
    if (sy < 0)
        sy = 0;
    SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
    SetForegroundWindow(screen_handle);

    ShowWindow(screen_handle, SW_NORMAL);
    screen_dispatch();

    memset(screen_keys, 0, sizeof(int) * 512);
    memset(screen_fb, 0, w * h * 4);

    return 0;
}

int screen_close(void)
{
    if (screen_dc)
    {
        if (screen_ob)
        {
            SelectObject(screen_dc, screen_ob);
            screen_ob = NULL;
        }
        DeleteDC(screen_dc);
        screen_dc = NULL;
    }
    if (screen_hb)
    {
        DeleteObject(screen_hb);
        screen_hb = NULL;
    }
    if (screen_handle)
    {
        CloseWindow(screen_handle);
        screen_handle = NULL;
    }
    return 0;
}

static LRESULT screen_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        screen_exit = 1;
        break;
    case WM_KEYDOWN:
        screen_keys[wParam & 511] = 1;
        break;
    case WM_KEYUP:
        screen_keys[wParam & 511] = 0;
        break;
    case WM_MOUSEMOVE:
        curMouseX = LOWORD(lParam);
        curMouseY = HIWORD(lParam);
        // std::cout
        //     << "wParam = " << wParam
        //     << ",  x = " << LOWORD(lParam)
        //     << ",  y = " << HIWORD(lParam)
        //     << std::endl;
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void screen_dispatch(void)
{
    MSG msg;
    while (1)
    {
        if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            break;
        if (!GetMessage(&msg, NULL, 0, 0))
            break;
        DispatchMessage(&msg);
    }
}

void screen_update(void)
{
    HDC hDC = GetDC(screen_handle);
    BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
    ReleaseDC(screen_handle, hDC);
    screen_dispatch();
}

//=====================================================================
// main
//=====================================================================

static DWORD preTickTime = 0;
static DWORD curTickTime = 0;
static DWORD deltaTime = 0.0f;
static DWORD frameCount = 0;

void UpdateTime()
{
    if (preTickTime == 0)
    {
        preTickTime = GetTickCount();
        return;
    }
    frameCount++;
    curTickTime = GetTickCount();
    deltaTime = curTickTime - preTickTime;
    preTickTime = curTickTime;

    // printf("Tick: %u  ", curTickTime);
}

static float avgDeltaTime = 0.0f;
static int sampleCount = 100;
static float dftFrameWeight = 1.0f / (float)sampleCount;
static int fpsPrintInterval = 0.0f;
// @deltaTime: ms
float GetFPS(float deltaTime)
{
    if (frameCount == 0)
        return 0.0f;

    float curFrameWeight = frameCount > sampleCount ? dftFrameWeight : 1.0f / (float)frameCount;

    avgDeltaTime = avgDeltaTime * (1 - curFrameWeight) + deltaTime * curFrameWeight;

    float fps = 1.0f / avgDeltaTime * 1000;

    fpsPrintInterval += deltaTime;
    if (fpsPrintInterval > 1000)
    {
        fpsPrintInterval = 0;
        printf("FPS: %f\n", fps);
    }

    return fps;
}

float xOffset, yOffset;
void UpdateCameraView(Camera *camera)
{
    // 视角移动
    if (screen_keys[0x41]) // A-左
        camera->MoveX(-0.01f);
    if (screen_keys[0x44]) // D-右
        camera->MoveX(0.01f);
    if (screen_keys[0x57]) // W
        camera->MoveZ(0.01f);
    if (screen_keys[0x53]) // S
        camera->MoveZ(-0.01f);
    if (screen_keys[0x58]) // X-下
        camera->MoveY(0.01f);
    if (screen_keys[VK_SPACE]) // Space-上
        camera->MoveY(-0.01f); // d3d 屏幕坐标从上到下 Y 从小到大

    if (firstMouse)
    {
        preMouseX = curMouseX;
        preMouseY = curMouseY;
        firstMouse = false;
    }

    xOffset = curMouseX - preMouseX;
    yOffset = curMouseY - preMouseY; // reversed since y-coordinates go from bottom to top

    preMouseX = curMouseX;
    preMouseY = curMouseY;

    if (xOffset != 0 || yOffset != 0)
    {
        camera->MoveYawPitch(xOffset, yOffset);
    }

    camera->Update();
}

void DrawCall(Render *render, Object *obj)
{
    render->DrawObject(obj);
}

void TestLoadTexture(Render *render);
void TestDrawPixel(Render *render);
void TestDrawScanline(Render *render);
void TestDrawLine(Render *render);
void TestDrawTriangle2D(Render *render);
void TestGetCubeObj(Object *obj);

float objRotDeg = 0.05f;
void TestRotateObject(Object *obj)
{
    // 物体旋转
    if (screen_keys[VK_LEFT])
        obj->transform.DoRotateY(-objRotDeg);
    if (screen_keys[VK_RIGHT])
        obj->transform.DoRotateY(objRotDeg);
    if (screen_keys[VK_UP])
        obj->transform.DoRotateX(-objRotDeg);
    if (screen_keys[VK_DOWN])
        obj->transform.DoRotateX(objRotDeg);

    obj->transform.SetTRS();
}

int main()
{
    TCHAR *title = _T("LXRender");
    screen_init(SCREEN_WIDTH, SCREEN_HEIGHT, title);

    Render render = Render(SCREEN_WIDTH, SCREEN_HEIGHT, screen_fb);
    render.Clear();

    TestLoadTexture(&render);
    // TestDrawPixel(&render);
    // TestDrawScanline(&render);
    // TestDrawLine(&render);
    // TestDrawTriangle2D(&render);

    Object obj;
    TestGetCubeObj(&obj);
    DrawCall(&render, &obj);

    int fps;
    while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0)
    {
        UpdateCameraView(&render.camera);
        TestRotateObject(&obj);

        render.Clear();
        DrawCall(&render, &obj);

        screen_update();
        // Sleep(100);

        UpdateTime();
        GetFPS(deltaTime);
    }

    return 0;
}

void TestDrawPixel(Render *render)
{
    int x, y;
    Color color = Color(1.0f, 0, 0, 1.0f);
    UINT32 c = color.ToInt32();

    for (x = 1; x < 100; x++)
        for (y = 1; y < 100; y++)
            render->DrawPixel(x, y, c);
}

void TestDrawLine(Render *render)
{
    Color color = Color(0, 1.0f, 0, 1.0f);
    render->DrawLine(10, 10, 500, 400, color.ToInt32());
}

void TestDrawScanline(Render *render)
{
    Vertex step = Vertex(Vector(1, 0, 0, 1), Color(-0.005f, 0, 0, 0), 1);

    int y;
    for (y = 1; y < 100; y++)
    {
        Vertex v = Vertex(Vector(100, y, 0, 1), Color(1.0f, 0, 0, 1), 1);
        Scanline scanline = Scanline(v, step, 100, y, 200);
        render->DrawScanLine(&scanline);
    }
}

void TestDrawTriangle2D(Render *render)
{
    Vertex v1 = Vertex(Vector(400, 100, 0, 1), Color(1.0f, 0, 0, 1), Texcoord(0.5f, 0.0f), 1);
    Vertex v2 = Vertex(Vector(160, 500, 0, 1), Color(0, 1.0f, 0, 1), Texcoord(0.0f, 1.0f), 1);
    Vertex v3 = Vertex(Vector(640, 500, 0, 1), Color(0, 0, 1.0f, 1), Texcoord(1.0f, 1.0f), 1);
    render->DrawTriangle2D(&v1, &v2, &v3);
}

void TestLoadTexture(Render *render)
{
    static char *texPath = ".\\Res\\bull.jpg";
    render->device.LoadTexture(texPath);
}

void TestGetCubeObj(Object *obj)
{
    static float *data = new float[192]{
        // {x, y, z} {r, g, b} {u, v}

        // back: 0123
        1.0f, -1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 1.0f, 1.0f,  // 0[0]
        1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 1.0f, 0.0f,   // 1[1]
        -1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.0f, 0.0f,  // 2[2]
        -1.0f, -1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.0f, 1.0f, // 3[3]

        // forward: 4567
        1.0f, -1.0f, -1.0f, 0.7f, 1.0f, 0.7f, 1.0f, 1.0f,  // 4[4]
        1.0f, 1.0f, -1.0f, 0.7f, 1.0f, 0.7f, 1.0f, 0.0f,   // 5[5]
        -1.0f, 1.0f, -1.0f, 0.7f, 1.0f, 0.7f, 0.0f, 0.0f,  // 6[6]
        -1.0f, -1.0f, -1.0f, 0.7f, 1.0f, 0.7f, 0.0f, 1.0f, // 7[7]

        // top: 1256
        1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 1.0f, 1.0f, 0.0f,   // 1[8]
        -1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 1.0f, 0.0f, 0.0f,  // 2[9]
        1.0f, 1.0f, -1.0f, 0.7f, 0.7f, 1.0f, 1.0f, 1.0f,  // 5[10]
        -1.0f, 1.0f, -1.0f, 0.7f, 0.7f, 1.0f, 0.0f, 1.0f, // 6[11]

        // bottom: 0347
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f,   // 0[12]
        -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 0.0f, 1.0f,  // 3[13]
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 0.0f,  // 4[14]
        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.7f, 0.0f, 0.0f, // 7[15]

        // left: 2367
        -1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 0.0f, 0.0f,   // 2[16]
        -1.0f, -1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f,  // 3[17]
        -1.0f, 1.0f, -1.0f, 0.7f, 1.0f, 1.0f, 0.0f, 1.0f,  // 6[18]
        -1.0f, -1.0f, -1.0f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f, // 7[19]

        // right: 0145
        1.0f, -1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f,  // 0[20]
        1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 0.0f,   // 1[21]
        1.0f, -1.0f, -1.0f, 1.0f, 0.7f, 1.0f, 0.0f, 1.0f, // 4[22]
        1.0f, 1.0f, -1.0f, 1.0f, 0.7f, 1.0f, 0.0f, 0.0f,  // 5[23]

    };

    // 逆时针为正面
    static int *triangles = new int[36]{
        0, 1, 2, 0, 2, 3,       // back
        4, 6, 5, 4, 7, 6,       // forward
        8, 10, 9, 9, 10, 11,    // top
        12, 13, 14, 13, 15, 14, // bottom
        16, 18, 17, 17, 18, 19, // right
        20, 23, 21, 20, 22, 23  // left
    };

    Transform transform;
    transform.SetTranslate(0.0f, 0.0f, 3.0f);
    transform.SetScale(1.0f, 1.0f, 1.0f);
    transform.SetTRS();
    std::cout << "transform add = " << &transform << std ::endl;
    std::cout << "transform size = " << sizeof(transform) << std ::endl;

    obj->Init(data, 192, 24, triangles, 12, &transform);
}
