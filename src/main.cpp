#include "engine/gameview.hpp"
#include "engine/gui.hpp"
#include "engine/renderer.hpp"
#include "engine/scene.hpp"

#include <cstdlib>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <toml++/toml.hpp>

int main() {
    auto window = engine::renderer::init_glfw();
    engine::gui::init(window);
    engine::gameview::init();
    engine::scene::load("game/scene.toml");

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



    // try {
    //     engine::Shaders uberShaders("shaders/uber.vert", "shaders/uber.frag");
    //     engine::GUI gui(renderer);
    //
    //     engine::Scene scene;
    //     scene.load("game/scene.toml");
    //     engine::GameView gameview;
    //
    //     // init
    //     // engine::Object test(uberShaders, renderer, "Bird test", 1, 1,
    //     //                     "assets/sprites/yellowbird-midflap.png");
    //     // Bird bird(uberShaders, renderer, "Main bird", Bird::Red);
    //
    //     // auto tbl = toml::parse_file("game/scene.toml");
    //     // std::cout << "Table look\n" << tbl << std::endl;
    //
    //     renderer.load(gui, gameview, scene);
    // } catch (const std::runtime_error &err) {
    //     std::println(stderr, "Error :: {}", err.what());
    //     return EXIT_FAILURE;
    // }

    engine::renderer::close_glfw(window);
    return EXIT_SUCCESS;
}
