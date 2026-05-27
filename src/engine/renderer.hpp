#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {
class Object;
class GUI;
class GameView;
class Scene;
class Renderer {
    static inline int screenWidth = 800;
    static inline int screenHeight = 600;

    GLFWwindow *window = nullptr;

  public:
    Renderer();
    ~Renderer();

    void load(const GUI &, const GameView &, const Scene &);
    auto getWindow() const { return window; }
};
} // namespace engine
