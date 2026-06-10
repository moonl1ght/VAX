#pragma once

#include "gridWorld.h"
#include "logger.h"
#include "modelLoader.h"
#include "sceneNode.h"
#include "commandBuffer.h"
#include "pipeline.h"

namespace vax::rl::gw {
// TODO: move to generic scene graph
class GwSceneGraph final {
  public:
    GwSceneGraph() {};

    ~GwSceneGraph() = default;

    GwSceneGraph(const GwSceneGraph& other) = delete;
    GwSceneGraph& operator=(const GwSceneGraph& other) = delete;
    GwSceneGraph(GwSceneGraph&& other) noexcept = delete;
    GwSceneGraph& operator=(GwSceneGraph&& other) noexcept = delete;

    bool load(vax::objects::ModelLoader& modelLoader, const env::GridWorld& gridWorld, VkQueue submitQueue);
    // TODO: refactor models loading
    void loadDrawableModels(vax::vk::CommandBuffer& commandBuffer);

    void draw(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("GwSceneGraph");
    std::vector<vax::objects::SceneNode> _envNodes;
    std::unique_ptr<vax::objects::SceneNode> _agentNode;
};
} // namespace vax::rl::gw