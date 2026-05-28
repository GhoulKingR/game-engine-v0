#include "gui.hpp"
#include "gameview.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "scene.hpp"

#include <cstdint>

void engine::gui::init(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void engine::gui::close() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void engine::gui::render(uint32_t gameViewTexture) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowMetricsWindow();

    // Render game to viewport
    ImGuiWindowFlags gvFlags = 0;
    gvFlags |= ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Game View", nullptr, gvFlags);
    auto windowSize = ImGui::GetContentRegionAvail();
    engine::gameview::set_viewport(
        {static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)});
    ImGui::Image(
        reinterpret_cast<void *>(static_cast<intptr_t>(gameViewTexture)),
        windowSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    // object tree
    scene::renderTree();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
