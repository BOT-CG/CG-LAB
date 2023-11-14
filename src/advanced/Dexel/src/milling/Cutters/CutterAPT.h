#pragma once

#include "Cutter.h"

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

    std::unique_ptr<Cutter> clone() const override
    {
        return std::make_unique<CutterAPT>(*this);
    }
    float getMaxRadius() const override
    {
        return R;
    }
    float getMaxHeight() const override
    {
        return totalHeight();
    }
    float getDexelHeightZ(float distance) const override;
    void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const override;

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
};
