#include "scene.hpp"
#include "gameview.hpp"
#include "objects/object.hpp"

#include <cfloat>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <toml++/impl/array.hpp>
#include <toml++/impl/json_formatter.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/table.hpp>
#include <toml++/toml.hpp>
#include <variant>
#include <vector>

#include <imgui/imgui.h>

static std::vector<engine::object::Object> objects;
static std::optional<std::filesystem::path> loadedScene = std::nullopt;

static engine::object::Object getObject(const std::string &type, toml::table *tbl) {
    if (type == "Camera") {
        return engine::object::Camera(tbl);
    } else {
        return engine::object::SceneObject(tbl);
    }
}

void engine::project::scene::load(const std::filesystem::path &path, std::string &error_text) {
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
                objects.push_back(getObject(type, table));
            }
        }
    }
}

void engine::project::scene::unload() {
    objects.clear();
    loadedScene = std::nullopt;
}

void engine::project::scene::draw() {
    for (auto &obj : objects) {
        std::visit(
            [](auto &_o){ _o.draw(); },
            obj
        );
    }
}

// renders tree in the gui
void engine::project::scene::renderTree() {
    static bool open = false;
    static std::optional<uint32_t> selected;

    if (loadedScene.has_value()) {
        ImGui::Begin("Scene");
        auto _objects_iter = std::ranges::views::zip(std::views::iota(0), objects);
        for (const auto &[i, obj] : _objects_iter) {
            ImGui::Bullet();
            bool _s = selected.has_value()
                    && (selected.value() == static_cast<uint32_t>(i));

            auto name = std::visit([](const auto &n){ return n.name; }, obj);
            if (ImGui::Selectable(name.c_str(), _s && open)) {
                selected = i;
                open = true;
            }
        }
        ImGui::NewLine();
        ImGui::Button("+ New object", ImVec2(-FLT_MIN, 0.0f));
        ImGui::End();
    }

    if (open) {
        auto &obj = objects.at(selected.value());
        ImGui::Begin("Inspector", &open);
        std::visit([](auto &_o){_o.inspector();}, obj);
        ImGui::Button("+ Add component", ImVec2(-FLT_MIN, 0.0f));
        ImGui::End();
    }
}

void engine::project::scene::save(const std::filesystem::path &path) {
    toml::array finalObjs;
    for (auto &obj : objects) {
        finalObjs.push_back(
            std::visit(
                [](auto &_a){ return _a.to_table(); }, 
                obj
            )
        );
    }
    auto finalTable = toml::table{{"objects", finalObjs}};

    std::ofstream outFile(path);
    outFile << finalTable;
    outFile.close();
}

std::optional<std::filesystem::path> engine::project::scene::current() {
    return loadedScene;
}
