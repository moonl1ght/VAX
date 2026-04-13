#pragma once

#include "Buffer.hpp"
#include "luna.h"
#include "vkUtils.h"
#include "texture.h"
#include "vkObjectBuilder.h"

class TextureLoader final : public vax::VkObjectBuilder {
public:
    vax::textures::Texture* loadTexture(std::string path, bool isAutoLoadImageView = true);
};