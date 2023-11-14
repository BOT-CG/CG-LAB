#pragma once

#include "Cutter.h"

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

    std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterTaperEndmill>(*this);
    }
    float getMaxRadius() const override
    {
        return topRadius();
    }
    float getMaxHeight() const override
    {
        return height;
    }
    float getDexelHeightZ(float distance) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;

private:
    const float bottomDiameter;
    const float topDiameter;
    const float beta;
    const float height;

    constexpr inline float bottomRadius() const { return bottomDiameter / 2.0f; }
    constexpr inline float topRadius() const { return topDiameter / 2.0f; }
};
