#include "gameview.hpp"
#include "scene.hpp"
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

static int viewportWidth = 500;
static int viewportHeight = 500;
static unsigned int viewTexture = 0;
static unsigned int FBO = 0;
static unsigned int RBO = 0;

engine::vec2i engine::gameview::getviewport() {
    return {viewportWidth, viewportHeight};
}

static void constructRenderTexture() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenTextures(1, &viewTexture);
    glBindTexture(GL_TEXTURE_2D, viewTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           viewTexture, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewportWidth,
                          viewportHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO);
}

void engine::gameview::cleanup() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1, &viewTexture);
}

void engine::gameview::set_viewport(engine::vec2i val) {
    if (viewportWidth != val[0] || viewportHeight != val[1]) {
        viewportWidth = val[0];
        viewportHeight = val[1];
        cleanup();
        constructRenderTexture();
    }
}

void engine::gameview::init() { constructRenderTexture(); }
bool engine::gameview::is_preview() { return true; }

uint32_t engine::gameview::render() {
    glViewport(0, 0, viewportWidth, viewportHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene::draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return viewTexture;
}

glm::mat4 engine::gameview::calculate_aspect_ratio() {
    auto pixelSize = 1.0f / glm::vec2(viewportWidth, viewportHeight);
    auto scale = glm::identity<glm::mat4>();
    scale = glm::scale(scale, glm::vec3(pixelSize.x, pixelSize.y, 1.0));
    return scale;
}
