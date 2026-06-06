#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <glm/glm.hpp>
#include <unistd.h>
#include <vector>

#include "common.hpp"

namespace engine {
    namespace component {
        struct Transform {
            vec2<float> scale {1, 1};
            vec2<float> translate {0, 0};
            float rotate = 0;

#ifdef NDEBUG
            void inspector(const std::string &prefix = "");
#endif
            glm::mat4 model() const;

            Transform() = default;
            Transform(vec2<float> scale, vec2<float> translate, float rotate);
            Transform(const Transform &) = delete;
            Transform operator=(const Transform &) = delete;
            Transform(Transform &&) = delete;
            void operator=(Transform &&) = delete;
        };

        struct Component {
            bool hidden = false;
            virtual void draw(glm::mat4 &) {}
#ifdef NDEBUG
            virtual void inspector(uint32_t) {}
#endif
            virtual ~Component() = default;
        };

        struct Sprite : public Component {
            Transform transform;
            uint32_t current_texture = 0;

            void draw(glm::mat4 &) override;
#ifdef NDEBUG
            void inspector(uint32_t) override;
#endif

            Sprite(const Sprite &) = delete;
            Sprite operator=(const Sprite &) = delete;
            Sprite(Sprite &&) = delete;
            Sprite& operator=(Sprite &&) = delete;
            Sprite(int w, int h, std::vector<std::filesystem::path>);
            ~Sprite();

        private:
            vec2<int> size{0, 0};
            static inline uint32_t objCount = 0;
            uint32_t VBO = 0, EBO = 0, VAO = 0,
                     indexCount = 0;
            std::vector<uint32_t> textures;
            std::vector<std::filesystem::path> texturePaths;
        };

        struct Timer : public Component {
            void setTimeout(std::function<void()>, uint32_t duration_ms, uint32_t times = 1);
            void draw(glm::mat4 &) override; // hijack `draw` function for poll

        private:
            std::chrono::time_point<std::chrono::system_clock> target;
            uint32_t count = 0, _duration = 0;
            std::function<void()> lambda = nullptr;
        };

        struct Physics : public Component {
            float gravity = 9.8;

#ifdef NDEBUG
            void inspector(uint32_t);
#endif
        };
    }

}
