#pragma once

#include "commandBuffer.h"
#include "drawContext.h"
#include "gridWorldDescriptor.h"
#include "gwenv.h"
#include "logger.h"
#include "modelLoader.h"
#include "modelsController.h"
#include "vaxMath.h"
#include "roverModelProxy.h"
#include "sceneNode.h"
#include <memory>

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
        vax::objects::ModelsController& modelsController,
        const vax::rl::gw::env::GridWorldDrawableDescriptor& descriptor
    );

    void draw(const vax::renderer::DrawContext& drawContext);

    void update(float deltaTime);

    void moveAgentTo(
        vax::math::Position2DFloat position, vax::rl::gw::AgentOrientation orientation, bool withAnimation = false
    );

  private:
    vax::Logger _logger = vax::Logger("GwSceneGraph");
    std::vector<vax::objects::SceneNode> _envNodes;
    std::shared_ptr<vax::objects::SceneNode> _agentNode;
    std::unique_ptr<vax::rl::models::RoverModelProxy> _roverModelProxy;
};
} // namespace vax::rl::gw