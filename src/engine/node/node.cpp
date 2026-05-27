#include "node.hpp"
#include <memory>
#include <string>

void engine::node::Node::draw() const {
    for (const auto &ptr : children) {
        ptr->draw();
    }
}

std::unique_ptr<engine::node::Node>
engine::node::Node::construct(toml::table &tbl) {
    auto node = std::make_unique<engine::node::Node>();
    node->name = tbl["name"].value_or("");
    return node;
}

