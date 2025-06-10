#ifndef TextureLoader_hpp
#define TextureLoader_hpp

#include <iostream>

#include "Buffer.hpp"
#include "luna.h"
#include "VKUtils.hpp"
#include "Texture.hpp"

namespace TextureLoader {

    Texture* loadTexture(VKStack* vkStack, std::string path, bool isAutoLoadImageView = true);

}  // namespace TextureLoader

#endif