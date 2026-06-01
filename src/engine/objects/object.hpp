#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <toml++/toml.hpp>
#include <vector>

namespace engine {
using vec2 = std::array<float, 2>;
using vec2i = std::array<int, 2>;

namespace object {
struct Object {
    static inline uint32_t objCount = 0;
    std::string name;
    vec2 scale{0};
    vec2i translate{0};
    float rotate = 0;

    virtual std::string type() { return "Object"; }
    virtual void draw() {}
    virtual ~Object() {}
    virtual void inspector(bool show_title = true);
    Object(toml::table *tbl);
    virtual toml::table to_table();
};

struct Sprite : public Object {
    virtual std::string type() override { return "Sprite"; }
    Sprite(toml::table *tbl, std::filesystem::path &scenePath);
    void draw() override;
    void inspector(bool show_title = true) override;
    toml::table to_table() override;
    ~Sprite();

    vec2i size{0};
    uint32_t current_texture = 0;
    std::vector<std::filesystem::path> texturePaths;
  private:
    uint32_t VBO = 0, EBO = 0, VAO = 0, indexCount = 0;
    std::vector<uint32_t> textures;
};

struct Camera : public Object {
    vec2i viewport;
    Camera(toml::table *tbl);
    virtual std::string type() override { return "Camera"; }
    void draw() override;
    void inspector(bool show_title = true) override;
    toml::table to_table() override;
    ~Camera();

  private:
    uint32_t previewVBO = 0, previewEBO = 0, previewVAO = 0, indexCount = 0;
};
} // namespace object
} // namespace engine
