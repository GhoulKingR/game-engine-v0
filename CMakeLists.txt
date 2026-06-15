cmake_minimum_required(VERSION 4.0)
project(gev0)

find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)

function(link_imgui TARGET)
    add_library(${TARGET}
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_impl_sdl3.cpp
        imgui/imgui_impl_opengl3.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
    )
    target_include_directories(${TARGET} PRIVATE
        "${PROJECT_SOURCE_DIR}/_internal_include"
    )
    target_link_libraries(${TARGET}
        OpenGL::GL
        SDL3::SDL3
        glm::glm
    )
endfunction()

add_library(${PROJECT_NAME} STATIC
    components.cpp
    controls.cpp
    engine.cpp
    glad.c
    objects.cpp
    scene.cpp
    textures.cpp
    shaders/shaders.cpp
)

if (DEBUG)
    link_imgui(imgui_lib)
    target_link_libraries(${PROJECT_NAME}
        imgui_lib
        OpenGL::GL
        SDL3::SDL3
        glm::glm
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        -DNDEBUG
    )
else()
    target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        SDL3::SDL3
        glm::glm
    )
endif()

if (SANITIZER)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -g
        -O0
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address,undefined
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3
    )
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -fno-exceptions
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include/"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/_internal_include"
)
