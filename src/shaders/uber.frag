#version 330 core

in vec2 UV;
out vec4 FragColor;
uniform sampler2D oTexture;

void main() {
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 color = texture(oTexture, UV);
    if (color.a == 0) {
        discard;
    }

    FragColor = color;
}
