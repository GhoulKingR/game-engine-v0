#pragma once

#include <glm/glm.hpp>
#include "scene.hpp"

namespace engine {
class GameView {
    unsigned int viewTexture = 0;
    unsigned int FBO = 0;
    unsigned int RBO = 0;

  public:
    int viewportWidth = 500;
    int viewportHeight = 500;

    GameView();
    uint32_t render(const Scene &) const;
    glm::mat4 aspectRatio() const;
};
} // namespace engine
