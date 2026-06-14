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
        std::vector<engine::component::IComponent *> components;

    public:
        std::string             name;
        component::Transform    transform;
        virtual void update(float) {}
                void _draw()        noexcept;
        Object(const char *name = nullptr);
        ~Object() = default;

#ifdef NDEBUG
        void _inspector() noexcept;
#endif
    };
}
