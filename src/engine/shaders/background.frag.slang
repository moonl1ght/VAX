[shader("fragment")]
float4 fragMain(float2 uv: TEXCOORD0) : SV_Target {
    float3 topColor = float3(0.1f, 0.2f, 0.4f);
    float3 bottomColor = float3(0.05f, 0.05f, 0.1f);

    float3 gradient = lerp(topColor, bottomColor, uv.y);

    return float4(gradient, 1.0f);
}
