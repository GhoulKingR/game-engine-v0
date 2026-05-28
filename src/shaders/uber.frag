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
