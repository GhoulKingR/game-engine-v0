#include <controls.hpp>
#include <map>
#include <string>
#include <tuple>
#include <utility>

struct KeyState {
    bool is_held = false;
    bool just_pressed = false;
    bool just_released = false;
};

static std::pmr::multimap<
    std::string,
    std::pair<SDL_Keycode, KeyState>
> inputMap;

namespace engine {
    namespace controls {
        void registerAction(const char *action, SDL_Keycode key_code) {
            inputMap.insert({action, {key_code, KeyState{}}});
        }

        void update(SDL_Event *event) {
            if (event->type != SDL_EVENT_KEY_DOWN && event->type != SDL_EVENT_KEY_UP) {
                return;
            }

            for (auto &[_, pair] : inputMap) {
                if (pair.first == event->key.key) {
                    if (event->type == SDL_EVENT_KEY_DOWN) {
                        if (!pair.second.is_held) {
                            pair.second.just_pressed = true;
                        }
                        pair.second.is_held = true;
                    }
                    else if (event->type == SDL_EVENT_KEY_UP) {
                        pair.second.just_released = true;
                        pair.second.is_held = false;
                    }
                }
            }
        }

        bool isActionJustPressed(const char *action) {
            auto actions = inputMap.equal_range(action);
            for (auto i = actions.first; i != actions.second; ++i) {
                if (i->second.second.just_pressed) {
                    return true;
                }
            }
            return false;
        }

        void clearFrameStates() {
            for (auto &[_, pair] : inputMap) {
                pair.second.just_pressed = false;
                pair.second.just_released = false;
            }
        }
    }
}
