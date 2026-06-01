#include "renderer.hpp"
#include "gameview.hpp"
#include <cstdint>
#include <cstdio>
#include <format>
#include <print>
#include <cstdlib>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

static int screenWidth = 800;
static int screenHeight = 600;
static SDL_Window *_window = nullptr;
static SDL_GLContext _ctx;

void engine::renderer::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::println(stderr, "Error initializing SDL: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

#if __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    _window = SDL_CreateWindow("GL Engine", screenWidth, screenHeight,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (_window == nullptr) {
        std::println(stderr, "Failed to initialize GLFW window");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) {
        std::println(stderr, "Failed to create opengl context: {}",
                     SDL_GetError());
        SDL_DestroyWindow(_window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_GL_MakeCurrent(_window, _ctx);
    SDL_GL_SetSwapInterval(1);
    SDL_ShowWindow(_window);

    if (!gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        SDL_GL_DestroyContext(_ctx);
        SDL_DestroyWindow(_window);
        SDL_Quit();
        std::println("Failed to initialize GLAD");
        exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);
}

void engine::renderer::cleanup() {
    SDL_GL_DestroyContext(_ctx);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

static bool running = true;
static float _zoom = 1.0;
static glm::vec2 _pan_delta(0.0, 0.0);
static void processInput() {
    SDL_Event event;
    static bool scrolling = false;
    static bool zooming = false;
    const uint64_t SCROLL_TIMEOUT_NS = 10000000; // 100 milliseconds
    static uint64_t lastScroll = 0;
    static uint64_t lastZoom = 0;


    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                   event.window.windowID == SDL_GetWindowID(_window)) {
            running = false;
        } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            SDL_GetWindowSizeInPixels(_window, &screenWidth, &screenHeight);
            glViewport(0, 0, screenWidth, screenHeight);
        } else if (event.type == SDL_EVENT_PINCH_UPDATE) {
            _zoom = event.pinch.scale;
            zooming = true;
            lastZoom = SDL_GetTicksNS();
        } else if (event.type == SDL_EVENT_PINCH_END) {
            _zoom = 1.0;
        } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                _pan_delta = glm::vec2(-event.wheel.x, -event.wheel.y);
            } else {
                _pan_delta = glm::vec2(event.wheel.x, event.wheel.y);
            }
            scrolling = true;
            lastScroll = SDL_GetTicksNS();
        }
    }
    
    if (scrolling) {
        auto current = SDL_GetTicksNS();
        if (current - lastScroll > SCROLL_TIMEOUT_NS) {
            scrolling = false;
            _pan_delta.x = 0.0;
            _pan_delta.y = 0.0;
        }
    }

    if (zooming) {
        auto current = SDL_GetTicksNS();
        if (current - lastZoom > SCROLL_TIMEOUT_NS) {
            zooming = false;
            _zoom = 1.0;
        }
    }
}
float engine::renderer::zoom() { return _zoom; }
glm::vec2 engine::renderer::scroll() { return _pan_delta; }

void engine::renderer::loop(std::function<void()> callback) {
    while (running) {
        processInput();
        callback();
        SDL_GL_SwapWindow(_window);
    }
}

engine::vec2i engine::renderer::get_viewport() {
    return {screenWidth, screenHeight};
}

void engine::renderer::set_viewport(const engine::vec2i &val) {
    screenWidth = val[0];
    screenHeight = val[1];
}

SDL_Window *engine::renderer::window() { return _window; }
SDL_GLContext engine::renderer::context() { return _ctx; }
void engine::renderer::closeWindow() { running = false; }
