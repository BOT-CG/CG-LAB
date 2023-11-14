#version 330 core
out vec4 ColorFragment;

in GS_OUT
{
    vec3 positionFragment;
    vec3 normal;
}
fs_in;

uniform vec3 colorLight;
uniform vec3 colorObject;
uniform vec3 positionView;
uniform vec3 positionLight;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * colorLight;

    vec3 directionLight = normalize(positionLight - fs_in.positionFragment);
    vec3 diffuse = colorLight * max(dot(fs_in.normal, directionLight), 0.0);

    vec3 directionView = normalize(positionView - fs_in.positionFragment);
    vec3 directionReflect = reflect(-directionLight, fs_in.normal);
    vec3 directionHalfway = normalize(directionLight + directionView);
    vec3 specular = colorLight * pow(max(dot(fs_in.normal, directionHalfway), 0.0), 32.0);

    vec3 result = (ambient + diffuse + specular) * colorObject;
    ColorFragment = vec4(result, 1.0);
}
