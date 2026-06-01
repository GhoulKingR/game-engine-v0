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
        fs::path p(path);
        fs::current_path(p.parent_path());
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
    // separate paths into files and directories
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

    // directories
    for (const auto &directory : directories) {
        if (ImGui::TreeNode(directory.filename().c_str())) {
            showDirectory(directory);
            ImGui::TreePop();
        }
    }

    // files
    ImGui::Indent();
    static fs::path _file;
    for (const auto &file : files) {
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
