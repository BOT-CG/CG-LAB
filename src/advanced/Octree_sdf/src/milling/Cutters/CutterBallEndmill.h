#pragma once

#include "Cutter.h"
#include "OBB3D.h"
#include "Vector3D.h"

class CutterBallEndmill : public Cutter {
public:
    explicit CutterBallEndmill(float _radius, float _height)
        : radius(_radius)
        , height(_height)
    {
    }
    ~CutterBallEndmill() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterBallEndmill>(*this);
    }
    OBB3Df getBBox(const Posture& currentPosture) const override;
    bool isInside(const Posture& currentPosture, const Vector3Df& p) const override;
    float signedDistance(const Posture& currentPosture, const Vector3Df& p) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

    float getMaxRadius() const override
    {
        return radius;
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;


private:
    const float radius;
    const float height;

    inline bool isBetweenEndFlat(const float z) const
    {
        return z >= 0 && z <= height;
    }
    inline bool isBetweenEndBall(const float z) const
    {
        return z >= 0 && z <= radius;
    }
    inline float distanceToBallCenter(const float x, const float z) const
    {
        return std::hypot(x, z - radius);
    }
    inline float signedDistanceToTopFlat(const float x, const float z) const
    {
        if (x <= radius) {
            return -(z - height);
        }

        return -std::hypot(x - radius, z - height);
    }
    inline float signedDistanceToSideSurface(const float x) const
    {
        return radius - x;
    }
    inline float signedDistanceToBallSurface(const float x, const float z) const
    {
        return radius - distanceToBallCenter(x, z);
    }

    void generateBottomBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
};
