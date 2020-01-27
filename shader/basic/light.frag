#version 410 core

uniform vec3 lightColor;
uniform vec2 screenSize;
uniform int glow;

in vec3 Center;
in float Dist;

out vec4 fragColor;

void main() {
    if (glow != 1) {
        fragColor = vec4(lightColor, 1.0);
        return;
    }
    vec2 centerCoord = ((Center.xy + 1.0) / 2.0) * screenSize;
    float dist = Dist / distance(centerCoord, gl_FragCoord.xy);
    float u = dist * dist * 40000;
    float a = u - 0.4;
    fragColor = vec4(lightColor.r*u, lightColor.g*u, lightColor.b*u, a);
}
