#pragma once
#include "renderer.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace engine {
class GUI {
    const Renderer &renderer;

  public:
    GUI(const Renderer &renderer);
    ~GUI();
    void render(ImVec2, uint32_t,
                   const std::vector<std::reference_wrapper<Object>> &) const;
};
} // namespace engine
