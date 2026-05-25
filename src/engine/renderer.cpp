#include "renderer.hpp"
#include "object.hpp"

#ifdef ENGINE
#include <cstdint>
#endif

#include <functional>
#include <stdexcept>
#include <cstdlib>
#include <print>
#include <ranges>

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
#ifndef ENGINE
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif

    window = glfwCreateWindow(screenWidth, screenHeight, "Flappy Bird GL",
                              nullptr, nullptr);
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
#ifndef ENGINE
            gameWidth = screenWidth;
            gameHeight = screenHeight;
#endif
        });

#ifdef ENGINE
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glGenFramebuffers(1, &gameViewFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gameViewFBO);
    glGenTextures(1, &gameViewTexture);
    glBindTexture(GL_TEXTURE_2D, gameViewTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gameWidth, gameHeight, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           gameViewTexture, 0);

    glGenRenderbuffers(1, &gameViewRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gameViewRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gameWidth,
                          gameHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, gameViewRBO);
#endif
}

engine::Renderer::~Renderer() {
#ifdef ENGINE
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
#endif
    glfwDestroyWindow(window);
    glfwTerminate();
}

void engine::Renderer::load(
    const std::vector<std::reference_wrapper<Object>> &objects) {
    while (!glfwWindowShouldClose(window)) {
#ifdef ENGINE
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glViewport(0, 0, gameWidth, gameHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, gameViewFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, gameViewRBO);
#endif
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto obj : objects) {
            obj.get().draw();
        }
#ifdef ENGINE
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glClearColor(0.1, 0.1, 0.1, 1.0);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // construct GUI elements
        constructEngineGUI(objects);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

#ifdef ENGINE
void engine::Renderer::constructEngineGUI(
    const std::vector<std::reference_wrapper<Object>> &objects) {
    ImGui::ShowMetricsWindow();

    // Render game to viewport
    ImGuiWindowFlags gvFlags = 0;
    gvFlags |= ImGuiWindowFlags_NoResize;
    gvFlags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Game View", nullptr, gvFlags);
    auto gameViewportSize = ImVec2(gameWidth, gameHeight);
    ImGui::Image(
        reinterpret_cast<void *>(static_cast<intptr_t>(gameViewTexture)),
        gameViewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    static unsigned int selectedComponent = 0;

    ImGui::Begin("Objects");
    for (auto [i, obj] : std::views::zip(std::views::iota(1), objects)) {
        ImGui::Bullet();
        const char *name = obj.get().name.c_str();
        if (ImGui::SmallButton(name)) {
            selectedComponent = i;
        }
    }
    ImGui::End();

    if (selectedComponent > 0) {
        Object &obj = objects[selectedComponent - 1].get();
        auto &transforms = obj.transforms;

        // clang-format off
        ImGui::Begin(obj.name.c_str());
        ImGui::Text("Transform");
        ImGui::Indent();
            ImGui::InputFloat2("Translate", transforms.translate);
            ImGui::InputFloat2("Scale", transforms.scale);
            ImGui::SliderFloat("Rotate", &transforms.rotate, -360.0f, 360.0f);
        ImGui::Unindent();
        ImGui::End();
        // clang-format on
    }
}
#endif

glm::mat4 engine::Renderer::aspectRatio() const {
    auto resolution = glm::vec2(gameWidth, gameHeight);
    auto max = glm::compMax(resolution);
    auto scaleRatio = max > 0 ? (resolution / max) : glm::vec2(1.0f, 1.0f);
    return glm::scale(glm::identity<glm::mat4>(),
                      glm::vec3(scaleRatio.y, scaleRatio.x, 1.0));
}
