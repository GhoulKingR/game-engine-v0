#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders.hpp"
#include <string>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>

auto engine::Shaders::getSource(const std::string &path) {
    std::ifstream inputFile(path);
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

engine::Shaders::Shaders(const std::string &vsource,
                         const std::string &fsource) {
    int success;
    char infoLog[512];

    const auto vShaderCode = getSource(vsource);
    const auto vShaderPtr = vShaderCode.c_str();
    const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderPtr, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::format("VERTEX SHADER :: {}", infoLog));
    }

    const auto fShaderCode = getSource(fsource);
    const auto fShaderPtr = fShaderCode.c_str();
    const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderPtr, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glDeleteShader(vertexShader);
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        throw std::runtime_error(std::format("FRAGMENT SHADER :: {}", infoLog));
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        throw std::runtime_error(std::format("SHADER PROGRAM :: {}", infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

engine::Shaders::~Shaders() { glDeleteProgram(shaderProgram); }

void engine::Shaders::use() const { glUseProgram(shaderProgram); }

void engine::Shaders::setMat4(const std::string &name,
                              const glm::mat4 &data) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                       GL_FALSE, glm::value_ptr(data));
}
