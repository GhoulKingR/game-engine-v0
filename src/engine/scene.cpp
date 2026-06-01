#include "scene.hpp"
#include "gameview.hpp"
#include "objects/object.hpp"

#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <toml++/impl/array.hpp>
#include <toml++/impl/json_formatter.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/table.hpp>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>

#include <imgui/imgui.h>

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
    else {
        std::println("Node '{}' does not exist", type);
        return nullptr;
    }
}

void engine::project::scene::load(const std::filesystem::path &path,
                                  std::string &error_text) {
    if (loadedScene.has_value()) {
        if (loadedScene.value() == path) {
            return;
        }
        unload();
    }

    loadedScene = path;
    gameview::reset();

    // try {
    auto tbl = toml::parse_file(loadedScene->c_str());
    if (!tbl) {
        std::stringstream ss;
        ss << "Error parsing scene: " << tbl.error();
        error_text = ss.str();
        ImGui::OpenPopup("Filesystem error");
    }

    if (auto objs = tbl.table()["objects"].as_array()) {
        for (auto &&node : *objs) {
            if (auto table = node.as_table()) {
                auto type = (*table)["type"].value_or("");
                auto result = getObject(type, table);
                if (result != nullptr) {
                    objects.push_back(std::move(result));
                }
            }
        }
    }
}

void engine::project::scene::unload() {
    objects.clear();
    loadedScene = std::nullopt;
}

void engine::project::scene::draw() {
    for (const auto &obj : objects) {
        obj->draw();
    }
}

// renders tree in the gui
void engine::project::scene::renderTree() {
    static bool open = false;
    static std::optional<uint32_t> selected;

    ImGui::Begin("Objects");
    for (const auto &[i, obj] :
         std::ranges::views::zip(std::views::iota(0), objects)) {
        ImGui::Bullet();
        bool _s = selected.has_value() &&
                  (selected.value() == static_cast<uint32_t>(i));
        if (ImGui::Selectable(obj->name.c_str(), _s && open)) {
            selected = i;
            open = true;
        }
    }
    ImGui::End();

    if (open) {
        object::Object *obj = objects[selected.value()].get();
        ImGui::Begin("Inspector", &open);
        obj->inspector();
        ImGui::End();
    }
}

void engine::project::scene::save(const std::filesystem::path &path) {
    toml::array finalObjs;
    for (const auto &obj : objects) {
        finalObjs.push_back(obj->to_table());
    }
    auto finalTable = toml::table{{"objects", finalObjs}};

    std::ofstream outFile(path);
    outFile << finalTable;
    outFile.close();
}

std::optional<std::filesystem::path> engine::project::scene::current() {
    return loadedScene;
}
