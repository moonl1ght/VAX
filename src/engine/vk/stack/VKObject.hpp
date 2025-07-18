#pragma once

#include "luna.h"
#include "VKStack.hpp"

class VKObject {
protected:
    VKStack* stack = nullptr;

    VKObject(VKStack* stack) : stack(stack) {}

    virtual ~VKObject() = default;
};