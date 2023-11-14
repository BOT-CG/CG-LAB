#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in TES_OUT
{
   vec3 normal;
} gs_in[];

// in vec3 tes_normal[];

out GS_OUT
{
    vec3 positionFragment;
    vec3 normal;
}
gs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 BoxSize;

vec4 P1;
vec4 P2;
vec4 P3;

void main(){
    P1 = gl_in[0].gl_Position;
    P2 = gl_in[1].gl_Position;
    P3 = gl_in[2].gl_Position;

    if(length(gs_in[0].normal) > 8){
        return;
    }

    vec3 ab = vec3(P2)- vec3(P1);
    vec3 ac = vec3(P3)- vec3(P1);
    vec3 normal = normalize(cross(ab,ac));
    vec3 MaskNormal = gs_in[0].normal;//tes_normal[0];//

    if(dot(normal,MaskNormal) < 0.0f){
        normal = -normal;
    }
   


    if(length(gs_in[0].normal) > 4.0f  && (abs(normal.z) < 0.9) ){
        vec4 P4 =  gl_in[2].gl_Position;
        normal = gs_in[0].normal;
        normal = normalize(normal);
        //旋转三个顶点至和法线垂直
        float Max_Z = max(P1.z, max(P2.z, P3.z));
        float Min_Z = min(P1.z, min(P2.z, P3.z));

        vec3 averagePoint = (P1.xyz + P2.xyz + P3.xyz) / 3.0f;

        //构建与normal垂直的三角面片
        float length = distance(P1.xy, P2.xy);
        length = min(length, distance(P1.xy, P3.xy));
        length = min(length, distance(P2.xy, P3.xy));
        length  = length * (3.0f);
        vec3 tempNornal = vec3(0.0,0.0,1.0f);

        vec3 Dir = normalize(cross(tempNornal, normal));

        P1.xyz = averagePoint + Dir * length;
        P2.xyz = averagePoint + Dir * length;
        P3.xyz = averagePoint - Dir * length;
        P4.xyz = averagePoint - Dir * length;
        P1.z = Max_Z;
        P2.z = Min_Z;
        P3.z = Max_Z;
        P4.z = Min_Z;
        //将三角面片投影到屏幕
    
    normal = normalize(normalMatrix * normal);

    float MaxLength = max(BoxSize.x, max(BoxSize.y, BoxSize.z));

    P1.xyz = (P1.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;
    P2.xyz = (P2.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;
    P3.xyz = (P3.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;
    P4.xyz = (P4.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;
    vec3 FragPos1 = vec3(model * P1);
    vec3 FragPos2 = vec3(model * P2);
    vec3 FragPos3 = vec3(model * P3);
    vec3 FragPos4 = vec3(model * P4);
    P1 = projection * view * model * P1;
    P2 = projection * view * model * P2;
    P3 = projection * view * model * P3;
    P4 = projection * view * model * P4;

    gl_Position = P1;
    gs_out.positionFragment = FragPos1;
    gs_out.normal = normal;
    EmitVertex();
    gl_Position = P2;
    gs_out.positionFragment = FragPos2;
    gs_out.normal = normal;
    EmitVertex();
    gl_Position = P3;
    gs_out.positionFragment = FragPos3;
    gs_out.normal = normal;
    EmitVertex();
    gl_Position = P4;
    gs_out.positionFragment = FragPos4;
    gs_out.normal = normal;
    EmitVertex();
    EndPrimitive();

    return;

    // gl_Position = P1;
    // gs_out.positionFragment = FragPos1;
    // gs_out.normal = normal;
    // EmitVertex();
    // gl_Position = P4;
    // gs_out.positionFragment = FragPos4;
    // gs_out.normal = normal;
    // EmitVertex();
    // gl_Position = P3;
    // gs_out.positionFragment = FragPos3;
    // gs_out.normal = normal;
    // EmitVertex();
    // EndPrimitive();
    // return;

    }


    normal = normalize(normalMatrix * normal);

    float MaxLength = max(BoxSize.x, max(BoxSize.y, BoxSize.z));

    P1.xyz = (P1.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;
    P2.xyz = (P2.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;
    P3.xyz = (P3.xyz - (BoxSize.xyz / 2.0f)) / MaxLength;



    vec3 FragPos1 = vec3(model * P1);
    vec3 FragPos2 = vec3(model * P2);
    vec3 FragPos3 = vec3(model * P3);

    P1 = projection * view * model * P1;
    P2 = projection * view * model * P2;
    P3 = projection * view * model * P3;


    gl_Position = P1;
    gs_out.positionFragment = FragPos1;
    gs_out.normal = normal;
    EmitVertex();

    gl_Position = P2;
    gs_out.positionFragment = FragPos2;
    gs_out.normal = normal;
    EmitVertex();

    gl_Position = P3;
    gs_out.positionFragment = FragPos3;
    gs_out.normal = normal;
    EmitVertex();

    EndPrimitive();

}