#version 410 core

uniform vec4 alpha;

in vec4 vertColor;

out vec4 fragColor;

void main() {
    fragColor = vertColor * alpha;
}
