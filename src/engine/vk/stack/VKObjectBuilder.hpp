#pragma once

#include "VKEngine.hpp"

class VKObjectBuilder {
public:
    VKObjectBuilder(VKEngine* vkEngine) : vkEngine(vkEngine) {}

    VKEngine* vkEngine;
};