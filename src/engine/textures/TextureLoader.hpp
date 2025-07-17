#ifndef TextureLoader_hpp
#define TextureLoader_hpp

#include <iostream>

#include "Buffer.hpp"
#include "luna.h"
#include "VKUtils.hpp"
#include "Texture.hpp"
#include "VKStack.hpp"

namespace TextureLoader {

    Texture* loadTexture(VKStack* vkStack, std::string path, bool isAutoLoadImageView = true);
    Texture* createDepthTexture(VKStack* vkStack, VkFormat format);

}  // namespace TextureLoader

#endif