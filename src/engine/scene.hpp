#pragma once

#include "node/node.hpp"

namespace engine {
class Scene {
    std::unique_ptr<node::Node> parent = nullptr;
    const char *path = nullptr;

  public:
    void load(const char *);
    void draw() const;

    auto getRoot() const { return parent.get(); }
};
} // namespace engine
