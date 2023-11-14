#version 330 core
out vec4 ColorFragment;

in vec3 Normal;
in vec3 PositionFragment;

uniform vec3 colorLight;
uniform vec3 colorObject;
uniform vec3 positionView;
uniform vec3 positionLight;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * colorLight;

    vec3 directionLight = normalize(positionLight - PositionFragment);
    vec3 diffuse = colorLight * max(dot(Normal, directionLight), 0.0);

    vec3 directionView = normalize(positionView - PositionFragment);
    vec3 directionReflect = reflect(-directionLight, Normal);
    vec3 directionHalfway = normalize(directionLight + directionView);
    vec3 specular = colorLight * pow(max(dot(Normal, directionHalfway), 0.0), 32.0);

    vec3 result = (ambient + diffuse + specular) * colorObject;
    ColorFragment = vec4(result, 1.0);
}
