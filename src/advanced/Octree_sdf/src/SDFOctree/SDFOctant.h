#pragma once

#include "AABB3D.h"
#include "BBox3D.h"
#include "Morton.h"
#include "OBB3D.h"
#include "Posture.h"
#include "Vector3D.h"
#include <array>
#include <cstdint>
#include <execution>
#include <functional>
#include <map>
#include <set>
#include <tuple>
#include <vector>

template <uint32_t MaxLevel>
class SDFOctant {
public:
    SDFOctant() = default;
    virtual ~SDFOctant() = default;

    SDFOctant(const uint32_t _level, const uint64_t _id) noexcept
        : Level(_level)
        , id(_id)
    {
    }

    constexpr bool isChildAchieveMaxLevel() const { return Level + 1 == MaxLevel; }
    constexpr float edgeLength() const { return 1.0f / (1 << Level); }
    constexpr float halfEdgeLength() const { return edgeLength() / 2.0f; }
    constexpr float edgeLengthGL() const { return edgeLength() * 2.0f; }
    constexpr float halfEdgeLengthGL() const { return edgeLengthGL() / 2.0f; }

    constexpr Vector3Df getCoord() const
    {
        auto [x, y, z] = Morton::decode(id);
        return Vector3Df((float)x, (float)y, (float)z) * edgeLength() + halfEdgeLength();
    }

    constexpr Vector3Df getCoordGL() const
    {
        return getCoord() * 2.0f - 1.0f;
    }

    constexpr Vector3Df getVertexCoordGL(size_t index) const
    {
        return getCoordGL() + CubicVertices[index] * halfEdgeLengthGL();
    }

    constexpr inline float halfWorkpieceMaxEdge() const { return workpieceMaxEdge / 2.0f; }
    // constexpr inline Vector3Df coordinateLocalToGLWithoutOffset(const Vector3Df& p) const
    // {
    //     return p / halfWorkpieceMaxEdge();
    // }
    constexpr inline Vector3Df coordinateGLToLocalWithoutOffset(const Vector3Df& p) const
    {
        return p * halfWorkpieceMaxEdge();
    }
    constexpr Vector3Df getVertexCoordLocal(size_t index) const
    {
        return this->coordinateGLToLocalWithoutOffset(this->getVertexCoordGL(index));
    }
    constexpr inline float edgeLengthWorkpiece() const { return workpieceMaxEdge * edgeLength(); }

    AABB3Df getBBoxGL() const
    {
        return AABB3Df(getCoordGL(), halfEdgeLengthGL());
    }

    constexpr uint64_t getChildId(size_t i) const
    {
        return (uint64_t)i | (id << 3);
    }

    static float getUnionSDF(const std::vector<std::function<float(const Vector3Df&)>>& sdfs, const Vector3Df& p)
    {
        // return std::max_element(std::execution::par, sdfs.begin(), sdfs.end(), [&](const auto& sdf1, const auto& sdf2) {
        //     return sdf1(p) < sdf2(p);
        // })->
        // operator()(p);
        auto MaxSdf = std::max_element(std::execution::par, sdfs.begin(), sdfs.end(), [&](const auto& sdf1, const auto& sdf2) {
            return sdf1(p) < sdf2(p);
        });
        return (*MaxSdf)(p);
    }

    static float getUnionSDFWithCutterPosition(const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs, const Vector3Df& p, Posture& CutterPosture)
    {

        auto MaxSdf = std::max_element(std::execution::par, sdfs.begin(), sdfs.end(), [&](const auto& sdf1, const auto& sdf2) {
            Posture CutterPosture1, CutterPosture2;
            return sdf1(p, CutterPosture1) < sdf2(p, CutterPosture2);
        });
        return (*MaxSdf)(p, CutterPosture);
    }

    static float getBrickSDF(const std::function<float(const Vector3Df&, const Posture& CutterPosture)>& sdf, const Vector3Df& p, const std::array<Posture, 8>& CutterPosture)
    {
        auto Max_Posture = std::max_element(std::execution::par, CutterPosture.begin(), CutterPosture.end(), [&](const auto& CutterPosture1, const auto& CutterPosture2) {
            return sdf(p, CutterPosture1) < sdf(p, CutterPosture2);
        });
        return sdf(p, *Max_Posture);
    }

    bool isAllVertexInside(const std::function<float(const Vector3Df&)>& sdf) const
    {
        return std::all_of(std::execution::seq, VerticesIndices.begin(), VerticesIndices.end(), [&](size_t i) {
            return sdf(getVertexCoordLocal(i)) >= 0.0f;
        });
    }
    bool isAllVertexInside(const std::vector<std::function<float(const Vector3Df&)>>& sdfs) const
    {
        return std::all_of(std::execution::seq, VerticesIndices.begin(), VerticesIndices.end(), [&](size_t i) {
            return getUnionSDF(sdfs, getVertexCoordLocal(i)) >= 0.0f;
        });
    }
    bool isAllVertexInside(const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs) const
    {
        return std::all_of(std::execution::seq, VerticesIndices.begin(), VerticesIndices.end(), [&](size_t i) {
            Posture CutterPosture;
            return getUnionSDFWithCutterPosition(sdfs, getVertexCoordLocal(i), CutterPosture) >= 0.0f;
            // return getUnionSDF(sdfs, getVertexCoordLocal(i)) >= 0.0f;
        });
    }

    bool isAllVertexOutside(const std::function<float(const Vector3Df&)>& sdf) const
    {
        return std::all_of(std::execution::seq, VerticesIndices.begin(), VerticesIndices.end(), [&](size_t i) {
            return sdf(getVertexCoordLocal(i)) <= 0.0f;
        });
    }
    bool isAllVertexOutside(const std::vector<std::function<float(const Vector3Df&)>>& sdfs) const
    {
        return std::all_of(std::execution::seq, VerticesIndices.begin(), VerticesIndices.end(), [&](size_t i) {
            return getUnionSDF(sdfs, getVertexCoordLocal(i)) <= 0.0f;
        });
    }
    bool isAllVertexOutside(const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs) const
    {
        return std::all_of(std::execution::seq, VerticesIndices.begin(), VerticesIndices.end(), [&](size_t i) {
            Posture CutterPosture;
            return getUnionSDFWithCutterPosition(sdfs, getVertexCoordLocal(i), CutterPosture) <= 0.0f;
            // return getUnionSDF(sdfs, getVertexCoordLocal(i)) <= 0.0f;
        });
    }

    virtual void initialize(const AABB3Df& bbox) = 0;
    virtual void initialize(const std::function<float(const Vector3Df&)>& sdf) = 0;
    virtual void initialize(const std::array<std::array<std::vector<float>, (1 << MaxLevel) + 1>, (1 << MaxLevel) + 1>& gridIntersectionHeights) = 0;
    virtual void fillPoints(std::set<std::tuple<float, float, float>>& points) const = 0;
    virtual void initializeWithPointDistanceMap(const std::map<std::tuple<float, float, float>, float>& pointDistanceMap) = 0;
    virtual void fillDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const = 0;
    virtual void generateMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const = 0;

    virtual void subtract(const BBox3Df& bbox, const std::function<float(const Vector3Df&)>& sdf) = 0;
    virtual void subtract(const std::vector<OBB3Df>& bboxs, const std::function<float(const Vector3Df&)>& sdf) = 0;
    virtual void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&)>>& sdfs) = 0;
    virtual float signedDistance(const Vector3Df& p) const = 0;

    virtual void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs) = 0;

    virtual double getNodeVolume() = 0;

    virtual void initializeBlankDistance() = 0;
    
    const uint32_t Level = 0;
    const uint64_t id = 0;

    static float workpieceMaxEdge;

protected:
    // static constexpr float delta = 1e-4f;
    // static constexpr float dfDelta = 1e-4f;
    // static constexpr float IntersectionTolerance = 1e-4f;
    static constexpr std::array<size_t, 8> VerticesIndices = { 0, 1, 2, 3, 4, 5, 6, 7 };
    static constexpr std::array<Vector3Df, 8> CubicVertices = {
        Vector3Df(-1.0f, -1.0f, -1.0f),
        Vector3Df(-1.0f, -1.0f, 1.0f),
        Vector3Df(-1.0f, 1.0f, -1.0f),
        Vector3Df(-1.0f, 1.0f, 1.0f),
        Vector3Df(1.0f, -1.0f, -1.0f),
        Vector3Df(1.0f, -1.0f, 1.0f),
        Vector3Df(1.0f, 1.0f, -1.0f),
        Vector3Df(1.0f, 1.0f, 1.0f)
    };
};

template <uint32_t MaxLevel>
float SDFOctant<MaxLevel>::workpieceMaxEdge = 1.0f;
