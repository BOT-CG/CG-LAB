#include "CutterFlatEndmill.h"

#include "SignedDistanceFunctions.h"

OBB3Df CutterFlatEndmill::getBBox(const Posture& currentPosture) const
{
    return createBBox(currentPosture, radius(), height);
}

bool CutterFlatEndmill::isInside(const Posture& currentPosture, const Vector3Df& p) const
{
    float x = distanceToZAxis(currentPosture, p);
    float z = projectToZAxisLength(currentPosture, p);
    return isBetweenEndFlat(z) && signedDistanceToSideSurface(x) >= 0;
}

float CutterFlatEndmill::signedDistance(const Posture& currentPosture, const Vector3Df& p) const
{
    // float x = distanceToZAxis(currentPosture, p);
    // float z = projectToZAxisLength(currentPosture, p);
    // return isBetweenEndFlat(z) ? signedDistanceToSideSurface(x) : signedDistanceToBothEndFlat(x, z);
    return SignedDistanceFunctions::cylinder(p, radius(), height, currentPosture.center + currentPosture.direction.normalize() * height / 2.0f, currentPosture.direction.normalize());
}

void CutterFlatEndmill::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    vertices.clear();
    normals.clear();
    generateTopMesh(radius(), height, vertices, normals);
    generateSideMesh(radius(), 0.0f, height, 0.0f, vertices, normals);
    generateBottomMesh(radius(), vertices, normals);
}

float CutterFlatEndmill::getDexelHeightZ(float distance) const
{
    return 0;
}

bool CutterFlatEndmill::getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
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
            CutterPoint = (SP.dot(SE) < 0.0f) ? sweptVolmueParam.StartDexel : sweptVolmueParam.EndDexel;
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

