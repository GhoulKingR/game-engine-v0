#pragma once

#include "objects.hpp"
#include <vector>

namespace engine {
    // Game scene (collection of objects for rendering)
    struct Scene {
        std::vector<Object *> objects;
        virtual void update(float) {}
    };

    // helper functions for managing the scene data
    // The only ones needed to be used is load and unload.
    namespace scene {
        // load a scene object to view
        void load(Scene *);
        // unload the current scene object to view
        void unload();

        void _loop(float, bool = false);
        void _inspector();
    }
}
