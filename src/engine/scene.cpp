#include "scene.hpp"
#include "objects/object.hpp"

#include <cstdint>
#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <toml++/impl/json_formatter.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/table.hpp>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

static std::vector<std::unique_ptr<engine::object::Object>> objects;
static std::optional<std::filesystem::path> loadedScene = std::nullopt;

static std::unique_ptr<engine::object::Object>
getObject(const std::string &type, toml::table *tbl) {
    if (type == "Object")
        return std::make_unique<engine::object::Object>(tbl);
    if (type == "Camera")
        return std::make_unique<engine::object::Camera>(tbl);
    if (type == "Sprite")
        return std::make_unique<engine::object::Sprite>(tbl,
                                                        loadedScene.value());
    else
        throw std::runtime_error(std::format("Node '{}' does not exist", type));
}

void engine::scene::load(std::string_view path) {
    loadedScene = path;

    try {
        auto tbl = toml::parse_file(loadedScene->c_str());
        if (auto objs = tbl["objects"].as_array()) {
            for (auto &&node : *objs) {
                if (auto table = node.as_table()) {
                    auto type = (*table)["type"].value_or("");
                    auto result = getObject(type, table);
                    objects.push_back(std::move(result));
                }
            }
        }
    } catch (const toml::parse_error &err) {
        throw std::runtime_error(err);
    }
}

void engine::scene::draw() {
    for (const auto &obj : objects) {
        obj->draw();
    }
}

// renders tree in the gui
static std::optional<uint32_t> selected;
void engine::scene::renderTree() {
    ImGui::Begin("Objects");
    for (const auto &[i, obj] :
         std::ranges::views::zip(std::views::iota(0), objects)) {
        ImGui::Bullet();
        if (ImGui::SmallButton(obj->name.c_str())) {
            selected = i;
        }
    }
    ImGui::End();

    if (selected.has_value()) {
        object::Object *obj = objects[selected.value()].get();
        ImGui::Begin(
            std::format("Inspector: {} ({})", obj->name, obj->type()).c_str());
        obj->inspector();
        ImGui::End();
    }
}
