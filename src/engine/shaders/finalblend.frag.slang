import layoutAndBindings;

[vk::binding(0, 0)]
Sampler2D inputTexture;

[vk::binding(0, 1)]
Texture2D<uint2> inputMaskTexture;

[shader("fragment")]
float4 fragMain(float2 uv: TEXCOORD0) : SV_Target {
    float3 topColor = float3(0.1f, 0.2f, 0.4f);
    float3 bottomColor = float3(0.55f, 0.55f, 0.55f);



    float3 gradient = lerp(bottomColor, topColor, uv.y);

    uv.y = 1.0f - uv.y;
    float4 sceneColor = inputTexture.Sample(uv);
    // float4 maskColor = inputMaskTexture.Sample(uv);
    float3 finalColor = gradient;

    uint width, height;
    inputMaskTexture.GetDimensions(width, height);

    uint2 pixelCoords = uint2(uv * float2(width, height));
    uint2 closestEdgePixel = inputMaskTexture[pixelCoords];

    // float2 texel = fwidth(uv);
    // const float borderPx = 3.0f;

    // float dilated = maskColor.a;
    // float3 borderColor = float3(0.0f, 0.0f, 0.0f);
    // for (int dx = -1; dx <= 1; dx++) {
    //     for (int dy = -1; dy <= 1; dy++) {
    //         if (dx == 0 && dy == 0) continue;
    //         float4 mask = maskAt(uv + texel * borderPx * float2(dx, dy));
    //         dilated = max(dilated, mask.a);
    //         borderColor = lerp(borderColor, mask.rgb, mask.a / (mask.a == 0.0f ? 1.0f : mask.a));
    //     }
    // }

    // float borderMask = saturate(dilated - maskColor.a);

    // finalColor = lerp(finalColor, sceneColor.rgb, sceneColor.a);
    // borderMask = borderMask / (borderMask == 0.0f ? 1.0f : borderMask);
    // finalColor = lerp(finalColor, borderColor, borderMask);
    // finalColor = lerp(sceneColor.rgb, finalColor, borderMask);

    if (closestEdgePixel.x != 60000)
    {
        float dist = distance(float2(pixelCoords), float2(closestEdgePixel));

        float outlineWidth = 30.0f;
        if (dist > 0.0f && dist <= outlineWidth)
        {
            float edgeAlpha = smoothstep(outlineWidth, outlineWidth - 1.0f, dist);
            
            float finalAlpha = 1.0f * edgeAlpha;

            float3 blendedColor = lerp(sceneColor.rgb, float3(1.0f, 0.0f, 0.0f), finalAlpha);
            
            return float4(blendedColor, sceneColor.a);
        }
    }

    // Если обводки нет или мы внутри объекта — возвращаем чистую сцену
    return sceneColor;

    // return float4(finalColor, 1.0f);
}