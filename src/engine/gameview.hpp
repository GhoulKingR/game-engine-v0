#pragma once

#include <glm/glm.hpp>

namespace engine {
using vec2 = std::array<int, 2>;
namespace gameview {
void init();
glm::mat4 calculate_aspect_ratio();
uint32_t render();

engine::vec2 getviewport();
void set_viewport(engine::vec2 val);
} // namespace gameview
} // namespace engine
