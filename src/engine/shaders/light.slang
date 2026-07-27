import uniforms;
import layoutAndBindings;

float calculateSunShadow(float3 worldPos, float3 normal, float3 lightDir, LightData light) {
    if (light.shadowMapIndex == NO_SHADOW_MAP_INDEX) {
        return 1.0f;
    }

    float4 lightSpacePos = mul(light.lightSpaceMatrix, float4(worldPos, 1.0f));
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    float2 shadowUV = projCoords.xy * 0.5f + 0.5f;

    shadowUV.y = 1.0f - shadowUV.y;

    if (projCoords.z > 1.0f || shadowUV.x < 0.0f || shadowUV.x > 1.0f || shadowUV.y < 0.0f || shadowUV.y > 1.0f) {
        return 1.0f;
    }

    float closestDepth = globalShadowTextures[light.shadowMapIndex].Sample(
        globalSamplers[light.shadowMapSamplerIndex],
        shadowUV
    ).r;

    float bias = max(0.000005f * (1.0f - dot(normal, lightDir)), 0.00005f);

    float currentDepth = projCoords.z;
    float softness = 0.002f;
    float diff = closestDepth - (currentDepth + bias);
    float shadowFactor = 1.0f - clamp(diff / softness, 0.0f, 1.0f);

    return shadowFactor;
}