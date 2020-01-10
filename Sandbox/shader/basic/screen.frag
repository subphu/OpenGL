#version 410 core

uniform sampler2D texture0;
uniform int useFilter;

in vec2 TexCoords;

out vec4 fragColor;

const float offset = 1.0 / 300.0;
vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right
);

vec3 filterWith(float kernel[9]);

void main() {
    float blur[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    float sharpen[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
    vec3 color = texture(texture0, TexCoords).rgb;
    color = useFilter == 1 ? filterWith(sharpen) : color;
    color = useFilter == 2 ? filterWith(blur) : color;
    color = useFilter == 3 ? vec3(1) - color : color;
    color = useFilter == 4 ? vec3(0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b) : color;
    
    fragColor = vec4(color, 1.0);
}

vec3 filterWith(float kernel[9]) {
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(texture0, TexCoords.st + offsets[i]));
    }
    
    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++) color += sampleTex[i] * kernel[i];
    
    return color;
}
