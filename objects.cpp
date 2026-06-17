#include "components.hpp"
#include <format>
#include <objects.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NDEBUG
#include "imgui/imgui.h"
#endif

engine::Object::Object(const char *name) {
    if (name == nullptr) {
        objectCount++;
        this->name = std::format("Object #{}", objectCount);
    } else {
        this->name = name;
    }
}

void engine::object::_draw(Object *obj) noexcept
{
    auto model = obj->transform.model();
    for (auto &_comp : obj->_components)
        _comp->draw(model);
}

#ifdef NDEBUG
void engine::object::_inspector(Object *obj) noexcept
{
    uint32_t i = 0;

    ImGui::Begin("Inspector");
    ImGui::SeparatorText(obj->name.c_str());
    obj->transform.inspector();

    for (auto &_comp : obj->_components)
        _comp->inspector(++i);

    ImGui::End();
}
#endif
