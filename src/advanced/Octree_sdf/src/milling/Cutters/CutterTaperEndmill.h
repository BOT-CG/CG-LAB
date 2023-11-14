#pragma once

#include "Cutter.h"
#include "OBB3D.h"
#include "Vector3D.h"

#include <cmath>

class CutterTaperEndmill : public Cutter {
public:
    explicit CutterTaperEndmill(float _diameter, float _beta, float _height)
        : bottomDiameter(_diameter)
        , topDiameter(_diameter + 2 * _height * std::tan(_beta))
        , beta(_beta)
        , height(_height)
    {
    }
    ~CutterTaperEndmill() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterTaperEndmill>(*this);
    }
    OBB3Df getBBox(const Posture& currentPosture) const override;
    bool isInside(const Posture& currentPosture, const Vector3Df& p) const override;
    float signedDistance(const Posture& currentPosture, const Vector3Df& p) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

    float getMaxRadius() const override
    {
        return topRadius();
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;


private:
    const float bottomDiameter;
    const float topDiameter;
    const float beta;
    const float height;

    constexpr inline float bottomRadius() const { return bottomDiameter / 2.0f; }
    constexpr inline float topRadius() const { return topDiameter / 2.0f; }
    inline bool isBetweenEndFlat(const float z) const
    {
        return z >= 0 && z <= height;
    }
    inline float distanceToZAxisOnSideSurface(const float z) const
    {
        return bottomRadius() + (topRadius() - bottomRadius()) * z / height;
    }
    inline float signedDistanceToBottomFlat(const float x, const float z) const
    {
        if (x <= distanceToZAxisOnSideSurface(z)) {
            return z;
        }

        return -std::hypot(x - bottomRadius(), z);
    }
    inline float signedDistanceToTopFlat(const float x, const float z) const
    {
        if (x <= topRadius()) {
            return -(z - height);
        }

        return -std::hypot(x - topRadius(), z - height);
    }
    inline float signedDistanceToSideSurface(const float x, const float z) const
    {
        float d = Vector3Df(x, 0, z).distanceToLineSegment(Vector3Df(bottomRadius(), 0, 0), Vector3Df(topRadius(), 0, height));
        return x <= distanceToZAxisOnSideSurface(z) ? d : -d;
    }
};
