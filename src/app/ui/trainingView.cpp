#include "trainingView.h"
#include "imgui.h"

using namespace vax::ui;
using namespace vax::rl::gw;

void TrainingView::updateImGui() {
    _mainThreadRunner.processThreadQueue();
    ImGui::Begin("Training");
    ImGui::SetNextWindowSize(ImVec2(480, 220), ImGuiCond_Always);
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("%s", _trainingStatus.c_str());
    ImGui::End();
}

void TrainingView::startTraining() {
    _trainingManager = std::make_unique<vax::rl::gw::GWTrainingManager>();
    _trainingManager->startTraining(_mainThreadRunner, [this](TrainingStatus trainingStatus) {
        _trainingStatus = trainingStatus.message;
        if (trainingStatus.isCompleted) {
            _trainingManager.reset();
        }
    });
}