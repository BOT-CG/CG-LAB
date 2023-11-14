#include "CutterTaperEndmill.h"

float CutterTaperEndmill::getDexelHeightZ(float distance) const
{
    if (distance < bottomRadius()) {
        return 0;
    }

    return height * (distance - bottomRadius()) / (topRadius() - bottomRadius());
}

void CutterTaperEndmill::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    vertices.clear();
    normals.clear();
    generateTopMesh(topRadius(), height, vertices, normals);
    generateSideMesh(bottomRadius(), 0.0f, height, beta, vertices, normals);
    generateBottomMesh(bottomRadius(), vertices, normals);
}

bool CutterTaperEndmill::getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
{
    return false;
}