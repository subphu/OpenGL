#version 410 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;

out vec3 Center;
out float Dist;

void main() {
    mat4 clip = projection * view * model;
    gl_Position = clip * vec4(position, 1.0);
    vec4 centerPos = clip * vec4(0.0, 0.0, 0.0, 1.0);
    Center = centerPos.xyz / centerPos.w;
    
    mat4 translate = mat4(1.0);
    translate[3] = view[3];
    clip = projection * translate * model;
    vec4 cornerPos1 = clip * vec4(0.0,  1.0, 0.0, 1.0);
    vec4 cornerPos2 = clip * vec4(0.0, -1.0, 0.0, 1.0);
    vec3 corner1 = cornerPos1.xyz / cornerPos1.w;
    vec3 corner2 = cornerPos2.xyz / cornerPos2.w;
    Dist = distance(corner1, corner2);
    
}

