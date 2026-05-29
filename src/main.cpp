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
        engine::gameview::render();
        engine::gui::render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    engine::gui::close();
    engine::shader::cleanup();
    engine::gameview::cleanup();
    engine::renderer::close_glfw(window);
    return EXIT_SUCCESS;
}
