#pragma once

#include "luna.h"
#include "model.h"
#include "mesh.h"
#include "pipelineManager.h"
#include "shaderUniforms.h"

namespace vax::objects {
    class DrawableModel : public Model {
    public:
        struct DrawContext {

        };

        std::unique_ptr<vax::objects::Mesh> mesh;

        explicit DrawableModel(std::unique_ptr<vax::objects::Mesh> mesh) : mesh(std::move(mesh)) {};

        ~DrawableModel() {};

        DrawableModel(const DrawableModel& other) = delete;
        DrawableModel& operator=(const DrawableModel& other) = delete;

        void draw(vax::vk::Engine* vkEngine, VkCommandBuffer commandBuffer, const vax::vk::PipelineManager& pipelineManager, float time);
    };
}