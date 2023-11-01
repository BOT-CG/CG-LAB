#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos; 
// texture samplers
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blendRatio;
uniform vec3 lightColor;
uniform vec3 lightPos;


void main()
{
	// linearly interpolate between both textures (80% container, 10% awesomeface)
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec3 result = (ambient + diffuse) * lightColor;
	FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), blendRatio)*vec4(result,1.0);
	
}