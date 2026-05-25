#pragma once

#include "../engine/object.hpp"
#include "../engine/renderer.hpp"
#include "../engine/shaders.hpp"

#include <format>
#include <string>
#include <unordered_map>

class Bird : public engine::Object {
  public:
    enum Type { Yellow, Red, Blue };

    Bird(const engine::Shaders &shaders, const engine::Renderer &renderer,
         const std::string &name, Type type)
        : engine::Object(shaders, renderer, name) {

        static const auto to_str = std::unordered_map<Type, const char *>{
            {Yellow, "yellow"}, {Red, "red"}, {Blue, "blue"}};
        textures = genTextures({
            std::format("assets/sprites/{}bird-upflap.png", to_str.at(type)),
            std::format("assets/sprites/{}bird-midflap.png", to_str.at(type)),
            std::format("assets/sprites/{}bird-downflap.png", to_str.at(type)),
        });
        auto [vertices, indices] = genQuad(1.0f, 0.7058823529f);
        loadVertices(vertices, indices);
    }
};
