#ifndef DrawableModel_hpp
#define DrawableModel_hpp

#include "luna.h"
#include "model.h"
#include "mesh.h"
#include "pipelineManager.h"
#include "ShaderUniforms.h"

class DrawableModel : public Model {
public:
    std::unique_ptr<vax::objects::Mesh> mesh;

    DrawableModel(std::unique_ptr<vax::objects::Mesh> mesh): mesh(std::move(mesh)) {};
    ~DrawableModel() { };

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    void draw(vax::vk::Engine* vkEngine, VkCommandBuffer commandBuffer, const vax::vk::PipelineManager& pipelineManager, float time);
};

#endif