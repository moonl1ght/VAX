#pragma once

#include "vkEngine.h"

namespace vax {
    class VkObjectBuilder {
    public:
        VkObjectBuilder(vax::VkEngine* vkEngine) : vkEngine(vkEngine) {}

        vax::VkEngine* vkEngine;
    };
}