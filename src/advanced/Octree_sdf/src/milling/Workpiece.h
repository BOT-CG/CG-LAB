#pragma once

#include "AABB3D.h"
#include "BBox3D.h"
#include "OBB3D.h"
#include "SDFOctree.h"
#include "Triangle3D.h"
#include "Vector3D.h"

#include <functional>
#include <map>
#include <set>
#include <tuple>
#include <vector>

class Workpiece {
public:
    Workpiece() = default;
    virtual ~Workpiece() = default;

    Workpiece(const Workpiece& other)
        : sdfOctree(other.sdfOctree)
        , maxEdge(other.maxEdge)
        , offset(other.offset)
    {
    }

    explicit Workpiece(float length, float width, float height, const Vector3Df& offset = { 0, 0, 0 })
    {
        initialize(length, width, height, offset);
        setWorkpieceVolume(calculateWorkpieceVolume());
    }

    explicit Workpiece(const std::function<float(const Vector3Df&)>& sdf)
    {
        initialize(sdf);
        setWorkpieceVolume(calculateWorkpieceVolume());
    }

    explicit Workpiece(const std::vector<Triangle3Df>& triangles)
    {
        initialize(triangles);
        setWorkpieceVolume(calculateWorkpieceVolume());
    }


    void initialize(float length, float width, float height, const Vector3Df& offset = { 0, 0, 0 });
    void initialize(const std::function<float(const Vector3Df&)>& sdf);
    void initialize(const std::vector<Triangle3Df>& triangles);
    void fillDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const;
    void generateMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
    void generateMeshLocal(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
    void subtract(const BBox3Df& bbox, const std::function<float(const Vector3Df&)>& sdf);
    void subtract(const std::vector<OBB3Df>& bboxs, const std::function<float(const Vector3Df&)>& sdf);
    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&)>>& sdfs);
    float signedDistance(const Vector3Df& p) const;
    void saveToSTLFile(const std::string& filename) const;
    inline float getMaxEdge() const { return maxEdge; }
    inline Vector3Df getOffset() const { return offset; }

    double calculateWorkpieceVolume()
    {
        return sdfOctree.getWorkPieceVolume();
    }

    double getWorkpieceVolume() const
    {
        return workpieceVolume;
    }

    void setWorkpieceVolume(double volume)
    {
        workpieceVolume = volume;
    }

    double workpieceVolume = 0.0;

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs);


private:
    static constexpr uint32_t MaxLevel = 8;
    SDFOctree<MaxLevel> sdfOctree;

    float maxEdge;
    Vector3Df offset = { 0, 0, 0 };

    constexpr inline float halfMaxEdge() const { return maxEdge / 2.0f; }
    constexpr inline Vector3Df coordinateLocalToGLWithoutOffset(const Vector3Df& p) const
    {
        return p / halfMaxEdge();
    }
    constexpr inline Vector3Df coordinateGLToLocalWithoutOffset(const Vector3Df& p) const
    {
        return p * halfMaxEdge();
    }
    constexpr inline Vector3Df coordinateLocalToGL(const Vector3Df& p) const
    {
        return (p - offset) / halfMaxEdge();
    }
    constexpr inline Vector3Df coordinateGLToLocal(const Vector3Df& p) const
    {
        return p * halfMaxEdge() + offset;
    }
};
