#version 410 core

struct Light { vec3 ambient, diffuse, specular; };
struct AttenConst { float linear, quadratic; };
struct PointLight {
    vec3 position;
    Light light;
    AttenConst constant;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform PointLight lights[4];

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec3 TangentLightPos[4];

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    TexCoords = texCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;
    
    for (int i = 0; i < 4; i++) {
        TangentLightPos[i] = TBN * lights[i].position;
    }
    
    gl_Position = projection * view * vec4(FragPos, 1.0);

}
