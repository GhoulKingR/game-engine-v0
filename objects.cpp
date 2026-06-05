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
            this->name = std::format("Object #{}", name);
        } else {
            this->name = name;
        }
    }

    void Object::_draw() {
        auto model = components.transform.model();

        for (auto sprite : components.get<component::Sprite>()) {
            sprite.get().draw(model);
        }
    }

#ifdef NDEBUG
    void Object::_inspector() {
        ImGui::Begin("Inspector");
            ImGui::SeparatorText(name.c_str());
            components.inspector();
        ImGui::End();
    }
#endif
}
