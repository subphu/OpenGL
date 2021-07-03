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
uniform float maxDistance;
uniform int totalPointLights;
uniform int totalSpotLights;
uniform bool useTexture;

uniform samplerCube depthMap[4];
uniform DirLight dirLight;
uniform PointLight pointLights[10];
uniform SpotLight spotLights[10];
uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 fragColor;

float getDepth(int texIdx, vec3 fragDistance);
float calcPointLightShadow(vec3 fragDistance, int idx);
vec3 calcLight(Light light, vec3 direction, vec3 normal, vec3 viewDir);
vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight pointlight, vec3 normal, vec3 distance, vec3 viewDir);
vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 distance, vec3 viewDir);

float calcIntensity(vec3 lightDir, vec3 spotLightDir, float cutOff, float outerCutOff);
float calcAttenuation(AttenConst constant, float distance);

void main() {
//    vec3 fragToLight = FragPos - pointLights[1].position;
//    float closestDepth = texture(depthMap[1], fragToLight).r;
//    fragColor = vec4(vec3(closestDepth), 1.0);
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < totalPointLights; i++) {
        vec3 distance = pointLights[i].position - FragPos;
        float shadow = 1.0 - calcPointLightShadow(-distance, i);
        result += calcPointLight(pointLights[i], norm, distance, viewDir) * shadow;
    }

    for(int i = 0; i < totalSpotLights; i++)
        result += calcSpotLight(spotLights[i], norm, spotLights[i].position - FragPos, viewDir);

    fragColor = vec4(result, 1.0);
}

vec3 gridSamplingDisk[20] = vec3[] (
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float calcPointLightShadow(vec3 fragDistance, int idx) {
    float shadow = 0.0;
    float bias = 0.5;

    float fragDepth = length(fragDistance) - bias;
    float closestDepth = getDepth(idx, fragDistance);
    closestDepth *= maxDistance;
    shadow += (fragDepth > closestDepth) ? 1 : 0;

    return shadow ;
}

float getDepth(int texIdx, vec3 fragDistance) {
    float closestDepth;
    switch (texIdx) {
        case 0: closestDepth = texture(depthMap[0], fragDistance).r; break;
        case 1: closestDepth = texture(depthMap[1], fragDistance).r; break;
        case 2: closestDepth = texture(depthMap[2], fragDistance).r; break;
        case 3: closestDepth = texture(depthMap[3], fragDistance).r; break;
    }
    return closestDepth;
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

vec3 calcLight(Light light, vec3 direction, vec3 normal, vec3 viewDir) {
    vec3 materialDiffuse;
    vec3 materialAmbient;
    vec3 materialSpecular;
    float shininess;
    
    if (useTexture) {
        materialDiffuse = texture(material.textureDiffuse, TexCoords).rgb;
        materialAmbient = materialDiffuse * 0.1;
        materialSpecular = materialDiffuse;
        shininess = 32;
    } else {
        materialDiffuse = material.diffuse;
        materialAmbient = material.ambient;
        materialSpecular = material.specular;
        shininess = material.shininess;
    }
    
    vec3 ambient = light.ambient * materialAmbient;
    
    float cosAngle = max(dot(normal, direction), 0.0);
    vec3 diffuse = light.diffuse * cosAngle * materialDiffuse;
    
    vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = light.specular * spec * materialSpecular;
    return (ambient + diffuse + specular);
}
