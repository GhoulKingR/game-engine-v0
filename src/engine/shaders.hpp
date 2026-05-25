#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine {
class Shaders {
    static auto getSource(const std::string &);
    unsigned int shaderProgram;

  public:
    Shaders(const std::string &, const std::string &);
    ~Shaders();
    void use() const;
    void setMat4(const std::string &name, const glm::mat4 &data) const;
};
} // namespace engine
