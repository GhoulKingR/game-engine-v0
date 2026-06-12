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
        std::vector<std::reference_wrapper<engine::component::Component>> components;

    public:
        std::string name;
        virtual void update(float) {}
        void _draw();
        component::Transform transform;
        Object(const char *name = nullptr);
        ~Object() = default;

#ifdef NDEBUG
        std::string getName() const { return name; }
        void _inspector();
#endif
    };
}
