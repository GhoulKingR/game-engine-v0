#ifdef NDEBUG
#include "imgui/imgui.h"
#include <objects.hpp>
#endif

#include <scene.hpp>

static engine::Scene* currentScene = nullptr;

#ifdef NDEBUG
void engine::scene::_inspector()
{
    if (currentScene == nullptr) return;
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


#ifdef NDEBUG
void engine::scene::_loop(float deltaTime, bool paused) {
    if (currentScene == nullptr) return;

    if (!paused)
#else
void engine::scene::_loop(float deltaTime) {
#endif
    {
        if (currentScene->update) currentScene->update(deltaTime);
        for (auto &obj : currentScene->objects)
            if (obj->update) obj->update(deltaTime);
    }

    for (auto &obj : currentScene->objects)
        object::_draw(obj);
}

// Never used anywhere. But it's here just in case
void engine::scene::unload()                    { currentScene = nullptr; }
void engine::scene::load(engine::Scene *_scene) { currentScene = _scene; }
