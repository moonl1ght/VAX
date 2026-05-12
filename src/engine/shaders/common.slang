import layoutAndBindings;
import uniforms;

float4 sampleBaseColor(uint materialIndex, float2 uv) {
    uint baseColorTextureIndex = materials[materialIndex].baseColorTextureIndex;
    if (baseColorTextureIndex == NO_TEXTURE_FLAG)
        return materials[materialIndex].baseColor;
    return globalTextures[baseColorTextureIndex].Sample(globalSamplers[0], uv);
}
