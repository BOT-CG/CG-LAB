#pragma once

#include "Cutter.h"
#include "OBB3D.h"
#include "Vector3D.h"

class CutterFlatEndmill : public Cutter {
public:
    explicit CutterFlatEndmill(float _diameter, float _height)
        : diameter(_diameter)
        , height(_height)
    {
    }
    ~CutterFlatEndmill() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterFlatEndmill>(*this);
    }
    OBB3Df getBBox(const Posture& currentPosture) const override;
    bool isInside(const Posture& currentPosture, const Vector3Df& p) const override;
    float signedDistance(const Posture& currentPosture, const Vector3Df& p) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

    float getMaxRadius() const override
    {
        return diameter * 0.5f;
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;


private:
    const float diameter;
    const float height;

    constexpr inline float radius() const { return diameter / 2.0f; }
    inline bool isBetweenEndFlat(const float z) const
    {
        return z > 0 && z < height;
    }
    inline float signedDistanceToBothEndFlat(const float x, const float z) const
    {
        float d = z <= 0 ? z : height - z;
        if (x <= radius()) {
            return d;
        }

        return -(float)std::hypot(x - radius(), d);
    }
    inline float signedDistanceToSideSurface(const float x) const
    {
        return radius() - x;
    }
};
