#version 410 core

uniform samplerCube skybox;
uniform int reflection;
uniform float refractRatio;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 Position;

out vec4 fragColor;

void main() {
    vec3 I = normalize(Position - viewPos);
    vec3 R = (reflection == 1) ?
        reflect(I, normalize(Normal)) :
        refract(I, normalize(Normal), refractRatio);
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}
