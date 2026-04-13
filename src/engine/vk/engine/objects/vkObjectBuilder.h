#pragma once

#include "vkEngine.h"

namespace vax {
    class VkObjectBuilder {
    public:
        VkObjectBuilder(vax::vk::Engine* vkEngine) : vkEngine(vkEngine) {}

        vax::vk::Engine* vkEngine;
    };
}