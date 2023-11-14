#include "GeometryMilling.h"

#include "GLCompute.h"

#include <fstream>

void GeometryMilling::initializeWorkpiece(float length, float width, float height, bool isOriginAtCenter, const Vector3Df& position)
{
    Vector3Df offset = isOriginAtCenter ? -Vector3Df(length / 2.0f, width / 2.0f, height / 2.0f) : Vector3Df(0, 0, 0);
    workpiece = Workpiece(length, width, height, position);

    cutterParm.BoxSize = Vector3Df(length, width, height);
    std::cout << "BoxSize: " << cutterParm.BoxSize << std::endl;
    // ncProgram.setWorkpieceOffset(offset + workpiece.getPosition());
    ncProgram.setWorkpieceOffset(Vector3Df(0.0, 0.0, 0.0));
    ncProgram.setWorkpieceScale(workpiece.getScale());
    workpieceMaxEdge = std::max({ length, width, height });

    workpieceOffset = offset;
    workpiecePosition = position;
    initialWorkpiece = workpiece;
}

void GeometryMilling::initializeWorkpiece(const std::vector<Triangle3Df>& triangles)
{
    // TODO: implement this

    workpiece.mrrDexel.InitializeDexelArray(triangles);
    cutterParm.BoxSize = workpiece.mrrDexel.BoxSize;
    workpiece.mrrDexel.InitDexelTriangle_RenderType();
}

void GeometryMilling::loadNCProgram(const std::string& ncCode)
{
    ncProgram.load(ncCode);
}

bool GeometryMilling::executeNCProgram()
{
    std::vector<DexelCutterSweptVolume> cutterSweptVolumes;
    cutterSweptVolumes[0].cutter;
    bool hasMoreLines = ncProgram.execute(cutterSweptVolumes);
    // std::cout << "subtract_begin" << std::endl;
    if (!cutterSweptVolumes.empty()) {
        workpiece.subtract(cutterSweptVolumes);
        workpiece.mrrDexel.cutter = cutterSweptVolumes[0].cutter.release();
    }
    // std::cout << "subtract_End" << std::endl;
    return hasMoreLines;
}

void GeometryMilling::clear()
{
    workpiece = {};
    initialWorkpiece = {};
    ncProgram.clear();
}

void GeometryMilling::reset()
{
    workpiece = initialWorkpiece;
    ncProgram.reset();
}

void GeometryMilling::addCutter(float d, float r, float e, float f, float alpha, float beta, float h)
{
    ncProgram.addCutter(d, r, e, f, alpha, beta, h);

    cutterParm = CutterParm(unsigned int(CutterFactory::getType(d, r, e, f, alpha, beta, h)), d, r, e, f, alpha, beta, h, cutterParm.BoxSize);
}

void GeometryMilling::generateWorkpieceMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    workpiece.generateMesh(vertices, normals);
}

void GeometryMilling::computeDifferenceBetweenDesignModelAndWorkpiece(const std::vector<Triangle3Df>& triangles, const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, std::vector<PointAndDistance>& pointDistance)
{
    if (triangles.empty()) {
        return;
    }

    std::vector<float> triangleVertices;
    std::vector<float> triangleNormals;
    for (const auto& triangle : triangles) {
        for (int i = 0; i < 3; ++i) {
            triangleVertices.push_back(triangle.Vertices[i].x);
            triangleVertices.push_back(triangle.Vertices[i].y);
            triangleVertices.push_back(triangle.Vertices[i].z);
        }
        triangleNormals.push_back(triangle.Normal.x);
        triangleNormals.push_back(triangle.Normal.y);
        triangleNormals.push_back(triangle.Normal.z);
    }

    pointDistance.resize(vertices.size());
    std::transform(std::execution::par, vertices.begin(), vertices.end(), pointDistance.begin(), [&](const auto& p) {
        return PointAndDistance { transformFrom11(p), 0.0f };
    });

    GLCompute::computeSTL2SDF(triangleVertices, triangleNormals, pointDistance);

    std::transform(std::execution::par, pointDistance.begin(), pointDistance.end(), pointDistance.begin(), [&](const auto& p) {
        return PointAndDistance { transformTo11(p.point), p.distance };
    });
}

void GeometryMilling::saveWorkpieceToSTLFile(const std::string& filename) const
{
    // TODO: implement this
    // workpiece.saveToSTLFile(filename);
}

bool GeometryMilling::generateCurrentCutterMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    if (ncProgram.isCutterChanged()) {
        vertices.clear();
        normals.clear();
        ncProgram.generateCurrentCutterMesh(vertices, normals);
        // std::transform(std::execution::par, vertices.begin(), vertices.end(), vertices.begin(), [&](const auto& p) {
        //     return (p - workpiecePosition);
        // });
        // std::cout << "workpiecePosition: " << workpiecePosition << std::endl;
        return true;
    }

    return false;
}

void GeometryMilling::GetDexelStaticSurfaceVertices()
{
    workpiece.mrrDexel.GetPatchVertices();
    // workpiece.mrrDexel.FillStaticRenderList();
}
