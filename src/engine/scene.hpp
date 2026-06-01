#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace engine {
namespace project {
namespace scene {

void load(const std::filesystem::path &, std::string &);
void save(const std::filesystem::path &);
void unload();
void renderTree();
void draw();

std::optional<std::filesystem::path> current();

} // namespace scene
} // namespace project
} // namespace engine
