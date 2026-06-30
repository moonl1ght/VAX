#include "roverView.h"
#include "imgui.h"
#include "qlConfig.h"

using namespace vax::ui;
using namespace vax::rl;
using namespace vax::vk;
using namespace vax;

void RoverView::updateImGui() {
    _uiEngine.get().updateUiStart();
    ImGui::Begin("Rover");
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

    _drawableScene = std::make_unique<DrawableScene>(engine);
    _drawableScene->resize();
    _drawableScene->loadScene(_gridWorld->getDrawableDescriptor(), engine.queueManager->graphicsQueue);
    _gridWorld->linkSceneGraph(_drawableScene->sceneGraph());
    inputController.addObserver(_drawableScene.get());
    inputController.addObserver(_gridWorld.get());
}