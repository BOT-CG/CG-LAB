#pragma once

#include "BBox3D.h"
#include "OBB3D.h"
#include "SDFOctant.h"
#include "SDFOctantLeaf.h"
#include "Vector3D.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <execution>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <tuple>
#include <vector>

template <uint32_t MaxLevel>
class SDFOctantNode : public SDFOctant<MaxLevel> {
public:
    using SDFOctant<MaxLevel>::SDFOctant;

    SDFOctantNode() = default;
    virtual ~SDFOctantNode() = default;

    SDFOctantNode(const SDFOctantNode<MaxLevel>& other)
        : SDFOctant<MaxLevel>(other)
        , solidMask(other.solidMask)
        , leafMask(other.leafMask)
    {
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();
            if (other.children[i] != nullptr) {
                if (dynamic_cast<SDFOctantNode<MaxLevel>*>(other.children[i].get()) != nullptr) {
                    c = std::make_unique<SDFOctantNode<MaxLevel>>(*dynamic_cast<SDFOctantNode<MaxLevel>*>(other.children[i].get()));
                } else {
                    c = std::make_unique<SDFOctantLeaf<MaxLevel>>(*dynamic_cast<SDFOctantLeaf<MaxLevel>*>(other.children[i].get()));
                }
            }
        });
    }

    explicit SDFOctantNode(const SDFOctantLeaf<MaxLevel>& leaf)
        : SDFOctant<MaxLevel>(leaf.Level, leaf.id)
    {
        this->solidMask = 0b11111111;
        this->leafMask = 0b11111111;
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();
            auto cl = leaf.getSubOctantLeaf(i);
            // if (cl.checkAllPositive() || cl.checkAllNegative()) {
            //     std::lock_guard<std::mutex> lock(m);
            //     this->setEmptyLeaf(i);
            //     return;
            // }
            c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
            //使用叶子节点的距离场来初始化当前子节点的距离场
        });
    }


    void initialize(const AABB3Df& bbox) override
    {

        this->reset();
        this->solidMask = 0b11111111;
        this->leafMask = 0b11111111;
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();
            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));
            if (bbox.isInsideStrictly(cl.getBBoxGL())) {
                cl.initialize(bbox);
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                return;
            }
            if (!bbox.intersects(cl.getBBoxGL())) {
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                return;
            }
            cl.initialize(bbox);
            if (this->isChildAchieveMaxLevel() || cl.checkAllZero() || cl.checkSevenZero() || cl.checkSixZero() || cl.checkTolerance(bbox)) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                return;
            }
            {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
            }
            c = std::make_unique<SDFOctantNode<MaxLevel>>(this->Level + 1, this->getChildId(i));
            c->initialize(bbox);
        });
    }

    void initialize(const std::function<float(const Vector3Df&)>& sdf) override
    {
        this->reset();
        this->solidMask = 0b11111111;
        this->leafMask = 0b11111111;
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();
            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));
            cl.initialize(sdf);

            if (cl.checkAllNegative()) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                return;
            }

            if (this->isChildAchieveMaxLevel() || cl.checkAllPositive() || cl.checkTolerance(sdf)) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                return;
            }

            {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
            }
            c = std::make_unique<SDFOctantNode<MaxLevel>>(this->Level + 1, this->getChildId(i));
            c->initialize(sdf);
        });
    }

    void initialize(const std::array<std::array<std::vector<float>, (1 << MaxLevel) + 1>, (1 << MaxLevel) + 1>& gridIntersectionHeights) override
    {
        this->reset();
        this->solidMask = 0b11111111;
        this->leafMask = 0b11111111;
        std::mutex m;

        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();
            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));
            cl.initialize(gridIntersectionHeights);

            if (cl.checkAllNegative()) {
                if ((this->Level + 1) <= 5) {
                    {
                        std::lock_guard<std::mutex> lock(m);
                        this->setNonLeaf(i);
                    }
                    c = std::make_unique<SDFOctantNode<MaxLevel>>(this->Level + 1, this->getChildId(i));
                    c->initialize(gridIntersectionHeights);
                    return;
                }
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                return;
            }

            if (this->isChildAchieveMaxLevel() || cl.checkAllPositive()) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                return;
            }

            {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
            }
            c = std::make_unique<SDFOctantNode<MaxLevel>>(this->Level + 1, this->getChildId(i));
            c->initialize(gridIntersectionHeights);
        });
    }

    void initializeBlankDistance() override
    {
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            if (c != nullptr) {
                c->initializeBlankDistance();
            }
        });
    };

    void fillPoints(std::set<std::tuple<float, float, float>>& points) const override
    {
        std::for_each(this->children.begin(), this->children.end(), [&](auto& c) {
            if (c != nullptr) {
                c->fillPoints(points);
            }
        });
    }

    void initializeWithPointDistanceMap(const std::map<std::tuple<float, float, float>, float>& pointDistanceMap) override
    {
        std::for_each(std::execution::seq, this->children.begin(), this->children.end(), [&](auto& c) {
            if (c != nullptr) {
                c->initializeWithPointDistanceMap(pointDistanceMap);
            }
        });
    }

    void fillDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const override
    {
        std::for_each(this->children.begin(), this->children.end(), [&](auto& c) {
            if (c != nullptr) {
                c->fillDistances(centers, sizes, distances);
            }
        });
    }

    void generateMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override
    {
        std::for_each(this->children.begin(), this->children.end(), [&](auto& c) {
            if (c != nullptr) {
                c->generateMesh(centers, sizes, vertices, normals);
            }
        });
    }

    void subtract(const BBox3Df& bbox, const std::function<float(const Vector3Df&)>& sdf) override
    {
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();

            if (isEmptyLeaf(i)) {
                return;
            }

            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));

            if (!bbox.intersects(cl.getBBoxGL())) {
                return;
            }

            if (cl.isAllVertexInside(sdf)) {
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                c.reset();
                return;
            }

            if (isNonLeaf(i)) {
                c->subtract(bbox, sdf);
                return;
            }

            if (!this->isChildAchieveMaxLevel() && cl.isAllVertexOutside(sdf)) {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
                c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
                c->subtract(bbox, sdf);
                return;
            }

            if (c == nullptr) {
                this->setUninitialized(i);
                return;
            }

            auto* clp = dynamic_cast<SDFOctantLeaf<MaxLevel>*>(c.get());
            std::copy(clp->v.begin(), clp->v.end(), cl.v.begin());
            cl.updateDistance(sdf);
            if (this->isChildAchieveMaxLevel() || cl.checkAllZero() || cl.checkSevenZero() || cl.checkSixZero() || cl.checkTolerance(sdf)) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                std::lock_guard<std::mutex> lock(m);
                this->setSolidLeaf(i);
                return;
            }
            std::lock_guard<std::mutex> lock(m);
            this->setNonLeaf(i);
            c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
            c->subtract(bbox, sdf);
        });
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::function<float(const Vector3Df&)>& sdf) override
    {
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();

            if (isEmptyLeaf(i)) {
                return;
            }

            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));

            const auto clBBox = cl.getBBoxGL();
            if (std::none_of(std::execution::par, bboxs.begin(), bboxs.end(), [&](const auto& bbox) { return bbox.intersects(clBBox); })) {
                return;
            }

            if (cl.isAllVertexInside(sdf)) {
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                c.reset();
                return;
            }

            if (isNonLeaf(i)) {
                c->subtract(bboxs, sdf);
                return;
            }

            if (!this->isChildAchieveMaxLevel() && cl.isAllVertexOutside(sdf)) {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
                c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
                c->subtract(bboxs, sdf);
                return;
            }

            if (c == nullptr) {
                this->setUninitialized(i);
                return;
            }

            auto* clp = dynamic_cast<SDFOctantLeaf<MaxLevel>*>(c.get());
            std::copy(clp->v.begin(), clp->v.end(), cl.v.begin());
            cl.updateDistance(sdf);
            if (this->isChildAchieveMaxLevel() || cl.checkAllZero() || cl.checkSevenZero() || cl.checkSixZero() || cl.checkTolerance(sdf)) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                std::lock_guard<std::mutex> lock(m);
                this->setSolidLeaf(i);
                return;
            }
            std::lock_guard<std::mutex> lock(m);
            this->setNonLeaf(i);
            c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
            c->subtract(bboxs, sdf);
        });
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&)>>& sdfs) override
    {
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();

            if (isEmptyLeaf(i)) {
                return;
            }

            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));

            const auto clBBox = cl.getBBoxGL();
            if (std::none_of(std::execution::par, bboxs.begin(), bboxs.end(), [&](const auto& bbox) { return bbox.intersects(clBBox); })) {
                return;
            }

            if (cl.isAllVertexInside(sdfs)) {
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                c.reset();
                return;
            }

            if (isNonLeaf(i)) {
                c->subtract(bboxs, sdfs);
                return;
            }

            if (!this->isChildAchieveMaxLevel() && cl.isAllVertexOutside(sdfs)) {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
                c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
                c->subtract(bboxs, sdfs);
                return;
            }

            if (c == nullptr) {
                this->setUninitialized(i);
                return;
            }

            auto* clp = dynamic_cast<SDFOctantLeaf<MaxLevel>*>(c.get());
            std::copy(clp->v.begin(), clp->v.end(), cl.v.begin());
            cl.updateDistance(sdfs);
            if (this->isChildAchieveMaxLevel() || cl.checkAllZero() || cl.checkSevenZero() || cl.checkSixZero() || cl.checkTolerance(sdfs)) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                std::lock_guard<std::mutex> lock(m);
                this->setSolidLeaf(i);
                return;
            }
            std::lock_guard<std::mutex> lock(m);
            this->setNonLeaf(i);
            c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
            c->subtract(bboxs, sdfs);
        });
    }

    void subtract(const std::vector<OBB3Df>& bboxs, const std::vector<std::function<float(const Vector3Df&, Posture&)>>& sdfs) override
    {
        std::mutex m;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            size_t i = &c - this->children.data();

            if (isEmptyLeaf(i)) {
                return;
            }

            auto cl = SDFOctantLeaf<MaxLevel>(this->Level + 1, this->getChildId(i));

            const auto clBBox = cl.getBBoxGL();
            if (std::none_of(std::execution::par, bboxs.begin(), bboxs.end(), [&](const auto& bbox) { return bbox.intersects(clBBox); })) {
                return;
            }

            if (cl.isAllVertexInside(sdfs)) {
                std::lock_guard<std::mutex> lock(m);
                this->setEmptyLeaf(i);
                c.reset();
                return;
            }

            if (isNonLeaf(i)) {
                c->subtract(bboxs, sdfs);
                return;
            }

            if (!this->isChildAchieveMaxLevel() && cl.isAllVertexOutside(sdfs)) {
                std::lock_guard<std::mutex> lock(m);
                this->setNonLeaf(i);
                c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
                c->subtract(bboxs, sdfs);
                return;
            }

            if (c == nullptr) {
                this->setUninitialized(i);
                return;
            }

            auto* clp = dynamic_cast<SDFOctantLeaf<MaxLevel>*>(c.get());
            std::copy(clp->v.begin(), clp->v.end(), cl.v.begin());
            std::copy(clp->CubeCutterPosture.begin(), clp->CubeCutterPosture.end(), cl.CubeCutterPosture.begin());
            std::copy(clp->BlankDistance.begin(), clp->BlankDistance.end(), cl.BlankDistance.begin());
            cl.updateDistance(sdfs);

            if (this->isChildAchieveMaxLevel() || cl.checkAllZero() || cl.checkSevenZero() || cl.checkSixZero() || cl.checkTolerance(sdfs)) {
                c = std::make_unique<SDFOctantLeaf<MaxLevel>>(cl);
                std::lock_guard<std::mutex> lock(m);
                this->setSolidLeaf(i);
                return;
            }
            std::lock_guard<std::mutex> lock(m);
            this->setNonLeaf(i);
            c = std::make_unique<SDFOctantNode<MaxLevel>>(dynamic_cast<SDFOctantLeaf<MaxLevel>&>(*c));
            c->subtract(bboxs, sdfs);
        });
    }

    float signedDistance(const Vector3Df& p) const override
    {
        auto it = std::find_if(std::execution::par, this->children.begin(), this->children.end(), [&](const auto& c) {
            return c->getBBoxGL().isInside(p);
        });

        if (it != this->children.end()) {
            return (*it)->signedDistance(p);
        } else {
            return std::numeric_limits<float>::max();
        }
    }

    void reset()
    {
        for (auto& child : children) {
            child.reset();
        }
        this->solidMask = 0;
        this->leafMask = 0;
    }

    bool isSolidLeaf(size_t index) const
    {
        return solidMask.test(index) && leafMask.test(index);
    }
    void setSolidLeaf(size_t index)
    {
        solidMask.set(index);
        leafMask.set(index);
    }
    bool isNonLeaf(size_t index) const
    {
        return solidMask.test(index) && !leafMask.test(index);
    }
    void setNonLeaf(size_t index)
    {
        solidMask.set(index);
        leafMask.reset(index);
    }
    bool isEmptyLeaf(size_t index) const
    {
        return !solidMask.test(index) && leafMask.test(index);
    }
    void setEmptyLeaf(size_t index)
    {
        solidMask.reset(index);
        leafMask.set(index);
    }
    bool isUninitialized(size_t index) const
    {
        return !solidMask.test(index) && !leafMask.test(index);
    }
    void setUninitialized(size_t index)
    {
        solidMask.reset(index);
        leafMask.reset(index);
    }

    // MRR
    double getNodeVolume() override
    {
        return std::transform_reduce(std::execution::par, this->children.begin(), this->children.end(), 0.0,
            std::plus<>(),
            [&](auto& c) {
                size_t i = &c - this->children.data();
                if (isEmptyLeaf(i) || c == nullptr) {
                    return 0.0;
                }
                return c->getNodeVolume();
            });
    }

protected:
    std::bitset<8> solidMask = 0;
    std::bitset<8> leafMask = 0;
    std::array<std::unique_ptr<SDFOctant<MaxLevel>>, 8> children = { nullptr };
};
