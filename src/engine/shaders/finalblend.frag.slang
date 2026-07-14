#include "jfa/jfa.h"
import layoutAndBindings;

[vk::binding(0, 0)]
Sampler2D inputTexture;

[vk::binding(1, 0)]
Texture2D<uint> inputMask;

[vk::binding(0, 1)]
Texture2D<uint2> inputJFATexture;

[shader("fragment")]
float4 fragMain(float2 uv: TEXCOORD0) : SV_Target {
    float3 topColor = float3(0.1f, 0.2f, 0.4f);
    float3 bottomColor = float3(0.55f, 0.55f, 0.55f);

    float3 gradient = lerp(bottomColor, topColor, uv.y);

    uv.y = 1.0f - uv.y;
    float4 sceneColor = inputTexture.Sample(uv);
    float3 finalColor = gradient;

    uint width, height;
    inputJFATexture.GetDimensions(width, height);

    uint2 pixelCoords = uint2(uv * float2(width, height));
    uint2 closestEdgePixel = inputJFATexture[pixelCoords];

    if (closestEdgePixel.x != MAX_UINT)
    {
        float dist = distance(float2(pixelCoords), float2(closestEdgePixel));

        float outlineWidth = 3.0f;
        float3 outlineColor = float3(1.0f, 0.0f, 0.0f);
        if (dist > 0.0f && dist <= outlineWidth) {

            uint currentObjId = inputMask[pixelCoords];
            uint edgeObjId = inputMask[closestEdgePixel];
            
            bool drawOutline = false;

            if (currentObjId != edgeObjId) {
               drawOutline = true;
            }

            if (drawOutline) {
                float edgeAlpha = smoothstep(outlineWidth, 0, dist);
                float finalAlpha = 0.9f * edgeAlpha;
                return float4(lerp(sceneColor.rgb, outlineColor, finalAlpha), sceneColor.a);
            }
            // float edgeAlpha = smoothstep(outlineWidth, outlineWidth - 1.0f, dist);
            
            // float finalAlpha = 1.0f * edgeAlpha;

            // // float edgeAlpha = smoothstep(outlineWidth, outlineWidth - 1.0f, dist);
            // return float4(lerp(sceneColor.rgb, outlineColor, edgeAlpha), sceneColor.a);
        }
    }

    return lerp(float4(gradient, 1.0f), sceneColor, sceneColor.a);

    // return float4(finalColor, 1.0f);
}