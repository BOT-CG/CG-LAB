#pragma once

#include "AABB3D.h"
#include "Cutter.h"
#include "CutterFactory.h"
#include "NCProgram.h"
#include "Triangle3D.h"
#include "Vector3D.h"
#include "Workpiece.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <vector>

class GeometryMilling {
public:
    GeometryMilling() = default;
    virtual ~GeometryMilling() = default;

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

    inline void addCutter(float d, float r, float e, float f, float alpha, float beta, float h)
    {
        ncProgram.addCutter(d, r, e, f, alpha, beta, h);
    }

    void clear();
    void reset();
    void generateWorkpieceMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;

    void generateStaticWorkpieceMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);
    void fillStaticWorkpieceDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances);
    void fillWorkpieceDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const;
    void computeDifferenceBetweenDesignModelAndWorkpiece(const std::vector<Triangle3Df>& triangles, const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, std::vector<PointAndDistance>& pointDistance);
    void saveWorkpieceToSTLFile(const std::string& filename) const;

    double LastMillingTime = 0;
    bool generateCurrentCutterMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const;

    Cutter* getCurrentCutter()
    {
        return ncProgram.getCurrenrCutter();
    };

private:
    std::unique_ptr<Workpiece> workpiece;
    NCProgram ncProgram;

    float workpieceMaxEdge;
    Vector3Df workpieceOffset = { 0, 0, 0 };
    Vector3Df workpiecePosition = { 0, 0, 0 };
    std::unique_ptr<Workpiece> initialWorkpiece;
    std::fstream fout = std::fstream("subtract_time.txt", std::ios::out);

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

    double calculateWorkpieceVolume()
    {
        return workpiece->calculateWorkpieceVolume();
    }

    double getWorkpieceVolume()
    {
        return workpiece->getWorkpieceVolume();
    }
    void setWorkpieceVolume(double volume)
    {
        workpiece->setWorkpieceVolume(volume);
    }
};
