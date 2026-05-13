import layoutAndBindings;
import uniforms;

float4 sampleBaseColor(PBRMaterial material, float2 uv) {
    uint baseColorTextureIndex = material.baseColorTextureIndex;
    uint baseColorTextureSamplerIndex = material.baseColorTextureSamplerIndex;
    if (baseColorTextureIndex == NO_TEXTURE_FLAG)
        return material.baseColor;
    return globalTextures[baseColorTextureIndex].Sample(globalSamplers[baseColorTextureSamplerIndex], uv);
}

float4 sampleAmbientOcclusion(PBRMaterial material, float2 uv) {
    uint aoTextureIndex = material.aoTextureIndex;
    uint aoTextureSamplerIndex = material.aoTextureSamplerIndex;
    if (aoTextureIndex == NO_TEXTURE_FLAG)
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return globalTextures[aoTextureIndex].Sample(globalSamplers[aoTextureSamplerIndex], uv);
}

float4 sampleMetallicRoughness(PBRMaterial material, float2 uv) {
    uint metallicRoughnessTextureIndex = material.metallicRoughnessTextureIndex;
    uint metallicRoughnessTextureSamplerIndex = material.metallicRoughnessTextureSamplerIndex;
    if (metallicRoughnessTextureIndex == NO_TEXTURE_FLAG)
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return globalTextures[metallicRoughnessTextureIndex].Sample(
        globalSamplers[metallicRoughnessTextureSamplerIndex], uv
    );
}

float4 sampleNormalMap(PBRMaterial material, float2 uv) {
    uint normalMapTextureIndex = material.normalMapTextureIndex;
    uint normalMapTextureSamplerIndex = material.normalMapTextureSamplerIndex;
    if (normalMapTextureIndex == NO_TEXTURE_FLAG)
        return float4(0.5f, 0.5f, 1.0f, 1.0f);
    return globalTextures[normalMapTextureIndex].Sample(
        globalSamplers[normalMapTextureSamplerIndex], uv
    );
}

float4 sampleEmissive(PBRMaterial material, float2 uv) {
    uint emissiveTextureIndex = material.emissiveTextureIndex;
    uint emissiveTextureSamplerIndex = material.emissiveTextureSamplerIndex;
    if (emissiveTextureIndex == NO_TEXTURE_FLAG)
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return globalTextures[emissiveTextureIndex].Sample(
        globalSamplers[emissiveTextureSamplerIndex], uv
    );
}