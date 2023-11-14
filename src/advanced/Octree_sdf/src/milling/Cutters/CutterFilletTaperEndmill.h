#pragma once

#include "Cutter.h"
#include "OBB3D.h"
#include "Vector3D.h"

#include <cmath>

class CutterFilletTaperEndmill : public Cutter {
public:
    explicit CutterFilletTaperEndmill(float _diameter, float _filletRadius, float _beta, float _height)
        : diameter(_diameter)
        , bottomDiameter(_diameter - 2 * _filletRadius * (std::cos(_beta) - std::tan(_beta) * (1 - std::sin(_beta))))
        , middleDiameter(_diameter + 2 * _filletRadius * std::tan(_beta) * (1 - std::sin(_beta)))
        , topDiameter(_diameter + 2 * _height * std::tan(_beta))
        , filletRadius(_filletRadius)
        , beta(_beta)
        , height(_height)
    {
    }
    ~CutterFilletTaperEndmill() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterFilletTaperEndmill>(*this);
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
    const float diameter;
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
    inline float distanceToFilletBallCenter(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToPoint(Vector3Df(bottomRadius(), 0, filletRadius));
    }
    inline float distanceToFilletBallSurface(const float x, const float z) const
    {
        Vector3Df p(x, 0, z);
        Vector3Df c(bottomRadius(), 0, filletRadius);
        Vector3Df p1(bottomRadius(), 0, 0);
        Vector3Df p2(middleRadius(), 0, bottomHeight());
        Vector3Df cp = p - c, cp1 = p1 - c, cp2 = p2 - c;
        float a = cp1.angleToLine(cp2), a1 = cp.angleToLine(cp1), a2 = cp.angleToLine(cp2);
        if (a1 <= a && a2 <= a) {
            return std::abs(filletRadius - distanceToFilletBallCenter(x, z));
        }

        float d1 = p.distanceToPoint(p1), d2 = p.distanceToPoint(p2);
        return std::min(d1, d2);
    }

    inline float distanceToBottomFlat(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToLineSegment({ 0, 0, 0 }, { bottomRadius(), 0, 0 });
    }
    inline float signedDistanceToTopFlat(const float x, const float z) const
    {
        if (x <= topRadius()) {
            return -(z - height);
        }

        return -std::hypot(x - topRadius(), z - height);
    }

    inline float distanceToSideSurface(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToLineSegment(Vector3Df(middleRadius(), 0, bottomHeight()), Vector3Df(topRadius(), 0, height));
    }

    void generateFilletBallMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
};
