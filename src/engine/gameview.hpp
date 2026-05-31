#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace engine {
using vec2i = std::array<int, 2>;

namespace gameview {
void init();
void cleanup();
glm::mat4 calculate_aspect_ratio();
void render();
void renderGUI();
bool is_preview();
void reset();

engine::vec2i getviewport();
void set_viewport(engine::vec2i val);
} // namespace gameview
} // namespace engine
