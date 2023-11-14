#version 330 core
out vec4 ColorFragment;

in vec3 Normal;
in vec3 PositionFragment;
in float Tolerance;

uniform vec3 colorLight;
uniform vec3 colorNormal;
uniform vec3 colorGouge;
uniform vec3 colorExcess;
uniform vec3 positionView;
uniform vec3 positionLight;
uniform float toleranceGouge;
uniform float toleranceExcess;

float sigmoid(float x)
{
    return (1 / (1 + exp(-x)));
}

void main()
{
    vec3 colorObject = colorNormal;
    // if (Tolerance > 0 && Tolerance > toleranceGouge) {
    //     colorObject = colorGouge;
    // } else if (Tolerance < 0 && -Tolerance > toleranceExcess) {
    //     colorObject = colorExcess;
    // }

    float a = ((sigmoid(Tolerance) + 1.0f)/ 2.0f);
    colorObject.x = a ;
    colorObject.z = a ;
    colorObject  = normalize(colorObject);


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
