#pragma once

#include "luna.h"
#include "VKEngine.hpp"

class VKObject {
protected:
    VKEngine* stack = nullptr;

    VKObject(VKEngine* stack) : stack(stack) {}

    virtual ~VKObject() = default;
};