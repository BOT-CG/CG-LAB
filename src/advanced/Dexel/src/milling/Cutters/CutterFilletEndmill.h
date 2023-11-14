#pragma once

#include "Cutter.h"

class CutterFilletEndmill : public Cutter {
public:
    explicit CutterFilletEndmill(float _diameter, float _filletRadius, float _height)
        : diameter(_diameter)
        , filletRadius(_filletRadius)
        , height(_height)
    {
    }
    ~CutterFilletEndmill() override = default;

    std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterFilletEndmill>(*this);
    }
    float getMaxRadius() const override
    {
        return radius();
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;
    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;

private:
    const float diameter;
    const float filletRadius;
    const float height;

    constexpr inline float radius() const { return diameter / 2.0f; }
    void generateFilletBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
};
