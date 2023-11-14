#pragma once

#include "Cutter.h"

class CutterFlatEndmill : public Cutter {
public:
    explicit CutterFlatEndmill(float _diameter, float _height)
        : diameter(_diameter)
        , height(_height)
    {
    }
    ~CutterFlatEndmill() override = default;

    std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterFlatEndmill>(*this);
    }
    float getMaxRadius() const override
    {
        return diameter * 0.5f;
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
    const float height;
    constexpr inline float radius() const { return diameter / 2.0f; }
};
