#include "DrawableModel.hpp"

void DrawableModel::draw(VKStack* vkStack, VkCommandBuffer commandBuffer, PipelineManager* pipelineManager, float time) {
    mesh->draw(vkStack, commandBuffer, pipelineManager, time);
}