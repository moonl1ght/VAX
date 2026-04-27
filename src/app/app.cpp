#include "app.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

using namespace vax;

bool App::run() {
    if (!setup()) {
        return false;
    }
    try {
        mainLoop();
        cleanup();
    }
    catch (const std::exception& e) {
        _logger.error("Failed to run app: {}", e.what());
    }
    return true;
}

bool App::setup() {
    _window = std::make_unique<vk::Window>();
    if (!_window->load()) {
        return false;
    }
    _engine = std::make_unique<vk::Engine>(*_window);
    _engine->setup();
    _renderer = std::make_unique<Renderer>(*_engine);
    _renderer->prepare();
    _scene = std::make_unique<Scene>(*_engine);
    _scene->load();

    ImGui::CreateContext();

    VkDescriptorPoolSize pool_size = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    vkCreateDescriptorPool(_engine->device->vkDevice, &pool_info, nullptr, &_imguiDescriptorPool);

    ImGui_ImplSDL3_InitForVulkan(_window->window);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _engine->instance;
    init_info.PhysicalDevice = _engine->device->vkPhysicalDevice;
    init_info.Device = _engine->device->vkDevice;
    init_info.QueueFamily = _engine->device->getQueueFamilyIndices().graphicsFamily.value();
    init_info.Queue = _engine->queueManager->graphicsQueue;
    init_info.DescriptorPool = _imguiDescriptorPool;
    init_info.RenderPass = _engine->renderPass->getVkRenderPass();
    init_info.MinImageCount = 2;
    init_info.ImageCount = static_cast<uint32_t>(_engine->swapchain->swapchainImages.size());
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info);

    return true;
}

void App::cleanup() {
    _logger.info("Cleaning up...");
    vkDeviceWaitIdle(_engine->device->vkDevice);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(_engine->device->vkDevice, _imguiDescriptorPool, nullptr);

    _renderer = nullptr;
    _scene = nullptr;

    _engine->cleanup();

    _window->destroyWindow();
    SDL_Quit();
    _logger.info("Cleanup complete!");
}

void App::mainLoop() {
    if (_window->window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    static bool running = true;
    while (running) {
        SDL_Event event;
        try {
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL3_ProcessEvent(&event);
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                    break;
                }
            }
        }
        catch (const std::exception& e) {
            _logger.error("Failed to poll events: {}", e.what());
        }
        SDL_Delay(16);
        loopUpdate();
    }
}

void App::loopUpdate() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Performance");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    _scene->update(timestamp);
    if (!_renderer->render(_scene.get(), timestamp, draw_data)) {
        _logger.error("Failed to render scene!");
    }
}