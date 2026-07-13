
[vk::binding(0, 0)]
Texture2D<uint> inputMask;

[vk::binding(1, 0)]
RWTexture2D<uint2> textureA;

[shader("compute")]
[numthreads(16, 16, 1)]
void compMain(uint3 dtid : SV_DispatchThreadID)
{
    uint2 pixelCoords = dtid.xy;

    uint objectId = inputMask.Load(int3(pixelCoords, 0));

    uint isSelected = objectId > 0;
    uint2 point = isSelected * pixelCoords + (1 - isSelected) * uint2(60000, 60000);

    textureA[pixelCoords] = point;
}