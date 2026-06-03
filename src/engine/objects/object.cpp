#include "object.hpp"
#include "../gameview.hpp"
#include "../shaders.hpp"
#include "../scene.hpp"
#include "component.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <print>
#include <ranges>
#include <string_view>
#include <toml++/impl/table.hpp>
#include <utility>

#include <imgui/imgui.h>
#include <variant>
#include <vector>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <stb_image.h>

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

namespace comp = engine::object::component;
static comp::Component getComponent(std::string_view key, toml::table *tbl) {
    if (key == "transform") {
        return engine::object::component::Transform(tbl);
    } else if (key == "sprite") {
        auto path = engine::project::scene::current();
        return engine::object::component::Sprite(tbl, path.value());
    } else {
        std::println(stderr, "Invalid key {}", key);
        exit(EXIT_FAILURE);
    }
}

static std::vector<comp::Component> loadComponents(toml::table *tbl) {
    auto tables = (*tbl) |
        std::ranges::views::filter([](const auto &t){ return t.second.is_table(); }) |
        std::ranges::views::transform([](const auto &_t) {
            const auto &[key, table] = _t;
            auto t = table.as_table();
            return getComponent(key.str(), t);
        });
    return std::vector(tables.begin(), tables.end());
}

engine::object::SceneObject::SceneObject(toml::table *tbl) {
    static uint32_t objCount = 0;
    auto name = (*tbl)["name"].value<std::string>();

    if (name.has_value()) {
        this->name = name.value();
    } else {
        objCount++;
        this->name = std::format("SceneObject #{}", objCount);
    }

    components = loadComponents(tbl);
}

toml::table engine::object::SceneObject::to_table() {
    toml::table tbl;
    tbl.insert_or_assign("name", name);

    for (const auto &comp : components) {
        auto table = std::visit(
            [](const auto &_c) { return _c.to_table(); },
            comp
        );
        tbl.insert_or_assign(table.first, table.second);
    }

    return tbl;
}


void engine::object::SceneObject::inspector() {
    ImGui::SeparatorText(name.c_str());

    for (auto &comp : components) {
        std::visit(
            [](auto &_c){ _c.inspector(); },
            comp
        );
    }
}

void engine::object::SceneObject::draw() {
    auto model = glm::identity<glm::mat4>();

    for (auto &comp : components) {
        std::visit(
            overloaded{
                [&model](component::Transform &t) {
                    model = glm::translate(model, glm::vec3(t.translate[0], t.translate[1], 0.0));
                    model = glm::rotate(model, glm::radians(t.rotate), glm::vec3(0.0, 0.0, 1.0));
                    model = glm::scale(model, glm::vec3(t.scale[0], t.scale[1], 1.0f));
                },
                [&model](component::Sprite &s) {
                    s.draw(model);
                }
            },
            comp
        );
    }
}

engine::object::SceneObject::SceneObject(SceneObject &&_other) {
    name = std::move(_other.name);
    components = std::move(_other.components);
}

engine::object::SceneObject&
engine::object::SceneObject::operator=(SceneObject &&_other) {
    name = std::move(_other.name);
    components = std::move(_other.components);
    return *this;
}

static std::pair<std::vector<float>, std::vector<uint32_t>>
genQuad(float width, float height) {
    const auto h_width = width / 2.0f;
    const auto h_height = height / 2.0f;

    return {
        {
            -h_width, -h_height, 0.0f, 0.0f,
             h_width, -h_height, 1.0f, 0.0f,
            -h_width,  h_height, 0.0f, 1.0f,
             h_width,  h_height, 1.0f, 1.0f,
         },
         {2, 0, 1, 1, 3, 2}
    };
}

engine::object::Camera::Camera(toml::table *tbl) {
    // : transform() {
    auto [vertices, indices] = genQuad(1.0f, 1.0f);
    glGenBuffers(1, &previewVBO);
    glGenBuffers(1, &previewEBO);
    glGenVertexArrays(1, &previewVAO);
    glBindVertexArray(previewVAO);

    glBindBuffer(GL_ARRAY_BUFFER, previewVBO);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
        vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previewEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
        indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    indexCount = indices.size();
    viewport = {
        (*tbl)["size"][0].value_or(0),
        (*tbl)["size"][1].value_or(0)
    };

    static uint32_t objCount = 0;
    auto name = (*tbl)["name"].value<std::string>();

    if (name.has_value()) {
        this->name = name.value();
    } else {
        objCount++;
        this->name = std::format("Camera #{}", objCount);
    }

    transform = std::move(
            std::get<component::Transform>(loadComponents(tbl)[0])
    );
}

void engine::object::Camera::inspector() {
    ImGui::SeparatorText(std::format("{} (Camera)", name).c_str());

    transform.inspector();
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Indent();
        ImGui::InputInt2("Size", viewport.data());
    ImGui::Unindent();
    ImGui::Separator();
}

void engine::object::Camera::draw() {
    auto shdr = shader::default_shader();
    shader::use(shdr);
    shader::setMat4(shdr, "aspectRatio", gameview::calculate_aspect_ratio());

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(transform.translate[0], transform.translate[1], 0.0));
    model = glm::rotate(model, glm::radians(transform.rotate), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(transform.scale[0], transform.scale[1], 1.0f));
    model = glm::scale(model, glm::vec3(viewport[0], viewport[1], 1.0f));
    shader::setMat4(shdr, "model", model);

    shader::setInt(shdr, "useColor", 1);
    shader::setVec3(shdr, "iColor", glm::vec3(1.0, 0.0, 1.0));

    glBindVertexArray(previewVAO);
    glBindBuffer(GL_ARRAY_BUFFER, previewVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previewEBO);
    glDrawElements(GL_LINE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

engine::object::Camera::~Camera() {
    if (previewVBO != 0) glDeleteBuffers(1, &previewVBO);
    if (previewEBO != 0) glDeleteBuffers(1, &previewEBO);
    if (previewVAO != 0) glDeleteVertexArrays(1, &previewVAO);
}

toml::table engine::object::Camera::to_table() {
    auto _transform = transform.to_table();
    return toml::table {
        {"type", "Camera"},
        {"name", name},
        {"size", toml::array{viewport[0], viewport[1]}},
        {_transform.first, _transform.second},
    };
}

engine::object::Camera::Camera(Camera &&_other) {
    name = std::move(_other.name);
    viewport = std::move(_other.viewport);
    transform = std::move(_other.transform);
    transform = std::move(_other.transform);

    previewVBO = _other.previewVBO; _other.previewVBO = 0;
    previewEBO = _other.previewEBO; _other.previewEBO = 0;
    previewVAO = _other.previewVAO; _other.previewVAO = 0;

    indexCount = _other.indexCount;
}

engine::object::Camera& engine::object::Camera::operator=(Camera &&_other) {
    name = std::move(_other.name);
    viewport = std::move(_other.viewport);
    transform = std::move(_other.transform);
    transform = std::move(_other.transform);

    previewVBO = _other.previewVBO; _other.previewVBO = 0;
    previewEBO = _other.previewEBO; _other.previewEBO = 0;
    previewVAO = _other.previewVAO; _other.previewVAO = 0;

    indexCount = _other.indexCount;
    return *this;
}
