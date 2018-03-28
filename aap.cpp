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


struct plane
{
    v3 Normal;
    f32 Dist;
};

#define ARRAY_SIZE(M__ARRAY) (sizeof(M__ARRAY[0])/sizeof(M__ARRAY))

plane Planes[] = {
    { v3(0.0f, 1.0f, 0.0f), 0 }
};

#define SCREEN_X 150
#define SCREEN_Y 40
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

int main()
{
    v3 cameraOrigin = {1,1,1};
    f32 maxRayDistance = 256.0f;

    while(1)
    {

    printf("%f %f %f\n", cameraOrigin.x, cameraOrigin.y, cameraOrigin.z);

    for(u32 y = 0;
            y < ScreenSize.y;
            y++)
    {
        for(u32 x = 0;
                x < ScreenSize.x;
                x++)
        {
            v2 homogenousScreenCoord = {
                (2.0f * (static_cast<f32>(x) / static_cast<f32>(ScreenSize.x)) - 1.0f),
                (2.0f * (static_cast<f32>(y) / static_cast<f32>(ScreenSize.y)) - 1.0f)
            };

            v3 rayDirection = {
                homogenousScreenCoord.x,
                homogenousScreenCoord.y,
                1.0f
            };

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

                if(t > maxRayDistance || 0 >= t) continue;

                const f32 colorStep = 1.0f / AsciiGradientSize;
                f32 distance = t / maxRayDistance;

                u32 charIndex = distance / colorStep;
                
//                printf("%f %f %d %f\n", colorStep, distance, charIndex, t);
                BackBuffer[y][x] = AsciiGradient[charIndex];
            }
        }
    }

    for(s32 y = ScreenSize.y - 1;
            y >= 0;
            y--)
    {
        printf("%.*s\n", SCREEN_X, BackBuffer[y]);
    }

    getchar();
    cameraOrigin.y += 1.0f;

    }
}
