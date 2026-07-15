#pragma once

#include "animationGroup.h"
#include "commandBuffer.h"
#include "drawContext.h"
#include "frameTime.h"
#include "gridWorldDescriptor.h"
#include "gwenv.h"
#include "logger.h"
#include "modelLoader.h"
#include "modelsController.h"
#include "roverModelProxy.h"
#include "sceneNode.h"
#include "vaxMath.h"
#include <memory>

namespace vax::rl {
// TODO: move to generic scene graph
class GwSceneGraph final {
  public:
    GwSceneGraph() {};

    ~GwSceneGraph() = default;

    GwSceneGraph(const GwSceneGraph& other) = delete;
    GwSceneGraph& operator=(const GwSceneGraph& other) = delete;
    GwSceneGraph(GwSceneGraph&& other) noexcept = delete;
    GwSceneGraph& operator=(GwSceneGraph&& other) noexcept = delete;

    bool load(vax::engine::ModelsController& modelsController, const vax::rl::GridWorldDrawableDescriptor& descriptor);

    void draw(const vax::engine::DrawContext& drawContext);

    void update(const vax::engine::FrameTime& frameTime);

    void moveAgentTo(
        vax::math::Position2DFloat position,
        vax::rl::AgentOrientation orientation,
        bool withAnimation,
        float moveSpeed = 2.0f,
        float rotationSpeed = 1.0f
    );

    bool isMovingAgent() const;

    void resetInstancesHighlight(std::string instanceId);

    void highlightInstance(std::string instanceId, uint32_t instanceIndex, vax::engine::Color color);

    void setOnAllAnimationsCompleted(std::function<void()> onAllAnimationsCompleted);

  private:
    vax::Logger _logger = vax::Logger("GwSceneGraph");
    std::vector<vax::engine::SceneNode> _envNodes;
    std::shared_ptr<vax::engine::SceneNode> _agentNode;
    std::unique_ptr<vax::rl::RoverModelProxy> _roverModelProxy;
    std::optional<vax::AnimationGroup> _animations;
    std::optional<std::function<void()>> _onAllAnimationsCompleted;
};
} // namespace vax::rl