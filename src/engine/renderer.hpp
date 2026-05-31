#pragma once

#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <SDL3/SDL.h>

namespace engine {
using vec2i = std::array<int, 2>;

namespace renderer {
void init();
void cleanup();
vec2i get_viewport();
void set_viewport(const vec2i &);
void closeWindow();
void loop(std::function<void()>);
float zoom();
glm::vec2 scroll();
SDL_Window *window();
SDL_GLContext context();
} // namespace renderer
} // namespace engine
