#include "scene.hpp"
#include "node/node.hpp"

#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <stdexcept>

#include <string>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/table.hpp>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>

static auto getNode(const std::string &name) {
    if (name == "Node")
        return engine::node::Node::construct;
    else
        throw std::runtime_error(std::format("Node '{}' does not exist", name));
}

static std::unique_ptr<engine::node::Node>
getTree(std::vector<std::unique_ptr<engine::node::Node>> &nodes,
        std::multimap<uint32_t, uint32_t> &relationships, uint32_t pos) {
    auto node = std::move(nodes[pos]);
    auto children = relationships.equal_range(pos);
    for (auto child = children.first; child != children.second; child++) {
        auto i = child->second;
        auto n = getTree(nodes, relationships, i);
        n->parent = node.get();
        node->children.push_back(std::move(n));
    }
    return node;
}

static auto constructTree(toml::table &tbl) {
    auto table_nodes = tbl["nodes"];
    std::vector<std::unique_ptr<engine::node::Node>> nodes;
    std::multimap<uint32_t, uint32_t> relationships;

    if (auto array = table_nodes.as_array()) {
        int i = 0;
        array->for_each([&i, &nodes, &relationships](auto &&node) {
            auto &node_data = *node.as_table();
            auto node_type = node_data["type"].value_or("");
            nodes.push_back(std::move(getNode(node_type)(node_data)));

            if (auto children = node_data["children"].as_array()) {
                children->for_each([i, &relationships](auto &&node) {
                    relationships.insert(
                        {static_cast<uint32_t>(i),
                         static_cast<uint32_t>(node.value_or(0))});
                });
            }
            i++;
        });
    }

    return getTree(nodes, relationships, 0);
}

void engine::Scene::load(const char *path) {
    if (path == nullptr) {
        throw std::runtime_error("Scene load path is empty");
    }

    if (this->path == nullptr) {
        this->path = path;
    }

    try {
        auto tbl = toml::parse_file(path);
        parent = constructTree(tbl);
    } catch (const toml::parse_error &err) {
        throw std::runtime_error(err);
    }
}

void engine::Scene::draw() const {
    if (parent != nullptr) {
        parent->draw();
    }
}
