#include "app.h"
#include "profiler.h"
#include "renderdoc.h"

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

    _uiLayer = std::make_unique<ui::UILayer>(*_engine, *_window);
    _uiLayer->setup();

    _renderer = std::make_unique<renderer::Renderer>(*_engine, *_uiLayer);
    _renderer->prepare();

    _drawableScene = std::make_unique<DrawableScene>(*_engine);
    _drawableScene->resize();
    _drawableScene->load(_engine->queueManager->graphicsQueue);
    _inputController.addObserver(_drawableScene.get());

    return true;
}

void App::cleanup() {
    _logger.info("Cleaning up...");
    vkDeviceWaitIdle(_engine->device->vkDevice);
    _uiLayer->cleanup();

    _renderer = nullptr;
    _drawableScene = nullptr;

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
        while (SDL_PollEvent(&event)) {
            _inputController.handleEvent(event);
            _uiLayer->processEvents(event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }
        }
        SDL_Delay(16);
        loopUpdate();
        FrameMark;
    }
}

void App::loopUpdate() {
    ZoneScoped;
    _uiLayer->update();
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    vax::SceneUpdateContext sceneUpdateContext{
        .deltaTime = timestamp
    };
    _drawableScene->update(sceneUpdateContext);
    if (!_renderer->render(_drawableScene.get(), timestamp)) {
        _logger.error("Failed to render scene!");
    }
}