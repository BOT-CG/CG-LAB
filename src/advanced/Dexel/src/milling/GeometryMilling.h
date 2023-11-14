#pragma once

#include "Cutter.h"
#include "NCProgram.h"
#include "Triangle3D.h"
#include "Vector3D.h"
#include "Workpiece.h"

class GeometryMilling {
public:
    GeometryMilling() = default;
    ~GeometryMilling() = default;

    void initializeWorkpiece(float length, float width, float height, bool isOriginAtCenter = true, const Vector3Df& position = { 0, 0, 0 });
    void initializeWorkpiece(const std::vector<Triangle3Df>& triangles);

    void loadNCProgram(const std::string& ncCode);
    bool executeNCProgram();

    size_t getCurrentLine() const
    {
        return ncProgram.getCurrentLine();
    }
    Posture getCurrentPosture() const
    {
        return ncProgram.getCurrentPosture();
    }
    Posture getCurrentPostureGL() const
    {
        auto posture = ncProgram.getCurrentPosture();
        return Posture { transformTo11(posture.center), posture.direction };
    }
    std::vector<std::string> getNCCodeLines() const
    {
        return ncProgram.getNCCodeLines();
    }

    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h);

    inline double getMRR() const { return materialRemovalRate; }
    inline double getMillingTime() const { return ncProgram.getMillingTime(); }
    inline bool getRecordMRR() const { return recordMRR; }
    inline void setRecordMRR(bool value) { recordMRR = value; }

    void clear();
    void reset();
    void generateWorkpieceMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;
    void computeDifferenceBetweenDesignModelAndWorkpiece(const std::vector<Triangle3Df>& triangles, const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, std::vector<PointAndDistance>& pointDistance);
    void saveWorkpieceToSTLFile(const std::string& filename) const;

    double LastMillingTime = 0;
    bool generateCurrentCutterMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;

    void GetDexelStaticSurfaceVertices();

public:
    Workpiece workpiece;
    CutterParm cutterParm;

private:
    NCProgram ncProgram;
    // Workpiece workpiece;

    float workpieceMaxEdge;
    Vector3Df workpieceOffset = { 0, 0, 0 };
    Vector3Df workpiecePosition = { 0, 0, 0 };
    Workpiece initialWorkpiece;

    double materialRemovalRate;
    bool recordMRR = false;

    constexpr inline float halfWorkpieceMaxEdge() const { return workpieceMaxEdge / 2.0f; }
    constexpr inline Vector3Df coordinateLocalToGLWithoutOffset(const Vector3Df& p) const
    {
        return p / halfWorkpieceMaxEdge();
    }
    constexpr inline Vector3Df coordinateGLToLocalWithoutOffset(const Vector3Df& p) const
    {
        return p * halfWorkpieceMaxEdge();
    }
    constexpr inline Vector3Df transformTo11(const Vector3Df& p) const
    {
        return (p - workpieceOffset) / halfWorkpieceMaxEdge();
    }
    constexpr inline Vector3Df transformFrom11(const Vector3Df& p) const
    {
        return p * halfWorkpieceMaxEdge() + workpieceOffset;
    }
};
