#include "roverView.h"
#include "imgui.h"

using namespace vax::ui;

void RoverView::updateImGui() {
    _uiEngine.get().updateUiStart();
    ImGui::Begin("Rover");
    ImGui::End();
    _uiEngine.get().updateUiEnd();
}