#version 330 core
out vec4 ColorFragment;

in vec4 color;

void main()
{
    ColorFragment = color;
}
