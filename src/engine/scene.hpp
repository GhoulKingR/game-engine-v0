#pragma once

#include <filesystem>

namespace engine {
namespace project {
namespace scene {

void load(const std::filesystem::path &);
void save(const std::filesystem::path &);
void unload();
void renderTree();
void draw();

} // namespace scene
} // namespace project
} // namespace engine
