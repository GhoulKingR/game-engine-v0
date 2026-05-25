#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "renderer.hpp"
#include "object.hpp"
#include "shaders.hpp"

std::pair<std::vector<float>, std::vector<uint32_t>>
engine::Object::genQuad(float width, float height) {
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
         {0, 1, 2, 2, 1, 3}
    };
    // clang-format on
}

std::vector<uint32_t>
engine::Object::genTextures(const std::vector<std::string> &paths) {
    struct TextureData {
        int width, height, nrChannels;
        stbi_uc *data;
    };

    stbi_set_flip_vertically_on_load(true);
    std::vector<TextureData> textureData;
    std::ranges::transform(
        paths.cbegin(), paths.cend(), std::back_inserter(textureData),
        [](const auto &path) {
            TextureData result;
            result.data = stbi_load(path.c_str(), &result.width, &result.height,
                                    &result.nrChannels, 0);
            if (result.data == NULL) {
                throw std::runtime_error(
                    std::format("Failed to load texture '{}'", path));
            }
            return result;
        });

    std::vector<uint32_t> textures(textureData.size(), 0);
    glGenTextures(textureData.size(), textures.data());

    for (auto [i, data] : std::ranges::views::zip(textures, textureData)) {
        glBindTexture(GL_TEXTURE_2D, i);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data.data);
    }

    return textures;
}

engine::Object::Object(const Shaders &shaders, const Renderer &renderer,
                       const std::string &name)
    : shaders(shaders), renderer(renderer), name(name) {}

void engine::Object::loadVertices(const std::vector<float> &vertices,
                                  const std::vector<uint32_t> &indices) {
    size = indices.size();

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
}

engine::Object::~Object() {
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (textures.size() > 0)
        glDeleteTextures(textures.size(), textures.data());
}

void engine::Object::draw() const {
    shaders.use();

    shaders.setMat4("aspectRatio", renderer.aspectRatio());

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(transforms.translate[0],
                                            transforms.translate[1], 0.0));
    model = glm::scale(
        model, glm::vec3(transforms.scale[0], transforms.scale[1], 1.0f));
    model = glm::rotate(model, glm::radians(transforms.rotate),
                        glm::vec3(0.0, 0.0, 1.0));
    shaders.setMat4("model", model);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindTexture(GL_TEXTURE_2D, textures.at(currentTexture));

    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
}
