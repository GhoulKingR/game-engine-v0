#include "node.hpp"

#include <memory>
#include <string>
#include <dlfcn.h>

void engine::node::Node::draw() const {
    for (const auto &ptr : children) {
        ptr->draw();
    }
}

std::unique_ptr<engine::node::Node>
engine::node::Node::construct(toml::table &tbl) {
    // typedef void (*RegisterFunc)(const char *, void (*)(void *));

    auto node = std::make_unique<engine::node::Node>();
    node->name = tbl["name"].value_or("");

    auto s = tbl["script"].value<std::string>();
    if (s.has_value()) {
        node->script = dlopen(s->c_str(), RTLD_LAZY);
    }
    return node;
}

engine::node::Node::~Node() { dlclose(script); }
