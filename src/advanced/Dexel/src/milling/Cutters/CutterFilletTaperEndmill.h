#pragma once

#include "Cutter.h"

#include <cmath>

class CutterFilletTaperEndmill : public Cutter {
public:
    explicit CutterFilletTaperEndmill(float _diameter, float _filletRadius, float _beta, float _height)
        : bottomDiameter(_diameter - 2 * _filletRadius * (std::cos(_beta) - std::tan(_beta) * (1 - std::sin(_beta))))
        , middleDiameter(_diameter + 2 * _filletRadius * std::tan(_beta) * (1 - std::sin(_beta)))
        , topDiameter(_diameter + 2 * _height * std::tan(_beta))
        , filletRadius(_filletRadius)
        , beta(_beta)
        , height(_height)
    {
    }
    ~CutterFilletTaperEndmill() override = default;

    std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterFilletTaperEndmill>(*this);
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
    const float middleDiameter;
    const float topDiameter;
    const float filletRadius;
    const float beta;
    const float height;

    constexpr inline float bottomRadius() const { return bottomDiameter / 2.0f; }
    constexpr inline float middleRadius() const { return middleDiameter / 2.0f; }
    constexpr inline float topRadius() const { return topDiameter / 2.0f; }
    inline float bottomHeight() const { return filletRadius * (1 - std::sin(beta)); }
    inline float topHeight() const { return height - bottomHeight(); }
    void generateFilletBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
};
