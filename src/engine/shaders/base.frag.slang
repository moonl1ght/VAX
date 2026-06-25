import layoutAndBindings;
import common;
import uniforms;
import sharedUtils;
import pbr;
import math;

struct FragmentInput {
    float4 position : SV_Position;
    float4 tangent : TANGENT;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
    uint packedColor : PACKED_COLOR;
};

[shader("fragment")]
float4 fragMain(uniform DrawPushConstants pushConstants, FragmentInput input)
    : SV_Target {
    PBRMaterial material = materials[pushConstants.materialIndex];

    float4 albedo = sampleBaseColor(material, input.uv);

    return albedo;
}