#version 330 core
layout(location = 0) in vec3 aCenter;
layout(location = 1) in vec3 CubeNormal;
layout(location = 2) in float VertexHeigth[8];

out VS_OUT
{
    vec3 SideNormal;
    float VertexHeigth[8];
}
vs_out;

uniform vec3 BoxSize;

void main()
{
    float MaxLength = max(BoxSize.x, max(BoxSize.y, BoxSize.z));
    vs_out.SideNormal = CubeNormal;
    vs_out.VertexHeigth = VertexHeigth;
    for (int i = 0; i < 8; i++) {
        vs_out.VertexHeigth[i] = ((VertexHeigth[i] - (BoxSize.z / 2.0f)) / (MaxLength))*2.0f;
    }
    gl_Position = vec4(aCenter, 1.0);
    gl_Position.xyz = ((gl_Position.xyz - (BoxSize / 2.0f)) / (MaxLength) )*2.0f;
}
