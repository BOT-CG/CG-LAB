#pragma once

#include "BBox3D.h"
#include "MarchingCubes.h"
#include "OBB3D.h"
#include "SDFOctant.h"
#include "TrilinearInterpolation.h"
#include "Vector3D.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <execution>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <tuple>
#include <vector>

template <uint32_t MaxLevel>
class SDFOctantLeaf : public SDFOctant<MaxLevel> {
public:
    using SDFOctant<MaxLevel>::SDFOctant;

    SDFOctantLeaf() = default;
    virtual ~SDFOctantLeaf() = default;

    SDFOctantLeaf(const SDFOctantLeaf& other)
        : SDFOctant<MaxLevel>(other)
        , v(other.v)
        , CubeCutterPosture(other.CubeCutterPosture)
        , BlankDistance(other.BlankDistance)

    {
    }
    explicit SDFOctantLeaf(float _v[8]) noexcept
    {
        std::copy(_v, _v + 8, this->v.begin());
    }
    explicit SDFOctantLeaf(float _v1, float _v2, float _v3, float _v4, float _v5, float _v6, float _v7, float _v8) noexcept
    {
        this->v[0] = _v1;
        this->v[1] = _v2;
        this->v[2] = _v3;
        this->v[3] = _v4;
        this->v[4] = _v5;
        this->v[5] = _v6;
        this->v[6] = _v7;
        this->v[7] = _v8;
    }

    void initialize(const AABB3Df& bbox) override
    {
        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            d = bbox.signedDistanceToPoint(this->getVertexCoordGL(i));
        });
        this->UpdateVolume();
    }

    void initialize(const std::function<float(const Vector3Df&)>& sdf) override
    {
        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            d = sdf(this->getVertexCoordGL(i));
        });
        this->UpdateVolume();
    }

    void initialize(const std::array<std::array<std::vector<float>, (1 << MaxLevel) + 1>, (1 << MaxLevel) + 1>& gridIntersectionHeights) override
    {
        constexpr uint32_t gridEdgePointCount = (1 << MaxLevel) + 1;
        constexpr float gridUnitLength = 2.0f / static_cast<float>(1 << MaxLevel);

        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            auto coord = this->getVertexCoordGL(i);
            uint32_t x = static_cast<uint32_t>(coord.x / gridUnitLength) + gridEdgePointCount / 2;
            uint32_t y = static_cast<uint32_t>(coord.y / gridUnitLength) + gridEdgePointCount / 2;
            if (std::find_if(gridIntersectionHeights[x][y].begin(), gridIntersectionHeights[x][y].end(), [&](float h) {
                    return h == coord.z;
                })
                != gridIntersectionHeights[x][y].end()) {
                d = 0;
                return;
            }
            std::count_if(gridIntersectionHeights[x][y].begin(), gridIntersectionHeights[x][y].end(), [&](float h) {
                return h > coord.z;
            }) % 2 == 1
                ? d = 1
                : d = -1;
        });
        this->UpdateVolume();
    }

    void fillPoints(std::set<std::tuple<float, float, float>>& points) const override
    {
        for (size_t i = 0; i < 8; i++) {
            auto coord = this->getVertexCoordGL(i);
            points.insert(std::make_tuple(coord.x, coord.y, coord.z));
        }
    }

    void initializeWithPointDistanceMap(const std::map<std::tuple<float, float, float>, float>& pointDistanceMap) override
    {
        std::for_each(std::execution::seq, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            auto coord = this->getVertexCoordGL(i);
            auto key = std::make_tuple(coord.x, coord.y, coord.z);
            auto it = pointDistanceMap.find(key);
            if (it != pointDistanceMap.end()) {
                d *= it->second;
            } else {
                d = 0;
            }
        });
    }

    void fillDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const override
    {
        centers.push_back(this->getCoordGL());
        sizes.push_back(this->edgeLengthGL());
        std::copy(this->v.begin(), this->v.end(), std::back_inserter(distances));
    }

    void generateMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override
    {
        centers.push_back(this->getCoordGL());
        sizes.push_back(this->edgeLengthGL());

        if (checkAllPositive() || checkAllNegative()) {
            return;
        }

        if (checkAllZero()) {
            buildAllZero(vertices, normals);
            return;
        }

        if (checkSevenZero()) {
            buildSevenZero(sevenZeroIndex(), vertices, normals);
            return;
        }

        if (checkSixZero()) {
            buildSixZero(sixZeroIndex(), vertices, normals);
            return;
        }

        MarchingCubes mc(this->getCoordGL(), this->edgeLengthGL(), this->v);
        mc.generateMesh(vertices, normals);
    }



    void initializeBlankDistance() override
    {
        std::for_each(std::execution::par, this->BlankDistance.begin(), this->BlankDistance.end(), [&](float& d) {
            size_t Index = &d - this->BlankDistance.data();
            d = this->v[Index];
        });
    };

    void subtract(const BBox3Df& bbox, const std::function<float(const Vector3Df&)>& sdf) override
    {
        this->updateDistance(sdf);
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::function<float(const Vector3Df&)>& sdf) override
    {
        this->updateDistance(sdf);
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&)>>& sdfs) override
    {
        this->updateDistance(sdfs);
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs) override
    {
        this->updateDistance(sdfs);
    }

    float signedDistance(const Vector3Df& p) const override
    {
        return TIDistance(p);
    }

    void updateDistance(const std::function<float(const Vector3Df&)>& sdf)
    {
        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            d = std::min(d, -sdf(this->getVertexCoordLocal(i)));
        });
    }
    void updateDistance(const std::vector<std::function<float(const Vector3Df&)>>& sdfs)
    {
        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            d = std::min(d, -this->getUnionSDF(sdfs, this->getVertexCoordLocal(i)));
        });
    }

    void updateDistance(const std::function<float(const Vector3Df&, Posture&)>& sdf)
    {
        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            Posture tempPosture = Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f));
            float sdf = -sdf(this->getVertexCoordLocal(i), tempPosture);
            if (sdf < d) {
                d = sdf;
                CubeCutterPosture[i] = tempPosture;
            }
            // d = std::min(d, -this->getUnionSDF(sdfs, this->getVertexCoordLocal(i)));
        });
    }

    void updateDistance(const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs)
    {
        std::for_each(std::execution::par, this->v.begin(), this->v.end(), [&](float& d) {
            size_t i = &d - this->v.data();
            Posture tempPosture = Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f));
            float sdf = -this->getUnionSDFWithCutterPosition(sdfs, this->getVertexCoordLocal(i), tempPosture);
            if (sdf < d) {
                d = sdf;
                CubeCutterPosture[i] = tempPosture;
                // std::cout<<tempPosture.center <<std::endl;
            }

            // d = std::min(d, -this->getUnionSDF(sdfs, this->getVertexCoordLocal(i)));
        });
    }

    SDFOctantLeaf<MaxLevel> getSubOctantLeaf(size_t index) const
    {
        auto sdfOctantLeaf = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(index));

        std::for_each(std::execution::par, sdfOctantLeaf.v.begin(), sdfOctantLeaf.v.end(), [&](float& d) {
            size_t i = &d - sdfOctantLeaf.v.data();
            auto p = (sdfOctantLeaf.getVertexCoordGL(i) - this->getCoordGL()) / this->halfEdgeLengthGL();
            d = TIDistance(coordGL2TI(p));
        });

        std::for_each(std::execution::par, sdfOctantLeaf.BlankDistance.begin(), sdfOctantLeaf.BlankDistance.end(), [&](float& d) {
            size_t i = &d - sdfOctantLeaf.BlankDistance.data();
            auto p = (sdfOctantLeaf.getVertexCoordGL(i) - this->getCoordGL()) / this->halfEdgeLengthGL();
            d = TIDistance_ForBlank(coordGL2TI(p));
        });
        return sdfOctantLeaf;
    }

    Vector3Df coordGL2TI(const Vector3Df& p) const
    {
        return p * 0.5f + 0.5f;
    }

    float TIDistance(const Vector3Df& p) const
    {
        if (checkSevenZero()) {
            auto p0 = std::find_if(std::execution::seq, this->v.begin(), this->v.end(), [](float d) { return d != 0.0f; });
            auto i0 = std::distance(this->v.begin(), p0);
            auto [x, y, z] = Morton::decode(i0);
            auto bbox = AABB3Df(Vector3Df((float)x, (float)y, (float)z), 1.0f);
            return bbox.signedDistanceToPoint(p);
        } else if (checkSixZero()) {
            auto p0 = std::find_if(std::execution::seq, EdgeIndices.begin(), EdgeIndices.end(), [&](const std::array<int, 2>& e) {
                return this->v[e[0]] != 0.0f && this->v[e[1]] != 0.0f;
            });
            auto i0 = std::distance(this->EdgeIndices.begin(), p0);
            auto [x1, y1, z1] = Morton::decode(EdgeIndices[i0][0]);
            auto [x2, y2, z2] = Morton::decode(EdgeIndices[i0][1]);
            auto bbox = AABB3Df((Vector3Df((float)x1, (float)y1, (float)z1) + Vector3Df((float)x2, (float)y2, (float)z2)) / 2.0f, 1.0f);
            return bbox.signedDistanceToPoint(p);
        }
        return TrilinearInterpolation::L(this->v.data(), p.x, p.y, p.z);
    }

    float TIDistance_ForBlank(const Vector3Df& p) const
    {
        if (checkSevenZero()) {
            auto p0 = std::find_if(std::execution::seq, this->BlankDistance.begin(), this->BlankDistance.end(), [](float d) { return d != 0.0f; });
            auto i0 = std::distance(this->BlankDistance.begin(), p0);
            auto [x, y, z] = Morton::decode(i0);
            auto bbox = AABB3Df(Vector3Df((float)x, (float)y, (float)z), 1.0f);
            return bbox.signedDistanceToPoint(p);
        } else if (checkSixZero()) {
            auto p0 = std::find_if(std::execution::seq, EdgeIndices.begin(), EdgeIndices.end(), [&](const std::array<int, 2>& e) {
                return this->BlankDistance[e[0]] != 0.0f && this->BlankDistance[e[1]] != 0.0f;
            });
            auto i0 = std::distance(this->EdgeIndices.begin(), p0);
            auto [x1, y1, z1] = Morton::decode(EdgeIndices[i0][0]);
            auto [x2, y2, z2] = Morton::decode(EdgeIndices[i0][1]);
            auto bbox = AABB3Df((Vector3Df((float)x1, (float)y1, (float)z1) + Vector3Df((float)x2, (float)y2, (float)z2)) / 2.0f, 1.0f);
            return bbox.signedDistanceToPoint(p);
        }
        return TrilinearInterpolation::L(this->BlankDistance.data(), p.x, p.y, p.z);
    }

    bool checkTolerance(const BBox3Df& bbox) const
    {
        return std::all_of(std::execution::seq, Centers.begin(), Centers.end(), [&](const Vector3Df& p) {
            auto p1 = coordGL2TI(p);
            auto p2 = this->getCoordGL() + p * this->halfEdgeLengthGL();
            return std::abs(TIDistance(p1) - bbox.signedDistanceToPoint(p2)) < TITolerance;
        });
    }

    bool checkTolerance(const std::function<float(const Vector3Df&)>& sdf) const
    {
        return std::all_of(std::execution::seq, Centers.begin(), Centers.end(), [&](const Vector3Df& p) {
            auto p1 = coordGL2TI(p);
            auto p2 = this->getCoordGL() + p * this->halfEdgeLengthGL();
            return std::abs(TIDistance(p1) - sdf(this->coordinateGLToLocalWithoutOffset(p2))) < TITolerance;
        });
    }
    bool checkTolerance(const std::vector<std::function<float(const Vector3Df&)>>& sdfs) const
    {
        return std::all_of(std::execution::seq, Centers.begin(), Centers.end(), [&](const Vector3Df& p) {
            auto p1 = coordGL2TI(p);
            auto p2 = this->getCoordGL() + p * this->halfEdgeLengthGL();
            return std::abs(TIDistance(p1) - this->getUnionSDF(sdfs, this->coordinateGLToLocalWithoutOffset(p2))) < TITolerance;
        });
    }
    bool checkTolerance(const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs) const
    {
        return std::all_of(std::execution::seq, Centers.begin(), Centers.end(), [&](const Vector3Df& p) {
            auto p1 = coordGL2TI(p);
            auto p2 = this->getCoordGL() + p * this->halfEdgeLengthGL();
            Posture CutterPosture;
            return std::abs(TIDistance(p1) - this->getUnionSDFWithCutterPosition(sdfs, this->coordinateGLToLocalWithoutOffset(p2), CutterPosture)) < TITolerance;
        });
    }

    bool checkAllPositive() const
    {
        return std::all_of(std::execution::seq, this->v.begin(), this->v.end(), [](float d) {
            return d > 0.0f;
        });
    }

    bool checkAllNegative() const
    {
        return std::all_of(std::execution::seq, this->v.begin(), this->v.end(), [](float d) {
            return d < 0.0f;
        });
    }

    bool checkAllZero() const
    {
        return std::all_of(std::execution::seq, this->v.begin(), this->v.end(), [](float d) {
            return d == 0.0f;
        });
    }

    void buildAllZero(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        for (size_t i = 0; i < 6; i++) {
            buildFace(i, vertices, normals);
        }
    }

    bool checkSevenZero() const
    {
        return std::count_if(std::execution::seq, this->v.begin(), this->v.end(), [](float d) {
            return d == 0.0f;
        }) == 7;
    }

    int sevenZeroIndex() const
    {
        auto it = std::find_if(std::execution::seq, this->v.begin(), this->v.end(), [](float d) {
            return d != 0.0f;
        });
        if (it == this->v.end()) {
            return -1;
        }
        int index = (int)std::distance(this->v.begin(), it);
        return index;
    }

    void buildSevenZero(size_t index, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        std::for_each(std::execution::seq, FaceIndices.begin(), FaceIndices.end(), [&](const std::array<int, 4>& f) {
            if (std::none_of(f.begin(), f.end(), [&](int i) { return i == index; })) {
                buildFace(&f - FaceIndices.data(), vertices, normals);
            }
        });
    }

    bool checkSixZero() const
    {
        return std::count_if(std::execution::seq, this->v.begin(), this->v.end(), [](float d) { return d == 0.0f; }) == 6
            && std::any_of(std::execution::seq, EdgeIndices.begin(), EdgeIndices.end(), [&](const std::array<int, 2>& e) {
                   return this->v[e[0]] * this->v[e[1]] > 0.0f;
               });
    }

    int sixZeroIndex() const
    {
        if (std::count_if(std::execution::seq, this->v.begin(), this->v.end(), [](float d) { return d == 0.0f; }) != 6) {
            return -1;
        }
        auto it = std::find_if(std::execution::seq, EdgeIndices.begin(), EdgeIndices.end(), [&](const std::array<int, 2>& e) {
            return this->v[e[0]] * this->v[e[1]] > 0.0f;
        });
        if (it == EdgeIndices.end()) {
            return -1;
        }
        return (int)std::distance(EdgeIndices.begin(), it);
    }

    void buildSixZero(size_t index, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        // for (int i = 0; i < 6; i++) {
        //     bool flag = true;
        //     for (int j = 0; j < 4; j++) {
        //         if (FaceIndices[i][j] == EdgeIndices[index][0] || FaceIndices[i][j] == EdgeIndices[index][1]) {
        //             flag = false;
        //             break;
        //         }
        //     }
        //     if (flag) {
        //         buildFace(i, vertices, normals);
        //     }
        // }
        std::for_each(std::execution::seq, FaceIndices.begin(), FaceIndices.end(), [&](const std::array<int, 4>& f) {
            if (std::none_of(f.begin(), f.end(), [&](int i) {
                    return i == EdgeIndices[index][0] || i == EdgeIndices[index][1];
                })) {
                buildFace(&f - FaceIndices.data(), vertices, normals);
            }
        });
    }

    void buildFace(size_t index, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
    {
        static constexpr std::array<size_t, 6> indices = { 0, 1, 2, 3, 2, 1 };
        std::transform(indices.begin(), indices.end(), std::back_inserter(vertices), [&](size_t i) {
            return this->CubicVertices[FaceIndices[index][i]] * this->halfEdgeLengthGL() + this->getCoordGL();
        });
        std::fill_n(std::back_inserter(normals), 6, FaceNormals[index]);
    }

    void UpdateVolume()
    {
#if 1
        double NegativeVolume = 0.0f;
        double PositiveVolume = 0.0f;
        std::for_each(std::execution::seq, this->v.begin(), this->v.end(), [&](float& d) {
            if (d > 0.0f) {
                PositiveVolume += d;
            } else {
                NegativeVolume += std::abs(d);
            }
        });
        LeafVolume = PositiveVolume / (std::abs(NegativeVolume) + PositiveVolume);

        // int positiveVolume = std::count_if(std::execution::par, this->v.begin(), this->v.end(), [&](float d) {
        //     return d > 0.0f;
        // });
        // LeafVolume = (double)positiveVolume / (double)this->v.size();
#else
        constexpr int numSamples = 1 << 3;
        std::vector<int> samples(numSamples * numSamples * numSamples);
        std::iota(samples.begin(), samples.end(), 0);
        int positiveVolume = std::count_if(std::execution::par, samples.begin(), samples.end(), [&](int i) {
            int x = i % numSamples;
            int y = (i / numSamples) % numSamples;
            int z = i / (numSamples * numSamples);
            Vector3Df p = Vector3Df(x, y, z) / (numSamples - 1.0f);
            return this->TIDistance(p) >= 0.0f;
        });
        LeafVolume = (double)positiveVolume / (double)(numSamples * numSamples * numSamples);

#endif
    }

    double getNodeVolume() override
    {
        this->UpdateVolume();
        // double Mask = 8.0f;
        // this->UpdateVolume();
        // std::cout << "Level: " << this->Level << " Volume: " << Volume << std::endl;
        // std::cout << "this->edgeLength(): " << this->edgeLength() << std::endl;
        // return LeafVolume * std::pow(this->edgeLength(), 3.0f) * 100.0f;
        // return LeafVolume * std::pow(Mask, MaxLevel - this->Level);
        return LeafVolume * std::pow(this->edgeLengthWorkpiece(), 3.0f);
    }

    std::array<float, 8> v = { 0.0f };
    double LeafVolume = 1.0f;
    std::array<Posture, 8> CubeCutterPosture = {
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f)),
        Posture(Vector3Df(10000.0f, 10000.0f, 10000.0f), Vector3Df(10000.0f, 10000.0f, 10000.0f))
    };
    std::array<float, 8> BlankDistance = { 0.0f };

private:
    static constexpr float TITolerance = 1e-4f;
    static constexpr std::array<Vector3Df, 19> Centers = {
        Vector3Df(0.0f, -1.0f, -1.0f),
        Vector3Df(-1.0f, 0.0f, -1.0f),
        Vector3Df(1.0f, 0.0f, -1.0f),
        Vector3Df(0.0f, 1.0f, -1.0f),

        Vector3Df(0.0f, -1.0f, 1.0f),
        Vector3Df(-1.0f, 0.0f, 1.0f),
        Vector3Df(1.0f, 0.0f, 1.0f),
        Vector3Df(0.0f, 1.0f, 1.0f),

        Vector3Df(0.0f, 0.0f, -1.0f),

        Vector3Df(-1.0f, -1.0f, 0.0f),
        Vector3Df(1.0f, -1.0f, 0.0f),
        Vector3Df(-1.0f, 1.0f, 0.0f),
        Vector3Df(1.0f, 1.0f, 0.0f),

        Vector3Df(0.0f, 0.0f, 1.0f),

        Vector3Df(0.0f, -1.0f, 0.0f),
        Vector3Df(-1.0f, 0.0f, 0.0f),
        Vector3Df(1.0f, 0.0f, 0.0f),
        Vector3Df(0.0f, 1.0f, 0.0f),

        Vector3Df(0.0f, 0.0f, 0.0f)
    };
    static constexpr std::array<std::array<int, 2>, 12> EdgeIndices = {
        { { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
            { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
            { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 } }
    };
    static constexpr std::array<Vector3Df, 6> FaceNormals = {
        Vector3Df(-1.0f, 0.0f, 0.0f),
        Vector3Df(0.0f, -1.0f, 0.0f),
        Vector3Df(0.0f, 0.0f, -1.0f),
        Vector3Df(0.0f, 0.0f, 1.0f),
        Vector3Df(0.0f, 1.0f, 0.0f),
        Vector3Df(1.0f, 0.0f, 0.0f)
    };
    static constexpr std::array<std::array<int, 4>, 6> FaceIndices = {
        { { 0, 1, 2, 3 },
            { 0, 4, 1, 5 },
            { 0, 2, 4, 6 },
            { 1, 5, 3, 7 },
            { 2, 3, 6, 7 },
            { 4, 6, 5, 7 } }
    };
    static constexpr std::array<Vector3Df, 8> CubeVertices = {
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
