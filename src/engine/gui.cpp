#include "gui.hpp"
#include "gameview.hpp"
#include "imgui/imgui_impl_sdl3.h"
#include "nfd.h"
#include "project.hpp"
#include "renderer.hpp"
#include "scene.hpp"

#include <format>
#include <print>
#include <string>
#include <nfd.hpp>

void engine::gui::init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui_ImplSDL3_InitForOpenGL(renderer::window(), renderer::context());
    ImGui_ImplOpenGL3_Init();
}

void engine::gui::close() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

constexpr std::string ctrl_modifier(const char *combination) {
#if __APPLE__
    return std::format("Command + {}", combination);
#else
    return std::format("Ctrl + {}", combination);
#endif
}

void engine::gui::render() {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    static bool show_metrics = false;
    if (show_metrics) {
        ImGui::ShowMetricsWindow();
    }

#ifdef DEBUG
    static bool show_demo = false;
    if (show_demo) {
        ImGui::ShowDemoWindow();
    }
#endif

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New project")) {
            }
            if (ImGui::MenuItem("Open project")) {
                NFD::UniquePath outPath;
                nfdfilteritem_t filters[1];
                filters[0] = {.name = "Project", .spec = "project"};
                auto result = NFD::OpenDialog(outPath, filters, 1);

                if (result == NFD_OKAY) {
                    project::load(outPath.get());
                } else if (result == NFD_CANCEL) {
                } else {
                    std::println(stderr, "Error: {}", NFD::GetError());
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save scene", ctrl_modifier("S").c_str())) {
                auto current = project::scene::current();
                if (current.has_value()) {
                    project::scene::save(current.value());
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", ctrl_modifier("Q").c_str())) {
                renderer::closeWindow();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Project")) {
            if (ImGui::MenuItem("Run")) {
            }
            if (ImGui::MenuItem("Project settings")) {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Run")) {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Show metrics", nullptr, &show_metrics);
#ifdef DEBUG
            ImGui::MenuItem("Show demo", nullptr, &show_demo);
#endif
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // object tree
    gameview::renderGUI();
    project::renderWindow();
    project::scene::renderTree();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
