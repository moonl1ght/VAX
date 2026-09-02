#include "statsView.h"

using namespace vax::ui;

void StatsView::update(const vax::engine::FrameTime& frameTime) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImVec2 top_right = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 10.0f, viewport->WorkPos.y + 10.0f);

    ImGui::SetNextWindowPos(top_right, ImGuiCond_FirstUseEver, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(480, 250), ImGuiCond_FirstUseEver);
    ImGui::Begin("Stats");

    showSystemInfo();
    ImGui::End();
}

void StatsView::showSystemInfo() const {
    SystemInfo::MemoryStats memoryStats = _systemInfo.getRAMStats();
    ImGui::Text("RAM usage: %.2f%% (%dMB / %dMB)", memoryStats.usagePercentage, memoryStats.used, memoryStats.total);

    ImGui::Separator();

    SystemInfo::GPUStats gpuStats = _systemInfo.getGPUStats();
    ImGui::Text("GPU Info:");
    ImGui::Text("%s", gpuStats.name.c_str());
    ImGui::Text(
        "Usage: %.2f%% (%dMB / %dMB)",
        gpuStats.memoryStats.usagePercentage,
        gpuStats.memoryStats.used,
        gpuStats.memoryStats.total
    );
}