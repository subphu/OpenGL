#version 410 core

uniform samplerCube environmentMap;

in vec3 Position;

out vec4 fragColor;

void main() {
    vec3 envColor = texture(environmentMap, Position).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));
    
    fragColor = vec4(envColor, 1.0);
}
