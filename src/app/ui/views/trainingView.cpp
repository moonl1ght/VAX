#include "trainingView.h"
#undef Status
#include "imgui.h"

using namespace vax::ui;
using namespace vax::rl;

void TrainingView::update(const vax::engine::FrameTime& frameTime) {
    _mainThreadRunner.processThreadQueue();
    ImGui::Begin("Training");
    ImGui::SetNextWindowSize(ImVec2(480, 220), ImGuiCond_Always);
    ImGui::Text("%s", _trainingStatus.c_str());
    ImGui::End();
}

void TrainingView::startTraining() {
    _trainingManager = std::make_unique<vax::rl::GWTrainingManager>();
    _trainingManager->startTraining(_mainThreadRunner, [this](TrainingStatus trainingStatus) {
        _trainingStatus = trainingStatus.message;
        if (trainingStatus.isCompleted) {
            _trainingManager.reset();
        }
    });
}