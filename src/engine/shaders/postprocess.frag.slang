import layoutAndBindings;

[shader("fragment")]
float4 fragMain(float2 uv: TEXCOORD0) : SV_Target {
    float3 sceneColor = postProcessTexture[0].Sample(uv).rgb;

    return float4(sceneColor, 1.0f);
}