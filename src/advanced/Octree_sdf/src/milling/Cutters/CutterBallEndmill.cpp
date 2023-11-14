#include "CutterBallEndmill.h"

#include "SignedDistanceFunctions.h"

#include <execution>

OBB3Df CutterBallEndmill::getBBox(const Posture& currentPosture) const
{
    return createBBox(currentPosture, radius, height);
}

bool CutterBallEndmill::isInside(const Posture& currentPosture, const Vector3Df& p) const
{
    float x = distanceToZAxis(currentPosture, p);
    float z = projectToZAxisLength(currentPosture, p);

    if (!isBetweenEndFlat(z)) {
        return false;
    }

    if (isBetweenEndBall(z)) {
        return signedDistanceToBallSurface(x, z) >= 0;
    } else {
        return signedDistanceToSideSurface(x) >= 0;
    }
}

float CutterBallEndmill::signedDistance(const Posture& currentPosture, const Vector3Df& p) const
{
    // float x = distanceToZAxis(currentPosture, p);
    // float z = projectToZAxisLength(currentPosture, p);

    // if (z <= radius) {
    //     return signedDistanceToBallSurface(x, z);
    // } else if (z < height) {
    //     return signedDistanceToSideSurface(x);
    // }

    // return signedDistanceToTopFlat(x, z);

    float d1 = SignedDistanceFunctions::cylinder(p, radius, height - radius, currentPosture.center + currentPosture.direction.normalize() * (height + radius) / 2.0f, currentPosture.direction.normalize());
    float d2 = SignedDistanceFunctions::sphere(p, radius, currentPosture.center + currentPosture.direction.normalize() * radius);
    return std::max(d1, d2);
}

void CutterBallEndmill::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    vertices.clear();
    normals.clear();
    generateTopMesh(radius, height, vertices, normals);
    generateSideMesh(radius, radius, height, 0.0f, vertices, normals);
    generateBottomBallMesh(vertices, normals);
}

void CutterBallEndmill::generateBottomBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    // Generate the vertices and normals for the bottom sphere face
    constexpr int latitudeSegments = 128;
    constexpr int longitudeSegments = 128;

    std::vector<Vector3Df> verticesBottom;
    std::vector<Vector3Df> normalsBottom;

    for (int i = 0; i <= latitudeSegments; ++i) {
        float theta = std::numbers::pi_v<float> * 0.5f + i * std::numbers::pi_v<float> / (2 * latitudeSegments);
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= longitudeSegments; ++j) {
            float phi = j * 2 * std::numbers::pi_v<float> / longitudeSegments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = radius * sinTheta * cosPhi;
            float y = radius * sinTheta * sinPhi;
            float z = radius * cosTheta;

            verticesBottom.emplace_back(x, y, z + radius);
            normalsBottom.push_back(Vector3Df(x, y, z).normalize());
        }
    }

    for (int i = 0; i < latitudeSegments; ++i) {
        for (int j = 0; j < longitudeSegments; ++j) {
            int first = (i * (longitudeSegments + 1)) + j;
            int second = first + longitudeSegments + 1;

            vertices.push_back(verticesBottom[first]);
            vertices.push_back(verticesBottom[second]);
            vertices.push_back(verticesBottom[first + 1]);

            normals.push_back(normalsBottom[first]);
            normals.push_back(normalsBottom[second]);
            normals.push_back(normalsBottom[first + 1]);

            vertices.push_back(verticesBottom[first + 1]);
            vertices.push_back(verticesBottom[second]);
            vertices.push_back(verticesBottom[second + 1]);

            normals.push_back(normalsBottom[first + 1]);
            normals.push_back(normalsBottom[second]);
            normals.push_back(normalsBottom[second + 1]);
        }
    }
}

float CutterBallEndmill::getDexelHeightZ(float distance) const
{
    return radius - std::sqrt(radius * radius - distance * distance);
}

bool CutterBallEndmill::getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
{
    // std::cout << "GetCutterDexelRange_start" << std::endl;
    if (sweptVolmueParam.type == SweptVolmueParam::Type::Line) {
        float z = 10000.0;
        float distance;
        Vector3Df TempPoint = Vector3Df(Dexel_X, Dexel_Y, 0);
        Vector3Df SP, ES, EP, SE;
        SE = sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel;
        float sinDir = (sweptVolmueParam.EndDexel.z - sweptVolmueParam.StartDexel.z) / SE.length();
        float cosDir = sqrt(1.0f - sinDir * sinDir);
        float ScaleSE = ((sweptVolmueParam.CutterRadiusDexel / cosDir) * sinDir) / (SE.length());
        //将StartDexel向EndDexel移动CutterRadiusDexel * sinnDir
        Vector3Df CorrectStartDexel = sweptVolmueParam.StartDexel + SE * ScaleSE;
        Vector3Df CorrectEndDexel = sweptVolmueParam.EndDexel + SE * ScaleSE;
        SP = TempPoint - CorrectStartDexel;
        EP = TempPoint - CorrectEndDexel;
        SE = CorrectEndDexel - CorrectStartDexel;
        ES = CorrectStartDexel - CorrectEndDexel;
        SP.z = 0.0f;
        SE.z = 0.0f;
        EP.z = 0.0f;
        ES.z = 0.0f;
        distance = SP.dot(SE) / SE.length();
        distance = sqrt(SP.length() * SP.length() - pow(distance, 2));

        if (distance > sweptVolmueParam.CutterRadiusDexel) {
            return false;
        }
        if (SP.dot(SE) < 0.0f || (EP.dot(ES) < 0.0f)) {
            //点在线段之外
            if (SP.dot(SE) < 0.0f)
                CutterPoint = sweptVolmueParam.StartDexel;
            else
                CutterPoint = sweptVolmueParam.EndDexel;

            TempPoint.z = CutterPoint.z;
            distance = (TempPoint - CutterPoint).length();
            if (distance <= sweptVolmueParam.CutterRadiusDexel) {
                Heigth = CutterPoint.z + getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
                return true;
            }
        } else {

            if(std::abs(sweptVolmueParam.StartDexel.z - sweptVolmueParam.EndDexel.z) < 0.0001f){
                CutterPoint = sweptVolmueParam.StartDexel;
                TempPoint.z = CutterPoint.z;
                SP = TempPoint - CutterPoint;
                SE = sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel;
                float v = SE.dot(SP) / SE.dot(SE);
                CutterPoint = sweptVolmueParam.StartDexel + SE * v;
                distance = (TempPoint - CutterPoint).length();
                if (distance <= sweptVolmueParam.CutterRadiusDexel) {
                    Heigth = CutterPoint.z + getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
                    // std::cout<<"CutterPoint.z:"<<CutterPoint.z<<std::endl;
                    return true;
                }
                return false;
            }
            float offset = (sweptVolmueParam.CutterRadiusDexel / cosDir) * sinDir;
            float L = sqrt(pow(sweptVolmueParam.CutterRadiusDexel, 2) - pow(distance, 2));
            offset = ((sweptVolmueParam.CutterRadiusDexel - L) / sweptVolmueParam.CutterRadiusDexel) * offset;

            //点在线段之内
            float v = SE.dot(SP) / SE.dot(SE);
            v = SE.dot(SP) / SE.length();
            v = v + offset * cosDir;
            v = v / SE.length();

            SE = sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel;
            // 记得判断计算出的刀位点是否在线段内
            // v = std::min(1.0f, v);
            CutterPoint = sweptVolmueParam.StartDexel + SE * v; //要看示意图
            TempPoint.z = CutterPoint.z;
            distance = (TempPoint - CutterPoint).length();
            if (distance <= sweptVolmueParam.CutterRadiusDexel) {
                TempPoint.z = CutterPoint.z;
                distance = (TempPoint - CutterPoint).length();
                Heigth = CutterPoint.z + getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
                return true;
            }
        }
        return false;
    } else if (sweptVolmueParam.type == SweptVolmueParam::Type::Arc) {
        float distance = Vector3Df(Dexel_X, Dexel_Y, sweptVolmueParam.Center.z).distanceToPoint(sweptVolmueParam.Center);
        // return false;
        if (distance > (sweptVolmueParam.RadiusDexel + sweptVolmueParam.CutterRadiusDexel) || distance < (sweptVolmueParam.RadiusDexel - sweptVolmueParam.CutterRadiusDexel)) {
            return false;
        }
        // std::cout << "Type::Arc" << std::endl;
        Vector3Df TempPoint = Vector3Df(Dexel_X, Dexel_Y, sweptVolmueParam.Center.z);
        Vector3Df CS = sweptVolmueParam.StartDexel - sweptVolmueParam.Center;
        Vector3Df CE = sweptVolmueParam.EndDexel - sweptVolmueParam.Center;
        Vector3Df CP = TempPoint - sweptVolmueParam.Center;
        if ((sweptVolmueParam.StartDexel - sweptVolmueParam.Center).length() <= 0.00001) {
            CutterPoint = sweptVolmueParam.StartDexel;
        } else {
            float angle = CS.angleToLine(CE);
            float angle1 = CS.angleToLine(CP);
            float angle2 = CE.angleToLine(CP);
            if (sweptVolmueParam.RadiusDexel > 0.0f) { //弧度小于等于180
                if (angle1 < angle && angle2 < angle) { //在扇形区域内//vs.dot(ve) >= 0.0f
                    CutterPoint = sweptVolmueParam.Center + CP * sweptVolmueParam.RadiusDexel / CP.length();
                } else { //判断是否在起点或者终点范围内
                    float distance1 = pow((TempPoint.x - sweptVolmueParam.StartDexel.x), 2) + pow((TempPoint.y - sweptVolmueParam.StartDexel.y), 2);
                    float distance2 = pow((TempPoint.x - sweptVolmueParam.EndDexel.x), 2) + pow((TempPoint.y - sweptVolmueParam.EndDexel.y), 2);
                    if (distance1 <= sweptVolmueParam.CutterRadiusDexel || distance2 <= sweptVolmueParam.CutterRadiusDexel) {
                        CutterPoint = (distance1 < distance2) ? sweptVolmueParam.StartDexel : sweptVolmueParam.EndDexel;
                    } else {
                        return false;
                    }
                }
            } else {
                if (!(angle1 < angle && angle2 < angle)) { //在扇形区域内
                    CutterPoint = sweptVolmueParam.Center + CP.normalize() * std::abs(sweptVolmueParam.RadiusDexel);
                } else { //判断是否在起点或者终点范围内
                    float distance1 = (TempPoint - sweptVolmueParam.StartDexel).length();
                    float distance2 = (TempPoint - sweptVolmueParam.EndDexel).length();
                    if (distance1 <= sweptVolmueParam.CutterRadiusDexel || distance2 <= sweptVolmueParam.CutterRadiusDexel) {
                        CutterPoint = (distance1 < distance2) ? sweptVolmueParam.StartDexel : sweptVolmueParam.EndDexel;
                    } else {
                        return false;
                    }
                }
            }
        }

        distance = (TempPoint - CutterPoint).length();
        if (distance <= sweptVolmueParam.CutterRadiusDexel) {
            Heigth = CutterPoint.z + getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
            // Heigth = minHeight;
            return true;
        }
        return false;
    } else if (sweptVolmueParam.type == SweptVolmueParam::Type::Scatter) {
        float distance = Vector3Df(Dexel_X, Dexel_Y, sweptVolmueParam.StartDexel.z).distanceToPoint(sweptVolmueParam.StartDexel);
        if (distance <= sweptVolmueParam.CutterRadiusDexel) {
            float minHeight = sweptVolmueParam.StartDexel.z + getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
            Heigth = minHeight;
            CutterPoint = sweptVolmueParam.StartDexel;
            return true;
        } else {
            return false;
        }
    }
    return false;
}


