#pragma once

#include <functional>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {
class Object;
class Renderer {
    static inline int gameWidth = 288;
    static inline int gameHeight = 512;

#ifdef ENGINE
    static inline int screenWidth = 800;
    static inline int screenHeight = 600;

    unsigned int gameViewTexture = 0;
    unsigned int gameViewFBO = 0;
    unsigned int gameViewRBO = 0;

    void
    constructEngineGUI(const std::vector<std::reference_wrapper<Object>> &);
#else
    static inline int screenWidth = 288;
    static inline int screenHeight = 512;
#endif
    GLFWwindow *window = nullptr;

  public:
    Renderer();
    ~Renderer();
    glm::mat4 aspectRatio() const;

    void load(const std::vector<std::reference_wrapper<Object>> &);
};
} // namespace engine
