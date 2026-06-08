#ifdef NDEBUG
#include "imgui/imgui.h"
#endif

#include "objects.hpp"
#include <scene.hpp>

namespace engine {
    void Scene::_update(float deltaTime) {
        update(deltaTime);
        for (auto obj : objects) {
            obj->update(deltaTime);
        }
    }

    void Scene::_draw() {
        for (auto obj : objects) {
            obj->_draw();
        }
    }

#ifdef NDEBUG
    void Scene::_inspector() {
        static Object *selected = nullptr;
        ImGui::Begin("Scene tree");
            for (auto obj : objects) {
                ImGui::Bullet();
                if (ImGui::Selectable(
                    obj->getName().c_str(), obj == selected))
                {
                    selected = obj;
                }
            }
        ImGui::End();

        if (selected != nullptr) {
            selected->_inspector();
        }
    }
#endif
}
