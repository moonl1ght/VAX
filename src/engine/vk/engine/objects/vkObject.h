#pragma once

#include "luna.h"
#include "vkEngine.h"

namespace vax {
    class VkObject {
    protected:
        vax::vk::Engine* vkEngine = nullptr;

        VkObject(vax::vk::Engine* vkEngine) : vkEngine(vkEngine) {}

        virtual ~VkObject() = default;
    };
}