#version 410 core

uniform vec3 lightPos;
uniform float maxDistance;

in vec4 FragPos;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos);
    gl_FragDepth = lightDistance / maxDistance;
}

