#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 16) out;

in VS_OUT
{
    float size;
}
gs_in[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 pv = projection * view;
    vec4 vertices[8] = vec4[8](
        vec4(-1.0, -1.0, -1.0, 0.0),
        vec4(1.0, -1.0, -1.0, 0.0),
        vec4(-1.0, 1.0, -1.0, 0.0),
        vec4(1.0, 1.0, -1.0, 0.0),
        vec4(-1.0, -1.0, 1.0, 0.0),
        vec4(1.0, -1.0, 1.0, 0.0),
        vec4(-1.0, 1.0, 1.0, 0.0),
        vec4(1.0, 1.0, 1.0, 0.0));

    for (int i = 0; i < 8; i++) {
        vertices[i] = gl_in[0].gl_Position + 0.5 * gs_in[0].size * vertices[i];
    }

    int indices[16] = int[16](
        0, 1, 3, 2, 0, 4, 6, 2,
        5, 7, 3, 1, 5, 4, 6, 7);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            gl_Position = pv * vec4(vec3(model * vertices[indices[i * 8 + j]]), 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
