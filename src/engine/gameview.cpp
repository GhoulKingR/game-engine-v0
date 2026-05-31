#include "gameview.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include <cstdint>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>

#include <SDL3/SDL.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

static int viewportWidth = 500;
static int viewportHeight = 500;
static unsigned int viewTexture = 0;
static unsigned int FBO = 0;
static unsigned int RBO1 = 0;
static unsigned int RBO2 = 0;
static unsigned int multisampledFBO = 0;
static float _scale = 1.0;
static glm::vec2 _view_translate(0.0, 0.0);

const float SCROLL_SPEED = 0.1;

engine::vec2i engine::gameview::getviewport() {
    return {viewportWidth, viewportHeight};
}

void engine::gameview::reset() {
    _scale = 1.0;
    _view_translate.x = 0.0;
    _view_translate.y = 0.0;
}

static void constructRenderTexture() {
    // generate regular frame buffer object for rendering to texture
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

    // generate multisample frame buffer object to render to
    glGenFramebuffers(1, &multisampledFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampledFBO);

    // color buffer
    glGenRenderbuffers(1, &RBO1);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO1);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, viewportWidth,
                                     viewportHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, RBO1);

    // depth and stencil buffer
    glGenRenderbuffers(1, &RBO2);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
                                     viewportWidth, viewportHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO2);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void engine::gameview::cleanup() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteFramebuffers(1, &multisampledFBO);
    glDeleteRenderbuffers(1, &RBO1);
    glDeleteRenderbuffers(1, &RBO2);
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

void engine::gameview::render() {
    glViewport(0, 0, viewportWidth, viewportHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampledFBO);

    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    project::scene::draw();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glBlitFramebuffer(0, 0, viewportWidth, viewportHeight, 0, 0, viewportWidth,
                      viewportHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine::gameview::renderGUI() {
    // gui
    ImGuiWindowFlags gvFlags = 0;
    gvFlags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Game View", nullptr, gvFlags);

    if (ImGui::IsWindowHovered()) {
        _scale *= renderer::zoom();
        _view_translate += (renderer::scroll() * SCROLL_SPEED);
    }

    auto windowSize = ImGui::GetContentRegionAvail();
    engine::gameview::set_viewport(
        {static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)});
    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(viewTexture)),
                 windowSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

glm::mat4 engine::gameview::calculate_aspect_ratio() {
    auto pixelSize = 1.0f / glm::vec2(viewportWidth, viewportHeight);
    auto scale = glm::identity<glm::mat4>();
    scale = glm::translate(
        scale, glm::vec3(_view_translate.x, _view_translate.y, 0.0));
    scale = glm::scale(scale, glm::vec3(_scale, _scale, 1.0));
    scale = glm::scale(scale, glm::vec3(pixelSize.x, pixelSize.y, 1.0));
    return scale;
}
