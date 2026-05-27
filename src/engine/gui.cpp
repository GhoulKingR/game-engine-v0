#include "gui.hpp"
#include "gameview.hpp"
#include "imgui/imgui.h"
#include "node/node.hpp"
#include "scene.hpp"

#include <cstdint>

engine::GUI::GUI(const engine::Renderer &renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(renderer.getWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

engine::GUI::~GUI() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void engine::GUI::display_children(engine::node::Node *node) {
    if (ImGui::TreeNode(node->name.c_str())) {
        const auto &children = node->children;
        if (children.size() > 0) {
            for (const auto &child : children) {
                display_children(child.get());
            }
        }
        ImGui::TreePop();
    }
}

void engine::GUI::render(engine::GameView &gameView, uint32_t gameViewTexture,
                         const Scene &scene) const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowMetricsWindow();

    // Render game to viewport
    ImGuiWindowFlags gvFlags = 0;
    gvFlags |= ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Game View", nullptr, gvFlags);
    auto windowSize = ImGui::GetContentRegionAvail();
    gameView.viewportWidth = windowSize.x;
    gameView.viewportHeight = windowSize.y;
    ImGui::Image(
        reinterpret_cast<void *>(static_cast<intptr_t>(gameViewTexture)),
        ImVec2(gameView.viewportWidth, gameView.viewportHeight), ImVec2(0, 1),
        ImVec2(1, 0));
    ImGui::End();

    // scene tree
    ImGui::Begin("Scene Tree");
    auto root = scene.getRoot();
    if (root != nullptr) {
        display_children(root);
    }
    ImGui::End();

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
