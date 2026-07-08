import layoutAndBindings;
import uniforms;

struct FragmentInput {
    float4 position : SV_Position;
    float4 tangent : TANGENT;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
    uint packedColor : PACKED_COLOR;
    // float3 baryCoords : SV_Barycentrics;
    nointerpolation uint instanceIndex : INSTANCE_INDEX;
};

[shader("fragment")]
float4 fragMain(uniform DrawPushConstants pushConstants, FragmentInput input) : SV_Target {
    if ((pushConstants.flags & ObjectFlags::IsInstanceSelected) != 0) {
        return float4(0.0f, 0.0f, 1.0f, 1.0f / input.instanceIndex);
    }
    return float4(0.0f, 1.0f, 0.0f, 1.0f / input.instanceIndex);
}