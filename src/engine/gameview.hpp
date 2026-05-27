#pragma once

#include "imgui/imgui.h"
#include "object.hpp"
#include "scene.hpp"

namespace engine {
class GameView {
    int viewportWidth = 500;
    int viewportHeight = 500;

    unsigned int viewTexture = 0;
    unsigned int FBO = 0;
    unsigned int RBO = 0;

  public:
    GameView();
    uint32_t render(const Scene &) const;
    glm::mat4 aspectRatio() const;

    auto viewport() const { return ImVec2(viewportWidth, viewportHeight); }
};
} // namespace engine
