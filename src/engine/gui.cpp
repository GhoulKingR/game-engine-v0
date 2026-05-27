#include "gui.hpp"
#include "gameview.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "node/node.hpp"
#include "scene.hpp"

#include <cstdint>

static engine::node::Node *selectedComponent = nullptr;

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

    // scene tree
    scene::renderTree();

    if (selectedComponent != nullptr) {
        // auto &transforms = obj.transforms;

        // clang-format off
        ImGui::Begin(selectedComponent->name.c_str());
        ImGui::Text("Transform");
        // ImGui::Indent();
        //     ImGui::InputFloat2("Translate", transforms.translate);
        //     ImGui::InputFloat2("Scale", transforms.scale);
        //     ImGui::SliderFloat("Rotate", &transforms.rotate, -360.0f, 360.0f);
        // ImGui::Unindent();
        ImGui::End();
        // clang-format on
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
