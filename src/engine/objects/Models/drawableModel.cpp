#include "drawableModel.h"
#include "vkEngine.h"

using namespace vax::objects;

void DrawableModel::draw(
    vax::vk::Engine* vkEngine,
    VkCommandBuffer commandBuffer,
    const vax::vk::PipelineManager& pipelineManager,
    float time
) {
    DrawPushConstants drawPushConstants{};
    // drawPushConstants.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    // drawPushConstants.model = transform.getModelMatrix();
    drawPushConstants.model = glm::rotate(
        transform.getModelMatrix(), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f)
    );
    drawPushConstants.flags = ObjectFlags::None;
    vkCmdPushConstants(
        commandBuffer,
        pipelineManager.getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(DrawPushConstants),
        &drawPushConstants
    );
    _mesh->forceDraw(*vkEngine->queueManager, *vkEngine->commandManager, commandBuffer);
}