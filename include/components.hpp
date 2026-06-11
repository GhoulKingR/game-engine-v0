#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <unistd.h>
#include <vector>

#include "common.hpp"

namespace engine::component {
    struct Transform {
        vec2<float> scale {1, 1};
        vec2<float> translate {0, 0};
        float rotate = 0;

#ifdef NDEBUG
        void inspector(const std::string & = "");
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
        virtual void draw(const glm::mat4 &) {}
#ifdef NDEBUG
        virtual void inspector(uint32_t) {}
#endif
        virtual ~Component() = default;
    };

    struct Sprite : public Component {
        Transform transform;
        uint32_t current_texture = 0;

        void draw(const glm::mat4 &) override;
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
        void draw(const glm::mat4 &) override; // hijack `draw` function for poll

    private:
        std::chrono::time_point<std::chrono::system_clock> target;
        uint32_t count = 0, _duration = 0;
        std::function<void()> lambda = nullptr;
    };

    namespace collision {
        class CollisionShape {
            static inline uint32_t counter = 0;
            CollisionShape(const CollisionShape &) = delete;
            CollisionShape(CollisionShape &&) = delete;
            CollisionShape &operator=(const CollisionShape &) = delete;
            CollisionShape &operator=(CollisionShape &&) = delete;

        protected:
            bool hidden = false;
            uint32_t id;

        public:
            Transform transform;
            CollisionShape() {
                id = ++counter;
            }
#ifdef NDEBUG
            virtual void draw(const glm::mat4 &) {}
            virtual void inspector() {}
#endif
        };

        // class Circle: public CollisionShape {
        // };

        class Rectangle: public CollisionShape {
#ifdef NDEBUG
            uint32_t VBO = 0, VAO = 0, EBO = 0,
                     indexCount = 0;
#endif
            Rectangle(const Rectangle &) = delete;
            Rectangle(Rectangle &&) = delete;
            Rectangle &operator=(const Rectangle &) = delete;
            Rectangle &operator=(Rectangle &&) = delete;

        public:
            vec2<float> size;
            Rectangle();

#ifdef NDEBUG
            void draw(const glm::mat4 &) override;
            void inspector() override;
#endif
        };
    }

    struct Physics : public Component {
        float gravity = 9.8;
        std::vector<std::reference_wrapper<
            collision::CollisionShape>> collisionShapes;

        Physics() = default;
        Physics(const Physics &) = delete;
        Physics(Physics &&) = delete;
        Physics &operator=(const Physics &) = delete;
        Physics &operator=(Physics &&) = delete;

#ifdef NDEBUG
        void draw(const glm::mat4 &) override;
        void inspector(uint32_t) override;
#endif
    };
}
