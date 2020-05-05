#version 410 core

uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;

out vec3 Position;

void main() {
    Position = position;
    gl_Position = projection * view * vec4(position, 1.0);
}

