#version 410 core

uniform vec3 lightColor;
uniform vec2 screenSize;
uniform bool glow;

in vec3 Center;
in vec3 Corner;

out vec4 fragColor;

void main() {
    if (!glow) {
        fragColor = vec4(lightColor, 1.0);
        return;
    }
    vec2 centerCoord = ((Center.xy + 1.0) / 2.0) * screenSize;
    vec2 cornerCoord = ((Corner.xy + 1.0) / 2.0) * screenSize;
    float dist = distance(centerCoord, gl_FragCoord.xy) / distance(centerCoord, cornerCoord);
    float u = 1 / (dist * dist * dist * 1.5);
    float a = max((1 - dist), 0.0) * 3;
    
    vec3 color = pow(lightColor, vec3(2.2)) * u;
    fragColor = vec4(color, a*a);
}

