#pragma once

#include <toml++/toml.hpp>
#include <vector>

namespace engine {
namespace node {
class Node {
    void *script = nullptr;
    struct ScriptHelper {};

  public:
    ~Node();
    Node *parent = nullptr;
    std::string name;
    std::vector<std::unique_ptr<node::Node>> children;
    void draw() const;

    static std::unique_ptr<Node> construct(toml::table &);
};
} // namespace node
} // namespace engine
