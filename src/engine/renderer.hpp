#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {
using vec2 = std::array<int, 2>;
namespace renderer {
GLFWwindow *init_glfw();
void close_glfw(GLFWwindow *);
vec2 get_viewport();
void set_viewport(const vec2 &);
} // namespace renderer
} // namespace engine
