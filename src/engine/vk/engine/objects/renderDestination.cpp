#include "renderDestination.h"

using namespace vax::vk;

void vax::vk::RenderDestination::destroy() {
    for (auto framebuffer : swapchainFramebuffers) {
        vkDestroyFramebuffer(_device.get().vkDevice, framebuffer, nullptr);
    }
}