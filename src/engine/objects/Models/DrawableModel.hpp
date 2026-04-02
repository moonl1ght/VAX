#ifndef DrawableModel_hpp
#define DrawableModel_hpp

#include "luna.h"
#include "Model.hpp"
#include "Mesh.hpp"
#include "PipelineManager.hpp"
#include "ShaderUniforms.h"

class DrawableModel : public Model {
public:
    std::unique_ptr<Mesh> mesh;

    DrawableModel(std::unique_ptr<Mesh> mesh): mesh(std::move(mesh)) {};
    ~DrawableModel() { };

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    void draw(vax::VkEngine* vkEngine, VkCommandBuffer commandBuffer, PipelineManager* pipelineManager, float time);
};

#endif