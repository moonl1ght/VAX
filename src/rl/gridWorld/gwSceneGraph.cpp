#include "gwSceneGraph.h"
#include "gridWorldDescriptor.h"
#include "modelDescriptor.h"

using namespace vax::rl::gw;
using namespace vax;
using namespace vax::math;
using namespace vax::rl::math;
using namespace vax::renderer;

bool GwSceneGraph::load(
    objects::ModelsController& modelsController, const vax::rl::gw::env::GridWorldDrawableDescriptor& descriptor
) {
    // auto agentNode = modelsController.getPreloadedSceneNodeById(descriptor.agentDrawableDescriptor.id);
    // if (!agentNode.has_value()) {
    //     _logger.error("Failed to load agent model: {}", descriptor.agentDrawableDescriptor.id);
    //     return false;
    // }
    // _agentNode = std::make_unique<vax::objects::SceneNode>(std::move(agentNode.value()));
    // _roverModelProxy = std::make_unique<vax::rl::models::RoverModelProxy>();
    // _roverModelProxy->linkModelNode(_agentNode);

    // _agentNode->updateTransform([&](TransformHandle& transformHandle) {
    //     transformHandle.updateTransform([&](Transform& transform) {
    //         transform.updateRotationInDegrees({-90.0f, 0.0f, 0.0f});
    //     });
    // });

    _envNodes.reserve(descriptor.drawableDescriptors.size());
    for (const auto& drawableDescriptor : descriptor.drawableDescriptors) {
        auto node = modelsController.createSceneNodeById(drawableDescriptor.id);
        if (!node.has_value()) {
            _logger.error("Failed to load model: {}", drawableDescriptor.id);
            continue;
        }
        _envNodes.push_back(std::move(node.value()));
    }

    return true;
}

void GwSceneGraph::draw(const DrawContext& drawContext) {
    if (_agentNode) {
        _agentNode->draw(drawContext);
    } else {
        _logger.warning("Agent node not loaded!");
    }
    for (auto& node : _envNodes) {
        node.draw(drawContext);
    }
}

void GwSceneGraph::update(float deltaTime) {
    if (_roverModelProxy) {
        _roverModelProxy->update(deltaTime);
    } else {
        _logger.warning("Rover model proxy not loaded!");
    }
}

void GwSceneGraph::moveAgent(Position2DFloat position) {
    if (_agentNode) {
        _agentNode->updateTransform([&](TransformHandle& transformHandle) {
            transformHandle.updateTransform([&](Transform& transform) {
                transform.position = {position.x, 0.0f, position.y};
            });
        });
    } else {
        _logger.warning("Agent node not loaded!");
    }
}