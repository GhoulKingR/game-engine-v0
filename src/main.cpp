#include "engine/gameview.hpp"
#include "engine/gui.hpp"
#include "engine/renderer.hpp"
#include "engine/scene.hpp"
#include "engine/shaders.hpp"

#include <cstdlib>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <toml++/toml.hpp>

int main() {
    auto window = engine::renderer::init_glfw();
    engine::gui::init(window);
    engine::shader::init();
    engine::gameview::init();
    engine::scene::load("../game/scene.toml");

    while (!glfwWindowShouldClose(window)) {
        auto gvTexture = engine::gameview::render();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        engine::gui::render(gvTexture);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    engine::gui::close();
    engine::shader::cleanup();
    engine::gameview::cleanup();
    engine::renderer::close_glfw(window);
    return EXIT_SUCCESS;
}
