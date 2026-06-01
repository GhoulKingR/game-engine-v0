#include "project.hpp"
#include "scene.hpp"
#include <imgui/imgui.h>

#include <cassert>
#include <filesystem>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

static std::string error_text;
void engine::project::load(const char *path) {
    if (path != nullptr) {
        std::error_code ec;
        fs::path p(path);
        fs::current_path(p.parent_path(), ec);

        if (ec) {
            error_text = ec.message();
            ImGui::OpenPopup("Filesystem error");
        }
    }
}

static void handleFile(const fs::path &file) {
    if (file.extension() == ".scene") {
        engine::project::scene::load(file, error_text);
    } else {
        error_text = "Unable to handle file type";
        ImGui::OpenPopup("Filesystem error");
    }
}

static void showDirectory(const fs::path &path) {
    // separate paths into files and directories
    auto all = fs::directory_iterator(path) |
               std::ranges::views::transform([](const auto &p) {
                   if (p.is_directory()) {
                       return std::pair{"directory", p};
                   } else {
                       return std::pair{"file", p};
                   }
               });
    std::map<const char *, std::multiset<fs::path>> map;
    for (const auto &p : all) {
        map[p.first].insert(p.second);
    }

    // directories
    for (const auto &dir : map["directory"]) {
        if (ImGui::TreeNode(dir.filename().c_str())) {
            showDirectory(dir);
            ImGui::TreePop();
        }
    }

    // files
    ImGui::Indent();
    static fs::path _file;
    for (const auto &file : map["file"]) {
        if (ImGui::Selectable(file.filename().c_str(), _file == file)) {
            handleFile(file);
            _file = file;
        }
    }
    ImGui::Unindent();
}

void engine::project::renderWindow() {
    ImGui::Begin("Project");
    showDirectory(fs::current_path());

    if (ImGui::BeginPopupModal("Filesystem error")) {
        ImGui::Text("%s", error_text.c_str());
        if (ImGui::Button("Close")) {
            error_text = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}
