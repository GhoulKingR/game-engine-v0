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
        auto model = components.transform().has_value()
            ? components.transform()->model()
            : glm::identity<glm::mat4>();

        if (components.sprite().has_value()) {
            components.sprite()->draw(model);
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
