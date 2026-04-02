#pragma once

#include "luna.h"
#include "vkEngine.h"

class VKObject {
protected:
    vax::VkEngine* vkEngine = nullptr;

    VKObject() = default;
    VKObject(vax::VkEngine* vkEngine) : vkEngine(vkEngine) {}

    virtual ~VKObject() = default;
};