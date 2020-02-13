#version 410 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool invertNormal;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main() {
    int isInvert = invertNormal ? -1 : 1;
    Normal = transpose(inverse(mat3(model))) * (normal * isInvert);
    FragPos = vec3(model * vec4(position, 1.0));
    TexCoords = texCoords;
    
    gl_Position = projection * view * model * vec4(position, 1.0);
}
