#pragma once

// #include "CutterSweptVolume.h"
#include "Vector3D.h"
#include <iostream>
#include <memory>

class Cutter {
public:
    Cutter() = default;
    virtual ~Cutter() = default;

    virtual std::unique_ptr<Cutter> clone() const = 0;
    virtual float getMaxRadius() const = 0;
    virtual float getMaxHeight() const = 0;
    virtual float getDexelHeightZ(float distance) const = 0;

    virtual void generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const = 0;

    virtual bool getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) = 0;

protected:
    static void generateBottomMesh(float radius, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments = 128);
    static void generateTopMesh(float radius, float height, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments = 128);
    static void generateSideMesh(float bottomRadius, float bottomHeight, float topHeight, float beta, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals, int numSegments = 128);
};
