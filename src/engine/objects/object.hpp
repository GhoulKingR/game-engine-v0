#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <toml++/toml.hpp>

#include "component.hpp"

#include <glm/glm.hpp>
#include <variant>

namespace engine {
    using vec2 = std::array<float, 2>;
    using vec2i = std::array<int, 2>;

    namespace object {
        struct SceneObject {
            std::string name;
            std::vector<component::Component> components;

            SceneObject(toml::table *tbl);
            void draw();
            void inspector();
            toml::table to_table();

            SceneObject(const SceneObject &) = delete;
            SceneObject operator=(const SceneObject &) = delete;
            SceneObject(SceneObject &&);
            SceneObject& operator=(SceneObject &&);
        };

        struct Camera {
            std::string name;
            vec2i viewport;
            component::Transform transform;

            Camera(toml::table *tbl);
            ~Camera();
            void draw();
            void inspector();
            toml::table to_table();

            uint32_t previewVBO = 0,
                     previewEBO = 0,
                     previewVAO = 0,
                     indexCount = 0;
            
            Camera(const Camera &) = delete;
            Camera operator=(const Camera &) = delete;
            Camera(Camera &&);
            Camera& operator=(Camera &&);
        };

        using Object = std::variant<SceneObject, Camera>;
    }
}
