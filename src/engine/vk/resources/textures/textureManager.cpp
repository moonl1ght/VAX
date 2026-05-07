#include "textureManager.h"
#include "textureFactory.h"
#include "descriptorSetWriter.h"

using namespace vax;
using namespace vax::textures;

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
    uint32_t binding
) const {
    std::vector<const textures::Texture*> textures(_pool.size());
    for (auto& [id, texture] : _pool) {
        textures[texture.id()] = &texture;
    }

    for (const auto& texture : textures) {
        std::cout << texture->name() << std::endl;
    }

    // for (const auto& texture : textures) {
    //     descriptorWriter.writeTexture(*texture, binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    // }

    descriptorWriter.writeTextures(textures, binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
}