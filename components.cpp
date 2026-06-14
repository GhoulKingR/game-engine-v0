#include <array>
#include <components.hpp>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <objects.hpp>

#include <utility>
#include <variant>
#include <vector>
#include <ranges>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NDEBUG
#include <string>
#include <algorithm>
#include <format>
#endif

#include "engine.hpp"
#include "shaders/shaders.hpp"

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static std::pair<std::vector<float>, std::vector<uint32_t>> genQuad(float width, float height)
{
    const auto h_width = width / 2.0f;
    const auto h_height = height / 2.0f;

    return {
        {
            -h_width, -h_height, 0.0f, 0.0f,
             h_width, -h_height, 1.0f, 0.0f,
            -h_width,  h_height, 0.0f, 1.0f,
             h_width,  h_height, 1.0f, 1.0f,
         },
         {2, 0, 1, 1, 3, 2}
    };
}

engine::component::Transform::Transform(vec2<float> _scale, vec2<float> _translate, float _rotate)
{
    translate   = _translate;
    scale       = _scale;
    rotate      = _rotate;
}

#ifdef NDEBUG
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
        ImGui::Text("Transform (%s)", prefix);
        ImGui::Indent();
        ImGui::DragFloat2(std::format("Translate ({})", prefix).c_str(), translate.data());
        ImGui::DragFloat2(std::format("Scale ({})", prefix).c_str(), scale.data(), 0.01f);
        ImGui::DragFloat2(std::format("Rotate ({})", prefix).c_str(), &rotate, 1.0f);
        ImGui::Unindent();
        ImGui::NewLine();
    }
}
#endif

glm::mat4 engine::component::Transform::model() const noexcept
{
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model,   glm::vec3(translate.x, translate.y, 0.0)                            );
    model = glm::rotate(model,      glm::radians(rotate),                       glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model,       glm::vec3(scale.x, scale.y, 1.0f)                                   );
    return model;
}

engine::component::Sprite::Sprite(int width, int height, std::vector<Texture *> _tex)
{
    current_texture = 0;
    size            = {width, height};

    // load textures
    stbi_set_flip_vertically_on_load(true);
    textures = std::move(_tex);

    // create vertices and buffers
    auto [vertices, indices] = genQuad(size.x, size.y);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    indexCount = indices.size();
}

void engine::component::Sprite::draw(const glm::mat4 & model) noexcept
{
    if (!hidden)
    {
        auto shdr = shader::default_shader();
        shader::use(shdr);
        shader::setMat4(shdr, "aspectRatio", aspectRatio());
        shader::setInt(shdr, "useColor", 0);
        shader::setMat4(shdr, "model", model * transform.model());

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textures.at(current_texture)->id);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
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
        transform.inspector(std::format("Sprite #{} ", id).c_str());

        // display paths
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

engine::component::Sprite::Sprite(Sprite &&_other)
{
    size            = std::move(_other.size);
    current_texture = _other.current_texture;
    transform       = _other.transform;
    VBO             = _other.VBO;
    EBO             = _other.EBO;
    VAO             = _other.VAO;
    _other.VBO      = 0;
    _other.VAO      = 0;
    _other.EBO      = 0;
    indexCount      = _other.indexCount;
    textures        = std::move(_other.textures);
}

engine::component::Sprite &engine::component::Sprite::operator=(Sprite &&_other)
{
    size            = std::move(_other.size);
    current_texture = _other.current_texture;
    transform       = _other.transform;
    VBO             = _other.VBO;
    EBO             = _other.EBO;
    VAO             = _other.VAO;
    _other.VBO      = 0;
    _other.VAO      = 0;
    _other.EBO      = 0;
    indexCount      = _other.indexCount;
    textures        = std::move(_other.textures);
    return *this;
}

engine::component::Sprite::~Sprite()
{
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

#ifdef NDEBUG
void engine::component::Physics::inspector(uint32_t id) noexcept
{
    if (ImGui::CollapsingHeader(std::format("Physics #{}", id).c_str()))
    {
        ImGui::Indent();
        ImGui::DragFloat("gravity", &gravity, 0.01f);
        for (auto &_s : collisionShapes)
            std::visit([](auto &c){ c->inspector(); }, _s);
        ImGui::Unindent();
        ImGui::NewLine();
    }
}

void engine::component::Physics::draw(const glm::mat4 & model) noexcept
{
    if (!hidden && drawCollisionShapes())
        for (auto &_s : collisionShapes)
            std::visit([model](auto &c) { c->draw(model); }, _s);
}
#endif

void engine::component::Timer::setTimeout(std::function<void()> callback, uint32_t duration_ms, uint32_t times) noexcept
{
    target      = std::chrono::system_clock::now() + std::chrono::milliseconds(duration_ms);
    lambda      = callback;
    count       = times;
    _duration   = duration_ms;
}

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

static inline std::vector<engine::component::collision::Shape> allShapes;

engine::component::collision::Box::Box(Object *parent)
: parent(parent)
{
    allShapes.push_back(this);

#ifdef NDEBUG
    id = ++counter;

    auto [vertices, indices] = genQuad(1, 1);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    indexCount = indices.size();
#endif
}

engine::component::collision::Shape engine::component::collision::Box::checkCollision() const noexcept
{
    auto _mTranslate = parent->transform.translate + transform.translate;
    float myLeft   = _mTranslate.x - (this->size.x / 2.0f);
    float myRight  = _mTranslate.x + (this->size.x / 2.0f);
    float myTop    = _mTranslate.y + (this->size.y / 2.0f);
    float myBottom = _mTranslate.y - (this->size.y / 2.0f);

    for (auto &otherShape : allShapes)
    {
        if (std::holds_alternative<Box *>(otherShape))
        {
            auto otherBox = std::get<Box *>(otherShape);
            if (otherBox == this) continue;

            auto _oTranslate = otherBox->parent->transform.translate + otherBox->transform.translate;
            float otherLeft   = _oTranslate.x - (otherBox->size.x / 2.0f);
            float otherRight  = _oTranslate.x + (otherBox->size.x / 2.0f);
            float otherTop    = _oTranslate.y + (otherBox->size.y / 2.0f);
            float otherBottom = _oTranslate.y - (otherBox->size.y / 2.0f);

            // AABB Separation Axis Theorem
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
    std::erase_if(allShapes, [this](auto &val){
        if (std::holds_alternative<Box *>(val) && std::get<Box *>(val) == this)
            return true;
        else
            return false;
    });

    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

engine::component::collision::Box::Box(Box &&_other)
{
    id              = _other.id;
    size            = std::move(_other.size);
    transform       = _other.transform;
    VBO             = _other.VBO;
    EBO             = _other.EBO;
    VAO             = _other.VAO;
    _other.VBO      = 0;
    _other.VAO      = 0;
    _other.EBO      = 0;
    indexCount      = _other.indexCount;

    // replace old shape with new instance in allShapes vector
    std::replace_if(allShapes.begin(), allShapes.end(),
        [this](auto &val){
            if (std::holds_alternative<Box *>(val) && std::get<Box *>(val) == this)
                return true;
            else
                return false;
        }, this);
}

engine::component::collision::Box&
engine::component::collision::Box::operator=(Box &&_other)
{
    id              = _other.id;
    size            = std::move(_other.size);
    transform       = _other.transform;
    VBO             = _other.VBO;
    EBO             = _other.EBO;
    VAO             = _other.VAO;
    _other.VBO      = 0;
    _other.VAO      = 0;
    _other.EBO      = 0;
    indexCount      = _other.indexCount;

    // replace old shape with new instance in allShapes vector
    std::replace_if(allShapes.begin(), allShapes.end(),
        [this](auto &val){
            if (std::holds_alternative<Box *>(val) && std::get<Box *>(val) == this)
                return true;
            else
                return false;
        }, this);
    return *this;
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

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
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
#else
engine::component::collision::Box::Box(Box &&) = default;
engine::component::collision::Box& engine::component::collision::Box::operator=(Box &&) = default;
#endif
