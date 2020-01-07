#version 410 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 vTexCoord;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    vTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
