#include "DrawableModel.hpp"

void DrawableModel::draw(
    VKEngine* vkEngine, VkCommandBuffer commandBuffer, PipelineManager* pipelineManager, float time
) {
    DrawPushConstants drawPushConstants{};
    // drawPushConstants.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    // drawPushConstants.model = transform.getModelMatrix();
    drawPushConstants.model = glm::rotate(
        transform.getModelMatrix(), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f)
    );
    vkCmdPushConstants(
        commandBuffer,
        pipelineManager->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(DrawPushConstants),
        &drawPushConstants
    );
    mesh->draw(vkEngine, commandBuffer);
}