#include "renderer.hpp"
#include "gameview.hpp"
#include "gui.hpp"
#include "object.hpp"

#include <stdexcept>
#include <cstdlib>
#include <print>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

engine::Renderer::Renderer() {
    glfwSetErrorCallback([](int error, const auto desc) {
        std::println(stderr, "Error ({}): {}", error, desc);
    });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(screenWidth, screenHeight, "GL engine", nullptr,
                              nullptr);
    if (window == NULL) {
        throw std::runtime_error("Failed to initialize GLFW window");
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(window);
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow *window, int width, int height) {
            (void)window;
            glViewport(0, 0, width, height);
            screenWidth = width;
            screenHeight = height;
        });
}

engine::Renderer::~Renderer() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void engine::Renderer::load(
    const engine::GUI &gui, const engine::GameView &gv) {
    while (!glfwWindowShouldClose(window)) {
        auto gvTexture = gv.render();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui.render(gv.viewport(), gvTexture, gv.getObjects());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

