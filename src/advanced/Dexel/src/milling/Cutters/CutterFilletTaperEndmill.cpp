#include "CutterFilletTaperEndmill.h"

float CutterFilletTaperEndmill::getDexelHeightZ(float distance) const
{
    if (distance < bottomRadius()) {
        return 0;
    }

    if (distance < middleRadius()) {
        return filletRadius - std::sqrt(filletRadius * filletRadius - (distance - bottomRadius()) * (distance - bottomRadius()));
    }

    return bottomHeight() + height * (distance - middleRadius()) / (topRadius() - middleRadius());
}

void CutterFilletTaperEndmill::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    vertices.clear();
    normals.clear();
    generateTopMesh(topRadius(), height, vertices, normals);
    generateSideMesh(middleRadius(), bottomHeight(), height, beta, vertices, normals);
    generateFilletBallMesh(vertices, normals);
    generateBottomMesh(bottomRadius(), vertices, normals);
}

void CutterFilletTaperEndmill::generateFilletBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    constexpr int longitudeSegments = 128;
    constexpr int latitudeSegments = 128;

    const float e = bottomRadius();
    const float f = filletRadius;

    std::vector<Vector3Df> verticesFilletBall;
    std::vector<Vector3Df> normalsFilletBall;

    for (int i = 0; i <= longitudeSegments; ++i) {
        const float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(longitudeSegments);
        const float sinTheta = sin(theta);
        const float cosTheta = cos(theta);
        const Vector3Df centerOfBall = Vector3Df(e * cosTheta, e * sinTheta, f);

        const float x = middleRadius() * cosTheta, y = middleRadius() * sinTheta;
        const Vector3Df vertex = Vector3Df(x, y, bottomHeight());
        const Vector3Df normal = Vector3Df(x, y, -std::tan(beta) * std::hypot(x, y)).normalize();
        verticesFilletBall.push_back(vertex);
        normalsFilletBall.push_back(normal);
        for (int j = 0; j <= latitudeSegments; ++j) {
            const float phi = std::numbers::pi_v<float> * 0.5f + beta + static_cast<float>(j) * (std::numbers::pi_v<float> * 0.5f - beta) / static_cast<float>(latitudeSegments);
            const float sinPhi = sin(phi);
            const float cosPhi = cos(phi);
            const Vector3Df normal = Vector3Df(cosTheta * sinPhi, sinTheta * sinPhi, cosPhi);
            const Vector3Df vertex = centerOfBall + normal * filletRadius;

            verticesFilletBall.push_back(vertex);
            normalsFilletBall.push_back(normal);
        }
    }

    for (int i = 0; i < longitudeSegments; ++i) {
        for (int j = 0; j < latitudeSegments + 1; ++j) {
            const int first = (latitudeSegments + 2) * i + j;
            const int second = first + latitudeSegments + 2;

            vertices.push_back(verticesFilletBall[first]);
            vertices.push_back(verticesFilletBall[first + 1]);
            vertices.push_back(verticesFilletBall[second]);

            normals.push_back(normalsFilletBall[first]);
            normals.push_back(normalsFilletBall[first + 1]);
            normals.push_back(normalsFilletBall[second]);

            vertices.push_back(verticesFilletBall[second]);
            vertices.push_back(verticesFilletBall[first + 1]);
            vertices.push_back(verticesFilletBall[second + 1]);

            normals.push_back(normalsFilletBall[second]);
            normals.push_back(normalsFilletBall[first + 1]);
            normals.push_back(normalsFilletBall[second + 1]);
        }
    }
}

bool CutterFilletTaperEndmill::getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
{
    return false;
}
