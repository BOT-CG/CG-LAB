#pragma once

#include "BBox3D.h"
#include "OBB3D.h"
#include "SDFOctantNode.h"
#include "Vector3D.h"

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

template <uint32_t MaxLevel>
class SDFOctree {
public:
    SDFOctree() = default;
    virtual ~SDFOctree() = default;

    SDFOctree(const SDFOctree& other)
        : root(std::make_unique<SDFOctantNode<MaxLevel>>(*other.root))
    {
    }


    void initialize(const AABB3Df& bbox)
    {
        root = std::make_unique<SDFOctantNode<MaxLevel>>();
        root->initialize(bbox);
        initializeBlankDistance();
    }
    void initialize(const std::function<float(const Vector3Df&)>& sdf)
    {
        root = std::make_unique<SDFOctantNode<MaxLevel>>();
        root->initialize(sdf);
        initializeBlankDistance();
    }
    void initialize(const std::array<std::array<std::vector<float>, (1 << MaxLevel) + 1>, (1 << MaxLevel) + 1>& gridIntersectionHeights)
    {
        root = std::make_unique<SDFOctantNode<MaxLevel>>();
        root->initialize(gridIntersectionHeights);
    }
    void setWorkpieceMaxEdge(float workpieceMaxEdge)
    {
        if (root == nullptr) {
            return;
        }

        root->workpieceMaxEdge = workpieceMaxEdge;
    }
    void fillPoints(std::set<std::tuple<float, float, float>>& points) const
    {
        if (root == nullptr) {
            return;
        }

        points.clear();
        root->fillPoints(points);
    }
    void initializeWithPointDistanceMap(const std::map<std::tuple<float, float, float>, float>& pointDistanceMap)
    {
        if (root == nullptr) {
            return;
        }

        root->initializeWithPointDistanceMap(pointDistanceMap);
        initializeBlankDistance();
    }
    void fillDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const
    {
        if (root == nullptr) {
            return;
        }

        centers.clear();
        sizes.clear();
        distances.clear();
        root->fillDistances(centers, sizes, distances);
    }
    void generateMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        if (root == nullptr) {
            return;
        }

        centers.clear();
        sizes.clear();
        vertices.clear();
        normals.clear();
        root->generateMesh(centers, sizes, vertices, normals);
    }

    void generateStaticMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, std::function<float(const Vector3Df&, const Posture&)> sdf)
    {
        if (root == nullptr) {
            return;
        }

        vertices.clear();
        normals.clear();
        root->generateMesh(vertices, normals, sdf);
    };

    void subtract(const BBox3Df& bbox, const std::function<float(const Vector3Df&)>& sdf)
    {
        if (root == nullptr) {
            return;
        }

        root->subtract(bbox, sdf);
    }
    void subtract(const std::vector<OBB3Df>& bboxs, const std::function<float(const Vector3Df&)>& sdf)
    {
        if (root == nullptr) {
            return;
        }

        root->subtract(bboxs, sdf);
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&)>>& sdfs)
    {
        if (root == nullptr) {
            return;
        }

        root->subtract(bboxs, sdfs);
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs)
    {
        if (root == nullptr) {
            return;
        }

        root->subtract(bboxs, sdfs);
    }

    float signedDistance(const Vector3Df& p) const
    {
        if (root == nullptr) {
            return std::numeric_limits<float>::max();
        }

        return root->signedDistance(p);
    }

    double getWorkPieceVolume()
    {
        if (root == nullptr) {
            return 0.0f;
        }

        return root->getNodeVolume();
    }

    void initializeBlankDistance()
    {
        if (root == nullptr) {
            return;
        }

        root->initializeBlankDistance();
    }

private:
    std::unique_ptr<SDFOctantNode<MaxLevel>> root;
};
