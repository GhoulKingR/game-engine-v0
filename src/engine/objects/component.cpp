#include "component.hpp"
#include <toml++/impl/array.hpp>
#include <toml++/impl/table.hpp>
#include <utility>
#include <vector>
#include <imgui/imgui.h>
#include <ranges>

#include "object.hpp"
#include "../shaders.hpp"
#include "../gameview.hpp"

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

namespace comp = engine::object::component;
comp::Transform::Transform(toml::table *tbl) {
    translate = {
        (*tbl)["translate"][0].value_or(0),
        (*tbl)["translate"][1].value_or(0)
    };
    scale = {
        (*tbl)["scale"][0].value_or(1.0f),
        (*tbl)["scale"][1].value_or(1.0f)
    };
    rotate = (*tbl)["rotate"].value_or(0.0f);
}

void comp::Transform::inspector() {
    ImGui::Text("Transform");
    ImGui::Indent();
        ImGui::InputInt2("Translate", translate.data());
        ImGui::InputFloat2("Scale", scale.data());
        ImGui::DragFloat("Rotate", &rotate, 1.0f, -360.0f, 360.0f);
    ImGui::Unindent();
}

comp::Transform::Transform(Transform &&_other) {
    scale = std::move(_other.scale);
    translate = std::move(_other.translate);
    rotate = std::move(_other.rotate);
}

void comp::Transform::operator=(Transform &&_other) {
    scale = std::move(_other.scale);
    translate = std::move(_other.translate);
    rotate = std::move(_other.rotate);
}

glm::mat4 comp::Transform::model() const {
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(translate[0], translate[1], 0.0));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(scale[0], scale[1], 1.0f));
    return model;
}

std::pair<const char *, toml::table> comp::Transform::to_table() const {
    return {
        "transform",
        toml::table{
            {"translate", toml::array{translate[0], translate[1]}},
            {"scale", toml::array{scale[0], scale[1]}},
            {"rotate", rotate},
        }
    };
}


comp::Sprite::Sprite(toml::table *tbl, std::filesystem::path &scenePath) {
    current_texture = (*tbl)["current_texture"].value_or(0);
    size = {
        (*tbl)["size"][0].value_or(0),
        (*tbl)["size"][1].value_or(0),
    };

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
            std::println(stderr, "Failed to load texture: '{}'", path.c_str());
            continue;
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
    auto [vertices, indices] = genQuad(size[0], size[1]);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
            GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), 
            vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
}

void comp::Sprite::draw(glm::mat4 &model) {
    auto shdr = shader::default_shader();
    shader::use(shdr);
    shader::setMat4(shdr, "aspectRatio", gameview::calculate_aspect_ratio());

    shader::setMat4(shdr, "model", model);
    shader::setInt(shdr, "useColor", 0);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindTexture(GL_TEXTURE_2D, textures.at(current_texture));
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void comp::Sprite::inspector() {
    ImGui::Text("Sprite");
    ImGui::Indent();
        // display paths
        auto _paths = std::ranges::views::zip(std::views::iota(0u), texturePaths);
        for (auto [i, path] : _paths) {
            auto name = std::format("{} - {}", i, path.filename().c_str());
            if (ImGui::Selectable(name.c_str(), i == current_texture)) {
                current_texture = i;
            }
        }
    ImGui::Unindent();
}

comp::Sprite::~Sprite() {
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (textures.size() > 0) glDeleteTextures(textures.size(), textures.data());
}

std::pair<const char *, toml::table> comp::Sprite::to_table() const {
    auto paths = texturePaths
        |   std::views::transform([](auto p) {
                return std::filesystem::proximate(p).string();
            });
    toml::array _textures;
    for (const auto &p : paths) {
        _textures.push_back(p);
    }

    return {
        "sprite",
        toml::table{
            {"current", current_texture},
            {"size", toml::array{size[0], size[1]}},
            {"textures", _textures}
        }
    };
}

comp::Sprite::Sprite(Sprite &&_other) {
    size = std::move(_other.size);
    current_texture = _other.current_texture;

    VBO = _other.VBO;
    EBO = _other.EBO;
    VAO = _other.VAO;
    _other.VBO = 0;
    _other.VAO = 0;
    _other.EBO = 0;
    
    indexCount = _other.indexCount;
    textures = std::move(_other.textures);
    texturePaths = std::move(_other.texturePaths);
}

comp::Sprite& comp::Sprite::operator=(Sprite &&_other) {
    size = std::move(_other.size);
    current_texture = _other.current_texture;

    VBO = _other.VBO;
    EBO = _other.EBO;
    VAO = _other.VAO;
    _other.VBO = 0;
    _other.VAO = 0;
    _other.EBO = 0;
    
    indexCount = _other.indexCount;
    textures = std::move(_other.textures);
    texturePaths = std::move(_other.texturePaths);
    return *this;
}
