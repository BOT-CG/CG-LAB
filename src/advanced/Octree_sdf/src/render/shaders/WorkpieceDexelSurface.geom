#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 36) out;


in VS_OUT
{
    vec3 SideNormal;
    float VertexHeigth[8];
}
gs_in[];

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

uniform float size;

//计算顶点xy坐标
const int VertexMask[8] = int[8](
    0, 0,
    0, 1,
    1, 0,
    1, 1
);

const int NeighToSide[8] = int[8](
        1, 0 , 
        2, 3 , 
        3, 1 , 
        0, 2
);



vec3 Point[4]  = vec3[4](
    vec3(0.0f,0.0f,0.0f),
    vec3(0.0f,0.0f,0.0f),
    vec3(0.0f,0.0f,0.0f),
    vec3(0.0f,0.0f,0.0f)
);


void TestData(){
    mat4 pv = projection * view;
    //BottomFace
    vec3 normal =  vec3(0.0f,0.0f,1.0f);
    for(int i = 0;i < 4 ; i++){
        Point[i].x = gl_in[0].gl_Position.x + VertexMask[i * 2] * size;
        Point[i].y = gl_in[0].gl_Position.y + VertexMask[i * 2 + 1] * size;

        gs_out.positionFragment = vec3(model * vec4(Point[i],1.0f));
        gs_out.normal = normalize(normalMatrix * normal);
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
    }
    EndPrimitive();
    // vec3 vertex1 = vec3(0.0f,0.0f,0.0f);
    // vec3 vertex2 = vec3(0.0f,0.0f,0.0f);
    // vec3 vertex3 = vec3(0.0f,0.0f,0.0f);
    // vec3 vertex4 = vec3(0.0f,0.0f,0.0f);

    // vertex1.x = gl_in[0].gl_Position.x ;
    // vertex1.y = gl_in[0].gl_Position.y ;
    // gs_out.positionFragment = vec3(model * vec4(vertex1,1.0f));
    // gs_out.normal = normalize(normalMatrix * normal);
    // gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
    // EmitVertex();

    // vertex2.x = gl_in[0].gl_Position.x  + 0.5;
    // vertex2.y = gl_in[0].gl_Position.y ;
    // gs_out.positionFragment = vec3(model * vec4(vertex2,1.0f));
    // gs_out.normal = normalize(normalMatrix * normal);
    // gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
    // EmitVertex();

    // vertex3.x = gl_in[0].gl_Position.x  + 0.5;
    // vertex3.y = gl_in[0].gl_Position.y  + 0.5;
    // gs_out.positionFragment = vec3(model * vec4(vertex3,1.0f));
    // gs_out.normal = normalize(normalMatrix * normal);
    // gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
    // EmitVertex();

    // vertex4.x = gl_in[0].gl_Position.x ;
    // vertex4.y = gl_in[0].gl_Position.y  + 0.5;
    // gs_out.positionFragment = vec3(model * vec4(vertex4,1.0f));
    // gs_out.normal = normalize(normalMatrix * normal);
    // gl_Position = pv * vec4(gs_out.positionFragment, 1.0f);
    // EmitVertex();
    // EndPrimitive();
}

void AddSideFace(){
    //四个侧面
    mat4 pv = projection * view;
    for(int i = 0;i<4;i++){

        //每个侧面的四个顶点点
        int PointIndex1 = NeighToSide[i*2];//1
        int PointIndex2 = NeighToSide[i*2 + 1];//0
        
        Point[0].x = gl_in[0].gl_Position.x + VertexMask[PointIndex1 * 2] * size;
        Point[0].y = gl_in[0].gl_Position.y + VertexMask[PointIndex1 * 2 + 1] * size;
        Point[0].z = gs_in[0].VertexHeigth[PointIndex1 * 2];//Bot_0;
        Point[1].x = gl_in[0].gl_Position.x + VertexMask[PointIndex2 * 2] * size;
        Point[1].y = gl_in[0].gl_Position.y + VertexMask[PointIndex2 * 2 + 1] * size;
        Point[1].z = gs_in[0].VertexHeigth[PointIndex2 * 2];//Bot_1;

        Point[2].x = gl_in[0].gl_Position.x + VertexMask[PointIndex1 * 2] * size;
        Point[2].y = gl_in[0].gl_Position.y + VertexMask[PointIndex1 * 2 + 1] * size;
        Point[2].z = gs_in[0].VertexHeigth[PointIndex1 * 2 + 1];//Top_0;
        Point[3].x = gl_in[0].gl_Position.x + VertexMask[PointIndex2 * 2] * size;
        Point[3].y = gl_in[0].gl_Position.y + VertexMask[PointIndex2 * 2 + 1] * size;
        Point[3].z = gs_in[0].VertexHeigth[PointIndex2 * 2 + 1];//Top_1;

        vec3 normal;

        if(abs(length(gs_in[0].SideNormal))>0.6 && abs(length(gs_in[0].SideNormal))<2.0){
            normal = gs_in[0].SideNormal;
        }else{
            vec3 SideNormal = normalize(cross((Point[0] - Point[1]),(Point[0] - Point[3])));
            if(dot(gs_in[0].SideNormal,SideNormal)<0){
                SideNormal = gs_in[0].SideNormal * (-1.0f);
            }
            normal = SideNormal;
            normal = normalize(normal);
        }

        // if(abs(length(gs_in[0].SideNormal))>2){
        //     //使用面片顶点生成法向量而不是外部向量
        //     normal = normalize(cross((Point[0] - Point[1]),(Point[0] - Point[3])));
        // }

        normal = normalize(normalMatrix * normal);
        //124.143
        gs_out.positionFragment = vec3(model * vec4(Point[0],1.0f));
        gs_out.normal = normal;
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
        gs_out.positionFragment = vec3(model * vec4(Point[1],1.0f));
        gs_out.normal = normal;
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
        gs_out.positionFragment = vec3(model * vec4(Point[3],1.0f));
        gs_out.normal = normal;
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
        EndPrimitive();

        gs_out.positionFragment = vec3(model * vec4(Point[0],1.0f));
        gs_out.normal = normal;
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
        gs_out.positionFragment = vec3(model * vec4(Point[3],1.0f));
        gs_out.normal = normal;
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
        gs_out.positionFragment = vec3(model * vec4(Point[2],1.0f));
        gs_out.normal = normal;
        gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
        EmitVertex();
        EndPrimitive();

    }
}

void main(){
    if(gl_in[0].gl_Position.x< -1000.0f)
    return;
    // TestData();
    // return;
    mat4 pv = projection * view;
    //BottomFace
    for(int i = 0;i < 4 ; i++){
        Point[i].x = gl_in[0].gl_Position.x + VertexMask[i*2] * size;
        Point[i].y = gl_in[0].gl_Position.y + VertexMask[i*2+1] * size;
        Point[i].z = gs_in[0].VertexHeigth[i*2];
    }

    vec3 ab = Point[1] - Point[0];
    vec3 ac = Point[2] - Point[0];
    vec3 normal = normalize(cross(ab,ac));
    vec3 MaskNormal = vec3(0.0f,0.0f,-1.0f);

    if(dot(normal,MaskNormal) < 0.0f){
        normal = -normal;
    }
    normal = normalize(normalMatrix * normal);

    //130
    gs_out.positionFragment = vec3(model * vec4(Point[1],1.0f));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[3],1.0f));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[0],1.0f));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0f);
    EmitVertex();
    EndPrimitive();
    //320
    gs_out.positionFragment = vec3(model * vec4(Point[3],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[2],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[0],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    EndPrimitive();

    float NormalLength = abs(length(gs_in[0].SideNormal));

    //TopFace
    for(int i = 0;i < 4 ; i++){
        Point[i].x = gl_in[0].gl_Position.x + VertexMask[i * 2] * size;
        Point[i].y = gl_in[0].gl_Position.y + VertexMask[i * 2 + 1] * size;
        Point[i].z = gs_in[0].VertexHeigth[i * 2 + 1];
    }

    ab = Point[1] - Point[0];
    ac = Point[2] - Point[0];
    // normal = normalize(cross(ab,ac));
    normal = normalize(cross(ac,ab));

    // MaskNormal = vec3(0.0f,0.0f,1.0f);

    // if(dot(normal,MaskNormal) < 0.0f){
    //     normal = -normal;
    // }

    if(NormalLength > 4.0f){
        if(dot(normal,gs_in[0].SideNormal) < 0.0f){
            normal = gs_in[0].SideNormal * (-1.0f);
        }else{
            normal = gs_in[0].SideNormal;
        }

        // normal = gs_in[0].SideNormal;
        // normal.x = normal.x * (-1.0f);
        // normal = vec3(0.0f,0.0f,0.0f);
    }

    // if(abs(normal.z)<0.05){
    //     normal = vec3(0.0f,0.0f,0.0f);
    // }


    normal = normalize(normal);
    normal = normalize(normalMatrix * normal);

    //031
    gs_out.positionFragment = vec3(model * vec4(Point[0],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[3],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[1],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    EndPrimitive();
    //023
    gs_out.positionFragment = vec3(model * vec4(Point[0],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[2],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    gs_out.positionFragment = vec3(model * vec4(Point[3],1.0));
    gs_out.normal = normal;
    gl_Position = pv * vec4(gs_out.positionFragment,1.0);
    EmitVertex();
    EndPrimitive();

    if(NormalLength<4.0f){
        //SideFace
        if(abs(length(gs_in[0].SideNormal))>0.2){
            AddSideFace();
        }
    }

}

