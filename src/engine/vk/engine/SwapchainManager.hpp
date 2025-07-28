#pragma once

#include "luna.h"
#include "VKObject.hpp"

class SwapchainManager final : public VKObject {
public:
    SwapchainManager(VKEngine* engine) : VKObject(engine) {};
};