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

function (add_asset_target TARGET)
    set (multiValue SRC DST)
    cmake_parse_arguments (arg "" "" "${multiValue}" ${ARGN})
    set (ASSET_DIR "${arg_DST}/assets")
    add_custom_command(
        OUTPUT ${ASSET_DIR}
        COMMAND ${CMAKE_COMMAND}
                -E copy_directory
                "${arg_SRC}" "${ASSET_DIR}"
        DEPENDS ${arg_SRC}
        COMMENT "Copying asset files"
    )
    add_custom_target(
        ${TARGET}
        DEPENDS ${ASSET_DIR}
    )
endfunction()

function (add_shader_target TARGET)
    set (multiValue VERTEX FRAGMENT)
    cmake_parse_arguments (arg "" "" "${multiValue}" ${ARGN})
    set (SHADERS_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    add_custom_command(
        OUTPUT ${SHADERS_OUT}
        COMMAND ${CMAKE_COMMAND}
                -E make_directory
                ${SHADERS_OUT}
        COMMENT "Creating ${SHADERS_OUT} folder"
    )
    add_custom_command(
        OUTPUT "${SHADERS_OUT}/uber.vert"
        COMMAND ${CMAKE_COMMAND}
                -E copy
                "${PROJECT_SOURCE_DIR}/${arg_VERTEX}"
                "${SHADERS_OUT}/uber.vert"
        DEPENDS ${SHADERS_OUT} ${arg_VERTEX}
        COMMENT "Copying vertex shader from ${arg_VERTEX}"
    )
    add_custom_command(
        OUTPUT "${SHADERS_OUT}/uber.frag"
        COMMAND ${CMAKE_COMMAND}
                -E copy
                "${PROJECT_SOURCE_DIR}/${arg_FRAGMENT}"
                "${SHADERS_OUT}/uber.frag"
        DEPENDS ${SHADERS_OUT} ${arg_FRAGMENT}
        COMMENT "Copying fragment shader from ${arg_FRAGMENT}"
    )
    add_custom_target(
        ${TARGET}
        DEPENDS "${SHADERS_OUT}/uber.frag"
                "${SHADERS_OUT}/uber.vert"
            )
endfunction()

add_asset_target(assets
    SRC "${PROJECT_SOURCE_DIR}/game-assets"
    DST "${CMAKE_CURRENT_BINARY_DIR}"
)
add_shader_target(shaders
    VERTEX src/shaders/uber.vert
    FRAGMENT src/shaders/uber.frag
)

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

add_dependencies(${PROJECT_NAME} shaders assets)

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
)
target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)
