#include "app.h"
#include "renderdoc.h"
#include "notificationCenter.h"
#include "profiler.h"

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
    RenderDoc::init();
    vax::NotificationCenter::getInstance().setup();
    _window = std::make_unique<vk::Window>();
    if (!_window->load()) {
        return false;
    }
    _engine = std::make_unique<vk::Engine>(*_window);
    _engine->setup();

    _uiEngine = std::make_unique<ui::UIEngine>(*_engine, *_window);
    _uiEngine->setup();
    _menuView = std::make_unique<ui::MenuView>(*_uiEngine);
    _roverView = std::make_unique<ui::RoverView>(*_uiEngine);
    _trainingView = std::make_unique<ui::TrainingView>(*_uiEngine);

    _renderer = std::make_unique<engine::Renderer>(*_engine, *_uiEngine);

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

    _window->destroyWindow();
    SDL_Quit();
    _logger.info("Cleanup complete!");
}

void App::_mainLoop() {
    if (_window->window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    static bool running = true;
    int i = 0;
    while (running) {
        SDL_Event event;
        if (_appMode == AppMode::Demo) {
            while (SDL_PollEvent(&event)) {
                _inputController.handleEvent(event);
                _uiEngine->processEvents(event);
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                    break;
                }
            }
            // TODO: fix this
            SDL_Delay(16);
            _loopContinuousUpdate();
        } else if (SDL_WaitEvent(&event)) {
            do {
                _inputController.handleEvent(event);
                _uiEngine->processEvents(event);
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                    break;
                }
            } while (SDL_PollEvent(&event));
            _loopByEventUpdate();
        }
        FrameMark;
    }
}

void App::_updateTimestamp() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    _timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
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
    renderResult = _renderer->render(nullptr, _timestamp);

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
    vax::engine::SceneUpdateContext sceneUpdateContext{.deltaTime = _timestamp};
    if (firstTime) {
        _renderer->prepare(_roverView->drawableScene());
        firstTime = false;
    }
    _roverView->drawableScene()->update(sceneUpdateContext);

    renderResult = _renderer->render(_roverView->drawableScene(), _timestamp);

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