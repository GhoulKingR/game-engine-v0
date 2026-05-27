#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {
namespace gui {
void init(GLFWwindow *);
void close();
void render(uint32_t);
} // namespace gui
} // namespace engine
