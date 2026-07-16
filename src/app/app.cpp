#include "app.h"
#include "notificationCenter.h"
#include "profiler.h"
#include "renderdoc.h"
#include <SDL3/SDL_events.h>

using namespace vax;

bool App::run() {
    if (!_setup()) {
        return false;
    }
    try {
        _mainLoop();
        _cleanup();
    } catch (const std::exception& e) {
        _logger.error("Failed to run app: {}", e.what());
    }
    return true;
}

bool App::_setup() {
    if (!SDL_Vulkan_LoadLibrary(NULL)) {
        _logger.error("Failed to load Vulkan library: {}", SDL_GetError());
        return false;
    }
    RenderDoc::init();
    vax::NotificationCenter::getInstance().setup();
    _windowController = std::make_unique<WindowController>();
    _windowController->setupPrimaryWindow(vax::math::SizeUI{1920, 1080});
    if (!_windowController->getPrimaryWindow()->load(true)) {
        return false;
    }
    _engine = std::make_unique<vk::Engine>(*_windowController->getPrimaryWindow());
    _engine->setup();

    _uiEngine = std::make_unique<ui::UIEngine>(*_engine, *_windowController->getPrimaryWindow());
    _menuView = std::make_unique<ui::MenuView>(*_uiEngine);
    _roverView = std::make_unique<ui::RoverView>(*_uiEngine, *_windowController);
    _trainingView = std::make_unique<ui::TrainingView>(*_uiEngine);

    _renderer = std::make_unique<engine::Renderer>(*_engine, *_uiEngine);
    _renderer->setup();

    return true;
}

void App::_cleanup() {
    _logger.info("Cleaning up...");
    vkDeviceWaitIdle(_engine->device->vkDevice);
    _uiEngine->cleanup();

    _renderer = nullptr;
    _roverView = nullptr;
    _trainingView = nullptr;
    _menuView = nullptr;
    _uiEngine = nullptr;

    _engine->cleanup();

    _windowController->getPrimaryWindow()->destroyWindow();
    if (_windowController->isSecondaryWindowSetup()) {
        _windowController->getSecondaryWindow()->destroyWindow();
    }
    SDL_Quit();
    _logger.info("Cleanup complete!");
}

void App::_mainLoop() {
    if (!_windowController->isPrimaryWindowSetup()) {
        throw std::runtime_error("Window not initialized");
    }
    static bool running = true;
    int i = 0;
    SDL_WindowID primaryWindowID = _windowController->getPrimaryWindow()->getWindowID();

    while (running) {
        SDL_Event event;
        auto processEvent = [&](SDL_Event& event) {
            _inputController.handleEvent(event);
            _uiEngine->processEvents(event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                return false;
            } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                if (event.window.windowID == primaryWindowID) {
                    running = false;
                    return false;
                } else if (_windowController->isSecondaryWindowSetup()) {
                    if (event.window.windowID == _windowController->getSecondaryWindow()->getWindowID()) {
                        _windowController->getSecondaryWindow()->hide();
                        return true;
                    }
                }
            }
            return true;
        };
        if (_appMode == AppMode::Demo) {
            while (SDL_PollEvent(&event)) {
                if (!processEvent(event)) {
                    break;
                }
            }
            // TODO: fix this
            SDL_Delay(16);
            _loopContinuousUpdate();
        } else if (SDL_WaitEvent(&event)) {
            do {
                processEvent(event);
            } while (SDL_PollEvent(&event));
            _loopByEventUpdate();
        }
        FrameMark;
    }
}

void App::_updateTimestamp() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static uint32_t frameCount = 0;
    static auto previousTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    _frameTime.timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    _frameTime.deltaTime =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
    _frameTime.fps = 1.0f / _frameTime.deltaTime;
    ++frameCount;
    _frameTime.frameCount = frameCount;
    previousTime = currentTime;
}

void App::_loopByEventUpdate() {
    ZoneScoped;

    _uiEngine->updateUiStart();
    _menuView->updateImGui();
    _trainingView->updateImGui();
    _uiEngine->updateUiEnd();

    _checkActions();

    _updateTimestamp();

    bool renderResult = false;
    renderResult = _renderer->render(nullptr, _frameTime);

    if (!renderResult) {
        _logger.error("Failed to render scene!");
    }
}

void App::_loopContinuousUpdate() {
    static bool firstTime = true;
    ZoneScoped;

    _roverView->updateImGui();

    _checkActions();

    _updateTimestamp();

    bool renderResult = false;
    vax::engine::SceneUpdateContext sceneUpdateContext{.frameTime = _frameTime};
    if (firstTime) {
        _renderer->prepare(_roverView->drawableScene());
        firstTime = false;
    }
    _roverView->drawableScene()->update(sceneUpdateContext);

    renderResult = _renderer->render(_roverView->drawableScene(), _frameTime);

    if (!renderResult) {
        _logger.error("Failed to render scene!");
    }
}

void App::_checkActions() {
    auto menuViewAction = _menuView->popPendingAction();

    if (menuViewAction) {
        switch (menuViewAction.value()) {
        case ui::MenuView::Action::SHOW_ROVER_DEMO:
            _appMode = AppMode::Demo;
            _roverView->load(*_engine.get(), _inputController);
            break;
        case ui::MenuView::Action::TRAIN_Q_LEARNING:
            _trainingView->startTraining();
            _appMode = AppMode::Training;
            break;
        }
    }
}