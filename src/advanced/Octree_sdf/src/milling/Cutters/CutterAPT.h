#pragma once

#include "Cutter.h"
#include "OBB3D.h"
#include "Vector3D.h"

#include <cmath>
#include <limits>

class CutterAPT : public Cutter {
public:
    explicit CutterAPT(float _d, float _r, float _e, float _f, float _alpha, float _beta, float _h)
        : d(_d)
        , r(_r)
        , e(_e)
        , f(_f)
        , alpha(_alpha)
        , beta(_beta)
        , h(_h)
        , R(initializeR())
        , R1(initializeR1())
        , R2(initializeR2())
        , Huc(initializeHuc())
        , Hlc(initializeHlc())
        , Ht(initializeHt())
    {
    }
    ~CutterAPT() override = default;

    inline std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterAPT>(*this);
    }
    OBB3Df getBBox(const Posture& currentPosture) const override;
    bool isInside(const Posture& currentPosture, const Vector3Df& p) const override;
    float signedDistance(const Posture& currentPosture, const Vector3Df& p) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

    float getMaxRadius() const override
    {
        return R;
    }
    float getMaxHeight() const override
    {
        return totalHeight();
    }
    float getDexelHeightZ(float distance) const override;

    bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) override;


private:
    const float d, r, e, f, alpha, beta, h;
    const float R, R1, R2, Huc, Hlc, Ht;

    inline float initializeR() const
    {
        return d / 2.0f + (h - d * 0.5f * std::tan(alpha)) * std::tan(beta);
    }
    inline float initializeR1() const
    {
        float u = 2 * std::pow(std::cos(alpha), 2) * (f * std::tan(alpha) + e);
        float sq = u * u - 4 * std::pow(std::cos(alpha), 2) * (f * f + e * e - r * r);
        sq = std::max(sq, 0.0f);
        return (u + std::sqrt(sq)) / 2.0f;
    }
    inline float initializeR2() const
    {
        if (beta == 0.0f) {
            return R;
        }
        float v = (R - e) / std::tan(beta) - (h - f);
        float sq = std::pow(v * std::sin(2 * beta), 2) - 4 * (v * v - r * r) * std::pow(std::sin(beta), 2);
        sq = std::max(sq, 0.0f);
        return e + (v * std::sin(2 * beta) + std::sqrt(sq)) / 2.0f;
    }
    inline float initializeHuc() const
    {
        if (beta == 0.0f) {
            return 0.0f;
        }

        return (R - R2) / std::tan(beta);
    }
    inline float initializeHlc() const
    {
        return R1 * std::tan(alpha);
    }
    inline float initializeHt() const
    {
        return h - Huc - Hlc;
    }

    constexpr inline float totalHeight() const { return h * 2; }

    inline float distanceToZAxisOnLowerConeSurface(const float z) const
    {
        if (Hlc == 0.0f || z < 0.0f) {
            return std::numeric_limits<float>::lowest();
        }

        return R1 * z / Hlc;
    }
    inline float distanceToLowerConeSurface(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToLineSegment(Vector3Df(0, 0, 0), Vector3Df(R1, 0, Hlc));
    }

    inline float distanceToZAxisOnTorusBallSurface(const float z) const
    {
        return e + std::sqrt(r * r - (z - f) * (z - f));
    }
    inline float distanceToCornerTorusBallCenter(const float x, const float z) const
    {
        return std::hypot(x - e, z - f);
    }
    inline float distanceToCornerTorusBallSurface(const float x, const float z) const
    {
        Vector3Df p(x, 0, z);
        Vector3Df c(e, 0, f);
        Vector3Df p1(R1, 0, Hlc);
        Vector3Df p2(R2, 0, Ht + Hlc);
        Vector3Df cp = p - c, cp1 = p1 - c, cp2 = p2 - c;
        float a = cp1.angleToLine(cp2), a1 = cp.angleToLine(cp1), a2 = cp.angleToLine(cp2);
        if (a1 <= a && a2 <= a) {
            return std::abs(r - distanceToCornerTorusBallCenter(x, z));
        }

        float d1 = p.distanceToPoint(p1), d2 = p.distanceToPoint(p2);
        return std::min(d1, d2);
    }

    inline float distanceToZAxisOnUpperConeSurface(const float z) const
    {
        return R2 + (R - R2) * (z - Hlc - Ht) / Huc;
    }
    inline float distanceToUpperConeSurface(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToLineSegment(Vector3Df(R2, 0, Hlc + Ht), Vector3Df(R, 0, h));
    }

    inline float distanceToCylinderSurface(const float x, const float z) const
    {
        return Vector3Df(x, 0, z).distanceToLineSegment(Vector3Df(R, 0, h), Vector3Df(R, 0, totalHeight()));
    }

    inline float signedDistanceToTopFlat(const float x, const float z) const
    {
        if (x <= R) {
            return -(z - totalHeight());
        }

        return -std::hypot(x - R, z - totalHeight());
    }
};
