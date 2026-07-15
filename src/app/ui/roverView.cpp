#include "roverView.h"
#include "fileSystem.h"
#include "imgui.h"
#include "qlConfig.h"

using namespace vax::ui;
using namespace vax::rl;
using namespace vax::vk;
using namespace vax;

void RoverView::updateImGui() {
    _uiEngine.get().updateUiStart();
    ImGui::Begin("Rover demo");
    ImGui::SetWindowFontScale(1.5f);
    if (_isDemoLoaded) {
        if (_isDemoRunning) {
            ImGui::Text("Demo running");
        } else {
            ImGui::Text("Demo loaded");
            if (ImGui::Button("Start demo", ImVec2(-1, 55))) {
                _startDemo();
            }
            if (ImGui::Button("Change start position", ImVec2(-1, 55))) {
                _changeStartPosition();
            }
            if (ImGui::Button("Back", ImVec2(-1, 55))) {
                _toggleDemo();
            }
        }
    } else {
        if (ImGui::Button("Load demo", ImVec2(-1, 55))) {
            _toggleDemo();
        }
        if (ImGui::Button("Reinit grid", ImVec2(-1, 55))) {
            _reinitGrid();
        }
        if (ImGui::Button("Train", ImVec2(-1, 55))) {
            _startTraining();
        }
        if (_showTrainingStatus) {
            ImGui::Text("%s", _trainingStatus.c_str());
        }
    }
    ImGui::End();
    _uiEngine.get().updateUiEnd();
}

void RoverView::load(Engine& engine, InputController& inputController) {
    _gridWorld = std::make_unique<GridWorld>(QLearningConfig{
        .learningRate = 0.1,
        .gamma = 0.9,
        .epsilon = 0.3,
        .episodes = 100,
    });
    _gridWorld->createRandomGrid();

    _drawableScene = std::make_unique<vax::engine::DrawableScene>(engine);
    _drawableScene->resize();
    _drawableScene->loadScene(_gridWorld->getDrawableDescriptor(), engine.queueManager->graphicsQueue);
    _gridWorld->linkSceneGraph(_drawableScene->sceneGraph());
    inputController.addObserver(_drawableScene.get());
    inputController.addObserver(_gridWorld.get());
}

void RoverView::_startTraining() { _showTrainingStatus = true; }

void RoverView::_toggleDemo() {
    if (_isDemoLoaded) {
        _isDemoLoaded = false;
        return;
    }
    auto trainPath = RELATIVE_PATH("output/qlearning/");
    _isDemoLoaded = !_isDemoLoaded;
    auto latestFolder = vax::fs::getLatestFolder(trainPath);
    if (latestFolder.has_value()) {
        _gridWorld->load(latestFolder.value());
        _isDemoLoaded = true;
    }
}

void RoverView::_reinitGrid() { _gridWorld->reinitWorld(); }

void RoverView::_startDemo() {
    _isDemoRunning = true;
    _gridWorld->startDemo([this]() { _isDemoRunning = false; });
}

void RoverView::_changeStartPosition() { _gridWorld->changeAgentStartPosition(); }