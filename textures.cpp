#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <textures.hpp>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <stb_image.h>

namespace fs = std::filesystem;

engine::Texture::Texture(const char *path)
: path(path)
{
    int width, height, nrChannels;
    auto asset_path = fs::current_path() / path;
    auto data = stbi_load(asset_path.string().c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    
    if (data == nullptr)
    {
        fprintf(stderr, "Failed to load texture: '%s'", path);
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

engine::Texture::~Texture()
{
    if (id != 0) glDeleteTextures(1, &id);
}
