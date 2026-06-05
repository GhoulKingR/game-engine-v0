#pragma once

#include "components.hpp"
#include <optional>
#include <string>
#include <cstdint>

namespace engine {
    class Object {
        static inline uint32_t objectCount = 0;

    protected:
        std::string name;
        component::Components components;

        template<typename T> std::optional<T>
        getComponent(const char *);

    public:
        Object(const char *name = nullptr);
        virtual void update(float) {}
        void _draw();
        component::Components &getComponents() { return components; }

#ifdef NDEBUG
        std::string getName() const { return name; }
        void _inspector();
#endif
    };
}
