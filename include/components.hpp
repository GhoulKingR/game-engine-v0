#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <unistd.h>
#include <vector>

#include "common.hpp"
#include "textures.hpp"

namespace engine
{
    struct Object;

    namespace component
    {
        // Transform is not a component. Every object and component has a 
        // transform field in them.
        struct Transform
        {
            vec2<float> scale{1, 1};
            vec2<float> translate{0, 0};
            float       rotate = 0;
            glm::mat4   model() const noexcept;
            Transform() = default;
            Transform(vec2<float> scale, vec2<float> translate, float rotate);

#ifdef NDEBUG
            void        inspector(const char *prefix = nullptr) noexcept;
#endif
        };

        // Interface for components
        struct IComponent
        {
            bool hidden = false;
            virtual void draw(const glm::mat4&) noexcept {};
#ifdef NDEBUG
            virtual void inspector(uint32_t) noexcept = 0;
#endif
        };

        // Sprite component. Allows you to display sprites on the screen
        struct Sprite : public IComponent
        {
            Transform   transform;
            uint32_t    current_texture = 0;
            void draw(const glm::mat4 &) noexcept override;

            Sprite(const Sprite &) = delete;
            Sprite operator=(const Sprite &) = delete;
            Sprite(Sprite &&);
            Sprite &operator=(Sprite &&);
            Sprite(int w, int h, std::vector<Texture *>);
            ~Sprite();

        private:
            vec2<int>               size{0, 0};
            static inline uint32_t  objCount = 0;
            uint32_t                VBO = 0,
                                    EBO = 0,
                                    VAO = 0,
                                    indexCount = 0;
            std::vector<Texture *>  textures;

#ifdef NDEBUG
        public:
            void inspector(uint32_t) noexcept override;
#endif
        };

        // Timer component: Allows you to set timers with callbacks
        struct Timer : public IComponent
        {
            void setTimeout(std::function<void()>, uint32_t duration_ms, uint32_t times = 1) noexcept;
            void draw(const glm::mat4 &) noexcept override; // hijack `draw` function for event polling
        private:
            std::chrono::time_point<std::chrono::system_clock>  target;
            uint32_t                                            count = 0,
                                                                _duration = 0;
            std::function<void()>                               lambda = nullptr;
        };

        namespace collision
        {
            // Collision shape interface
            struct ICollisionShape
            {
                virtual ICollisionShape* checkCollision() const noexcept = 0;
#ifdef NDEBUG
                virtual void draw(const glm::mat4 &) const noexcept = 0;
                virtual void inspector() noexcept = 0;
#endif
            };


            // Box collision shape
            struct Box : public ICollisionShape
            {
                Transform        transform;
                Object*          parent;
                vec2<float>      size {0.0f, 0.0f};
                ICollisionShape* checkCollision() const noexcept override;

                Box(Object *);
                Box(Box &&);
                Box &operator=(Box &&);
                Box(const Box &) = delete;
                Box &operator=(const Box &) = delete;
#ifdef NDEBUG
                ~Box();
                void draw(const glm::mat4 &) const noexcept override;
                void inspector() noexcept override;
#endif

                // This is required even when it's not needed because it can cause
                // runtime overflow errors. I still don't understand how this was an 
                // issue, but it took me days to figure out.
            private:
                uint32_t VBO = 0, VAO = 0, EBO = 0, indexCount = 0;
                uint32_t id;
                static inline uint32_t counter = 0;
            };

        }

        // Physics component: Handles both physics related data, and
        // collision detection
        struct Physics : public IComponent
        {
            float gravity = 9.8;
            std::vector<collision::ICollisionShape *> collisionShapes;

#ifdef NDEBUG
            void draw(const glm::mat4 &) noexcept override;
            void inspector(uint32_t) noexcept override;
#endif
        };
    }

}
