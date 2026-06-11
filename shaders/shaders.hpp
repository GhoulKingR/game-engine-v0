#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine {
namespace shader {
void init();
uint32_t default_shader();
uint32_t load(const char *vSource, const char *fSource);
void use(uint32_t);
void setMat4(uint32_t program, const char *name, const glm::mat4 &data);
void setVec3(uint32_t program, const char *name, const glm::vec3 &data);
void setInt(uint32_t program, const char *name, int data);
void cleanup();
} // namespace shader
} // namespace engine
