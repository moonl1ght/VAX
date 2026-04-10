#ifndef DrawableModel_hpp
#define DrawableModel_hpp

#include "luna.h"
#include "Model.hpp"
#include "Mesh.hpp"
#include "pipelineManager.h"
#include "ShaderUniforms.h"

class DrawableModel : public Model {
public:
    std::unique_ptr<Mesh> mesh;

    DrawableModel(std::unique_ptr<Mesh> mesh): mesh(std::move(mesh)) {};
    ~DrawableModel() { };

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    void draw(vax::VkEngine* vkEngine, VkCommandBuffer commandBuffer, vax::PipelineManager* pipelineManager, float time);
};

#endif