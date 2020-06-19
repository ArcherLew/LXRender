
typedef unsigned int IUINT32;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} Color;

class Util
{
public:
    static int CMID(int x, int min, int max)
    {
        return (x < min) ? min : ((x > max) ? max : x);
    }

    static IUINT32 Color2Int32(Color *color)
    {
        int R = (int)(color->r * 255.0f);
        int G = (int)(color->g * 255.0f);
        int B = (int)(color->b * 255.0f);
        R = CMID(R, 0, 255);
        G = CMID(G, 0, 255);
        B = CMID(B, 0, 255);
        return (R << 16) | (G << 8) | (B);
    }
};