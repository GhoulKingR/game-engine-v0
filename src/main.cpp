#include "engine/renderer.hpp"
#include "engine/shaders.hpp"
#include "game/bird.hpp"

#include <cstdio>
#include <cstdlib>
#include <print>
#include <stdexcept>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    try {
        engine::Renderer renderer;
        engine::Shaders uberShaders("shaders/uber.vert", "shaders/uber.frag");

        // init
        // engine::Object test(uberShaders, renderer, "Bird test", 1, 1,
        //                     "assets/sprites/yellowbird-midflap.png");
        Bird bird(uberShaders, renderer, "Main bird", Bird::Red);

        renderer.load({bird});
    } catch (const std::runtime_error &err) {
        std::println(stderr, "Error :: {}", err.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
