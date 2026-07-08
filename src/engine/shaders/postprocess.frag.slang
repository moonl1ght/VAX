import layoutAndBindings;

[shader("fragment")]
float4 fragMain(float2 uv: TEXCOORD0) : SV_Target {
    uv.y = 1.0f - uv.y;
    float3 sceneColor = postProcessTexture[0].Sample(uv).rgb;

    float grayscale = dot(sceneColor, float3(0.2126, 0.7152, 0.0722));

    return float4(grayscale, grayscale, grayscale, 1.0f);
}