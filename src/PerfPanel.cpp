#include "../include/PerfPanel.h"

#include "../engine/third_party/imgui/imgui.h"

namespace {

float historyGetter(void *data, int idx) {
    auto *history = static_cast<const std::deque<float> *>(data);
    return (*history)[(size_t)idx];
}
} 

void PerfPanel::showUI() {
    ImGui::Begin("Performance");

    float frameMs = lastPhysicsMs_ + lastRenderMs_;
    float fps = frameMs > 0.0f ? 1000.0f / frameMs : 0.0f;


    float avgFrameMs = physics_.average() + render_.average();
    float avgFps = avgFrameMs > 0.0f ? 1000.0f / avgFrameMs : 0.0f;

    ImGui::Text("Particles (fluid): %zu", particleCount_);
    ImGui::Text("FPS: %.1f (avg %.1f)", fps, avgFps);
    ImGui::Text("Physics: %.2f ms (avg %.2f ms)", lastPhysicsMs_, physics_.average());
    ImGui::Text("Render:  %.2f ms (avg %.2f ms)", lastRenderMs_, render_.average());

    const auto &physicsHistory = physics_.history();
    if (!physicsHistory.empty()) {
        ImGui::PlotLines("Physics (ms)", &historyGetter, (void *)&physicsHistory,
                          (int)physicsHistory.size(), 0, nullptr, 0.0f, FLT_MAX, ImVec2(0, 60));
    }

    const auto &renderHistory = render_.history();
    if (!renderHistory.empty()) {
        ImGui::PlotLines("Render (ms)", &historyGetter, (void *)&renderHistory,
                          (int)renderHistory.size(), 0, nullptr, 0.0f, FLT_MAX, ImVec2(0, 60));
    }

    ImGui::End();
}
