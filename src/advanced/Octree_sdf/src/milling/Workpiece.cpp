#include "Workpiece.h"

#include "GLCompute.h"
#include "OBB3D.h"
#include "STLFile.h"

#include <algorithm>
#include <array>
#include <execution>
#include <mutex>



void Workpiece::initialize(const std::function<float(const Vector3Df&)>& sdf)
{
    sdfOctree.initialize(sdf);
}

void Workpiece::initialize(const std::vector<Triangle3Df>& triangles)
{
    if (triangles.empty()) {
        return;
    }

    float minX = std::min_element(std::execution::par, triangles.begin(), triangles.end(), [](const Triangle3Df& lhs, const Triangle3Df& rhs) {
        return lhs.minX() < rhs.minX();
    })->minX();
    float minY = std::min_element(std::execution::par, triangles.begin(), triangles.end(), [](const Triangle3Df& lhs, const Triangle3Df& rhs) {
        return lhs.minY() < rhs.minY();
    })->minY();
    float minZ = std::min_element(std::execution::par, triangles.begin(), triangles.end(), [](const Triangle3Df& lhs, const Triangle3Df& rhs) {
        return lhs.minZ() < rhs.minZ();
    })->minZ();
    float maxX = std::max_element(std::execution::par, triangles.begin(), triangles.end(), [](const Triangle3Df& lhs, const Triangle3Df& rhs) {
        return lhs.maxX() < rhs.maxX();
    })->maxX();
    float maxY = std::max_element(std::execution::par, triangles.begin(), triangles.end(), [](const Triangle3Df& lhs, const Triangle3Df& rhs) {
        return lhs.maxY() < rhs.maxY();
    })->maxY();
    float maxZ = std::max_element(std::execution::par, triangles.begin(), triangles.end(), [](const Triangle3Df& lhs, const Triangle3Df& rhs) {
        return lhs.maxZ() < rhs.maxZ();
    })->maxZ();
    float distanceX = maxX - minX;
    float distanceY = maxY - minY;
    float distanceZ = maxZ - minZ;
    constexpr float magicNumber = 1.0f;
    maxEdge = std::max({ distanceX, distanceY, distanceZ }) * magicNumber;
    offset = {
        std::midpoint(maxX, minX),
        std::midpoint(maxY, minY),
        std::midpoint(maxZ, minZ)
    };

    std::vector<Triangle3Df> triangles11(triangles.size());
    std::transform(std::execution::par, triangles.begin(), triangles.end(), triangles11.begin(), [&](const Triangle3Df& t) {
        return Triangle3Df {
            coordinateLocalToGL(t.Vertices[0]),
            coordinateLocalToGL(t.Vertices[1]),
            coordinateLocalToGL(t.Vertices[2]),
            t.Normal
        };
    });

    constexpr uint32_t gridEdgePointCount = (1 << MaxLevel) + 1;
    constexpr float gridUnitLength = 2.0f / static_cast<float>(1 << MaxLevel);
    static std::array<std::array<std::vector<float>, gridEdgePointCount>, gridEdgePointCount> gridIntersectionHeights;
    gridIntersectionHeights.fill({});

    auto transformIndexToPoint = [&](const size_t i, const size_t j) {
        return Vector3Df {
            (static_cast<float>(i) - gridEdgePointCount / 2) * gridUnitLength,
            (static_cast<float>(j) - gridEdgePointCount / 2) * gridUnitLength,
            0.0f
        };
    };

    auto getIntersectionZ = [&](const Vector3Df& p, std::vector<float>& intersectionHeights) {
        std::mutex mutex;
        std::for_each(std::execution::par, triangles11.begin(), triangles11.end(), [&](const Triangle3Df& triangle) {
            const Vector3Df a = Vector3Df(triangle.Vertices[0].x, triangle.Vertices[0].y, 0);
            const Vector3Df b = Vector3Df(triangle.Vertices[1].x, triangle.Vertices[1].y, 0);
            const Vector3Df c = Vector3Df(triangle.Vertices[2].x, triangle.Vertices[2].y, 0);
            if (Triangle3Df(a, b, c).isInside(p) && std::abs(triangle.Normal.z) > 0.0001f) {
                const float A = triangle.Normal.x, B = triangle.Normal.y, C = triangle.Normal.z;
                const float x0 = triangle.Vertices[0].x, y0 = triangle.Vertices[0].y, z0 = triangle.Vertices[0].z;
                const float z = (A * (x0 - p.x) + B * (y0 - p.y)) / C + z0;
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    intersectionHeights.push_back((z));
                }
            }
        });
    };

    std::for_each(std::execution::par, gridIntersectionHeights.begin(), gridIntersectionHeights.end(), [&](std::array<std::vector<float>, gridEdgePointCount>& lineIntersectionHeights) {
        const size_t i = &lineIntersectionHeights - gridIntersectionHeights.data();
        std::for_each(std::execution::par, lineIntersectionHeights.begin(), lineIntersectionHeights.end(), [&](std::vector<float>& intersectionHeights) {
            const size_t j = &intersectionHeights - lineIntersectionHeights.data();
            const Vector3Df point = transformIndexToPoint(i, j);
            getIntersectionZ(point, intersectionHeights);
        });
    });

    sdfOctree.initialize(gridIntersectionHeights);

    std::set<std::tuple<float, float, float>> points;
    sdfOctree.fillPoints(points);

    constexpr float scale = 1024.0f;

    std::vector<float> triangleVertices;
    for (const auto& triangle : triangles11) {
        for (int i = 0; i < 3; ++i) {
            triangleVertices.push_back(triangle.Vertices[i].x * scale);
            triangleVertices.push_back(triangle.Vertices[i].y * scale);
            triangleVertices.push_back(triangle.Vertices[i].z * scale);
        }
    }
    std::cout << "halfMaxEdge: " << halfMaxEdge() << std::endl;
    std::cout << "offset: " << offset << std::endl;
    std::cout << "scale: " << scale << std::endl;
    std::vector<PointAndDistance> pointDistance(points.size());
    std::transform(std::execution::par, points.begin(), points.end(), pointDistance.begin(), [&](const auto& p) {
        return PointAndDistance { { std::get<0>(p) * scale, std::get<1>(p) * scale, std::get<2>(p) * scale }, 0.0f };
    });

    GLCompute::computeSTL2UDF(triangleVertices, pointDistance);
    /*****************************************坐标系调整*********************************************/
    std::map<std::tuple<float, float, float>, float> pointDistanceMap;
    for (const auto& p : pointDistance) {
        pointDistanceMap[std::make_tuple(p.point.x / scale, p.point.y / scale, p.point.z / scale)] = (p.distance / scale) * halfMaxEdge();
    }
    sdfOctree.initializeWithPointDistanceMap(pointDistanceMap);
    sdfOctree.setWorkpieceMaxEdge(maxEdge);
    sdfOctree.initializeBlankDistance();
}

void Workpiece::initialize(float length, float width, float height, const Vector3Df& offset)
{
    maxEdge = std::max({ length, width, height });
    AABB3Df bbox(Vector3Df(0, 0, 0), length / 2.0f, width / 2.0f, height / 2.0f);
    AABB3Df bboxGL(coordinateLocalToGL(bbox.getMin()), coordinateLocalToGL(bbox.getMax()));
    this->offset = offset;
    sdfOctree.initialize(bboxGL);
    sdfOctree.setWorkpieceMaxEdge(maxEdge);
}

void Workpiece::fillDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const
{
    sdfOctree.fillDistances(centers, sizes, distances);
}

void Workpiece::generateMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    sdfOctree.generateMesh(centers, sizes, vertices, normals);
}

void Workpiece::generateMeshLocal(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    sdfOctree.generateMesh(centers, sizes, vertices, normals);
    std::transform(std::execution::par, vertices.begin(), vertices.end(), vertices.begin(), [&](const auto& v) {
        return coordinateGLToLocal(v);
    });
}

void Workpiece::subtract(const BBox3Df& bbox, const std::function<float(const Vector3Df&)>& sdf)
{
    auto bboxGL = OBB3Df(
        coordinateLocalToGLWithoutOffset(bbox.getCenter()),
        coordinateLocalToGLWithoutOffset(bbox.getAxis(0)),
        coordinateLocalToGLWithoutOffset(bbox.getAxis(1)),
        coordinateLocalToGLWithoutOffset(bbox.getAxis(2)));
    sdfOctree.subtract(bboxGL, [&](const Vector3Df& p) {
        return sdf(coordinateGLToLocalWithoutOffset(p));
    });
}
void Workpiece::subtract(const std::vector<OBB3Df>& bboxs, const std::function<float(const Vector3Df&)>& sdf)
{
    std::vector<OBB3Df> bboxGLs = bboxs;
    std::transform(std::execution::par, bboxGLs.begin(), bboxGLs.end(), bboxGLs.begin(), [&](const auto& bbox) {
        return OBB3Df(
            coordinateLocalToGLWithoutOffset(bbox.getCenter()),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(0)),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(1)),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(2)));
    });
    // sdfOctree.subtract(bboxGLs, [&](const Vector3Df& p) {
    //     return sdf(coordinateGLToLocalWithoutOffset(p));
    // });
    sdfOctree.subtract(bboxGLs, sdf);
}
void Workpiece::subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&)>>& sdfs)
{
    std::vector<OBB3Df> bboxGLs = bboxs;
    std::transform(std::execution::par, bboxGLs.begin(), bboxGLs.end(), bboxGLs.begin(), [&](const auto& bbox) {
        return OBB3Df(
            coordinateLocalToGLWithoutOffset(bbox.getCenter()),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(0)),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(1)),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(2)));
    });
    sdfOctree.subtract(bboxGLs, sdfs);
}

void Workpiece::subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs)
{
    std::vector<OBB3Df> bboxGLs = bboxs;
    std::transform(std::execution::par, bboxGLs.begin(), bboxGLs.end(), bboxGLs.begin(), [&](const auto& bbox) {
        return OBB3Df(
            coordinateLocalToGLWithoutOffset(bbox.getCenter()),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(0)),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(1)),
            coordinateLocalToGLWithoutOffset(bbox.getAxis(2)));
    });
    sdfOctree.subtract(bboxGLs, sdfs);
}

float Workpiece::signedDistance(const Vector3Df& p) const
{
    return sdfOctree.signedDistance(p);
}

void Workpiece::saveToSTLFile(const std::string& filename) const
{
    std::vector<Vector3Df> centers;
    std::vector<float> sizes;
    std::vector<Vector3Df> vertices;
    std::vector<Vector3Df> normals;
    generateMeshLocal(centers, sizes, vertices, normals);
    STLFile::Save(vertices, normals, filename);
}

