#pragma once

#include "vkEngine.h"

class VKObjectBuilder {
public:
    VKObjectBuilder(vax::VkEngine* vkEngine) : vkEngine(vkEngine) {}

    vax::VkEngine* vkEngine;
};