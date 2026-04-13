#pragma once

#include "luna.h"
#include "DrawableModel.hpp"
#include "vkObject.h"

class ModelLoader final: public vax::VkObject {
public:
    ModelLoader(vax::vk::Engine* vkEngine) : vax::VkObject(vkEngine) {};
    ~ModelLoader() = default;

    std::optional<DrawableModel*> loadModel(const std::string& path);
};