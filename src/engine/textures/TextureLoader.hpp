#pragma once

#include "Buffer.hpp"
#include "luna.h"
#include "vkUtils.h"
#include "Texture.hpp"
#include "vkObjectBuilder.h"

class TextureLoader final : public vax::VkObjectBuilder {
public:
    Texture* loadTexture(std::string path, bool isAutoLoadImageView = true);
};