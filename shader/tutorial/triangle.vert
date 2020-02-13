#version 410 core

in vec3 position;
in vec4 color;

out vec4 vertColor;

void main() {
    vertColor = color;
    gl_Position = vec4(position, 1.0);
}
