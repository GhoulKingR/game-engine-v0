#pragma once

#include <variant>
#include <array>
#include <toml++/toml.hpp>
#include <glm/glm.hpp>

namespace engine {
    using vec2 = std::array<float, 2>;
    using vec2i = std::array<int, 2>;

    namespace object {
        namespace component {
            struct Transform {
                vec2 scale{0};
                vec2i translate{0};
                float rotate = 0;

                Transform(){}
                Transform(toml::table *);
                void inspector();
                glm::mat4 model() const;
                std::pair<const char *, toml::table> to_table() const;

                Transform(const Transform &) = delete;
                Transform operator=(const Transform &) = delete;
                Transform(Transform &&);
                void operator=(Transform &&);
            };

            struct Sprite {
                vec2i size{0};
                uint32_t current_texture = 0;
                static inline uint32_t objCount = 0;
                uint32_t VBO = 0, EBO = 0, VAO = 0, indexCount = 0;
                std::vector<uint32_t> textures;
                std::vector<std::filesystem::path> texturePaths;

                Sprite(){}
                Sprite(toml::table *tbl, std::filesystem::path &scenePath);
                ~Sprite();
                void draw(glm::mat4 &);
                void inspector();
                std::pair<const char *, toml::table> to_table() const;

                Sprite(const Sprite &) = delete;
                Sprite operator=(const Sprite &) = delete;
                Sprite(Sprite &&);
                Sprite& operator=(Sprite &&);
            };

            using Component = std::variant<Transform, Sprite>;
        }
    }
}
