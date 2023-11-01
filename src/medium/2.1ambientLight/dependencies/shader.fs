#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blendRatio;
uniform vec3 lightColor;
void main()
{
	// linearly interpolate between both textures (80% container, 10% awesomeface)
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * lightColor;

	vec3 result = ambient;
	FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), blendRatio)*vec4(result,1.0);
	
}