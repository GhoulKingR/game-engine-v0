#include "project.hpp"
#include "scene.hpp"
#include <algorithm>
#include <imgui/imgui.h>

#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static std::string error_text;
void engine::project::load(const char *path) {
    assert(path != nullptr);
    try {
        fs::current_path(path);
    } catch (const fs::filesystem_error &err) {
        error_text = err.what();
        ImGui::OpenPopup("Filesystem error");
    }
}

static void handleFile(const fs::path &file) {
    if (file.extension() == ".scene") {
        engine::project::scene::load(file);
    } else {
        error_text = "Unable to handle file type";
        ImGui::OpenPopup("Filesystem error");
    }
}

static void showDirectory(const fs::path &path) {
    std::vector<fs::path> directories;
    std::vector<fs::path> files;
    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            directories.push_back(entry.path());
        } else {
            files.push_back(entry.path());
        }
    }

    std::sort(directories.begin(), directories.end());
    std::sort(files.begin(), files.end());
    for (const auto &directory : directories) {
        if (ImGui::TreeNode(directory.filename().c_str())) {
            showDirectory(directory);
            ImGui::TreePop();
        }
    }
    ImGui::Indent();
    for (const auto &file : files) {
        ImGui::Text("%s", file.filename().c_str());

        if (ImGui::IsItemClicked()) {
            handleFile(file);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
    }
    ImGui::Unindent();
}

void engine::project::renderWindow() {
    if (ImGui::BeginPopup("Filesystem error")) {
        ImGui::Text("%s", error_text.c_str());
        if (ImGui::Button("Close")) {
            error_text = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Begin("Project");
    showDirectory(fs::current_path());
    ImGui::End();
}
