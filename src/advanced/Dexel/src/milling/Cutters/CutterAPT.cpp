#include "CutterAPT.h"

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
void CutterAPT::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
}
