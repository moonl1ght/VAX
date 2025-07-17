#include "App.hpp"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->getVKStack()->framebufferResized = true;
}

void App::run() {
    setup();
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