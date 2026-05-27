#pragma once
#include "gameview.hpp"
#include "node/node.hpp"
#include "renderer.hpp"
#include "scene.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace engine {
class GUI {
    static void display_children(node::Node *node);
    node::Node *selectedComponent = nullptr;

  public:
    GUI(const Renderer &renderer);
    ~GUI();
    void render(GameView&, uint32_t, const Scene &) const;
};
} // namespace engine
