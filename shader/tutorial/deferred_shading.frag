#version 410 core

struct Light {
    vec3 position, color;
    float linear, quadratic, radius;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;
uniform vec2 screenSize;
uniform Light light;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 fragColor;

void main() {
    vec2 texCoords = gl_FragCoord.xy / screenSize;
    vec3 FragPos = texture(gPosition, texCoords).rgb;
    vec3 Normal = texture(gNormal, texCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, texCoords).rgb;
    float Specular = texture(gAlbedoSpec, texCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    
    // calculate distance between light source and current fragment
    float distance = length(light.position - FragPos);
    
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.color * spec * Specular;
    // attenuation
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;
    
    fragColor = vec4(lighting, 1.0);
}
