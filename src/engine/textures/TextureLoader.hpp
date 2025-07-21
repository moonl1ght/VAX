#ifndef TextureLoader_hpp
#define TextureLoader_hpp

#include <iostream>

#include "Buffer.hpp"
#include "luna.h"
#include "VKUtils.hpp"
#include "Texture.hpp"
#include "VKEngine.hpp"

namespace TextureLoader {

    Texture* loadTexture(VKEngine* vkEngine, std::string path, bool isAutoLoadImageView = true);
    Texture* createDepthTexture(VKEngine* vkEngine, VkFormat format);

}  // namespace TextureLoader

#endif