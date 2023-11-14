#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 36) out;
// layout(line_strip, max_vertices = 36) out;

in VS_OUT
{
    float size;
    float distances[8];
}
gs_in[];

out GS_OUT
{
    vec3 positionFragment;
    vec3 normal;
}
gs_out;

layout(std430, binding = 0) buffer BufferTriangleTable
{
    int TriangleTable[];
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

const vec3 CubicVertices[8] = vec3[8](
    vec3(-1.0f, -1.0f, -1.0f),
    vec3(-1.0f, -1.0f, 1.0f),
    vec3(-1.0f, 1.0f, -1.0f),
    vec3(-1.0f, 1.0f, 1.0f),
    vec3(1.0f, -1.0f, -1.0f),
    vec3(1.0f, -1.0f, 1.0f),
    vec3(1.0f, 1.0f, -1.0f),
    vec3(1.0f, 1.0f, 1.0f));
const int EdgeIndices[2 * 12] = int[2 * 12](
    0, 2, 2, 6, 6, 4, 4, 0,
    1, 3, 3, 7, 7, 5, 5, 1,
    0, 1, 2, 3, 6, 7, 4, 5);

const int indices[24] = int[24](
    0, 1, 2, 3,
    0, 4, 1, 5,
    0, 2, 4, 6,
    1, 5, 3, 7,
    2, 3, 6, 7,
    4, 6, 5, 7);
const vec3 normals[6] = vec3[6](
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, 0.0f, -1.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(1.0f, 0.0f, 0.0f));
const int edgeIndices[24] = int[24](
    0, 4, 1, 5, 2, 6, 3, 7,
    0, 2, 1, 3, 4, 6, 5, 7,
    0, 1, 2, 3, 4, 5, 6, 7);

void build_face(int index)
{
    mat4 pv = projection * view;
    for (int j = 0; j < 3; j++) {
        gs_out.positionFragment = vec3(model * vec4(CubicVertices[indices[index * 4 + j]] * gs_in[0].size / 2.0f + gl_in[0].gl_Position.xyz, 1.0f));
        gs_out.normal = normalize(normalMatrix * normals[index]);
        gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
        EmitVertex();
    }
    EndPrimitive();
    for (int j = 3; j > 0; j--) {
        gs_out.positionFragment = vec3(model * vec4(CubicVertices[indices[index * 4 + j]] * gs_in[0].size / 2.0f + gl_in[0].gl_Position.xyz, 1.0f));
        gs_out.normal = normalize(normalMatrix * normals[index]);
        gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
        EmitVertex();
    }
    EndPrimitive();
}

bool check_all_positive()
{
    for (int i = 0; i < 8; i++) {
        if (gs_in[0].distances[i] <= 0.0f) {
            return false;
        }
    }
    return true;
}

bool check_all_negative()
{
    for (int i = 0; i < 8; i++) {
        if (gs_in[0].distances[i] >= 0.0f) {
            return false;
        }
    }
    return true;
}

bool check_all_zero()
{
    for (int i = 0; i < 8; i++) {
        if (gs_in[0].distances[i] != 0.0f) {
            return false;
        }
    }
    return true;
}

void build_all_zero()
{
    for (int i = 0; i < 6; i++) {
        build_face(i);
    }
}

int check_seven_zero()
{
    int count = 0, index;
    for (int i = 0; i < 8; i++) {
        if (gs_in[0].distances[i] == 0.0f) {
            count++;
        } else {
            index = i;
        }
    }
    if (count == 7) {
        return index;
    } else {
        return -1;
    }
}

void build_seven_zero(int index)
{
    for (int i = 0; i < 6; i++) {
        bool flag = true;
        for (int j = 0; j < 4; j++) {
            if (indices[i * 4 + j] == index) {
                flag = false;
                break;
            }
        }
        if (flag) {
            build_face(i);
        }
    }
}

int check_six_zero()
{
    int count = 0, index = -1;
    for (int i = 0; i < 8; i++) {
        if (gs_in[0].distances[i] == 0.0f) {
            count++;
        }
    }
    if (count != 6) {
        return -1;
    }
    for (int i = 0; i < 12; i++) {
        if (gs_in[0].distances[edgeIndices[i * 2]] != 0.0f && gs_in[0].distances[edgeIndices[i * 2 + 1]] != 0.0f) {
            index = i;
        }
    }
    return index;
}

void build_six_zero(int index)
{
    for (int i = 0; i < 6; i++) {
        bool flag = true;
        for (int j = 0; j < 4; j++) {
            if (indices[i * 4 + j] == edgeIndices[index * 2] || indices[i * 4 + j] == edgeIndices[index * 2 + 1]) {
                flag = false;
                break;
            }
        }
        if (flag) {
            build_face(i);
        }
    }
}

void main()
{
    if (check_all_positive() || check_all_negative()) {
        return;
    }

    if (check_all_zero()) {
        build_all_zero();
        return;
    }

    int index = check_seven_zero();
    if (index != -1) {
        build_seven_zero(index);
        return;
    }

    index = check_six_zero();
    if (index != -1) {
        build_six_zero(index);
        return;
    }

    int cubeIndex = 0;
    for (int i = 0; i < 8; i++) {
        if (gs_in[0].distances[i] > 0.0f) {
            cubeIndex |= (1 << i);
        }
    }

    if (cubeIndex == 0 || cubeIndex == 255) {
        return;
    }

    vec3 vertices[3];
    mat4 pv = projection * view;
    for (int i = 0; i < 16 && TriangleTable[cubeIndex * 16 + i] != -1; i += 3) {
        for (int j = 0; j < 3; ++j) {
            int index0 = EdgeIndices[TriangleTable[cubeIndex * 16 + i + j] * 2];
            int index1 = EdgeIndices[TriangleTable[cubeIndex * 16 + i + j] * 2 + 1];
            vec3 v0 = vec3(gl_in[0].gl_Position) + CubicVertices[index0] * gs_in[0].size * 0.5f;
            vec3 v1 = vec3(gl_in[0].gl_Position) + CubicVertices[index1] * gs_in[0].size * 0.5f;
            vertices[j] = v0 + normalize(v1 - v0) * gs_in[0].size * gs_in[0].distances[index0] / (gs_in[0].distances[index0] - gs_in[0].distances[index1]);
        }
        vec3 normal = normalize(cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
        gs_out.positionFragment = vec3(model * vec4(vertices[0], 1.0f));
        gs_out.normal = normalize(normalMatrix * normal);
        gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
        EmitVertex();
        gs_out.positionFragment = vec3(model * vec4(vertices[1], 1.0f));
        gs_out.normal = normalize(normalMatrix * normal);
        gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
        EmitVertex();
        gs_out.positionFragment = vec3(model * vec4(vertices[2], 1.0f));
        gs_out.normal = normalize(normalMatrix * normal);
        gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
        EmitVertex();
        EndPrimitive();
    }
}
