#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <glm/glm.hpp>
#include <unistd.h>
#include <vector>

#include "common.hpp"

namespace engine
{
    class Object;

    namespace component
    {
        struct Transform
        {
            vec2<float> scale{1, 1};
            vec2<float> translate{0, 0};
            float rotate = 0;

            glm::mat4 model() const;
            Transform() = default;
            Transform(vec2<float> scale, vec2<float> translate, float rotate);

#ifdef NDEBUG
            void inspector(const char *prefix = nullptr);
#endif
        };

        struct Component
        {
            bool hidden = false;

            virtual ~Component() = default;
            virtual void draw(glm::mat4) {}
#ifdef NDEBUG
            virtual void inspector(uint32_t) {}
#endif
        };

        struct Sprite : public Component
        {
            Transform transform;
            uint32_t current_texture = 0;

            void draw(glm::mat4) override;

            Sprite(const Sprite &) = delete;
            Sprite operator=(const Sprite &) = delete;
            Sprite(Sprite &&);
            Sprite &operator=(Sprite &&);
            Sprite(int w, int h, std::vector<std::filesystem::path>);
            ~Sprite();

        private:
            vec2<int> size{0, 0};
            static inline uint32_t objCount = 0;
            uint32_t VBO = 0,
                     EBO = 0,
                     VAO = 0,
                     indexCount = 0;
            std::vector<uint32_t> textures;
            std::vector<std::filesystem::path> texturePaths;

#ifdef NDEBUG
        public:
            void inspector(uint32_t) override;
#endif
        };

        struct Timer : public Component
        {
            void setTimeout(std::function<void()>, uint32_t duration_ms, uint32_t times = 1);
            void draw(glm::mat4) override; // hijack `draw` function for poll

        private:
            std::chrono::time_point<std::chrono::system_clock> target;
            uint32_t count = 0,
                     _duration = 0;
            std::function<void()> lambda = nullptr;
        };
        
        namespace collision
        {
            struct Shape
            {
                Transform transform;
                Object &parent;
                enum Type
                {
                    BOX
                } type;
    
                Shape(Object &, Type type);
                ~Shape();
                virtual Shape* checkCollision()
                { return nullptr; }
    
            protected:
                static inline std::vector<Shape *> allShapes;

#ifdef NDEBUG
            public:
                virtual void draw() {}
                virtual void inspector() {}
#endif
            };

            struct Box : public Shape
            {
                vec2<float> size {0.0f, 0.0f};
                Box(Object &);
                Box* checkCollision() override;
#ifdef NDEBUG
                ~Box();
                Box(Box &&);
                Box &operator=(Box &&);

                void draw() override;
                void inspector() override;
            private:
                uint32_t VBO = 0, VAO = 0, EBO = 0, indexCount = 0;
                uint32_t id;
                static inline uint32_t counter = 0;
#endif
            };
        }

        struct Physics : public Component
        {
            float gravity = 9.8;
            std::vector<std::reference_wrapper<collision::Shape>> collisionShapes;

#ifdef NDEBUG
            void inspector(uint32_t);
#endif
        };
    }

}
