#pragma once

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <ranges>
#include <type_traits>
#include <utility>
#include <filesystem>
#include <glm/glm.hpp>
#include <variant>
#include <vector>

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

        template<typename... Ts>
        struct overloaded : Ts... {
            using Ts::operator()...;
        };
        template<typename T>
        concept TComponentType = std::is_same_v<T, Sprite>
            || std::is_same_v<T, Physics>;

        using ComponentType = std::variant<Sprite, Physics>;
        class Components {
            std::vector<ComponentType> _components;

        public:
            Transform transform {{1.0, 1.0}, {0.0, 0.0}, 0};
            void addComponent(ComponentType &&_comp) {
                _components.emplace_back(std::move(_comp));
            }

            const auto &all() const {
                return _components;
            }

            template<TComponentType T>
            auto get() {
                return _components
                    | std::ranges::views::filter([](auto &_c){
                          return std::holds_alternative<T>(_c);
                      })
                    | std::ranges::views::transform([](auto &_c){
                          return std::reference_wrapper(std::get<T>(_c));
                      });
            }

            void draw() {
                auto model = transform.model();
                for (auto &comp : _components) {
                    std::visit(overloaded{
                        [](const auto &) {},
                        [&model](Sprite &_c) {
                            _c.draw(model);
                        }
                    }, comp);
                }
            }

#ifdef NDEBUG
            void inspector() {
                transform.inspector();
                for (auto &comp : _components) {
                    std::visit(
                        [](auto &_c) { _c.inspector(); },
                        comp
                    );
                }
            }
#endif
        };
    }

}
