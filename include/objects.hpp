#pragma once

#include "components.hpp"
#include <string>
#include <cstdint>

namespace engine
{
    class Object
    {
        static inline uint32_t objectCount = 0;

    protected:
        std::vector<engine::component::Component *> components;

    public:
        std::string name;
        virtual void update(float) {}
        void _draw();
        component::Transform transform;
        Object(const char *name = nullptr);
        ~Object() = default;

#ifdef NDEBUG
        void _inspector();
#endif
    };
}
