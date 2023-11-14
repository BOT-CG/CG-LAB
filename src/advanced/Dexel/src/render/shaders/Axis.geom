#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 6) out;

out vec4 color;

uniform float axisSize;
uniform vec4 colorAxisX;
uniform vec4 colorAxisY;
uniform vec4 colorAxisZ;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos = gl_in[0].gl_Position;
    vec4 xDirection = vec4(1.0, 0.0, 0.0, 0.0);
    vec4 yDirection = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 zDirection = vec4(0.0, 0.0, 1.0, 0.0);
    mat4 pvm = projection * view * model;
    vec4 origin = pvm * pos;

    color = colorAxisX;
    gl_Position = origin;
    EmitVertex();
    gl_Position = pvm * (pos + xDirection * axisSize);
    EmitVertex();
    EndPrimitive();

    color = colorAxisY;
    gl_Position = origin;
    EmitVertex();
    gl_Position = pvm * (pos + yDirection * axisSize);
    EmitVertex();
    EndPrimitive();

    color = colorAxisZ;
    gl_Position = origin;
    EmitVertex();
    gl_Position = pvm * (pos + zDirection * axisSize);
    EmitVertex();
    EndPrimitive();
}
