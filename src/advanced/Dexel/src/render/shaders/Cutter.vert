#version 330 core
layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 PositionFragment;

uniform mat4 modelCutter;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 BoxSize;
void main()
{
    Normal = normalize(normalMatrix * aNormal);
    float MaxLength = max(BoxSize.x, max(BoxSize.y, BoxSize.z));
    vec3 tempaVertex = ((aVertex.xyz - (BoxSize / 2.0f)) / MaxLength);
    // vec3 tempaVertex = (aVertex- BoxSize.xyz / MaxLength - 1.0f) / 2.0f;
    PositionFragment = vec3(model * modelCutter * vec4(tempaVertex, 1.0));

    gl_Position = projection * view * vec4(PositionFragment, 1.0);
}
