#include "engine/gameview.hpp"
#include "engine/gui.hpp"
#include "engine/renderer.hpp"
// #include "engine/scene.hpp"
#include "engine/shaders.hpp"

#include <cstdlib>

#include <toml++/toml.hpp>

int main() {
    engine::renderer::init();
    engine::gui::init();
    engine::shader::init();
    engine::gameview::init();
    // engine::project::scene::load("../game/scene.toml");

    engine::renderer::loop([](){
        engine::gameview::render();
        engine::gui::render();
    });

    engine::gui::close();
    engine::shader::cleanup();
    engine::gameview::cleanup();
    engine::renderer::cleanup();
    return EXIT_SUCCESS;
}
