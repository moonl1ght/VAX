#include "uiLayer.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

using namespace vax::ui;
using namespace vax;

void UILayer::setup() {
    ImGui::CreateContext();

    VkDescriptorPoolSize pool_size = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 };
    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &pool_size,
    };
    vkCreateDescriptorPool(_vkEngine.get().device->vkDevice, &pool_info, nullptr, &_imguiDescriptorPool);

    ImGui_ImplSDL3_InitForVulkan(_window.get().window);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _vkEngine.get().instance;
    init_info.PhysicalDevice = _vkEngine.get().device->vkPhysicalDevice;
    init_info.Device = _vkEngine.get().device->vkDevice;
    init_info.QueueFamily = _vkEngine.get().device->getQueueFamilyIndices().graphicsFamily.value();
    init_info.Queue = _vkEngine.get().queueManager->graphicsQueue;
    init_info.DescriptorPool = _imguiDescriptorPool;
    init_info.RenderPass = _vkEngine.get().renderPass->getVkRenderPass();
    init_info.MinImageCount = 2;
    init_info.ImageCount = static_cast<uint32_t>(_vkEngine.get().swapchain->swapchainImages.size());
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info);
}

void UILayer::cleanup() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(_vkEngine.get().device->vkDevice, _imguiDescriptorPool, nullptr);
}

void UILayer::processEvents(SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void UILayer::update() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Performance");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();
    _imguiDrawData = ImGui::GetDrawData();
}

void UILayer::render(VkCommandBuffer commandBuffer) {
    if (_imguiDrawData == nullptr) {
        return;
    }
    ImGui_ImplVulkan_RenderDrawData(_imguiDrawData, commandBuffer);
}