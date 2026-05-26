#pragma once

#include "imgui/imgui.h"
#include "object.hpp"
#include <functional>
#include <initializer_list>
#include <vector>

namespace engine {
class GameView {
    int viewportWidth = 500;
    int viewportHeight = 500;

    unsigned int viewTexture = 0;
    unsigned int FBO = 0;
    unsigned int RBO = 0;

    std::vector<std::reference_wrapper<Object>> objects;

  public:
    GameView();
    void load(const std::initializer_list<std::reference_wrapper<Object>> &&);
    uint32_t render() const;
    glm::mat4 aspectRatio() const;

    auto viewport() const { return ImVec2(viewportWidth, viewportHeight); }
    const auto &getObjects() const { return objects; }
};
} // namespace engine
