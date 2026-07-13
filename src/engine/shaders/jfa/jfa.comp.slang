#include "jfa.h"

[vk::push_constant]
JFAPushConstants pc;

[vk::binding(0, 0)]
RWTexture2D<uint2> inputCoords;

[vk::binding(1, 0)]
RWTexture2D<uint2> outputCoords;

[shader("compute")]
[numthreads(16, 16, 1)]
void compMain(uint3 dtid : SV_DispatchThreadID)
{
    uint2 pixelCoords = dtid.xy;

    uint width, height;
    inputCoords.GetDimensions(width, height);

    if (pixelCoords.x >= width || pixelCoords.y >= height)
        return;

    uint2 bestSeed = uint2(MAX_UINT, MAX_UINT);
    float minDistance = float.maxValue;

    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            int2 neighborCoords = int2(pixelCoords) + int2(x, y) * int(pc.step);

            if (neighborCoords.x >= 0 && neighborCoords.x < int(width) &&
                neighborCoords.y >= 0 && neighborCoords.y < int(height))
            {
                uint2 seed = inputCoords[neighborCoords];

                if (seed.x != MAX_UINT && seed.y != MAX_UINT)
                {
                    float2 diff = float2(pixelCoords) - float2(seed);

                    float distanceSqr = diff.x * diff.x + diff.y * diff.y;

                    if (distanceSqr < minDistance)
                    {
                        minDistance = distanceSqr;
                        bestSeed = seed;
                    }
                }
            }
        }
    }

    outputCoords[pixelCoords] = bestSeed;
}