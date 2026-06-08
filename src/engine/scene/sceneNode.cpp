#include "sceneNode.h"

using namespace vax::objects;
using namespace vax;

void SceneNode::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    for (auto& drawableModel : drawableModels) {
        drawableModel.transformHandle.setModelMatrix(
            transformHandle.getModelMatrix() * drawableModel.transformHandle.getModelMatrix()
        );
        drawableModel.draw(commandBuffer, pipelineLayout);
    }
    for (auto& child : children) {
        child.draw(commandBuffer, pipelineLayout);
    }
}