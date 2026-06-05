#pragma once

#include <cstdint>
#include <initializer_list>
#include <optional>
#include <utility>
#include <filesystem>
#include <glm/glm.hpp>

#include "common.hpp"

namespace engine {
    namespace component {
        struct Transform {
            vec2<float> scale {1, 1};
            vec2<float> translate {0, 0};
            float rotate = 0;

#ifdef NDEBUG
            void inspector();
#endif
            glm::mat4 model() const;

            Transform(vec2<float> scale, vec2<float> translate, float rotate);
            Transform(const Transform &) = delete;
            Transform operator=(const Transform &) = delete;
            Transform(Transform &&);
            void operator=(Transform &&);
        };

        struct Sprite {
            uint32_t current_texture = 0;

            void draw(glm::mat4 &);
#ifdef NDEBUG
            void inspector();
#endif

            Sprite(const Sprite &) = delete;
            Sprite operator=(const Sprite &) = delete;
            Sprite(Sprite &&);
            Sprite& operator=(Sprite &&);
            Sprite(int w, int h,
                std::initializer_list<std::filesystem::path>&&);
            ~Sprite();

        private:
            vec2<int> size{0, 0};
            static inline uint32_t objCount = 0;
            uint32_t VBO = 0, EBO = 0, VAO = 0,
                     indexCount = 0;
            std::vector<uint32_t> textures;
            std::vector<std::filesystem::path> texturePaths;
        };

        struct Physics {
            float gravity = 9.8;

#ifdef NDEBUG
            void inspector();
#endif
        };

        class Components {
            std::optional<Transform> _transform;
            std::optional<Sprite> _sprite;
            std::optional<Physics> _physics;

        public:
            void addComponent(Transform &&t) {
                _transform.emplace(std::move(t));
            }
            void addComponent(Sprite &&s) {
                _sprite.emplace(std::move(s));
            }
            void addComponent(Physics &&p) {
                _physics.emplace(std::move(p));
            }

            auto &transform() { return _transform; }
            auto &sprite() { return _sprite; }
            auto &physics() { return _physics; }

#ifdef NDEBUG
            void inspector() {
                if (_transform.has_value()) _transform->inspector();
                if (_sprite.has_value()) _sprite->inspector();
                if (_physics.has_value()) _physics->inspector();
            }
#endif
        };
    }

}
