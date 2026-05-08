#include "textureManager.h"
#include "textureFactory.h"
#include "descriptorSetWriter.h"

using namespace vax;
using namespace vax::textures;

bool TextureManager::setup() {
    for (size_t i = 0; i < vax::MAX_GLOBAL_SAMPLERS; ++i) {
        if (auto sampler = vax::textures::Sampler::createSampler(_device.get())) {
            _globalSamplers.push_back(std::move(*sampler));
        } else {
            _logger.error("Failed to create global sampler");
            return false;
        }
    }
    return true;
}

void TextureManager::fullCleanup() {
    for (auto& [id, texture] : _pool) {
        texture._destroy();
    }
    _pool.clear();
}

TextureFactory TextureManager::createTextureFactory() const {
    return TextureFactory(_device.get(), _allocator, const_cast<TextureManager* const>(this));
}

std::optional<TextureManager::TextureResource> TextureManager::attach(textures::Texture&& texture) {
    texture._id = _lastId++;
    auto [it, inserted] = _pool.try_emplace(texture.id(), std::move(texture));
    if (!inserted) {
        _logger.error("Failed to attach texture to manager");
        return std::nullopt;
    }
    it->second._isDetached = false;
    return std::make_pair(it->first, &it->second);
}

std::optional<TextureManager::TextureResource> TextureManager::find(TextureHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return std::nullopt;
    return std::make_pair(handle, &it->second);
}

bool TextureManager::deleteTexture(TextureHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return false;
    it->second._destroy();
    _pool.erase(it);
    return true;
}

std::optional<textures::Texture> TextureManager::detach(TextureHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return std::nullopt;
    it->second._isDetached = true;
    _pool.erase(it);
    return std::move(it->second);
}

void TextureManager::updateDescriptorWriterWithAllTextures(
    vax::vk::DescriptorSetWriter& descriptorWriter,
    uint32_t binding,
    bool useSampler
) const {
    std::vector<const textures::Texture*> textures(_pool.size());
    for (auto& [id, texture] : _pool) {
        textures[texture.id()] = &texture;
    }

    if (textures.empty()) {
        return;
    }
    descriptorWriter.writeTextures(textures, binding, useSampler);
}

std::optional<TextureManager::SamplerResource> TextureManager::getGlobalSampler(GlobalSampler sampler) {
    auto index = static_cast<uint32_t>(sampler);
    if (index >= _globalSamplers.size()) {
        _logger.error("Global sampler index out of bounds");
        return std::nullopt;
    }
    return std::make_pair(SamplerHandle(index), &_globalSamplers[index]);
}