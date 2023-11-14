#pragma once

#include "OBB3D.h"
#include "Posture.h"
#include "Vector3D.h"
#include "utils.h"

#include <memory>
#include <string>
#include <vector>

class Cutter {
public:
    Cutter() = default;
    virtual ~Cutter() = default;

    virtual std::unique_ptr<Cutter> clone() const = 0;
    virtual OBB3Df getBBox(const Posture& currentPosture) const = 0;
    virtual bool isInside(const Posture& currentPosture, const Vector3Df& p) const = 0;
    virtual float signedDistance(const Posture& currentPosture, const Vector3Df& p) const = 0;
    virtual void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const = 0;

    virtual float getMaxRadius() const = 0;
    virtual float getMaxHeight() const = 0;
    virtual float getDexelHeightZ(float distance) const = 0;
    virtual bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) = 0;

protected:
    static inline OBB3Df createBBox(const Posture& currentPosture, float radius, float height)
    {
        Vector3Df c = currentPosture.center + currentPosture.direction.normalize() * height / 2.0f;
        Vector3Df axisZ = currentPosture.direction.normalize() * height / 2.0f;
        Vector3Df axisX = Vector3Df(axisZ.z, axisZ.z, -axisZ.x - axisZ.y).normalize() * radius;
        Vector3Df axisY = axisZ.cross(axisX).normalize() * radius;
        return { c, axisX, axisY, axisZ };
    }
    static inline Vector3Df convertToLocalCoordinate(const Posture& currentPosture, const Vector3Df& p)
    {
        return p - currentPosture.center;
    }
    static inline float projectToZAxisLength(const Posture& currentPosture, const Vector3Df& p)
    {
        return convertToLocalCoordinate(currentPosture, p).projection(currentPosture.direction.normalize());
    }
    static inline float distanceToZAxis(const Posture& currentPosture, const Vector3Df& p)
    {
        return convertToLocalCoordinate(currentPosture, p).distanceToLine(Vector3Df(0, 0, 0), currentPosture.direction.normalize());
    }

    static void generateBottomMesh(float radius, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments = 128);
    static void generateTopMesh(float radius, float height, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments = 128);
    static void generateSideMesh(float bottomRadius, float bottomHeight, float topHeight, float beta, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments = 128);
};
