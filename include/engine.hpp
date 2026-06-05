#pragma once

#include "scene.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace engine {
    void init(const char *title, uint32_t w, uint32_t h);
    void loadScene(Scene *);
    void start();
    void cleanup();
    glm::mat4 aspectRatio();
}
