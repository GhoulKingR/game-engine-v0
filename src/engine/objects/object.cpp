#include "object.hpp"
#include "../gameview.hpp"
#include "../shaders.hpp"

#include <format>
#include <ranges>
#include <stdexcept>
#include <toml++/impl/table.hpp>
#include <utility>

#include <imgui/imgui.h>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

engine::object::Object::Object(toml::table *tbl) {
    auto name = (*tbl)["name"].value<std::string>();
    if (name.has_value()) {
        this->name = name.value();
    } else {
        throw std::runtime_error("Missing important field \"name\"");
    }

    translate = {(*tbl)["translate"][0].value_or(0),
                 (*tbl)["translate"][1].value_or(0)};
    scale = {(*tbl)["scale"][0].value_or(1.0f),
             (*tbl)["scale"][1].value_or(1.0f)};
    rotate = (*tbl)["rotate"].value_or(0.0f);
}

void engine::object::Object::inspector(bool show_title) {
    if (show_title) {
        ImGui::SeparatorText(std::format("{} (Object)", name).c_str());
    }

    ImGui::Text("Transform");
    ImGui::Indent();
    ImGui::InputInt2("Translate", translate.data());
    ImGui::InputFloat2("Scale", scale.data());
    ImGui::SliderFloat("Rotate", &rotate, -360.0f, 360.0f);
    ImGui::Unindent();
}

static std::pair<std::vector<float>, std::vector<uint32_t>>
genQuad(float width, float height) {
    const auto h_width = width / 2.0f;
    const auto h_height = height / 2.0f;

    // clang-format off
    return {
        {
            -h_width, -h_height, 0.0f, 0.0f,
             h_width, -h_height, 1.0f, 0.0f,
            -h_width,  h_height, 0.0f, 1.0f,
             h_width,  h_height, 1.0f, 1.0f,
         },
         {2, 0, 1, 1, 3, 2}
    };
    // clang-format on
}

engine::object::Sprite::Sprite(toml::table *tbl,
                               std::filesystem::path &scenePath)
    : Object(tbl) {
    current_texture = (*tbl)["current_texture"].value_or(0);

    // load textures
    stbi_set_flip_vertically_on_load(true);
    auto sceneDirectory = scenePath.parent_path();
    if (auto arr = (*tbl)["textures"].as_array()) {
        arr->for_each([this, sceneDirectory](auto &&node) {
            texturePaths.push_back(sceneDirectory / node.value_or(""));
        });
    }
    std::vector<uint32_t> _textures(texturePaths.size(), 0);
    glGenTextures(_textures.size(), _textures.data());
    for (auto [texture, path] :
         std::ranges::views::zip(_textures, texturePaths)) {
        int width, height, nrChannels;
        auto data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data == nullptr) {
            throw std::runtime_error(
                std::format("Failed to load texture: '{}'", path));
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    textures = std::move(_textures);

    // create vertices and buffers
    auto [vertices, indices] =
        genQuad((*tbl)["size"][0].value_or(0), (*tbl)["size"][1].value_or(0));
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    indexCount = indices.size();
}

void engine::object::Sprite::draw() {
    auto shdr = shader::default_shader();
    shader::use(shdr);
    shader::setMat4(shdr, "aspectRatio", gameview::calculate_aspect_ratio());

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(translate[0], translate[1], 0.0));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(scale[0], scale[1], 1.0f));
    shader::setMat4(shdr, "model", model);

    shader::setInt(shdr, "useColor", 0);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindTexture(GL_TEXTURE_2D, textures.at(current_texture));
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void engine::object::Sprite::inspector(bool show_title) {
    if (show_title) {
        ImGui::SeparatorText(std::format("{} (Sprite)", name).c_str());
    }

    ImGui::Text("Current Texture");
    ImGui::Indent();
    for (auto [i, path] :
         std::ranges::views::zip(std::views::iota(0u), texturePaths)) {
        if (ImGui::Selectable(std::format("{} {}", i, path).c_str(),
                              i == current_texture)) {
            current_texture = i;
        }
    }
    ImGui::Unindent();
    ImGui::Separator();
    object::Object::inspector(false);
}

engine::object::Sprite::~Sprite() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(textures.size(), textures.data());
}

engine::object::Camera::Camera(toml::table *tbl) : Object(tbl) {
    auto [vertices, indices] = genQuad(1.0f, 1.0f);
    glGenBuffers(1, &previewVBO);
    glGenBuffers(1, &previewEBO);
    glGenVertexArrays(1, &previewVAO);
    glBindVertexArray(previewVAO);
    glBindBuffer(GL_ARRAY_BUFFER, previewVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previewEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    indexCount = indices.size();
    viewport = {(*tbl)["size"][0].value_or(0), (*tbl)["size"][1].value_or(0)};
}

void engine::object::Camera::inspector(bool show_title) {
    if (show_title) {
        ImGui::SeparatorText(std::format("{} (Camera)", name).c_str());
    }

    ImGui::Text("Camera");
    ImGui::Indent();
    ImGui::InputInt2("Size", viewport.data());
    ImGui::Unindent();
    ImGui::Separator();
    object::Object::inspector(false);
}

void engine::object::Camera::draw() {
    auto shdr = shader::default_shader();
    shader::use(shdr);
    shader::setMat4(shdr, "aspectRatio", gameview::calculate_aspect_ratio());

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(translate[0], translate[1], 0.0));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(scale[0], scale[1], 1.0f));
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
    glDeleteBuffers(1, &previewVBO);
    glDeleteBuffers(1, &previewEBO);
    glDeleteVertexArrays(1, &previewVAO);
}
