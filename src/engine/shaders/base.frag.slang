import base;

struct FragmentInput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float3 baryCoords : SV_Barycentrics;
};

float edgeFactor(float3 barycoords, float thickness)
{
    float edgeIntensity = min(barycoords.x, min(barycoords.y, barycoords.z));
    float lineSmoothing = fwidth(edgeIntensity);
    return smoothstep(0.0, lineSmoothing * thickness, edgeIntensity);
}

[shader("fragment")]
float4 fragMain(FragmentInput input)
    : SV_Target
{
    float val = edgeFactor(input.baryCoords, 1.0);

    // return float4(1, 1, 0, 1);

    // return materials[pushConstants.materialIndex].baseColor;

    uint baseColorTextureIndex = materials[pushConstants.materialIndex].baseColorTextureIndex;

    // float4 color = materials[pushConstants.materialIndex].baseColor;
    if (baseColorTextureIndex == NO_TEXTURE_FLAG) {
        // return float4(0, 1, 0, 1);
        return materials[pushConstants.materialIndex].baseColor;
    }
    // return float4(0, 0, (float)baseColorTextureIndex, 1);

    float4 color = globalTextures[baseColorTextureIndex].Sample(
        globalSamplers[0], input.uv
    );
    return lerp(color, float4(0, 0, 0, 1), 1.0 - val);
}
