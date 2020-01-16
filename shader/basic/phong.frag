#version 410 core

struct Material {
    vec3 ambient, diffuse, specular;
    float shininess;
};

struct Light {
    vec3 ambient, diffuse, specular;
};

uniform Light light;
uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

out vec4 fragColor;

void main() {
    vec3 ambient = material.ambient * light.ambient;
      
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.diffuse;
    
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;
        
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}
