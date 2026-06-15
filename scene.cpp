#ifdef NDEBUG
#include "imgui/imgui.h"
#include <objects.hpp>
#endif

#include <scene.hpp>

static engine::Scene* currentScene;

#ifdef NDEBUG
void engine::scene::_inspector()
{
    static Object *selected = nullptr;

    ImGui::Begin("Scene tree");
    for (auto &obj : currentScene->objects)
    {
        ImGui::Bullet();
        if (ImGui::Selectable(obj->name.c_str(), obj == selected))
            selected = obj;
    }
    ImGui::End();

    if (selected != nullptr)
        object::_inspector(selected);
}
#endif


void engine::scene::_loop(float deltaTime, bool paused)
{
#ifdef NDEBUG
    if (!paused)
#endif
    {
        currentScene->update(deltaTime);
        for (auto &obj : currentScene->objects)
            obj->update(deltaTime);
    }

    for (auto &obj : currentScene->objects)
        object::_draw(obj);
}

// Never used anywhere. But it's here just in case
void engine::scene::unload()                    { currentScene = nullptr; }
void engine::scene::load(engine::Scene *_scene) { currentScene = _scene; }
