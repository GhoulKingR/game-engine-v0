#include "components.hpp"
#include <format>
#include <objects.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NDEBUG
#include "imgui/imgui.h"
#endif

namespace engine {
    Object::Object(const char *name) {
        if (name == nullptr) {
            objectCount++;
            this->name = std::format("Object #{}", objectCount);
        } else {
            this->name = name;
        }
    }

    void Object::_draw() {
        auto model = transform.model();
        for (auto &_comp : components)
            _comp->draw(model);
    }

#ifdef NDEBUG
    void Object::_inspector() {
        uint32_t i = 0;
        ImGui::Begin("Inspector");
        ImGui::SeparatorText(name.c_str());
        transform.inspector();
        for (auto &_comp : components)
            _comp->inspector(++i);
        ImGui::End();
    }
#endif
}
