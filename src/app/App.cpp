#include "App.hpp"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->getEngine()->framebufferResized = true;
}

void App::run() {
    setup();
    mainLoop();
    cleanup();
}

bool App::setup() {
    if (!initWindow()) {
        return false;
    }
    _engine = new VKEngine(_window);
    _engine->setup();
    _descriptorSetManager = new DescriptorSetManager(_engine);
    _descriptorSetManager->initialize();
    _pipelineManager = new PipelineManager(_engine, _descriptorSetManager);
    _pipelineManager->initialize();
    _renderer = new Renderer(_engine, _pipelineManager, _descriptorSetManager);
    _renderer->prepare();
    _scene = new Scene(_engine);
    _scene->load();
    return true;
}

bool App::initWindow() {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    // SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    _window = SDL_CreateWindow(
        "Luna",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        window_flags
    );
    if (_window == nullptr) {
        Logger::getInstance().error("Failed to create window");
        return false;
    }
    return true;
}

void App::cleanup() {
    Logger::getInstance().log("Cleaning up...");

    vkDeviceWaitIdle(_engine->device->vkDevice);

    delete _descriptorSetManager;
    _descriptorSetManager = nullptr;
    delete _pipelineManager;
    _pipelineManager = nullptr;
    delete _renderer;
    _renderer = nullptr;
    delete _scene;
    _scene = nullptr;

    _engine->cleanup();

    if (_window != nullptr) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
    SDL_Quit();
    delete _engine;
    _engine = nullptr;
}

void App::mainLoop() {
    if (_window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
        loopUpdate();
    }
}

void App::loopUpdate() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    _scene->update(timestamp);
    if (!_renderer->render(_scene, timestamp)) {
        Logger::getInstance().error("Failed to render scene!");
    }
}