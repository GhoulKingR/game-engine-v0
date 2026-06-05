#pragma once

#include <SDL3/SDL.h>

namespace engine {
    namespace controls {
        void registerAction(const char *action, SDL_Keycode key_code);
        void update(SDL_Event *event);
        bool isActionJustPressed(const char *);
        void clearFrameStates();
    }
}
