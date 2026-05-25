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
