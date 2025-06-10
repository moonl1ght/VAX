#ifndef TextureLoader_hpp
#define TextureLoader_hpp

#include <iostream>

#include "Buffer.hpp"
#include "luna.h"
#define STB_IMAGE_IMPLEMENTATION
#include "./deps/stb_image.h"
#include "VKUtils.hpp"
#include "Texture.hpp"

namespace TextureLoader {

    Texture* loadTexture(VKStack* vkStack, std::string path, bool isAutoLoadImageView = true);

}  // namespace TextureLoader

#endif