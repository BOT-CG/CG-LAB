#version 330 core
layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 PositionFragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    Normal = normalize(normalMatrix * aNormal);
    PositionFragment = vec3(model * vec4(aVertex, 1.0));

    gl_Position = projection * view * vec4(PositionFragment, 1.0);
}
