#version 450 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 CutterPos;

out VS_OUT {
    vec3 normal;
    vec3 cutterPos;
} 
vs_out;

void main()
{
    vs_out.normal = Normal;
    vs_out.cutterPos= CutterPos;
    gl_Position = vec4(Position, 1.0);
}
