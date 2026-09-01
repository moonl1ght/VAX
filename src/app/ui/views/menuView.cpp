#include "menuView.h"
#undef Status
#include "imgui.h"

using namespace vax::ui;
using namespace vax;

void MenuView::update(const vax::engine::FrameTime& frameTime) {
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
    if (ImGui::Button("Train Q Learning", ImVec2(-1, 55))) {
        _pendingAction = Action::TRAIN_Q_LEARNING;
    }
    ImGui::Spacing();
    if (ImGui::Button("Physics Engine Demo", ImVec2(-1, 55))) {
        _pendingAction = Action::SHOW_PHYSICS_ENGINE_DEMO;
    }
    ImGui::End();

    if (_trainingView) {
        _trainingView->update(frameTime);
    }
}

AppMode MenuView::getNextAppMode() {
    auto action = _popPendingAction();
    if (action) {
        switch (action.value()) {
        case Action::SHOW_ROVER_DEMO:
            return vax::AppMode::Demo;
        case Action::TRAIN_Q_LEARNING:
            _trainingView = std::make_unique<TrainingView>();
            _trainingView->startTraining();
            return vax::AppMode::Training;
        case Action::SHOW_PHYSICS_ENGINE_DEMO:
            return vax::AppMode::PhysicsDemoMenu;
        }
    }
    return vax::AppMode::Menu;
}