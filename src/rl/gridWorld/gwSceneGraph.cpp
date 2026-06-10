#include "gwSceneGraph.h"
#include "gridWorld.h"
#include "gridWorldDescriptor.h"
#include "loaderDescriptor.h"

using namespace vax::rl::gw;
using namespace vax;

bool GwSceneGraph::load(objects::ModelLoader& modelLoader, const env::GridWorld& gridWorld, VkQueue submitQueue) {
    auto gridWorldDrawableDescriptor = gridWorld.getDrawableDescriptor();
    auto agentModel = modelLoader.loadSceneModel(gridWorldDrawableDescriptor.agentDrawableDescriptor, submitQueue);
    agentModel->updateTransform([](vax::math::TransformHandle& transformHandle) {
        transformHandle.updateTransform([](vax::math::Transform& transform) {
            transform.updateRotationInDegrees({-90.0f, 0.0f, 0.0f});
        });
    });
    if (!agentModel.has_value()) {
        _logger.error("Failed to load agent model: {}", gridWorldDrawableDescriptor.agentDrawableDescriptor.path);
        return false;
    }
    _agentNode = std::make_unique<vax::objects::SceneNode>(std::move(agentModel.value()));

   return true;
}

void GwSceneGraph::draw(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline) {
    if (_agentNode) {
        _agentNode->draw(commandBuffer, pipeline.vkPipelineLayout);
    } else {
        _logger.warning("Agent node not loaded!");
    }
}

void GwSceneGraph::loadDrawableModels(vax::vk::CommandBuffer& commandBuffer) {
    if (_agentNode) {
        _agentNode->loadDrawableModelsMeshes(commandBuffer);
    } else {
        _logger.warning("Agent node not loaded!");
    }
}