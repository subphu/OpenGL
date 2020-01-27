#version 410 core

uniform mat4 view;
uniform mat4 projection;
uniform float time;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 instance;
layout (location = 7) in float speed;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

mat4 rotationX( in float angle ) {
    return mat4(          1,          0,           0,    0,
                          0, cos(angle), -sin(angle),    0,
                          0, sin(angle),  cos(angle),    0,
                          0,          0,          0,    1);
}

mat4 rotationY( in float angle ) {
    return mat4( cos(angle),        0, sin(angle),    0,
                          0,      1.0,          0,    0,
                -sin(angle),        0, cos(angle),    0,
                          0,        0,          0,    1);
}

mat4 rotationZ( in float angle ) {
    return mat4( cos(angle), -sin(angle),       0,    0,
                 sin(angle),  cos(angle),       0,    0,
                          0,           1,       1,    0,
                          0,           0,       0,    1);
}

void main() {
    TexCoords = texCoords;
    mat4 model = instance * rotationY(time * (speed - 0.3) * 15);
    model[3] = model[3] * rotationX(time * speed);
    model[3] = model[3] * rotationY(time * speed);
    model[3] = model[3] * rotationZ(time * speed);
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(position, 1.0));
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
