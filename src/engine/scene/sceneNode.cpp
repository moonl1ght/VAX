#include "sceneNode.h"

using namespace vax::objects;
using namespace vax;

void SceneNode::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    // auto parentTransform = this->parentTransformMatrices.modelMatrix * this->transformHandle.getModelMatrix();
    for (auto& drawableModel : drawableModels) {
        drawableModel.transformMatrixHandle = parentTransformMatrices;
        drawableModel.draw(commandBuffer, pipelineLayout);
    }
    for (auto& child : children) {
        // child.parentTransform = parentTransform;
        child.draw(commandBuffer, pipelineLayout);
    }
}