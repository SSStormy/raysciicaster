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

#define ARRAY_SIZE(M__ARRAY) (sizeof(M__ARRAY)/sizeof(M__ARRAY[0]))

struct material
{
    b32 IsEmitter;
};

struct plane
{
    v3 Normal;
    f32 Dist;
    u32 MaterialIndex;
};

struct sphere
{
    v3 Origin;
    f32 SquaredRadius;
    u32 MaterialIndex;
};

static const material Materials[] = {
    { false },
    { true }
};

static const plane Planes[] = {
    { v3(0.0f, 1.0f, 0.0f), 0.0f,  1}
//    { v3(0.707f, 0.707f, 0.0f), 0.0f,  0}
};

static const sphere Spheres[] = {
    { v3(0.0f, 0.0f, 0.0f), 4.0f, 0},
    { v3(-4.0f, 8.0f, 4.0f), 8.0f, 0}
};

#define SCREEN_X 158
#define SCREEN_Y 78
static const iv2 ScreenSize = {SCREEN_X, SCREEN_Y};

const char AsciiGradient[] = 
#if 1
R"FOO($@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'. )FOO"
#else
R"FOO(@%#*+=-:. )FOO"
#endif
;

const u32 AsciiGradientSize = sizeof(AsciiGradient) - 2;

char BackBuffer[SCREEN_Y][SCREEN_X] = {};

static inline
void ClearBackBuffer()
{
    printf("\033c");
    memset(BackBuffer, ' ', sizeof(BackBuffer));
}

static const f32 maxRayDistance = 256.0f;

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


static inline
char GetAsciiChar(f32 t)
{
    t = t * t;
    static const f32 colorStep = 1.0f / AsciiGradientSize;
    f32 distance = t / maxRayDistance;

    s32 charIndex = distance / colorStep;
    
    if(charIndex > AsciiGradientSize)
        return AsciiGradient[AsciiGradientSize];
    if(0 > charIndex)
        return AsciiGradient[0];

    return AsciiGradient[charIndex];
}

static inline
b32 IsValidT(f32 t)
{
    return t > 0.002f && maxRayDistance >= t;
}

static inline
f32 MinF32(f32 a, f32 b)
{
    if(a > b) return b;
    return a;
}

b32 IsBetterResult(f32 told, f32 tnew)
{
    return told > tnew;
}

static inline
u8 SendRay(v3 origin, v3 direction, u8 bounce = 0)
{
    if(bounce > 20) return ' ';

    f32 finalT = FLT_MAX;
    v3 normal = {0,0,0};
    u32 materialIndex = 0;

    for(u32 planeIndex = 0;
            planeIndex < ARRAY_SIZE(Planes);
            planeIndex++)
    {
        plane pln = Planes[planeIndex];

        f32 topDot = -glm::dot(pln.Normal, origin) - pln.Dist;
        f32 botDot = glm::dot(pln.Normal, direction);
        if(botDot == 0) continue;

        f32 t = topDot / botDot;

        if(!IsValidT(t)) continue;

        if(IsBetterResult(finalT, t))
        {
            finalT = t;
            normal = pln.Normal;
            materialIndex = pln.MaterialIndex;
        }
    }

    for(u32 sphereIndex = 0;
            sphereIndex < ARRAY_SIZE(Spheres);
            sphereIndex++)
    {
        sphere sph = Spheres[sphereIndex];
        
        // (px - sx)^2 + (py - sy)^2 + ...
        // this is the (px - sx) part. p - s, where p is the locus point and s is the sphere origin.
        v3 cameraOriginRelativeToSphere = origin - sph.Origin;

        f32 a = glm::dot(direction, direction);
        if(a == 0.0f) continue;

        f32 b = 2.0f * glm::dot(cameraOriginRelativeToSphere, direction);
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
        
        if(IsBetterResult(finalT, t))
        {
            finalT = t;
            materialIndex = sph.MaterialIndex;
            v3 point = origin + (t * direction);
            normal = glm::normalize(point - sph.Origin);
        }
    }

    if(finalT== FLT_MAX) return ' ';

    v3 point = origin + (finalT * direction);
    f32 localDistance = glm::distance(point, origin);

    material mat = Materials[materialIndex];
    if(!mat.IsEmitter) // reflective
    {
        v3 newDir = direction - (2.0f*((glm::dot(normal, direction) * normal)));
        newDir = glm::normalize(newDir);
        u8 result = SendRay(point, newDir, bounce + 1);

        return result;
    }

    return GetAsciiChar(localDistance);
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
            
            BackBuffer[y][x] = SendRay(cameraOrigin, rayDirection);
        }
    }
}

int main()
{
    v3 cameraOrigin = {0.0f, 2.0f , -10.0f};

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
