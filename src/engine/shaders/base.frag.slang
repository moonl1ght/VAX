import layoutAndBindings;
import common;
import uniforms;
import sharedUtils;

struct FragmentInput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
    uint packedColor : PACKED_COLOR;
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
    float4 color = sampleBaseColor(pushConstants.materialIndex, input.uv);
    if ((pushConstants.flags & ObjectFlags::IsWireframe) != 0) {
        float val = edgeFactor(input.baryCoords, 1.0);
        return lerp(color, float4(0, 0, 0, 1), 1.0 - val);
    } else {
        return color;
    }
}
