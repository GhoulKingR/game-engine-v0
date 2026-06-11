#ifdef NDEBUG
#include "imgui/imgui.h"
#endif

#include "objects.hpp"
#include <scene.hpp>
#include <optional>

namespace engine
{
    void Scene::_update(float deltaTime)
    {
        update(deltaTime);
        for (auto &obj : objects)
        {
            obj.get().update(deltaTime);
        }
    }

    void Scene::_draw()
    {
        for (auto &obj : objects)
        {
            obj.get()._draw();
        }
    }

#ifdef NDEBUG
    void Scene::_inspector()
    {
        static std::optional<std::reference_wrapper<Object>> selected = std::nullopt;

        ImGui::Begin("Scene tree");
        for (auto &_o : objects)
        {
            ImGui::Bullet();
            auto &obj = _o.get();
            if (ImGui::Selectable(obj.getName().c_str(), selected.has_value() && &obj == &selected.value().get()))
            {
                selected = obj;
            }
        }
        ImGui::End();

        if (selected.has_value())
        {
            selected.value().get()._inspector();
        }
    }
#endif
}
