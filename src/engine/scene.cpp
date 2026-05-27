#include "scene.hpp"
#include "node/node.hpp"

#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/table.hpp>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

struct Entity{};
std::vector<Entity> entities;

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

static std::optional<std::string> loadedScene = std::nullopt;
static std::unique_ptr<engine::node::Node> root = nullptr;

void engine::scene::load(std::string_view path) {
    loadedScene = path;

    try {
        auto tbl = toml::parse_file(loadedScene->c_str());
        root = constructTree(tbl);
    } catch (const toml::parse_error &err) {
        throw std::runtime_error(err);
    }
}

void engine::scene::draw() {
    if (root != nullptr) {
        root->draw();
    }
}

static void renderItem(engine::node::Node *node) {
    if (ImGui::TreeNode(node->name.c_str())) {
        const auto &children = node->children;
        if (children.size() > 0) {
            for (const auto &child : children) {
                renderItem(child.get());
            }
        }
        ImGui::TreePop();
    }
}

// renders tree in the gui
void engine::scene::renderTree() {
    ImGui::Begin("Scene Tree");
    if (root != nullptr) {
        renderItem(root.get());
    }
    ImGui::End();
}
