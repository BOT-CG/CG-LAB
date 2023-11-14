#include "CutterAPT.h"

#include <algorithm>

OBB3Df CutterAPT::getBBox(const Posture& currentPosture) const
{
    return createBBox(currentPosture, R, totalHeight());
}

bool CutterAPT::isInside(const Posture& currentPosture, const Vector3Df& p) const
{
    float x = distanceToZAxis(currentPosture, p);
    float z = projectToZAxisLength(currentPosture, p);

    if (z < 0 || z > totalHeight()) {
        return false;
    }

    if (z <= Hlc) {
        return x <= distanceToZAxisOnLowerConeSurface(z);
    }

    if (z <= Hlc + Ht) {
        return x <= distanceToZAxisOnTorusBallSurface(z);
    }

    if (z <= h) {
        return x <= distanceToZAxisOnUpperConeSurface(z);
    }

    return x <= R;
}

float CutterAPT::signedDistance(const Posture& currentPosture, const Vector3Df& p) const
{
    float x = distanceToZAxis(currentPosture, p);
    float z = projectToZAxisLength(currentPosture, p);

    if (z >= totalHeight()) {
        return signedDistanceToTopFlat(x, z);
    }

    if (z >= h) {
        return R - x;
    }

    float d1 = distanceToLowerConeSurface(x, z);
    float d2 = distanceToCornerTorusBallSurface(x, z);
    float d3 = distanceToUpperConeSurface(x, z);
    float d4 = distanceToCylinderSurface(x, z);
    float d5 = signedDistanceToTopFlat(x, z);

    float d = std::min({ d1, d2, d3, d4, std::abs(d5) });
    return isInside(currentPosture, p) ? d : -d;
}

#define PI acos(-1)
class PoinOfCutterU {
public:
    PoinOfCutterU() {};
    PoinOfCutterU(float beta, float theta, float L, float h, float R1, float R2, float a)
    {
        x = (R1 + R2 * cos(PI * beta)) * cos(PI * theta) + a * (L - h + R2 * sin(PI * beta)) * tan(PI * beta) * cos(PI * theta);
        y = (R1 + R2 * cos(PI * beta)) * sin(PI * theta) + a * (L - h + R2 * sin(PI * beta)) * tan(PI * beta) * sin(PI * theta);
        z = h - R2 * sin(PI * beta) + a * (L - h + R2 * sin(PI * beta));
        x_normal = cos(PI * theta) * (L - h + R2 * sin(PI * beta)) * (R1 + R2 * cos(PI * beta) + a * (L - h + R2 * sin(PI * beta)) * tan(PI * beta));
        y_normal = sin(PI * theta) * (L - h + R2 * sin(PI * beta)) * (R1 + R2 * cos(PI * beta) + a * (L - h + R2 * sin(PI * beta)) * tan(PI * beta));
        z_normal = -(R1 + R2 * cos(PI * beta) + a * (L - h + R2 * sin(PI * beta)) * tan(PI * beta)) * (L - h + R2 * sin(PI * beta)) * tan(PI * beta);
    };
    float x;
    float y;
    float z;
    float x_normal;
    float y_normal;
    float z_normal;
};

class PoinOfCutterT {
public:
    PoinOfCutterT() {};
    PoinOfCutterT(float phi, float theta, float L, float h, float R1, float R2)
    {
        x = (R1 + R2 * cos(PI * phi)) * cos(PI * theta);
        y = (R1 + R2 * cos(PI * phi)) * sin(PI * theta);
        z = h - R2 * sin(PI * phi);
        x_normal = R2 * cos(PI * phi) * cos(PI * theta) * (R1 + R2 * cos(PI * phi));
        y_normal = R2 * cos(PI * phi) * sin(PI * theta) * (R1 + R2 * cos(PI * phi));
        z_normal = -R2 * sin(PI * phi) * (R1 + R2 * cos(PI * phi));
    };
    float x;
    float y;
    float z;
    float x_normal;
    float y_normal;
    float z_normal;
};

class PoinOfCutterL {
public:
    PoinOfCutterL() {};
    PoinOfCutterL(float alpha, float theta, float L, float h, float R1, float R2, float b)
    {
        x = b * (R1 + R2 * sin(PI * alpha)) * cos(PI * theta);
        y = b * (R1 + R2 * sin(PI * alpha)) * sin(PI * theta);
        z = b * (R1 + R2 * sin(PI * alpha)) * tan(PI * alpha);
        x_normal = b * (R1 + R2 * sin(PI * alpha)) * (R1 + R2 * sin(PI * alpha)) * tan(PI * alpha) * cos(PI * theta);
        y_normal = b * (R1 + R2 * sin(PI * alpha)) * (R1 + R2 * sin(PI * alpha)) * tan(PI * alpha) * sin(PI * theta);
        z_normal = -b * (R1 + R2 * sin(PI * alpha)) * (R1 + R2 * sin(PI * alpha));
    };
    float x;
    float y;
    float z;
    float x_normal;
    float y_normal;
    float z_normal;
};

void CutterAPT::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    float L = h;
    vertices.clear();
    // 生成顶点数据
    // 遍历参数范围，计算顶点坐标并发送顶点数据
    int stepTheta = 50; // theta 参数范围
    int stepA = 20; // a 参数范围
    int stepPhi = 20; // phi 参数值
    int stepB = 20; // b 参数范围
    // 需要传入的参数分别为L、R1、R2、theta、beta、h(h不能是任意值，是需要综合其余几个参数的计算值)
    //    const float L = 1.5f; // L 参数值a的步长
    //    const float R1 = 0.5f; // R1 参数值
    //    const float R2 = 0.3f; // R2 参数值
    //    const float beta = 1.0f / 6; // beta 参数值
    //    const float alpha = 1.0f / 6; // alpha 参数值
    //    const float h = R2/cos(PI*alpha)+R1*tan(PI*alpha);  // b的参数值，是固定的，根据R1、R2、alpha得出
    float thetaStep = 2.0f / stepTheta; // theta的步长
    float aStep = 1.0f / stepA; // a的步长
    float phiStep = (0.5f - alpha - beta) / stepPhi; // phi的步长
    float bStep = 1.0f / stepB; // b的步长
    float a = 1.0f; // a 初始值
    float theta = 0.0f; // theta 初始值
    float phi = beta; // phi的初始值
    float b = 1.0f; // b 初始值
    for (int stepa = 0; stepa < stepA; stepa++) {
        for (int steptheta = 0; steptheta < stepTheta; steptheta++) {
            PoinOfCutterU point1 = PoinOfCutterU(beta, theta, L, h, R1, R2, a);
            PoinOfCutterU point2 = PoinOfCutterU(beta, theta, L, h, R1, R2, a - aStep);
            PoinOfCutterU point3 = PoinOfCutterU(beta, theta + thetaStep, L, h, R1, R2, a - aStep);
            PoinOfCutterU point4 = PoinOfCutterU(beta, theta + thetaStep, L, h, R1, R2, a);
            vertices.push_back({ point1.x, point1.y, point1.z });
            vertices.push_back({ point1.x_normal, point1.y_normal, point1.z_normal });
            vertices.push_back({ point2.x, point2.y, point2.z });
            vertices.push_back({ point2.x_normal, point2.y_normal, point2.z_normal });
            vertices.push_back({ point3.x, point3.y, point3.z });
            vertices.push_back({ point3.x_normal, point3.y_normal, point3.z_normal });
            vertices.push_back({ point1.x, point1.y, point1.z });
            vertices.push_back({ point1.x_normal, point1.y_normal, point1.z_normal });
            vertices.push_back({ point3.x, point3.y, point3.z });
            vertices.push_back({ point3.x_normal, point3.y_normal, point3.z_normal });
            vertices.push_back({ point4.x, point4.y, point4.z });
            vertices.push_back({ point4.x_normal, point4.y_normal, point4.z_normal });
            theta += thetaStep;
        }
        a -= aStep;
        theta = 0.0f;
    }
    theta = 0.0f; // theta恢复初始值
    for (int stepphi = 0; stepphi < stepPhi; stepphi++) {
        for (int steptheta = 0; steptheta < stepTheta; steptheta++) {
            PoinOfCutterT point5 = PoinOfCutterT(phi, theta, L, h, R1, R2);
            PoinOfCutterT point6 = PoinOfCutterT(phi + phiStep, theta, L, h, R1, R2);
            PoinOfCutterT point7 = PoinOfCutterT(phi + phiStep, theta + thetaStep, L, h, R1, R2);
            PoinOfCutterT point8 = PoinOfCutterT(phi, theta + thetaStep, L, h, R1, R2);
            vertices.push_back({ point5.x, point5.y, point5.z });
            vertices.push_back({ point5.x_normal, point5.y_normal, point5.z_normal });
            vertices.push_back({ point6.x, point6.y, point6.z });
            vertices.push_back({ point6.x_normal, point6.y_normal, point6.z_normal });
            vertices.push_back({ point7.x, point7.y, point7.z });
            vertices.push_back({ point7.x_normal, point7.y_normal, point7.z_normal });
            vertices.push_back({ point5.x, point5.y, point5.z });
            vertices.push_back({ point5.x_normal, point5.y_normal, point5.z_normal });
            vertices.push_back({ point7.x, point7.y, point7.z });
            vertices.push_back({ point7.x_normal, point7.y_normal, point7.z_normal });
            vertices.push_back({ point8.x, point8.y, point8.z });
            vertices.push_back({ point8.x_normal, point8.y_normal, point8.z_normal });
            theta += thetaStep;
        }
        phi += phiStep;
        theta = 0.0f;
    }
    theta = 0.0f;
    for (int stepb = 0; stepb < stepB; stepb++) {
        for (int steptheta = 0; steptheta < stepTheta; steptheta++) {
            PoinOfCutterL point9 = PoinOfCutterL(alpha, theta, L, h, R1, R2, b);
            PoinOfCutterL point10 = PoinOfCutterL(alpha, theta, L, h, R1, R2, b - bStep);
            PoinOfCutterL point11 = PoinOfCutterL(alpha, theta + thetaStep, L, h, R1, R2, b - bStep);
            PoinOfCutterL point12 = PoinOfCutterL(alpha, theta + thetaStep, L, h, R1, R2, b);
            vertices.push_back({ point9.x, point9.y, point9.z });
            vertices.push_back({ point9.x_normal, point9.y_normal, point9.z_normal });
            vertices.push_back({ point10.x, point10.y, point10.z });
            vertices.push_back({ point10.x_normal, point10.y_normal, point10.z_normal });
            vertices.push_back({ point11.x, point11.y, point11.z });
            vertices.push_back({ point11.x_normal, point11.y_normal, point11.z_normal });
            vertices.push_back({ point9.x, point9.y, point9.z });
            vertices.push_back({ point9.x_normal, point9.y_normal, point9.z_normal });
            vertices.push_back({ point11.x, point11.y, point11.z });
            vertices.push_back({ point11.x_normal, point11.y_normal, point11.z_normal });
            vertices.push_back({ point12.x, point12.y, point12.z });
            vertices.push_back({ point12.x_normal, point12.y_normal, point12.z_normal });
            theta += thetaStep;
        }
        b -= bStep;
        theta = 0.0f;
    }
}


float CutterAPT::getDexelHeightZ(float distance) const
{
    if (distance < R1) {
        return distance * std::tan(alpha);
    }

    if (distance < R2) {
        return f - std::sqrt(r * r - (distance - e) * (distance - e));
    }

    if (distance < R) {
        return (distance - R2) * std::tan(beta) + Hlc + Ht;
    }

    if (distance == R) {
        return h;
    }

    return 0;
}

// bool CutterAPT::getSweptVolume(CutterSweptVolume* volume, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
// {
//
// }
bool CutterAPT::getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
{
    // std::cout<<"CutterAPT::getSweptVolume"<<std::endl;
    return false;
}
