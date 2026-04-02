#include "App.hpp"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->getEngine()->framebufferResized = true;
}

void App::run() {
    try {
        setup();
        mainLoop();
        cleanup();
    }
    catch (const std::exception& e) {
        Logger::getInstance().error("Failed to run app: {}", e.what());
    }
}

bool App::setup() {
    if (!initWindow()) {
        return false;
    }
    _engine = new vax::VkEngine(_window);
    _engine->setup();
    _renderer = new Renderer(_engine);
    _renderer->prepare();
    _scene = new Scene(_engine);
    _scene->load();
    return true;
}

bool App::initWindow() {
    try {
        if (!SDL_Vulkan_LoadLibrary(NULL)) {
            std::cout << "Failed to load Vulkan library: " << SDL_GetError() << std::endl;
            return false;
        }
        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
        _window = SDL_CreateWindow(
            "Luna",
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
    catch (const std::exception& e) {
        Logger::getInstance().error("Failed to create window: {}", e.what());
        return false;
    }
}

void App::cleanup() {
    // Logger::getInstance().log("Cleaning up...");

    vkDeviceWaitIdle(_engine->device->vkDevice);
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
    static bool running = true;
    while (running) {
        std::cout << "Main loop" << std::endl;
        SDL_Event event;
        try {
            // std::cout << "Error: " << SDL_GetError() << std::endl;
            while (SDL_PollEvent(&event)) {
                // std::cout << "Error: " << SDL_GetError() << std::endl;
                // std::cout << "Event: " << event.type << std::endl;
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                    break;
                }
            }
        }
        catch (const std::exception& e) {
            Logger::getInstance().error("Failed to poll events: {}", e.what());
        }
        SDL_Delay(16); 
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