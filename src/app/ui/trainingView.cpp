#include "trainingView.h"
#include "imgui.h"

using namespace vax::ui;

void TrainingView::updateImGui() {
    _uiEngine.get().updateUiStart();
    ImGui::Begin("Training");
    ImGui::End();
    _uiEngine.get().updateUiEnd();
}

void TrainingView::startTraining() {
    std::cout << "Start training" << std::endl;
}