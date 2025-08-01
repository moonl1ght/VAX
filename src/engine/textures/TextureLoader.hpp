#pragma once

#include "Buffer.hpp"
#include "luna.h"
#include "VKUtils.hpp"
#include "Texture.hpp"
#include "VKObjectBuilder.hpp"

class TextureLoader final : public VKObjectBuilder {
public:
    Texture* loadTexture(std::string path, bool isAutoLoadImageView = true);
};