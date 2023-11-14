#pragma once

#include "Cutter.h"

class CutterTaperFilletEndmill : public Cutter {
public:
    explicit CutterTaperFilletEndmill(float _diameter, float _filletRadius, float _e, float _f, float _height)
        : diameter(_diameter)
        , filletRadius(_filletRadius)
        , e(_e)
        , f(_f)
        , height(_height)
    {
    }
    ~CutterTaperFilletEndmill() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterTaperFilletEndmill>(*this);
    }
    OBB3Df getBBox(const Posture& currentPosture) const override;
    bool isInside(const Posture& currentPosture, const Vector3Df& p) const override;
    float signedDistance(const Posture& currentPosture, const Vector3Df& p) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override {};

    float getMaxRadius() const override
    {
        return diameter/ 2.0f;//记得修改
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;


private:
    const float diameter;
    const float filletRadius;
    const float e;
    const float f;
    const float height;

    constexpr inline float radius() const { return diameter / 2.0f; }
    inline float distanceToFilletBallCenter(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToPoint(Vector3Df(e, 0, f));
    }
    inline float signedDistanceToBottomFlat(const float x, const float z) const
    {
        if (x <= radius()) {
            return z;
        }

        return -std::hypot(x - radius(), z);
    }
    inline float signedDistanceToTopFlat(const float x, const float z) const
    {
        if (x <= radius()) {
            return -(z - height);
        }

        return -std::hypot(x - radius(), z - height);
    }
    inline float signedDistanceToSideSurface(const float x) const
    {
        return radius() - x;
    }
    inline float signedDistanceToFilletBallSurface(const float x, const float z) const
    {
        return filletRadius - distanceToFilletBallCenter(x, z);
    }
};
