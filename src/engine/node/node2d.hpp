#pragma once

#include "node.hpp"
#include <array>
#include <glm/glm.hpp>

namespace engine {
namespace node {
class Node2D : public Node {
  public:
    using vec2 = std::array<float, 2>;

    struct Transform {
        vec2 translate;
        vec2 scale;
        float rotate;
    } transform;
    float scale;
};
} // namespace node
} // namespace engine
