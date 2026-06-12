#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <print>
#include <vector>
#include <format>
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>

#include "shaders.hpp"
#include "uber.hpp"

static std::vector<uint32_t>    shaders;
static uint32_t                 defaultShader = 0;

uint32_t    engine::shader::default_shader()        { return defaultShader; }
void        engine::shader::use(uint32_t program)   { glUseProgram(program); }

static uint32_t loadShader(const char *vsource, const char *fsource)
{
    if (vsource == nullptr || fsource == nullptr)
    {
        std::println(stderr, "Missing vertex or fragment shader source codes");
        exit(EXIT_FAILURE);
    }

    int success;
    char infoLog[512];

    const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::println(stderr, "VERTEX SHADER :: {}", infoLog);
        exit(EXIT_FAILURE);
    }

    const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glDeleteShader(vertexShader);
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::println("FRAGMENT SHADER :: {}", infoLog);
        exit(EXIT_FAILURE);
    }

    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::println("SHADER PROGRAM :: {}", infoLog);
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void engine::shader::init()
{
    defaultShader = loadShader(uber_vert, uber_frag);
    shaders.push_back(defaultShader);
}

void engine::shader::cleanup()
{
    for (auto &program : shaders)
        glDeleteProgram(program);
}

void engine::shader::setMat4(uint32_t program, const char *name, glm::mat4 data)
{
    assert(name != nullptr);
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(data));
}

void engine::shader::setInt(uint32_t program, const char *name, int data)
{
    assert(name != nullptr);
    glUniform1i(glGetUniformLocation(program, name), data);
}

void engine::shader::setVec3(uint32_t program, const char *name, glm::vec3 data)
{
    assert(name != nullptr);
    glUniform3f(glGetUniformLocation(program, name), data.x, data.y, data.z);
}

void engine::shader::setFloat(uint32_t program, const char *name, float data)
{
    assert(name != nullptr);
    glUniform1f(glGetUniformLocation(program, name), data);
}
