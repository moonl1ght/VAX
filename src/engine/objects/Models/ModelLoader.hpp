#pragma once

#include "luna.h"
#include "DrawableModel.hpp"
#include "VKObject.hpp"

class ModelLoader final: public VKObject {
public:
    ModelLoader(VKEngine* stack) : VKObject(stack) {};
    ~ModelLoader() = default;

    std::optional<DrawableModel*> loadModel(const std::string& path);
};