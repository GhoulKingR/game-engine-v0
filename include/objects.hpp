#pragma once

#include "components.hpp"
#include <functional>
#include <string>
#include <cstdint>

namespace engine
{
    struct Object
    {
        std::vector<engine::component::IComponent *>    components;
        std::string                                     name;
        component::Transform                            transform;
        std::function<void(float)>                      update = nullptr;
        Object(const char *name = nullptr);

    private:
        static inline uint32_t objectCount = 0;
    };

    namespace object
    {
        void _draw(Object *)        noexcept;
#ifdef NDEBUG
        void _inspector(Object *)   noexcept;
#endif
    }
}
