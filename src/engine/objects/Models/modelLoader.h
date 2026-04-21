#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "vkObject.h"

namespace vax::objects {
    class ModelLoader final : public vax::VkObject {
    public:
        ModelLoader(vax::vk::Engine* vkEngine) : vax::VkObject(vkEngine) {};
        ~ModelLoader() = default;

        std::optional<DrawableModel*> loadModel(const std::string& path);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("ModelLoader");
    };
}