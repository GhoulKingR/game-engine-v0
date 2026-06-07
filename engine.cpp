#include "common.hpp"
#include "controls.hpp"
#include "shaders/shaders.hpp"
#include <cassert>
#include <chrono>
#include <engine.hpp>
#include <scene.hpp>
#include <unistd.h>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NDEBUG
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl3.h>

#define STARTING_WIDTH 800
#define STARTING_HEIGHT 800
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <print>

static SDL_Window *window = nullptr;
static SDL_GLContext ctx = nullptr;
static engine::vec2<int> viewport;

#ifdef NDEBUG
static engine::vec2<int> actual{
    STARTING_WIDTH,
    STARTING_HEIGHT
};

static struct {
    uint32_t viewTexture = 0;
    uint32_t FBO = 0;
    uint32_t RBO1 = 0;
    uint32_t RBO2 = 0;
    uint32_t mFBO = 0;
} gameview;

static void cleanupTextures() {
    glDeleteFramebuffers(1, &gameview.FBO);
    glDeleteFramebuffers(1, &gameview.mFBO);
    glDeleteRenderbuffers(1, &gameview.RBO1);
    glDeleteRenderbuffers(1, &gameview.RBO2);
    glDeleteTextures(1, &gameview.viewTexture);
}

static void constructRenderTexture() {
    // generate regular frame buffer object for rendering to texture
    glGenFramebuffers(1, &gameview.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gameview.FBO);
    glGenTextures(1, &gameview.viewTexture);
    glBindTexture(GL_TEXTURE_2D, gameview.viewTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewport.x, viewport.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameview.viewTexture, 0);

    // generate multisample frame buffer object to render to
    glGenFramebuffers(1, &gameview.mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gameview.mFBO);

    // color buffer
    glGenRenderbuffers(1, &gameview.RBO1);
    glBindRenderbuffer(GL_RENDERBUFFER, gameview.RBO1);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, viewport.x, viewport.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gameview.RBO1);

    // depth and stencil buffer
    glGenRenderbuffers(1, &gameview.RBO2);
    glBindRenderbuffer(GL_RENDERBUFFER, gameview.RBO2);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, viewport.x, viewport.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameview.RBO2);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif

void engine::init(const char *_title, uint32_t _width, uint32_t _height) {
    assert(_title != nullptr);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::println(stderr, "Error :: Failed to initialize SDL: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

#if __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

#ifdef NDEBUG
    window = SDL_CreateWindow(std::format("{} (Debug)", _title).c_str(),
        actual.x, actual.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
#else
    window = SDL_CreateWindow(_title, _width, _height, SDL_WINDOW_OPENGL);
#endif
    if (window == nullptr) {
        std::println(stderr, "Error :: Failed to initialize SDl window: {}", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    ctx = SDL_GL_CreateContext(window);
    if (ctx == nullptr) {
        std::println(stderr, "Error :: Failed to create GL context: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_GL_MakeCurrent(window, ctx);
    SDL_GL_SetSwapInterval(1);
    SDL_ShowWindow(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        std::println(stderr, "Error :: Failed to initialize GLAD");
        SDL_GL_DestroyContext(ctx);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);

    viewport.x = _width;
    viewport.y = _height;
    shader::init();

#ifdef NDEBUG
    glViewport(0, 0, actual.x, actual.y);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui_ImplSDL3_InitForOpenGL(window, ctx);
    ImGui_ImplOpenGL3_Init();

    constructRenderTexture();
#else
    glViewport(0, 0, _width, _height);
#endif
}

static engine::Scene* currentScene;
void engine::loadScene(engine::Scene *_scene) {
    currentScene = _scene;
}

#ifdef NDEBUG
static bool paused = false;
static void guiLoop() {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // main menu bar
    static bool metrics = false;
    static bool demo_window = false;
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Show metrics", nullptr, &metrics);
            ImGui::MenuItem("Show ImGui demo window", nullptr, &demo_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (metrics) {
        ImGui::ShowMetricsWindow();
    }
    if (demo_window) {
        ImGui::ShowDemoWindow();
    }

    // central game view
    ImGuiWindowFlags gvFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::Begin("Game view", nullptr, gvFlags);
    ImGui::Image(
        reinterpret_cast<void *>(static_cast<intptr_t>(gameview.viewTexture)),
        ImVec2(viewport.x, viewport.y),
        ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    currentScene->_inspector();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif

static void gameLoop(float deltaTime) {
    glViewport(0, 0, viewport.x, viewport.y);
#ifdef NDEBUG
    glBindFramebuffer(GL_FRAMEBUFFER, gameview.mFBO);
#endif

    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentScene != nullptr) {
#ifdef NDEBUG
        if (!paused) currentScene->_update(deltaTime);
#else
        currentScene->_update(deltaTime);
#endif
        currentScene->_draw();
    }

#ifdef NDEBUG
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gameview.mFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gameview.FBO);
    glBlitFramebuffer(
            0, 0, viewport.x, viewport.y,
            0, 0, viewport.x, viewport.y,
            GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glViewport(0, 0, actual.x, actual.y);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

static bool running = true;
static void processInput() {
    SDL_Event _event;
    while (SDL_PollEvent(&_event)) {
#ifdef NDEBUG
        ImGui_ImplSDL3_ProcessEvent(&_event);
#endif
        if (_event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        else if (_event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                 _event.window.windowID == SDL_GetWindowID(window)) {
            running = false;
        }
        else if (_event.type == SDL_EVENT_WINDOW_RESIZED) {
#ifdef NDEBUG
            SDL_GetWindowSizeInPixels(window, &(actual.x), &(actual.y));
            glViewport(0, 0, actual.x, actual.y);
        }
        else if(_event.type == SDL_EVENT_KEY_UP) {
            if (_event.key.key == SDLK_BACKSLASH) {
                paused = !paused;
            }
        }
#else
            SDL_GetWindowSizeInPixels(window,
                    &(viewport.x), &(viewport.y));
            glViewport(0, 0, viewport.x, viewport.y);
        }
#endif
        engine::controls::update(&_event);
    }
}

void engine::start() {
    auto lastTime = std::chrono::steady_clock::now();
    while (running) {
        auto currentTime = std::chrono::steady_clock::now();
        processInput();
        gameLoop(std::chrono::duration<float>(currentTime - lastTime).count());
        lastTime = currentTime;
#ifdef NDEBUG
        guiLoop();
#endif
        SDL_GL_SwapWindow(window);
        controls::clearFrameStates();
    }
}

glm::mat4 engine::aspectRatio() {
    auto pixelSize = 1.0f / glm::vec2(viewport.x, viewport.y);
    auto scale = glm::identity<glm::mat4>();
    scale = glm::scale(scale, glm::vec3(
        pixelSize.x * 2.0f, pixelSize.y * 2.0f, 1.0));
    return scale;
}

void engine::cleanup() {
#ifdef NDEBUG
    cleanupTextures();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif

    SDL_GL_DestroyContext(ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
