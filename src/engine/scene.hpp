#pragma once

#include <string_view>

namespace engine {
namespace scene {
    void load(std::string_view);
    void renderTree();
    void draw();
}
} // namespace engine
