#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_stdinc.h"
#include <components.hpp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <objects.hpp>

#include <ranges>
#include <print>
#include <utility>
#include <vector>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NDEBUG
#include <string>
#include <format>
#endif

#include "engine.hpp"
#include "shaders/shaders.hpp"

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Start Transform ----------------------------------------------------------------------------------------
engine::component::Transform::Transform(vec2<float> _scale, vec2<float> _translate, float _rotate)
{
    translate   = _translate;
    scale       = _scale;
    rotate      = _rotate;
}

#ifdef NDEBUG
// Transform object inspector panel
void engine::component::Transform::inspector(const char *prefix) noexcept
{
    if (prefix == nullptr)
    {
        ImGui::Text("Transform");
        ImGui::Indent();
        ImGui::DragFloat2("Translate", translate.data());
        ImGui::DragFloat2("Scale", scale.data(), 0.01f);
        ImGui::DragFloat2("Rotate", &rotate, 1.0f);
        ImGui::Unindent();
        ImGui::NewLine();
    }
    else
    {
        if (ImGui::CollapsingHeader(std::format("Transform ({})", prefix).c_str()))
        {
            ImGui::Indent();
            ImGui::DragFloat2(std::format("Translate ({})", prefix).c_str(), translate.data());
            ImGui::DragFloat2(std::format("Scale ({})", prefix).c_str(), scale.data(), 0.01f);
            ImGui::DragFloat2(std::format("Rotate ({})", prefix).c_str(), &rotate, 1.0f);
            ImGui::Unindent();
            ImGui::NewLine();
        }

        // If prefix is not nullptr, that means it's part of a component
        // So we have this branch so it renders the elements in a more
        // component-friendly manner.
    }
}
#endif

// Convert the transform's data into a singular model matrix
glm::mat4 engine::component::Transform::model() const noexcept
{
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model,   glm::vec3(translate.x, translate.y, 0.0)                            );
    model = glm::rotate(model,      glm::radians(rotate),                       glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model,       glm::vec3(scale.x, scale.y, 1.0f)                                   );
    return model;
}
// End Transform ----------------------------------------------------------------------------------------


// Start Sprite ----------------------------------------------------------------------------------------
engine::component::Sprite::Sprite(int width, int height, std::vector<Texture *> _tex)
{
    current_texture = 0;
    size            = {width, height};

    // load textures
    stbi_set_flip_vertically_on_load(true);
    textures = std::move(_tex);
}

void engine::component::Sprite::draw(const glm::mat4 & model) noexcept
{
    if (!hidden)
    {
        auto m = glm::identity<glm::mat4>();
        m = glm::scale(m, {size.x, size.y, 1.0f});
        m = model * transform.model() * m;
        auto shdr = shader::default_shader();
        shader::use(shdr);
        shader::setMat4(shdr, "aspectRatio", aspectRatio());
        shader::setInt(shdr, "useColor", 0);
        shader::setMat4(shdr, "model", m);

        bindQuad();
        glBindTexture(GL_TEXTURE_2D, textures.at(current_texture)->id);
        glDrawElements(GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, 0);
    }
}

#ifdef NDEBUG
void engine::component::Sprite::inspector(uint32_t id) noexcept
{
    auto s = std::format("Sprite #{}", id);
    if (ImGui::CollapsingHeader(s.c_str()))
    {
        ImGui::Indent();
        ImGui::Checkbox(std::format("hidden (#{})", id).c_str(), &hidden);

        // display transform inspector first
        transform.inspector(s.c_str());

        // display texture paths in inspector window
        for (auto [i, texture] : std::ranges::views::zip(std::views::iota(0u), textures))
        {
            auto name = std::format("{} (#{})", texture->path, id);
            if (ImGui::Selectable(name.c_str(), i == current_texture))
                current_texture = i;
        }
        ImGui::Unindent();
        ImGui::NewLine();
    }
}
#endif
// End Sprite ----------------------------------------------------------------------------------------

// Start Physics ----------------------------------------------------------------------------------------
#ifdef NDEBUG
void engine::component::Physics::inspector(uint32_t id) noexcept
{
    if (ImGui::CollapsingHeader(std::format("Physics #{}", id).c_str()))
    {
        ImGui::Indent();

        // it makes sense that the hidden checkbox is only visible if
        // collision shapes are drawn. It's also easy to implement so
        // no harm done really
        if (drawCollisionShapes())
            ImGui::Checkbox(std::format("hidden (#{})", id).c_str(), &hidden);

        ImGui::DragFloat("gravity", &gravity, 0.01f);

        // display inspector panels for all collision shapes
        for (auto &c : collisionShapes)
            c->inspector();

        ImGui::Unindent();
        ImGui::NewLine();
    }
}

void engine::component::Physics::draw(const glm::mat4 & model) noexcept
{
    if (!hidden && drawCollisionShapes())
        for (auto &c : collisionShapes)
            c->draw(model);
}
#endif
// End Phyiscs ----------------------------------------------------------------------------------------

// Start Timer ----------------------------------------------------------------------------------------
void engine::component::Timer::setTimeout(std::function<void()> callback,
                                          uint32_t duration_ms,
                                          uint32_t times) noexcept
{
    target      = std::chrono::system_clock::now() + std::chrono::milliseconds(duration_ms);
    lambda      = callback;
    count       = times;
    _duration   = duration_ms;
}

// Timer uses the draw method to poll its internals to simulate an
// async event loop.
void engine::component::Timer::draw(const glm::mat4 &) noexcept
{
    if (count > 0 && lambda != nullptr)
    {
        auto now = std::chrono::system_clock::now();
        if (now >= target)
        {
            count--;
            lambda();

            if (count == 0)
            {
                lambda = nullptr;
                _duration = 0;
            }
            else
            {
                target = std::chrono::system_clock::now() + std::chrono::milliseconds(_duration);
            }
        }
    }
}
// End Timer ----------------------------------------------------------------------------------------

// Start Sound --------------------------------------------------------------------------------------
// register a sound file to the component
void engine::component::Sound::addSound(const char *name, const char *src, bool looping)
{
    Data wav;
    wav._looping = looping;
    if (SDL_LoadWAV(src, &wav._spec, &wav._buffer, &wav._length) == 0)
    {
        std::println(stderr, "Could not open WAV file! SDL_Error: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    wav._stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wav._spec, nullptr, nullptr);
    _sounds[name] = wav;
}

void engine::component::Sound::play(const char *name)
{
    if (_sounds.contains(name))
    {
        auto &wav = _sounds[name];
        SDL_ResumeAudioStreamDevice(wav._stream);
        SDL_PutAudioStreamData(wav._stream, wav._buffer, wav._length);
    }
}
void engine::component::Sound::draw(const glm::mat4&) noexcept
{
    auto iterator = _sounds
        | std::ranges::views::filter([](auto &p){
            return p.second._looping; 
          });
    for (auto &[_, wav] : iterator)
    {
        if (SDL_GetAudioStreamQueued(wav._stream) < (int)wav._length)
            SDL_PutAudioStreamData(wav._stream, wav._buffer, wav._length);
    }
}

void engine::component::Sound::stop(const char *name)
{
    if (_sounds.contains(name))
    {
        Data &wav = _sounds[name];
        SDL_PauseAudioStreamDevice(wav._stream);
        SDL_ClearAudioStream(wav._stream);
    }
}

engine::component::Sound::~Sound()
{
    for (auto &[_, wav] : _sounds)
    {
        SDL_free(wav._buffer);
        SDL_free(wav._stream);
    }
}
// End Sound --------------------------------------------------------------------------------------

/// Physics collisions
// Box collider start -----------------------------------------------------------
static inline std::vector<engine::component::collision::ICollisionShape*> allShapes;

engine::component::collision::Box::Box(Object *parent)
: parent(parent)
{
    allShapes.push_back(this);
#ifdef NDEBUG
    id = ++counter;
#endif
}

engine::component::collision::ICollisionShape*
engine::component::collision::Box::checkCollision() const noexcept
{
    auto _mTranslate = parent->transform.translate + transform.translate;
    float myLeft   = _mTranslate.x - (this->size.x / 2.0f);
    float myRight  = _mTranslate.x + (this->size.x / 2.0f);
    float myTop    = _mTranslate.y + (this->size.y / 2.0f);
    float myBottom = _mTranslate.y - (this->size.y / 2.0f);

    for (auto &otherShape : allShapes)
    {
        if (otherShape == this) continue;

        if (auto otherBox = dynamic_cast<Box*>(otherShape))
        {
            auto _oTranslate = otherBox->parent->transform.translate + otherBox->transform.translate;
            float otherLeft   = _oTranslate.x - (otherBox->size.x / 2.0f);
            float otherRight  = _oTranslate.x + (otherBox->size.x / 2.0f);
            float otherTop    = _oTranslate.y + (otherBox->size.y / 2.0f);
            float otherBottom = _oTranslate.y - (otherBox->size.y / 2.0f);

            // Using AABB Separation Axis Theorem to detect collision.
            bool overlapX = (myLeft <= otherRight) && (myRight >= otherLeft);
            bool overlapY = (myBottom <= otherTop) && (myTop >= otherBottom);

            if (overlapX && overlapY)
                return otherBox;
        }
    }

    return nullptr;
}

#ifdef NDEBUG
engine::component::collision::Box::~Box()
{
    // deletes current element from the allShapes vector.
    std::erase(allShapes, this);
}

void engine::component::collision::Box::draw(const glm::mat4 &model) const noexcept
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shdr = shader::default_shader();
    shader::use(shdr);
    shader::setMat4(shdr, "aspectRatio", aspectRatio());
    shader::setInt(shdr, "useColor", 1);
    shader::setVec3(shdr, "iColor", {0.3f, 0.1f, 0.5f});
    shader::setFloat(shdr, "alpha", 0.5);

    auto m = glm::identity<glm::mat4>();
    m = glm::scale(m, {size.x, size.y, 0.0f});
    m = model * transform.model() * m;
    shader::setMat4(shdr, "model", m);

    bindQuad();
    glDrawElements(GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, 0);
}

void engine::component::collision::Box::inspector() noexcept
{
    auto prefix = std::format("Box #{}", id);
    if (ImGui::CollapsingHeader(prefix.c_str()))
    {
        ImGui::DragFloat2(std::format("Size ({})", prefix).c_str(), size.data());
        transform.inspector(prefix.c_str());
    }
}
#endif
// Box collider End -----------------------------------------------------------
