#pragma once

#include "Vector3D.h"

#include <cmath>
#include <numbers>
#include <ostream>
#include <string>

#if __has_include(<format>)
#define USE_FORMAT
#include <format>
#else
#include <iomanip>
#include <sstream>
#endif

class Posture {
public:
    Posture() = default;
    explicit Posture(const Vector3Df& center, const Vector3Df& direction = { 0, 0, 1.0f })
        : center(center)
        , direction(direction)
    {
    }
    ~Posture() = default;

    Vector3Df center;
    Vector3Df direction = { 0, 0, 1.0f };

    bool equal(const Posture& other) const
    {
        return center.equal(other.center) && direction.equal(other.direction);
    }
    std::string toString() const
    {
        std::string s;
#ifdef USE_FORMAT
        s = std::format("X{:.4f} Y{:.4f} Z{:.4f} I{:.4f} J{:.4f} K{:.4f}", center.x, center.y, center.z, direction.x, direction.y, direction.z);
#else
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << "X" << center.x << " Y" << center.y << " Z" << center.z << " I" << direction.x << " J" << direction.y << " K" << direction.z;
        s = oss.str();
#endif
        return s;
    }
};

class PostureLineInterpolator {
public:
    PostureLineInterpolator() = delete;
    explicit PostureLineInterpolator(const Posture& start, const Posture& end, float centerStep = 0, float directionStep = 0)
        : start(start)
        , end(end)
        , centerStep(centerStep)
        , directionStep(directionStep)
        , rotateAxis(start.direction.cross(end.direction))
        , distance(start.center.distanceToPoint(end.center))
        , angle(rotateAxis.equal({ 0, 0, 0 }) ? 0 : start.direction.angleToLine(end.direction))
        , tStep(initializeTStep())
    {
    }
    ~PostureLineInterpolator() = default;

    float getLength() const { return distance; }
    Posture getPosture(float t) const
    {
        t = std::clamp(t, 0.0f, 1.0f);
        const Vector3Df center = start.center * (1 - t) + end.center * t;
        if (std::isnan(angle) || floatEqual(angle, 0)) {
            return Posture(center, start.direction);
        }

        const Vector3Df direction = start.direction.rotate(rotateAxis, angle * t).normalize();
        return Posture(center, direction);
    }
    bool getPostureByStep(Posture& posture)
    {
        if (isStepEnd) {
            return true;
        }

        if (currentT >= 1.0f) {
            posture = end;
            isStepEnd = true;
            return false;
        }

        posture = getPosture(currentT);
        currentT += tStep;
        return false;
    }

private:
    const Posture start, end;
    const float centerStep, directionStep;

    const Vector3Df rotateAxis;
    const float distance, angle, tStep;

    float currentT = 0;
    bool isStepEnd = false;

    inline float initializeTStep() const
    {
        if (start.center.equal(end.center)) {
            return directionStep / angle;
        }
        if (std::isnan(angle) || floatEqual(angle, 0)) {
            return centerStep / distance;
        }
        return std::min(centerStep / distance, directionStep / angle);
    }
};

class PostureArcInterpolator {
public:
    enum class Plane {
        XY,
        ZX,
        YZ,
    };

    PostureArcInterpolator() = delete;
    explicit PostureArcInterpolator(const Posture& start, const Posture& end, Plane plane, float radius, bool isClockwise, float centerStep = 0, float directionStep = 0)
        : start(start)
        , end(end)
        , radius(radius)
        , isClockwise(isClockwise)
        , plane(plane)
        , centerStep(centerStep)
        , directionStep(directionStep)
        , centerOfCircle(Vector3Df::centerOfCircle(start.center, end.center, radius, isClockwise))
        , centerStart(start.center - centerOfCircle)
        , centerEnd(end.center - centerOfCircle)
        , rotateAxis(initializeRotateAxis())
        , angle(initializeAngle())
        , arcLength(angle * std::abs(radius))
        , tStep(centerStep / arcLength)
    {
    }
    ~PostureArcInterpolator() = default;

    float getLength() const { return arcLength; }
    Posture getPosture(float t) const
    {
        t = std::clamp(t, 0.0f, 1.0f);
        const Vector3Df center = centerOfCircle + centerStart.rotate(rotateAxis * (isClockwise ? -1.0f : 1.0f), angle * t);
        const Vector3Df direction = start.direction;
        return Posture(center, direction);
    }
    bool getPostureByStep(Posture& posture)
    {
        if (isStepEnd) {
            return true;
        }

        if (currentT >= 1.0f) {
            posture = end;
            isStepEnd = true;
            return false;
        }

        posture = getPosture(currentT);
        currentT += tStep;
        return false;
    }

private:
    const Plane plane;
    const Posture start, end;
    const float radius;
    const bool isClockwise;
    const float centerStep, directionStep;

    const Vector3Df centerOfCircle, centerStart, centerEnd, rotateAxis;
    const float angle, arcLength, tStep;

    float currentT = 0;
    bool isStepEnd = false;

    inline float initializeAngle() const
    {
        return radius > 0 ? centerStart.angleToLine(centerEnd) : 2 * std::numbers::pi_v<float> - centerStart.angleToLine(centerEnd);
    }
    inline Vector3Df initializeRotateAxis() const
    {
        switch (plane) {
        case Plane::XY:
            return { 0, 0, 1 };
        case Plane::ZX:
            return { 0, 1, 0 };
        case Plane::YZ:
            return { 1, 0, 0 };
        default:
            return { 0, 0, 1 };
        }
    }
};
