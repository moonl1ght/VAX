#pragma once

#include "commandBuffer.h"
#include "gridWorldDescriptor.h"
#include "logger.h"
#include "modelLoader.h"
#include "pipeline.h"
#include "roverModelProxy.h"
#include "sceneNode.h"
#include <memory>
#include "rlMath.h"
#include "drawContext.h"

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

    bool load(
        vax::objects::ModelLoader& modelLoader,
        const vax::rl::gw::env::GridWorldDrawableDescriptor& descriptor,
        VkQueue submitQueue
    );

    void loadDrawableModels(const vax::objects::MeshPBR::LoadMeshBuffersContext& context);

    void draw(const vax::renderer::DrawContext& drawContext);

    void update(float deltaTime);

    void moveAgent(rl::math::Position2DFloat position);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("GwSceneGraph");
    std::vector<vax::objects::SceneNode> _envNodes;
    std::shared_ptr<vax::objects::SceneNode> _agentNode;
    std::unique_ptr<vax::rl::models::RoverModelProxy> _roverModelProxy;
};
} // namespace vax::rl::gw