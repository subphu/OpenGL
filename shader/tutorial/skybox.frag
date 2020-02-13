#version 410 core

uniform samplerCube skybox;

in vec3 TexCoords;

out vec4 fragColor;

void main() {
    fragColor = texture(skybox, TexCoords);
}
