#include "app.h"

using namespace vax;

bool vax::App::run() {
    if (!setup()) {
        return false;
    }
    try {
        mainLoop();
        cleanup();
    }
    catch (const std::exception& e) {
        Logger::getInstance().error("Failed to run app: {}", e.what());
    }
    return true;
}

bool vax::App::setup() {
    _window = std::make_unique<vax::vk::Window>();
    if (!_window->load()) {
        return false;
    }
    _engine = std::make_unique<vax::vk::Engine>(*_window);
    _engine->setup();
    _renderer = std::make_unique<Renderer>(_engine.get());
    _renderer->prepare();
    _scene = std::make_unique<Scene>(_engine.get());
    _scene->load();
    return true;
}

void vax::App::cleanup() {
    _logger.info("Cleaning up...");
    vkDeviceWaitIdle(_engine->device->vkDevice);

    _renderer = nullptr;
    _scene = nullptr;

    _engine->cleanup();

    _window->destroyWindow();
    SDL_Quit();
    _logger.info("Cleanup complete!");
}

void vax::App::mainLoop() {
    if (_window->window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    static bool running = true;
    while (running) {
        // std::cout << "Main loop" << std::endl;
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

void vax::App::loopUpdate() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    _scene->update(timestamp);
    if (!_renderer->render(_scene.get(), timestamp)) {
        Logger::getInstance().error("Failed to render scene!");
    }
}