#version 330 core
layout(location = 0) in vec4 aVertex;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 PositionFragment;
out float Tolerance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    Normal = normalize(normalMatrix * aNormal);
    PositionFragment = vec3(model * vec4(aVertex.xyz, 1.0));
    Tolerance = aVertex.w;

    gl_Position = projection * view * vec4(PositionFragment, 1.0);
}
