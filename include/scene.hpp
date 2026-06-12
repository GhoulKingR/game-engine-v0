#pragma once

#include "objects.hpp"
#include <vector>

namespace engine {
    class Scene {
        virtual void init() {}

    protected:
        std::vector<Object *> objects;

    public:
        virtual void update(float) {}
        void _update(float);
        void _draw();
#ifdef NDEBUG
        void _inspector();
#endif
    };
}
