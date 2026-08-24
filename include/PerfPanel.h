// ImGui panel showing live physics/render frame-time graphs, built on top of _engine's
// IInspectable — no modification to _engine itself. Realizes the extension point already
// anticipated in engine/IInspectable.h's header comment.
#pragma once

#include "PerfTimer.h"
#include "../engine/include/IInspectable.h"

class PerfPanel : public IInspectable {
public:
    void beginPhysics() { physics_.begin(); }
    void endPhysics() { lastPhysicsMs_ = physics_.end(); }
    void beginRender() { render_.begin(); }
    void endRender() { lastRenderMs_ = render_.end(); }

    void setParticleCount(size_t n) { particleCount_ = n; }

    void showUI() override;

private:
    PerfTimer physics_, render_;
    float lastPhysicsMs_ = 0.0f, lastRenderMs_ = 0.0f;
    size_t particleCount_ = 0;
};
