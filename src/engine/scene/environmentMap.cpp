#include "environmentMap.h"

using namespace vax::scene;
using namespace vax;

void EnvironmentMap::load(const Descriptor& descriptor, VkQueue submitQueue) {
    _environmentMapData = EnvironmentMapData{
        .envMapTexture = NO_TEXTURE_FLAG,
        .envMapTextureSampler = static_cast<uint32_t>(GlobalSampler::CubeMapSampler),
        .envMapTextureIrradiance = NO_TEXTURE_FLAG,
        .envMapTextureIrradianceSampler = static_cast<uint32_t>(GlobalSampler::CubeMapSampler),
        .texBRDFLUT = NO_TEXTURE_FLAG,
        .texBRDFLUTSampler = static_cast<uint32_t>(GlobalSampler::CubeMapSampler),
    };
    for (const auto& texture : descriptor.textures) {
        auto textureResource = _textureLoader.get().loadTexture(texture.second, submitQueue);
        if (!textureResource.has_value()) {
            _logger.error("Failed to load texture: {}", texture.second);
            continue;
        }
        switch (texture.first) {
            case TextureType::EnvMap:
                _environmentMapData.envMapTexture = textureResource->first.id();
                break;
            case TextureType::EnvMapIrradiance:
                _environmentMapData.envMapTextureIrradiance = textureResource->first.id();
                break;
            case TextureType::BRDFLUT:
                _environmentMapData.texBRDFLUT = textureResource->first.id();
                break;
        }
    }
    VkDeviceSize bufferSize = sizeof(EnvironmentMapData);
    auto allocation = vk::Buffer::allocate(
        _device.get(),
        "environment_map_buffer",
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    if (!allocation.has_value())
        return;
    _buffer = std::make_unique<vk::Buffer>(std::move(*allocation));
    _buffer->map();
    auto mappedMemory = _buffer->mappedMemory();
    if (!mappedMemory.has_value())
        return;
    EnvironmentMapData* environmentMapDataPtr = static_cast<EnvironmentMapData*>(*mappedMemory);
    *environmentMapDataPtr = _environmentMapData;
}