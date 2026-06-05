#pragma once

static constexpr auto uber_vert = R"(
#version 330 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aTexUV;

uniform mat4 aspectRatio;
uniform mat4 model;

out vec2 UV;

void main() {
    gl_Position = aspectRatio * model * vec4(aPos, 0.0, 1.0);
    UV = aTexUV;
}
)";

static constexpr auto uber_frag = R"(
#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform int useColor = 0;
uniform vec3 iColor;
uniform sampler2D oTexture;

void main() {
    if (useColor == 1) {
        FragColor = vec4(iColor, 1.0);
    } else {
        vec4 color = texture(oTexture, UV);
        if (color.a == 0) {
            discard;
        }
        FragColor = color;
    }
}
)";
