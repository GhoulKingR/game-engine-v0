#include <cstdint>
#include <vector>
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders.hpp"
#include <string>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>

static auto getSource(const std::string &path) {
    std::ifstream inputFile(path);
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

static std::vector<uint32_t> shaders;

static uint32_t loadShader(const std::string &vsource,
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

    auto shaderProgram = glCreateProgram();
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
    return shaderProgram;
}

static uint32_t defaultShader = 0;
void engine::shader::init() {
    defaultShader = loadShader("shaders/uber.vert", "shaders/uber.frag");
    shaders.push_back(defaultShader);
}

uint32_t engine::shader::default_shader() { return defaultShader; }

void engine::shader::cleanup() {
    for (auto &program : shaders) {
        glDeleteProgram(program);
    }
}

void engine::shader::use(uint32_t program) { glUseProgram(program); }

void engine::shader::setMat4(uint32_t program, const std::string &name,
                             const glm::mat4 &data) {
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(data));
}

void engine::shader::setInt(uint32_t program, const std::string &name,
                            int data) {
    glUniform1i(glGetUniformLocation(program, name.c_str()), data);
}

void engine::shader::setVec3(uint32_t program, const std::string &name,
                             const glm::vec3 &data) {
    glUniform3f(glGetUniformLocation(program, name.c_str()), data.x, data.y,
                data.z);
}
