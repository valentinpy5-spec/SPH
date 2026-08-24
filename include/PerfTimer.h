// Minimal wall-clock timer with a rolling history, used to feed PerfPanel's live graphs.
// Pure std::chrono — no GL/ImGui dependency, so it stays usable outside a render context too.
#pragma once

#include <chrono>
#include <deque>
#include <numeric>

class PerfTimer {
public:
    void begin() { t0_ = std::chrono::high_resolution_clock::now(); }

    // Returns the elapsed time in milliseconds since begin(), and appends it to history().
    float end() {
        float ms = std::chrono::duration<float, std::milli>(
                       std::chrono::high_resolution_clock::now() - t0_)
                       .count();
        history_.push_back(ms);
        if (history_.size() > kHistorySize) history_.pop_front();
        return ms;
    }

    const std::deque<float> &history() const { return history_; }

    float average() const {
        if (history_.empty()) return 0.0f;
        return std::accumulate(history_.begin(), history_.end(), 0.0f) / (float)history_.size();
    }

private:
    std::chrono::high_resolution_clock::time_point t0_;
    std::deque<float> history_;
    static constexpr size_t kHistorySize = 120; // ~2s at 60 FPS
};
