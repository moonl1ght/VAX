#pragma once

#include "luna.h"
#include "vkEngine.h"

namespace vax {
    class VkObject {
    protected:
        vax::VkEngine* vkEngine = nullptr;

        VkObject(vax::VkEngine* vkEngine) : vkEngine(vkEngine) {}

        virtual ~VkObject() = default;
    };
}