#pragma once

#include "camera.h"
#include "frameTime.h"
#include "logger.h"
#include "node.h"
#include "sceneNode.h"

namespace vax::rl {
class GWAgentNode final {
  public:
    explicit GWAgentNode(std::unique_ptr<vax::engine::SceneNode> agentNode)
        : _agentNode(std::move(agentNode)) {
        _camera = vax::engine::Camera(
            vax::math::SizeUI(640, 480), vax::engine::Camera::Projection::perspective, glm::vec3(2.0f, 0.0f, 0.0f)
        );
    };

    GWAgentNode(GWAgentNode&& other) noexcept = default;
    GWAgentNode& operator=(GWAgentNode&& other) noexcept = default;

    GWAgentNode(const GWAgentNode& other) = delete;
    GWAgentNode& operator=(const GWAgentNode& other) = delete;

    ~GWAgentNode() = default;

    void update(const vax::engine::FrameTime& frameTime);

    vax::engine::SceneNode& agentNode() { return *_agentNode; }

    const vax::engine::SceneNode& agentNode() const { return *_agentNode; }

    vax::engine::Camera& camera() { return _camera; }

    const vax::engine::Camera& camera() const { return _camera; }

  private:
    vax::Logger _logger = vax::Logger("GWAgentNode");
    vax::engine::Camera _camera;
    std::unique_ptr<vax::engine::SceneNode> _agentNode;
    vax::engine::Node* _rightFrontWheel;
    vax::engine::Node* _leftFrontWheel;
    vax::engine::Node* _rightRearWheel;
    vax::engine::Node* _leftRearWheel;
    float _wheelSpinAngle = 0.0f;
};
} // namespace vax::rl