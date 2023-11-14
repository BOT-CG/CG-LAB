#pragma once

#include "Vector3D.h"

#include <cmath>
#include <numbers>

class SignedDistanceFunctions {
public:
    SignedDistanceFunctions() = default;
    ~SignedDistanceFunctions() = default;

    static float sphere(const Vector3Df& p, float radius, const Vector3Df& center)
    {
        return radius - p.distanceToPoint(center);
    }
    static float cylinder(const Vector3Df& p, float radius, float height, const Vector3Df& center, const Vector3Df& axis)
    {
        const Vector3Df v = p - center;
        const float dx = radius - v.distanceToLine(Vector3Df(0, 0, 0), axis);
        const float dz = height / 2.0f - v.projectionLength(axis);
        return -std::hypot(std::min(dx, 0.0f), std::min(dz, 0.0f)) + std::max(std::min(dx, dz), 0.0f);
    }
    static float roundedCylinder(const Vector3Df& p, float radius, float height, float roundingRadius, const Vector3Df& center, const Vector3Df& axis)
    {
        // const Vector3Df v = p - center;
        // const float dx = radius - roundingRadius - v.distanceToLine(Vector3Df(0, 0, 0), axis);
        // const float dz = height / 2.0f - roundingRadius - v.projectionLength(axis);
        // return -std::hypot(std::min(dx, 0.0f), std::min(dz, 0.0f)) + std::max(std::min(dx, dz), 0.0f) + roundingRadius;
        return cylinder(p, radius - roundingRadius, height - 2 * roundingRadius, center, axis) + roundingRadius;
    }
    static float cappedCone(const Vector3Df& p, float topRadius, float bottomRadius, float height, const Vector3Df& center, const Vector3Df& axis)
    {
        const Vector3Df v = p - center;
        const float x = v.distanceToLine(Vector3Df(0, 0, 0), axis);
        const float z = v.projection(axis);
        const Vector3Df qp1 = Vector3Df(std::max(x - (z > 0 ? topRadius : bottomRadius), 0.0f), std::abs(z) - height / 2.0f, 0);
        const Vector3Df ap = Vector3Df(x - topRadius, z - height / 2.0f, 0);
        const Vector3Df ab = Vector3Df(bottomRadius - topRadius, -height, 0);
        const Vector3Df qp2 = ap - ab * std::clamp(ap.dot(ab) / ab.dot(ab), 0.0f, 1.0f);
        const float s = (qp1.y < 0 && qp2.x < 0) ? 1.0f : -1.0f;
        return s * std::min(qp1.length(), qp2.length());
    }
    static float roundedCappedCone(const Vector3Df& p, float topRadius, float bottomRadius, float height, float roundingRadius, const Vector3Df& center, const Vector3Df& axis)
    {
        const float beta = std::atan((topRadius - bottomRadius) / height);
        return cappedCone(p, topRadius - roundingRadius * std::tan(std::numbers::pi_v<float> / 4.0f + beta / 2.0f), bottomRadius - roundingRadius * std::tan(std::numbers::pi_v<float> / 4.0f - beta / 2.0f), height - 2 * roundingRadius, center, axis) + roundingRadius;
    }
};
