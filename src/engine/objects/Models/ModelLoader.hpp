#pragma once

#include "luna.h"
#include "DrawableModel.hpp"
#include "VKObject.hpp"

class ModelLoader final: public VKObject {
public:
    ModelLoader(vax::VkEngine* vkEngine) : VKObject(vkEngine) {};
    ~ModelLoader() = default;

    std::optional<DrawableModel*> loadModel(const std::string& path);
};