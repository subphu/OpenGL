#version 410 core

struct Material {
    sampler2D textureDiffuse;
    vec3 ambient, diffuse, specular;
    float shininess;
};

struct Light { vec3 ambient, diffuse, specular; };
struct AttenConst { float linear, quadratic; };

struct DirLight {
    vec3 direction;
    Light light;
};

struct PointLight {
    vec3 position;
    Light light;
    AttenConst constant;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    Light light;
    AttenConst constant;
};

uniform vec3 viewPos;
uniform int totalPointLights;
uniform int totalSpotLights;
uniform int useTexture;

uniform DirLight dirLight;
uniform PointLight pointLights[10];
uniform SpotLight spotLights[10];
uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 fragColor;

vec3 calcLight(Light light, vec3 direction, vec3 normal, vec3 viewDir);
vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight pointlight, vec3 normal, vec3 distance, vec3 viewDir);
vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 distance, vec3 viewDir);

float calcIntensity(vec3 lightDir, vec3 spotLightDir, float cutOff, float outerCutOff);
float calcAttenuation(AttenConst constant, float distance);

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < totalPointLights; i++)
        result += calcPointLight(pointLights[i], norm, pointLights[i].position - FragPos, viewDir);

    for(int i = 0; i < totalSpotLights; i++)
        result += calcSpotLight(spotLights[i], norm, spotLights[i].position - FragPos, viewDir);
    
    fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-dirLight.direction);
    return calcLight(dirLight.light, lightDir, normal, viewDir);
}

vec3 calcPointLight(PointLight pointlight, vec3 normal, vec3 distance, vec3 viewDir) {
    vec3 lightDir = normalize(distance);
    
    float attenuation = calcAttenuation(pointlight.constant, length(distance));
    vec3 light = calcLight(pointlight.light, lightDir, normal, viewDir);
    
    return light * attenuation;
}

vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 distance, vec3 viewDir) {
    vec3 lightDir = normalize(distance);
    vec3 spotLightDir = normalize(-spotLight.direction);
    
    float attenuation = calcAttenuation(spotLight.constant, length(distance));
    float intensity = calcIntensity(lightDir, spotLightDir, spotLight.cutOff, spotLight.outerCutOff);
    vec3 light = calcLight(spotLight.light, lightDir, normal, viewDir);
    
    return light * attenuation * intensity;
}

float calcIntensity(vec3 lightDir, vec3 spotLightDir, float cutOff, float outerCutOff) {
    float theta = dot(lightDir, spotLightDir);
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    return intensity;
}

float calcAttenuation(AttenConst constant, float distance) {
    return 1.0 / (1 + constant.linear * distance + constant.quadratic * (distance * distance));
}

vec3 diffuse;
vec3 ambient;
vec3 specular;
float shininess;
vec3 calcLight(Light light, vec3 direction, vec3 normal, vec3 viewDir) {
    
    if (useTexture == 1) {
        diffuse = texture(material.textureDiffuse, TexCoords).rgb;
        ambient = diffuse * 0.1;
        specular = diffuse;
        shininess = 32;
    } else {
        diffuse = material.diffuse;
        ambient = material.ambient;
        specular = material.specular;
        shininess = material.shininess;
    }
    
    vec3 ambient = light.ambient * ambient;
    
    float cosAngle = max(dot(normal, direction), 0.0);
    vec3 diffuse = light.diffuse * cosAngle * diffuse;
    
    vec3 reflectDir = reflect(-direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * specular;
    return (ambient + diffuse + specular);
}
