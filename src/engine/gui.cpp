#include "gui.hpp"
#include "gameview.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "scene.hpp"

static GLFWwindow *_window = nullptr;
void engine::gui::init(GLFWwindow *window) {
    _window = window;
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

void engine::gui::render() {
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool show_metrics = true;
    if (show_metrics) {
        ImGui::ShowMetricsWindow();
    }

#if __APPLE__
    const char *exit_shortcut = "Command + Q";
#else
    const char *exit_shortcut = "Ctrl + Q";
#endif

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New project")) {
            }
            if (ImGui::MenuItem("Open project")) {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", exit_shortcut)) {
                glfwSetWindowShouldClose(_window, true);
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
            if (ImGui::MenuItem("Open scene")) {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Show metrics", nullptr, &show_metrics);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // object tree
    gameview::renderGUI();
    scene::renderTree();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
