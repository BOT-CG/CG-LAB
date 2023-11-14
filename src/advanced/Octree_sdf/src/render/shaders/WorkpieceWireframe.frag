#version 330 core
out vec4 ColorFragment;

uniform vec3 colorObject;

void main()
{
    ColorFragment = vec4(colorObject, 1.0);
}
