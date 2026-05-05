#include "textureWorker.h"

using namespace vax::textures;
using namespace vax;

void TextureWorkerInline::transitionTextureLayout(
    Texture& texture,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkImageAspectFlags aspectMask
) {
    VkImageMemoryBarrier2 imageBarrier{ };
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageBarrier.pNext = nullptr;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = oldLayout;
    imageBarrier.newLayout = newLayout;

    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageBarrier.subresourceRange = {
        .aspectMask = aspectMask,
        .baseMipLevel = 0,
        .levelCount = VK_REMAINING_MIP_LEVELS,
        .baseArrayLayer = 0,
        .layerCount = VK_REMAINING_ARRAY_LAYERS
    };
    imageBarrier.image = texture.image();

    VkDependencyInfo depInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = nullptr,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &imageBarrier
    };

    vkCmdPipelineBarrier2(_commandBuffer.get().vkCommandBuffer, &depInfo);
}


void TextureWorker::transitionTextureLayout(
    Texture& texture,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkImageAspectFlags aspectMask
) {
    auto commandBuffer = _commandManager.get().createSingleTimeCommandBuffer();
    auto inlineWorker = TextureWorkerInline(_device.get(), commandBuffer);
    commandBuffer.begin();
    inlineWorker.transitionTextureLayout(texture, oldLayout, newLayout, aspectMask);
    commandBuffer.end();
}


void TextureWorker::transitionTextureLayoutAndSubmit(
    VkQueue submitQueue,
    Texture& texture,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkImageAspectFlags aspectMask
) {
    auto commandBuffer = _commandManager.get().createSingleTimeCommandBuffer();
    auto inlineWorker = TextureWorkerInline(_device.get(), commandBuffer);
    commandBuffer.begin();
    inlineWorker.transitionTextureLayout(texture, oldLayout, newLayout, aspectMask);
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
}