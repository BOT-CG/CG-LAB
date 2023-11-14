#pragma once

#include "Cutter.h"
#include "Vector3D.h"

class CutterBallEndmill : public Cutter {
public:
    explicit CutterBallEndmill(float _radius, float _height)
        : radius(_radius)
        , height(_height)
    {
    }
    ~CutterBallEndmill() override = default;

    std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterBallEndmill>(*this);
    }
    float getMaxRadius() const override
    {
        return radius;
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;

private:
    const float radius;
    const float height;
    void generateBottomBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
};
