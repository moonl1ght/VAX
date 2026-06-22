#include "gwSceneGraph.h"
#include "gridWorldDescriptor.h"
#include "loaderDescriptor.h"

using namespace vax::rl::gw;
using namespace vax;
using namespace vax::math;
using namespace vax::rl::math;

bool GwSceneGraph::load(
    objects::ModelLoader& modelLoader,
    const vax::rl::gw::env::GridWorldDrawableDescriptor& descriptor,
    VkQueue submitQueue
) {
    std::cout << "Loading agent model: " << submitQueue << std::endl;
    auto agentModel = modelLoader.loadSceneModel(descriptor.agentDrawableDescriptor, submitQueue);
    std::cout << "Agent model loaded" << std::endl;
    agentModel->updateTransform([](vax::math::TransformHandle& transformHandle) {
        transformHandle.updateTransform([](vax::math::Transform& transform) {
            transform.updateRotationInDegrees({-90.0f, 0.0f, 0.0f});
        });
    });
    if (!agentModel.has_value()) {
        _logger.error("Failed to load agent model: {}", descriptor.agentDrawableDescriptor.path);
        return false;
    }
    _agentNode = std::make_unique<vax::objects::SceneNode>(std::move(agentModel.value()));
    _roverModelProxy = std::make_unique<vax::rl::models::RoverModelProxy>();
    _roverModelProxy->linkModelNode(_agentNode);

    _envNodes.reserve(descriptor.drawableDescriptors.size());
    for (const auto& drawableDescriptor : descriptor.drawableDescriptors) {
        std::cout << "Loading model: " << drawableDescriptor.path << std::endl;
        auto model = modelLoader.loadSceneModel(drawableDescriptor, submitQueue);
        if (!model.has_value()) {
            _logger.error("Failed to load model: {}", drawableDescriptor.path);
            continue;
        }
        model->updateTransform([&](vax::math::TransformHandle& transformHandle) {
            transformHandle.updateTransform([&](vax::math::Transform& transform) {
                transform.position = drawableDescriptor.initialTransform.position;
                transform.rotation = drawableDescriptor.initialTransform.rotation;
                transform.scale = drawableDescriptor.initialTransform.scale;
            });
        });
        _envNodes.push_back(std::move(model.value()));
    }
    return true;
}

void GwSceneGraph::draw(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline) {
    if (_agentNode) {
        _agentNode->draw(commandBuffer, pipeline.vkPipelineLayout);
    } else {
        _logger.warning("Agent node not loaded!");
    }
    for (auto& node : _envNodes) {
        node.draw(commandBuffer, pipeline.vkPipelineLayout);
    }
}

void GwSceneGraph::loadDrawableModels(vax::vk::CommandBuffer& commandBuffer) {
    if (_agentNode) {
        _agentNode->loadDrawableModelsMeshes(commandBuffer);
    } else {
        _logger.warning("Agent node not loaded!");
    }
    for (auto& node : _envNodes) {
        node.loadDrawableModelsMeshes(commandBuffer);
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