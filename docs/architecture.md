# GEV0 Architecture

GEV0 is a lightweight 2D game engine written in C++23. It is built as a static library and is designed to be linked into a separate game executable.

The engine currently provides:

* window creation and input through SDL3
* rendering through OpenGL
* vector and matrix math through GLM
* a scene/object/component structure
* optional inspector tooling through ImGui


## Engine Runtime Flow

A game using GEV0 follows this sequence:

```cpp
engine::init("Game Title", 800, 600);
engine::loadScene(&scene);
engine::start();
engine::cleanup();
```

`engine::init()` initializes SDL, creates the application window, creates the OpenGL context, loads OpenGL functions through GLAD, configures the viewport, and initializes the shader system.

`engine::loadScene()` sets the active scene used by the engine loop.

`engine::start()` runs the engine loop. While the engine is running, it processes input, updates and draws the active scene, renders inspector tooling when enabled, swaps the OpenGL window buffer, and clears frame-based input state.

`engine::cleanup()` releases engine resources, shuts down inspector tooling when enabled, destroys the OpenGL context, destroys the SDL window, and shuts down SDL.

The runtime flow can be visualized as:

```text
engine::init()
        ↓
engine::loadScene()
        ↓
engine::start()
        ↓
while running:
    processInput()
    gameLoop(deltaTime)
    guiLoop() when enabled
    SDL_GL_SwapWindow()
    controls::clearFrameStates()
        ↓
engine::cleanup()
```

## Game Loop

The game loop is the repeating cycle that keeps the engine running.

In GEV0, the loop is managed by `engine::start()`.

For every frame, the engine performs the following steps:

```text
while running:
    calculate delta time
    process input
    update and draw the active scene
    render inspector tooling when enabled
    swap the window buffer
    clear frame input state
```

The engine calculates `deltaTime` using the time difference between the current frame and the previous frame.

Using `deltaTime` allows game logic to be based on elapsed time instead of frame count. This ensures gameplay remains consistent across different hardware and frame rates.

For example, movement can be written as:

```cpp
position += velocity * deltaTime;
```

instead of:

```cpp
position += velocity;
```

This prevents objects from moving faster on high frame rate systems and slower on low frame rate systems.

## Scene System

A `Scene` represents a collection of objects that are updated and drawn together.

Internally, a scene stores object pointers:

```cpp
std::vector<Object *> objects;
```

Scenes expose a virtual `update(float)` function that game code can override:

```cpp
virtual void update(float) {}
```

During each frame, the engine calls the scene's internal `_update(float)` method. This method first runs the scene-level `update(float)` function, then updates every object stored in the scene:

```cpp
void Scene::_update(float deltaTime)
{
    update(deltaTime);
    for (auto &obj : objects)
        obj->update(deltaTime);
}
```

Drawing works in a similar way. The scene's internal `_draw()` method loops through every object and calls each object's internal draw function:

```cpp
void Scene::_draw() noexcept
{
    for (auto &obj : objects)
        obj->_draw();
}
```

The relationship between scenes, objects, and components is:

```text
Scene
└── Object
    ├── Object Transform
    └── Components
        ├── Sprite
        │   └── Sprite Transform
        ├── Timer
        └── Physics
            └── Collision Shapes
```

When inspector support is enabled, the scene also provides a scene tree. The scene tree lists the objects stored in the scene and allows one object to be selected for inspection.

```cpp
void Scene::_inspector()
```

A scene can represent a menu screen, a level, or a gameplay state.


## Object System

An `Object` represents a named entity that exists inside a scene.

An object currently contains:

```cpp
std::string             name;
component::Transform    transform;
std::vector<IComponent *> components;
```

Every object has:

* a name
* a world transform
* a collection of attached components

Objects can also implement their own update logic by overriding:

```cpp
virtual void update(float) {}
```

Objects compose gameplay behavior by attaching multiple components together.

For example:

```text
Bird Object
├── Transform
├── Sprite
├── Physics
└── Timer
```

```text
Pipe Object
├── Transform
└── Sprite
```

Objects are stored inside a scene.

### Object Rendering

Objects are responsible for propagating rendering operations to their components.

During drawing, an object generates a model matrix from its transform:

```cpp
auto model = transform.model();
```

The object then passes this model matrix to every attached component:

```cpp
for (auto &_comp : components)
    _comp->draw(model);
```

This allows each component to render itself using the owning object's transform.

### Object Naming

Objects can be created with a custom name.

If no name is provided, the engine automatically generates one.

Examples:

```text
Object #1
Object #2
Object #3
```

This naming system is primarily used by inspector tooling.


## Component System

GEV0 uses a component-based architecture.

Components inherit from a common base type:

```cpp
struct IComponent
```

Components can provide their own rendering behavior by overriding:

```cpp
virtual void draw(const glm::mat4&) noexcept {}
```

Built-in components currently include:

* `Sprite`
* `Timer`
* `Physics`

`Transform` is a built-in object property and is not an `IComponent`.

Some components also contain their own local transform.

For example, `Sprite` contains:

```cpp
Transform transform;
```

This allows a sprite to control its own visual placement independently from the owning object.

### Transform System

`Transform` is a built-in property of an object rather than an attachable component.

A transform stores:

- translation
- rotation
- scale

These values are converted into a model matrix through:

```cpp
glm::mat4 Transform::model()
```

The model matrix is then used by rendering components.

Sprite components also contain their own local transform, allowing visual adjustments to be applied independently from the owning object's transform.

### Timer Component

The `Timer` component provides delayed and repeated callback execution.

Timers are configured through:

```cpp
setTimeout(
    callback,
    duration_ms,
    times
)
```

Internally, timer polling is performed inside the component's `draw()` function.

### Physics and Collision

The current physics implementation supports collision shapes.

At the moment, collision uses axis-aligned bounding boxes (AABB).

Collision shapes are globally registered and checked against one another during collision tests.

Current collision support includes:

- `Box`

They can be initialized using the `physics.newComponent<CollisionShape>(args)` method. For example:

```cpp
engine::component::collision::Box& hitBox = physics.newComponent<engine::component::collision::Box>(this);
```

### Rendering Pipeline

Rendering flows through the engine in multiple stages:

```text
Engine
↓
Scene::_draw()
↓
Object::_draw()
↓
Component::draw()
↓
OpenGL
↓
GPU
```

Objects generate a model matrix from their transform and pass it to every attached component.

Components then use that model matrix to render themselves.

### Component Rendering

Components receive a model matrix generated by the owning object.

The component interface exposes:

```cpp
virtual void draw(const glm::mat4&) noexcept {}
```

Components can use this model matrix to render themselves relative to the object's transform.


## Math Utilities

GEV0 uses both GLM and a custom `vec2` implementation.

The custom `vec2` type is defined in `include/common.hpp` and provides lightweight two-dimensional vector operations.

It supports:

* addition
* subtraction
* scalar multiplication

The type also exposes direct access through `x` and `y` members.

GLM is used for matrix operations such as `glm::mat4`, while `vec2` is used for simpler two-dimensional engine data.


## Texture System

GEV0 includes a dedicated `Texture` type:

```cpp
struct Texture
{
    uint32_t id = 0;
    const char *path = nullptr;
};
```

A texture represents image data uploaded for rendering. The `id` field stores the texture identifier used by the rendering system, while `path` stores the source file path used to load the texture.

The `Sprite` component stores a list of texture pointers:

```cpp
std::vector<Texture *> textures;
```

This allows a sprite to reference one or more textures. The `current_texture` field controls which texture is currently being drawn:

```cpp
uint32_t current_texture = 0;
```

## Input System

GEV0 provides an action-based input system.

Actions are registered by associating a name with a keyboard key:

```cpp
registerAction(
    "jump",
    SDLK_SPACE
);
```

Game code can then query actions instead of directly checking keyboard keys:

```cpp
isActionJustPressed("jump");
```

This decouples gameplay code from physical input devices.

The current input flow is:

```text
Keyboard Input
↓
SDL Event
↓
controls::update()
↓
Input Action
↓
Game Logic
```

The input system currently supports:

* action registration
* action press detection
* frame-based state clearing

## Current Modules

The engine is organized around several core source files:

| File | Purpose |
| --- | --- |
| `engine.cpp` | Engine lifecycle, initialization, main loop, cleanup |
| `scene.cpp` | Scene update, drawing, and inspector integration |
| `objects.cpp` | Game object behavior and component handling |
| `components.cpp` | Built-in components such as Transform, Sprite, Timer, and Physics |
| `textures.cpp` | Texture loading and texture-related OpenGL handling |
| `controls.cpp` | Input handling and action mapping |
| `shaders/shaders.cpp` | Shader creation and rendering support |
| `glad.c` | OpenGL function loading |



## Dependencies

GEV0 depends on:

| Dependency | Purpose                                          |
| ---------- | ------------------------------------------------ |
| SDL3       | Window creation, input, and platform integration |
| OpenGL     | Rendering graphics through the GPU               |
| GLM        | Vector and matrix math                           |
| ImGui      | Optional debug/editor interface                  |
| GLAD       | OpenGL function loading                          |

