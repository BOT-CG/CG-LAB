#pragma once

#include "Cutter.h"
#include "utils.h"

#include <memory>

class CutterFactory {
public:
    CutterFactory() = default;
    ~CutterFactory() = default;

    enum class Type {
        APT,
        BallEndmill,
        FilletEndmill,
        FilletTaperEndmill,
        FlatEndmill,
        TaperEndmill,
        TaperFilletEndmill,
        Strange,
    };

    static Type getType(float d, float r, float e, float f, float alpha, float beta, float h);
    static std::unique_ptr<Cutter> create(float d, float r, float e, float f, float alpha, float beta, float h);

    inline static bool isFlatEndmill(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return r == 0.0f && e == 0.0f && f == 0.0f && alpha == 0.0f && beta == 0.0f;
    }
    inline static bool isFilletEndmill(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return floatEqual(e, d / 2.0f - r) && floatEqual(f, r) && alpha == 0.0f && beta == 0.0f;
    }
    inline static bool isFilletTaperEndmill(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return floatEqual(e, d / 2.0f - r) && floatEqual(f, r) && alpha == 0.0f && beta > 0.0f;
    }
    inline static bool isBallEndmill(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return floatEqual(d, 2.0f * r) && floatEqual(f, r) && e == 0.0f && alpha == 0.0f && beta == 0.0f;
    }
    inline static bool isTaperEndmill(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return r == 0.0f && e == 0.0f && f == 0.0f && alpha == 0.0f && beta > 0.0f;
    }
    inline static bool isTaperFilletEndmill(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return e < 0 && f < r && r > -e + d / 2.0f && alpha == 0.0f && beta == 0.0f;
    }
    inline static bool isStrange(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        return r == 0.0f && e == 0.0f && f == 0.0f && alpha < 0.0f && beta < 0.0f;
    }
};
