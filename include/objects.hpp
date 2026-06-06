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
        std::vector<std::unique_ptr<component::Component>> components;

        template<typename T> std::optional<T>
        getComponent(const char *);

    public:
        component::Transform transform;
        virtual void update(float) {}
        void _draw();
        auto &getComponents() { return components; }
        Object(const char *name = nullptr);
        ~Object() = default;

#ifdef NDEBUG
        std::string getName() const { return name; }
        void _inspector();
#endif
    };
}
