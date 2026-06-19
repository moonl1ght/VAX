#include "menuView.h"
#include "imgui.h"

using namespace vax::ui;

void MenuView::updateImGui() {
    _uiEngine.get().updateUiStart();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)
    );
    ImGui::SetNextWindowSize(ImVec2(480, 220), ImGuiCond_Always);
    ImGui::Begin(
        "VAX",
        nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar
    );
    ImGui::SetWindowFontScale(2.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    if (ImGui::Button("Show Rover Demo", ImVec2(-1, 55))) {
        _pendingAction = Action::SHOW_ROVER_DEMO;
    }
    ImGui::Spacing();
    ImGui::BeginDisabled();
    if (ImGui::Button("Train Q Learning", ImVec2(-1, 55))) {
        _pendingAction = Action::TRAIN_Q_LEARNING;
    }
    ImGui::EndDisabled();
    ImGui::End();
    _uiEngine.get().updateUiEnd();
}