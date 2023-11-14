#version 330 core
layout(location = 0) in vec3 aCenter;
layout(location = 1) in float aSize;
layout(location = 2) in float aDistances[8];

out VS_OUT
{
    float size;
    float distances[8];
}
vs_out;

void main()
{
    vs_out.size = aSize;
    vs_out.distances = aDistances;
    gl_Position = vec4(aCenter, 1.0);
}
