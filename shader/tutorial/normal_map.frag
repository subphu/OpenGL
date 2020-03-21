#version 410 core

struct Light { vec3 ambient, diffuse, specular; };
struct AttenConst { float linear, quadratic; };
struct PointLight {
    vec3 position;
    Light light;
    AttenConst constant;
};

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 viewPos;
uniform PointLight lights[4];

in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 TangentLightPos[4];

out vec4 fragColor;

vec3 calcLight(Light light, vec3 direction, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight pointlight, vec3 normal, vec3 distance, vec3 viewDir);
float calcAttenuation(AttenConst constant, float distance);

void main() {
    vec3 normal = texture(normalMap, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < 4; i++)
        result += calcPointLight(lights[i], normal, TangentLightPos[i] - TangentFragPos, viewDir);
    
    fragColor = vec4(result, 1.0);
}

vec3 calcPointLight(PointLight pointlight, vec3 normal, vec3 distance, vec3 viewDir) {
    vec3 lightDir = normalize(distance);
    
    float attenuation = calcAttenuation(pointlight.constant, length(distance));
    vec3 light = calcLight(pointlight.light, lightDir, normal, viewDir);
    
    return light * attenuation;
}

float calcAttenuation(AttenConst constant, float distance) {
    return 1.0 / (1 + constant.linear * distance + constant.quadratic * (distance * distance));
}

vec3 calcLight(Light light, vec3 direction, vec3 normal, vec3 viewDir) {
    vec3 materialDiffuse = texture(diffuseMap, TexCoords).rgb;
    vec3 materialAmbient = materialDiffuse * 0.1;
    vec3 materialSpecular = materialDiffuse;
    float shininess = 32;
    
    vec3 ambient = light.ambient * materialAmbient;
    
    float cosAngle = max(dot(normal, direction), 0.0);
    vec3 diffuse = light.diffuse * cosAngle * materialDiffuse;
    
    vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = light.specular * spec * materialSpecular;
    return (ambient + diffuse + specular);
}
