#include <cstdio> 
#include <cstdint> 
#include <cstring>
#include <glm/glm.hpp>

using v2 = glm::vec2;
using iv2 = glm::ivec2;
using v3 = glm::vec3;
using v4 = glm::vec4;

using b32 = bool;

using f32 = float;
using f64 = double;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

#define ARRAY_SIZE(M__ARRAY) (sizeof(M__ARRAY[0])/sizeof(M__ARRAY))

struct plane
{
    v3 Normal;
    f32 Dist;
};

struct sphere
{
    v3 Origin;
    f32 SquaredRadius;
};

plane Planes[] = {
    { v3(0.0f, 1.0f, 0.0f), 0.0f }
};

sphere Spheres[] = {
    { v3(0.0f, 0.0f, 0.0f), 4.0f }
};

#define SCREEN_X 158
#define SCREEN_Y 78
static const iv2 ScreenSize = {SCREEN_X, SCREEN_Y};

const char AsciiGradient[] = 
#if 0
R"FOO($@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'. )FOO"
#else
R"FOO(@%#*+=-:. )FOO"
#endif
;

const u32 AsciiGradientSize = sizeof(AsciiGradient) - 1;

char BackBuffer[SCREEN_Y][SCREEN_X] = {};

static inline
void ClearBackBuffer()
{
    printf("\033c");
    memset(BackBuffer, ' ', sizeof(BackBuffer));
}

static inline
void SwapBackBuffers()
{
    for(u32 y = 0;
            y < ScreenSize.y;
            y++)
    {
        for(u32 x = 0;
                x < ScreenSize.x;
                x++)
        {
            putchar(BackBuffer[y][x]);
        }
        putchar('\n');
    }
}

static const f32 maxRayDistance = 64.0f;

static inline
char GetAsciiChar(f32 t)
{
    static const f32 colorStep = 1.0f / AsciiGradientSize;
    f32 distance = t / maxRayDistance;

    u32 charIndex = distance / colorStep;
    
    if(charIndex > AsciiGradientSize)
        return AsciiGradient[AsciiGradientSize];

    return AsciiGradient[charIndex];
}

static inline
b32 IsValidT(f32 t)
{
    return t >= 0.0f && maxRayDistance >= t;
}

static inline
f32 MinF32(f32 a, f32 b)
{
    if(a > b) return b;
    return a;
}

static inline
void RaycastScene(v3 cameraOrigin)
{
    for(u32 y = 0;
            y < ScreenSize.y;
            y++)
    {
        for(u32 x = 0;
                x < ScreenSize.x;
                x++)
        {
            v3 rayDirection = {
                static_cast<f32>(x),
                static_cast<f32>(y),
                1.0f
            };

            rayDirection /= v3(ScreenSize.x, ScreenSize.y, 1.0f);
            rayDirection = (rayDirection * 2.0f) - 1.0f;

            rayDirection.y *= -1.0f;
            rayDirection = glm::normalize(rayDirection);

            for(u32 planeIndex = 0;
                    planeIndex < ARRAY_SIZE(Planes);
                    planeIndex++)
            {
                plane pln = Planes[planeIndex];

                f32 topDot = -glm::dot(pln.Normal, cameraOrigin) - pln.Dist;
                f32 botDot = glm::dot(pln.Normal, rayDirection);
                if(botDot == 0) continue;

                f32 t = topDot / botDot;

                if(!IsValidT(t)) continue;

                BackBuffer[y][x] = GetAsciiChar(t);
            }

            for(u32 sphereIndex = 0;
                    sphereIndex < ARRAY_SIZE(Spheres);
                    sphereIndex++)
            {
                sphere sph = Spheres[sphereIndex];
                
                // (px - sx)^2 + (py - sy)^2 + ...
                // this is the (px - sx) part. p - s, where p is the locus point and s is the sphere origin.
                v3 cameraOriginRelativeToSphere = cameraOrigin - sph.Origin;

                f32 a = glm::dot(rayDirection, rayDirection);
                if(a == 0.0f) continue;

                f32 b = 2.0f * glm::dot(cameraOriginRelativeToSphere, rayDirection);
                f32 c = glm::dot(cameraOriginRelativeToSphere, cameraOriginRelativeToSphere) - sph.SquaredRadius;

                f32 det = (b*b) - (4.0f * a * c);
                if(0.0f > det) continue;
                f32 detSquareRoot = glm::sqrt(det);

                f32 denom = 2.0f * a;

                f32 t1 = (-b + detSquareRoot) / denom;
                f32 t2 = (-b - detSquareRoot) / denom;

                if(!IsValidT(t2)) t2 = FLT_MAX;
                if(!IsValidT(t1)) t1 = FLT_MAX;

                f32 t = MinF32(t1, t2);
                if(t == FLT_MAX) continue;

                BackBuffer[y][x] = GetAsciiChar(t);
            }
        }
    }
}

int main()
{
    v3 cameraOrigin = {0.0f, 0.0f , -10.0f};

    while(1)
    {
        ClearBackBuffer();
        RaycastScene(cameraOrigin);
        SwapBackBuffers();

        char c = getchar();

        if(c == 'w') cameraOrigin.z += 1.0f;
        if(c == 's') cameraOrigin.z -= 1.0f;
        if(c == 'd') cameraOrigin.x += 1.0f;
        if(c == 'a') cameraOrigin.x -= 1.0f;
        if(c == 'q') cameraOrigin.y += 1.0f;
        if(c == 'e') cameraOrigin.y -= 1.0f;
    }
}
