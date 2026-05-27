#include "renderer.hpp"
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

static int screenWidth = 800;
static int screenHeight = 600;

GLFWwindow *engine::renderer::init_glfw() {
    glfwSetErrorCallback([](int error, const auto desc) {
        std::println(stderr, "Error ({}): {}", error, desc);
    });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(screenWidth, screenHeight, "GL engine",
                                   nullptr, nullptr);
    if (window == nullptr) {
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

    return window;
}

void engine::renderer::close_glfw(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

engine::vec2 engine::renderer::get_viewport() {
    return {screenWidth, screenHeight};
}
void engine::renderer::set_viewport(const engine::vec2 &val) {
    screenWidth = val[0];
    screenHeight = val[1];
}
