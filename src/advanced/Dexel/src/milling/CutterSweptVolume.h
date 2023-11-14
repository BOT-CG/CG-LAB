#pragma once

#include "Cutter.h"
// #include "CutterBallEndmill.h"
// #include "CutterSweptVolume.h"
#include "Posture.h"
#include "Vector3D.h"
#include <memory>

class DexelCutterSweptVolume {
public:
    DexelCutterSweptVolume() = default;
    DexelCutterSweptVolume(const DexelCutterSweptVolume& other)
        : //dexelGrid(other.dexelGrid),
        // type(other.type)
        offset(other.offset)
        // , scale(other.scale)
        , startWithOffset(other.startWithOffset)
        , endWithOffset(other.endWithOffset)
        , plane(other.plane)
        , radius(other.radius)
        // , isClockwise(other.isClockwise)
        , cutter(other.cutter->clone())
        , sweptVolmueParam(other.sweptVolmueParam)
    // , Min_X(other.sweptVolmueParam.Min_X)
    // , Min_Y(other.sweptVolmueParam.Min_Y)
    // , Max_X(other.sweptVolmueParam.Max_X)
    // , Max_Y(other.sweptVolmueParam.Max_Y)
    // , StartDexel(other.sweptVolmueParam.StartDexel)
    // , EndDexel(other.sweptVolmueParam.EndDexel)
    // , Direction(other.sweptVolmueParam.Direction)
    // , Normal(other.sweptVolmueParam.Normal)
    // , Center(other.sweptVolmueParam.Center)
    // , CutterRadiusDexel(other.sweptVolmueParam.CutterRadiusDexel)
    // , CutterHeightDexel(other.sweptVolmueParam.CutterHeightDexel)
    // , RadiusDexel(other.sweptVolmueParam.RadiusDexel)
    {
    }
    ~DexelCutterSweptVolume() = default;

    using Plane = PostureArcInterpolator::Plane;

    float getLength() const;
    void setOffset(const Vector3Df& offset);
    void setScale(float scale);
    void setCutter(std::unique_ptr<Cutter> cutter);
    void setScatter(const Posture& start);
    void setLine(const Posture& start, const Posture& end);
    void setArc(const Posture& start, const Posture& end, Plane plane, float radius, bool isClockwise);

    void setLineRange(const Posture& start, const Posture& end);
    void setScatterRange(const Posture& start);
    void setArcRange(const Posture& start, const Posture& end, Plane plane, float radius, bool isClockwise);
    bool GetCutterDexelRange(float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y);

    DexelCutterSweptVolume& operator=(const DexelCutterSweptVolume& other);

    // DexelGrid<1000, 1000> dexelGrid;

    // float Min_X = 10000, Min_Y = 10000, Max_X = -10000, Max_Y = -10000;
    // Vector3Df StartDexel, EndDexel, Direction, Normal, Center;
    // float CutterRadiusDexel, CutterHeightDexel, RadiusDexel;

public:
    Vector3Df offset = { 0, 0, 0 };
    // float scale = 1.0f;
    Posture startWithOffset, endWithOffset;
    Plane plane = Plane::XY;
    float radius;
    // bool isClockwise;
    std::unique_ptr<Cutter> cutter;
    SweptVolmueParam sweptVolmueParam;
    // Cutter* cutter;
};
