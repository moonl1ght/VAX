#include "gwSceneGraph.h"
#include "gridWorldDescriptor.h"
#include "modelDescriptor.h"

using namespace vax;
using namespace vax::math;
using namespace vax::rl;
using namespace vax::engine;

bool GwSceneGraph::load(
    engine::ModelsController& modelsController, const vax::rl::GridWorldDrawableDescriptor& descriptor
) {
    auto agentNode = modelsController.getPreloadedSceneNodeById(descriptor.agentDrawableDescriptor.id, 1);
    if (!agentNode.has_value()) {
        _logger.error("Failed to load agent model: {}", descriptor.agentDrawableDescriptor.id);
        return false;
    }
    _agentNode = std::make_unique<vax::engine::SceneNode>(std::move(agentNode.value()));
    _roverModelProxy = std::make_unique<vax::rl::RoverModelProxy>();
    _roverModelProxy->linkModelNode(_agentNode);

    _agentNode->updateTransform([&](TransformHandle& transformHandle) {
        transformHandle.updateTransform([&](Transform& transform) {
            transform.updateRotationInDegrees({-90.0f, 0.0f, 0.0f});
        });
    });

    _envNodes.reserve(descriptor.drawableDescriptors.size());
    for (const auto& drawableDescriptor : descriptor.drawableDescriptors) {
        auto node = modelsController.createSceneNodeById(
            drawableDescriptor.id, drawableDescriptor.transforms, drawableDescriptor.selectedInstanceDescriptors
        );
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

void GwSceneGraph::drawSelected(const DrawContext& drawContext) {
    if (_agentNode) {
        _agentNode->isSelected = true;
        _agentNode->draw(drawContext);
    } else {
        _logger.warning("Agent node not loaded!");
    }
    for (auto& node : _envNodes) {
        node.draw(drawContext, true);
    }
}

void GwSceneGraph::update(const engine::FrameTime& frameTime) {
    if (_animations.has_value()) {
        auto isCompleted = _animations->update(frameTime);
        if (_roverModelProxy) {
            _roverModelProxy->update(frameTime);
        } else {
            _logger.warning("Rover model proxy not loaded!");
        }
        if (isCompleted) {
            _animations = std::nullopt;
        }
    }
}

bool GwSceneGraph::isMovingAgent() const { return _animations.has_value(); }

void GwSceneGraph::moveAgentTo(Position2DFloat position, AgentOrientation orientation, bool withAnimation) {
    if (_agentNode) {
        if (withAnimation) {
            _animations = std::make_optional(vax::AnimationGroup(vax::AnimationGroup::Mode::SERIAL));
            auto previousPositionX = std::get<float>(_agentNode->getMetadata("position.x").value_or(position.x));
            auto previousPositionY = std::get<float>(_agentNode->getMetadata("position.y").value_or(position.y));
            auto orientationValue = static_cast<int>(orientation);
            auto previousOrientation = orientationValue;
            if (auto metadata = _agentNode->getMetadata("orientation");
                metadata.has_value() && std::holds_alternative<int>(*metadata)) {
                previousOrientation = std::get<int>(*metadata);
            }
            auto startRotation = _agentNode->getTransform().getRotationInDegrees().y;
            auto orientationDelta = orientationValue - previousOrientation;
            if (orientationDelta != 0) {
                orientationDelta = orientationDelta == 3 ? -1 : orientationDelta == -3 ? 1 : orientationDelta;
                float rotation = startRotation + orientationDelta * 90.0f;
                auto animation = vax::ValueAnimation(1.0f, startRotation, rotation);
                animation.addAnimationHandler([&](float value) {
                    _agentNode->updateTransform([&](TransformHandle& transformHandle) {
                        transformHandle.updateTransform([&](Transform& transform) {
                            transform.updateRotationInDegrees({-90.0f, value, 0.0f});
                        });
                    });
                });
                _animations->pushAnimation(std::move(animation));
            }
            float startPosition;
            float endPosition;
            bool isX = false;
            if (previousPositionX != position.x) {
                isX = true;
                startPosition = previousPositionX;
                endPosition = position.x;
            } else {
                startPosition = previousPositionY;
                endPosition = position.y;
            }
            auto moveAnimation = vax::ValueAnimation(2.0f, startPosition, endPosition);
            moveAnimation.addAnimationHandler([=, this](float value) {
                _agentNode->updateTransform([=](TransformHandle& transformHandle) {
                    transformHandle.updateTransform([=](Transform& transform) {
                        if (isX) {
                            transform.position = {value, 0.0f, position.y};
                        } else {
                            transform.position = {position.x, 0.0f, value};
                        }
                    });
                });
            });
            _animations->pushAnimation(std::move(moveAnimation));
        } else {
            float rotation = 0.0f;
            switch (orientation) {
            case AgentOrientation::NORTH:
                rotation = 90.0f;
                break;
            case AgentOrientation::SOUTH:
                rotation = 270.0f;
                break;
            case AgentOrientation::EAST:
                rotation = 0.0f;
                break;
            case AgentOrientation::WEST:
                rotation = 180.0f;
                break;
            }
            _agentNode->updateTransform([&](TransformHandle& transformHandle) {
                transformHandle.updateTransform([&](Transform& transform) {
                    transform.position = {position.x, 0.0f, position.y};
                    transform.updateRotationInDegrees({-90.0f, rotation, 0.0f});
                });
            });
        }
        _agentNode->setMetadata("orientation", static_cast<int>(orientation));
        _agentNode->setMetadata("position.x", position.x);
        _agentNode->setMetadata("position.y", position.y);
    } else {
        _logger.warning("Agent node not loaded!");
    }
}