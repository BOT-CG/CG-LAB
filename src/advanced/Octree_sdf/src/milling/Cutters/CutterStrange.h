#pragma once

#include "Cutter.h"
#include "OBB3D.h"
#include "Vector3D.h"

#include <functional>

class CutterStrange : public Cutter {
public:
    explicit CutterStrange(float _height, std::function<float(float, float)> _f = defaultF);
    ~CutterStrange() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterStrange>(*this);
    }
    OBB3Df getBBox(const Posture& currentPosture) const override;
    bool isInside(const Posture& currentPosture, const Vector3Df& p) const override;
    float signedDistance(const Posture& currentPosture, const Vector3Df& p) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override {};


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
    const float height;
    const float radius;
    const std::function<float(float, float)> f;

    static std::function<float(float, float)> defaultF;
};
