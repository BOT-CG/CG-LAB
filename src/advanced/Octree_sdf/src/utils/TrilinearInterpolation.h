#pragma once

#include "Vector3D.h"

#include <cmath>

class TrilinearInterpolation {
public:
    TrilinearInterpolation() = default;
    ~TrilinearInterpolation() = default;

    static float L(const float v000, const float v001, const float v010, const float v011,
        const float v100, const float v101, const float v110, const float v111,
        const float fx, const float fy, const float fz)
    {
        const float v0 = std::lerp(v000, v100, fx);
        const float v1 = std::lerp(v010, v110, fx);
        const float v2 = std::lerp(v001, v101, fx);
        const float v3 = std::lerp(v011, v111, fx);
        const float v4 = std::lerp(v0, v1, fy);
        const float v5 = std::lerp(v2, v3, fy);
        return std::lerp(v4, v5, fz);
    }
    static float L(const float* v, const float fx, const float fy, const float fz)
    {
        return L(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], fx, fy, fz);
    }
    static float X(const float v000, const float v001, const float v010, const float v011,
        const float v100, const float v101, const float v110, const float v111,
        const float fy, const float fz, const float l)
    {
        const float v0 = std::lerp(v000, v010, fy);
        const float v1 = std::lerp(v001, v011, fy);
        const float v2 = std::lerp(v100, v110, fy);
        const float v3 = std::lerp(v101, v111, fy);
        const float v4 = std::lerp(v0, v1, fz);
        const float v5 = std::lerp(v2, v3, fz);
        return (l - v4) / (v5 - v4);
    }
    static float X(const float* v, const float fy, const float fz, const float l)
    {
        return X(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], fy, fz, l);
    }
    static float Y(const float v000, const float v001, const float v010, const float v011,
        const float v100, const float v101, const float v110, const float v111,
        const float fx, const float fz, const float l)
    {
        const float v0 = std::lerp(v000, v100, fx);
        const float v1 = std::lerp(v001, v101, fx);
        const float v2 = std::lerp(v010, v110, fx);
        const float v3 = std::lerp(v011, v111, fx);
        const float v4 = std::lerp(v0, v1, fz);
        const float v5 = std::lerp(v2, v3, fz);
        return (l - v4) / (v5 - v4);
    }
    static float Y(const float* v, const float fx, const float fz, const float l)
    {
        return Y(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], fx, fz, l);
    }
    static float Z(const float v000, const float v001, const float v010, const float v011,
        const float v100, const float v101, const float v110, const float v111,
        const float fx, const float fy, const float l)
    {
        const float v0 = std::lerp(v000, v100, fx);
        const float v1 = std::lerp(v010, v110, fx);
        const float v2 = std::lerp(v001, v101, fx);
        const float v3 = std::lerp(v011, v111, fx);
        const float v4 = std::lerp(v0, v1, fy);
        const float v5 = std::lerp(v2, v3, fy);
        return (l - v4) / (v5 - v4);
    }
    static float Z(const float* v, const float fx, const float fy, const float l)
    {
        return Z(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], fx, fy, l);
    }
};
