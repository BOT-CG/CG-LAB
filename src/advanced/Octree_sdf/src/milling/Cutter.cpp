#include "Cutter.h"

#include <execution>

void Cutter::generateBottomMesh(float radius, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments)
{
    std::vector<Vector3Df> verticesBottom;
    std::vector<Vector3Df> normalsBottom;
    verticesBottom.resize(numSegments * 3);
    normalsBottom.resize(numSegments * 3);
    std::for_each(std::execution::par, verticesBottom.begin(), verticesBottom.end(), [&](Vector3Df& p) {
        size_t i = (&p - verticesBottom.data()) / 3;
        float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(numSegments);
        float nextTheta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i + 1) / static_cast<float>(numSegments);

        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        float nextX = radius * std::cos(nextTheta);
        float nextY = radius * std::sin(nextTheta);

        Vector3Df v[3] = {
            { nextX, nextY, 0.0f },
            { x, y, 0.0f },
            { 0.0f, 0.0f, 0.0f }
        };
        Vector3Df n[3] = {
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f }
        };

        std::copy(std::begin(v), std::end(v), &verticesBottom[i * 3]);
        std::copy(std::begin(n), std::end(n), &normalsBottom[i * 3]);
    });

    vertices.insert(vertices.end(), verticesBottom.begin(), verticesBottom.end());
    normals.insert(normals.end(), normalsBottom.begin(), normalsBottom.end());
}

void Cutter::generateTopMesh(float radius, float height, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments)
{
    std::vector<Vector3Df> verticesTop;
    std::vector<Vector3Df> normalsTop;
    verticesTop.resize(numSegments * 3);
    normalsTop.resize(numSegments * 3);
    std::for_each(std::execution::par, verticesTop.begin(), verticesTop.end(), [&](Vector3Df& p) {
        size_t i = (&p - verticesTop.data()) / 3;
        float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(numSegments);
        float nextTheta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i + 1) / static_cast<float>(numSegments);

        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        float nextX = radius * std::cos(nextTheta);
        float nextY = radius * std::sin(nextTheta);

        Vector3Df v[3] = {
            { x, y, height },
            { nextX, nextY, height },
            { 0.0f, 0.0f, height }
        };
        Vector3Df n[3] = {
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f }
        };

        std::copy(std::begin(v), std::end(v), &verticesTop[i * 3]);
        std::copy(std::begin(n), std::end(n), &normalsTop[i * 3]);
    });

    vertices.insert(vertices.end(), verticesTop.begin(), verticesTop.end());
    normals.insert(normals.end(), normalsTop.begin(), normalsTop.end());
}

void Cutter::generateSideMesh(float bottomRadius, float bottomHeight, float topHeight, float beta, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments)
{
    const float topRadius = bottomRadius + (topHeight - bottomHeight) * std::tan(beta);
    std::vector<Vector3Df> verticesSide;
    std::vector<Vector3Df> normalsSide;
    verticesSide.resize(numSegments * 6);
    normalsSide.resize(numSegments * 6);
    std::for_each(std::execution::par, verticesSide.begin(), verticesSide.end(), [&](Vector3Df& p) {
        size_t i = (&p - verticesSide.data()) / 6;
        float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(numSegments);
        float nextTheta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i + 1) / static_cast<float>(numSegments);

        float xTop = topRadius * std::cos(theta);
        float yTop = topRadius * std::sin(theta);
        float nextXTop = topRadius * std::cos(nextTheta);
        float nextYTop = topRadius * std::sin(nextTheta);
        float xBottom = bottomRadius * std::cos(theta);
        float yBottom = bottomRadius * std::sin(theta);
        float nextXBottom = bottomRadius * std::cos(nextTheta);
        float nextYBottom = bottomRadius * std::sin(nextTheta);

        Vector3Df v[6] = {
            { xBottom, yBottom, bottomHeight },
            { nextXBottom, nextYBottom, bottomHeight },
            { nextXTop, nextYTop, topHeight },
            { xBottom, yBottom, bottomHeight },
            { nextXTop, nextYTop, topHeight },
            { xTop, yTop, topHeight }
        };
        Vector3Df n[6] = {
            { xBottom, yBottom, -std::tan(beta) * std::hypot(xBottom, yBottom) },
            { nextXBottom, nextYBottom, -std::tan(beta) * std::hypot(nextXBottom, nextYBottom) },
            { nextXTop, nextYTop, -std::tan(beta) * std::hypot(nextXTop, nextYTop) },
            { xBottom, yBottom, -std::tan(beta) * std::hypot(xBottom, yBottom) },
            { nextXTop, nextYTop, -std::tan(beta) * std::hypot(nextXTop, nextYTop) },
            { xTop, yTop, -std::tan(beta) * std::hypot(xTop, yTop) }
        };

        std::copy(std::begin(v), std::end(v), &verticesSide[i * 6]);
        std::copy(std::begin(n), std::end(n), &normalsSide[i * 6]);
    });

    std::transform(std::execution::par, normalsSide.begin(), normalsSide.end(), normalsSide.begin(), [](const Vector3Df& n) {
        return n.normalize();
    });

    vertices.insert(vertices.end(), verticesSide.begin(), verticesSide.end());
    normals.insert(normals.end(), normalsSide.begin(), normalsSide.end());
}
