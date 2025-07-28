#pragma once

#include "luna.h"
#include "VKEngine.hpp"

class VKObject {
protected:
    VKEngine* vkEngine = nullptr;

    VKObject() = default;
    VKObject(VKEngine* vkEngine) : vkEngine(vkEngine) {}

    virtual ~VKObject() = default;
};