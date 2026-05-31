#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>

namespace engine {
namespace gui {
void init();
void close();
void render();
} // namespace gui
} // namespace engine
