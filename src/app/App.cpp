#include "App.hpp"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->getVKStack()->framebufferResized = true;
}

void App::run() {
    setup();
    model = Primitives2D::createPlane();
    model->mesh->loadBuffers(_vkStack);

    mainLoop();
    cleanup();
}

void App::setup() {
    initWindow();
    _vkStack = new VKStack(_window);
    _vkStack->setup();
    _descriptorSetManager = new DescriptorSetManager(_vkStack);
    _descriptorSetManager->initialize();
    _pipelineManager = new PipelineManager(_vkStack, _descriptorSetManager);
    _pipelineManager->initialize();
    _renderer = new Renderer(_vkStack, _pipelineManager, _descriptorSetManager);
    _renderer->prepare();
    _scene = new Scene(_vkStack);
    _scene->load();
}

void App::initWindow() {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(WIDTH, HEIGHT, "Luna App", nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void App::cleanup() {
    Logger::getInstance().log("Cleaning up...");

    delete _descriptorSetManager;
    _descriptorSetManager = nullptr;
    delete _pipelineManager;
    _pipelineManager = nullptr;
    delete _renderer;
    _renderer = nullptr;
    delete _scene;
    _scene = nullptr;

    _vkStack->cleanup();

    if (_window != nullptr) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
    glfwTerminate();
    delete _vkStack;
    _vkStack = nullptr;
}

void App::updateUniformBuffer(VkCommandBuffer commandBuffer, uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // UniformBufferObject ubo{};
    // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.proj = glm::perspective(glm::radians(45.0f), _vkStack->swapChainExtent.width / (float)_vkStack->swapChainExtent.height, 0.1f, 10.0f);
    // ubo.proj[1][1] *= -1;
    // memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    DrawPushConstants drawPushConstants{};
    drawPushConstants.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    vkCmdPushConstants(
        commandBuffer,
        _pipelineManager->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(DrawPushConstants),
        &drawPushConstants
    );
}

void App::mainLoop() {
    if (_window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        loopUpdate();
    }
    vkDeviceWaitIdle(_vkStack->device->vkDevice);
}

void App::loopUpdate() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    _scene->update(deltaTime);
    if (!_renderer->render(_scene, deltaTime)) {
        Logger::getInstance().error("Failed to render scene!");
    }
}