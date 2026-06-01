#pragma once

#include <filesystem>
#include <optional>

namespace engine {
namespace project {
namespace scene {

void load(const std::filesystem::path &);
void save(const std::filesystem::path &);
void unload();
void renderTree();
void draw();

std::optional<std::filesystem::path> current();

} // namespace scene
} // namespace project
} // namespace engine
