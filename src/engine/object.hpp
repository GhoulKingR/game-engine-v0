#pragma once

#include "renderer.hpp"
#include "shaders.hpp"
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

namespace engine {
class Object {
    uint32_t VAO = 0, VBO = 0, EBO = 0;
    uint32_t size;

  protected:
    const Shaders &shaders;
    const Renderer &renderer;

    std::vector<uint32_t> textures;
    uint32_t currentTexture = 0;
    static std::pair<std::vector<float>, std::vector<uint32_t>>
    genQuad(float width, float height);
    static std::vector<uint32_t> genTextures(const std::vector<std::string> &);
    void loadVertices(const std::vector<float> &,
                      const std::vector<uint32_t> &);

  public:
    struct Transforms {
        float translate[2] = {-0.8, 0.0};
        float scale[2] = {0.2, 0.2};
        float rotate;
    } transforms;

    const std::string name;
    // Object(const Shaders &, const Renderer &, const std::string &name,
    // uint64_t,
    //        uint64_t, const std::string &);
    Object(const Shaders &, const Renderer &, const std::string &);
    ~Object();
    void draw() const;
};
} // namespace engine
