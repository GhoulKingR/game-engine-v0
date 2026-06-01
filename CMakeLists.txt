cmake_minimum_required(VERSION 4.0)
project(flappybird)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

add_subdirectory("${PROJECT_SOURCE_DIR}/nativefiledialog-extended")

include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/glad.c
    src/engine/renderer.cpp
    src/engine/shaders.cpp
    src/engine/gui.cpp
    src/engine/gameview.cpp
    src/engine/scene.cpp
    src/engine/project.cpp
    src/engine/objects/object.cpp

    # ImGui files
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_sdl3.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    SDL3::SDL3
    glm::glm
    tomlplusplus::tomlplusplus
    nfd
    ${CMAKE_DL_LIBS}
)

if(SANITIZER)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -g
        -fno-omit-frame-pointer
        -O0
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
    )
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -DDEBUG
    -O3
    -fno-exceptions
)
target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)
