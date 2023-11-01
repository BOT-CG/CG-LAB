#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform float shininess;
uniform vec3 viewPos;
struct DirLight {
    vec3 direction;
    vec3 lightColor;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    vec3 lightColor;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define MAX_DIRECTIONAL_LIGHTS 4
uniform int numDirectionalLights;
uniform DirLight directionalLights[MAX_DIRECTIONAL_LIGHTS];  // MAX_DIRECTIONAL_LIGHTS is a predefined maximum

#define NR_POINT_LIGHTS 4  
uniform int numPointLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
float shine = 32.0;
void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = vec3(0.0);
    for(int i = 0; i < numDirectionalLights; i++)
        result += CalcDirLight(directionalLights[i], norm, viewDir);
    
    // phase 2: Point lights
    for(int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient*light.lightColor  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse *light.lightColor * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular*light.lightColor  * spec * vec3(texture(texture_diffuse1, TexCoords));
    
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * light.lightColor * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * light.lightColor * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * light.lightColor * spec * vec3(texture(texture_diffuse1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 
