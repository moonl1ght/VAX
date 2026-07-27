import uniforms;
import layoutAndBindings;

float calculateSunShadow(float3 worldPos, float3 normal, float3 lightDir, LightData light) {
    if (light.shadowMapIndex == NO_SHADOW_MAP_INDEX) {
        return 1.0f;
    }

    float4 lightSpacePos = mul(light.lightSpaceMatrix, float4(worldPos, 1.0f));
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    float2 shadowUV = projCoords.xy * 0.5f + 0.5f;

    if (projCoords.z > 1.0f || shadowUV.x < 0.0f || shadowUV.x > 1.0f || shadowUV.y < 0.0f || shadowUV.y > 1.0f) {
        return 1.0f; 
    }

    float closestDepth = globalShadowTextures[light.shadowMapIndex].Sample(
        globalSamplers[light.shadowMapSamplerIndex], 
        shadowUV
    ).r;

    float bias = max(0.005f * (1.0f - dot(normal, lightDir)), 0.0005f);

    float currentDepth = projCoords.z;
    float shadowFactor = (currentDepth - bias > closestDepth) ? 0.0f : 1.0f;

    // float currentDepth = projCoords.z;
    // float shadowFactor = (currentDepth - bias > closestDepth) ? 0.0f : 1.0f;

    // float currentDepth = projCoords.z;
    // float shadowFactor = (currentDepth + bias < closestDepth) ? 0.0f : 1.0f;

    return shadowFactor;
}