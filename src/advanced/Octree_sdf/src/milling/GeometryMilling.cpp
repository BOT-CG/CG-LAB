#include "GeometryMilling.h"

#include "GLCompute.h"
#include "OBB3D.h"

#include <chrono>
#include <execution>
#include <numeric>
#include <sstream>

void GeometryMilling::initializeWorkpiece(float length, float width, float height, bool isOriginAtCenter, const Vector3Df& position)
{
    Vector3Df offset = isOriginAtCenter ? Vector3Df(0, 0, 0) : Vector3Df(length / 2.0f, width / 2.0f, height / 2.0f);
    workpiece = std::make_unique<Workpiece>(length, width, height, offset);
    ncProgram.setWorkpieceOffset(offset + position);
    workpieceMaxEdge = workpiece->getMaxEdge();
    workpieceOffset = workpiece->getOffset();
    workpiecePosition = position;
    initialWorkpiece = std::make_unique<Workpiece>(*workpiece);
}

void GeometryMilling::initializeWorkpiece(const std::vector<Triangle3Df>& triangles)
{
    workpiece = std::make_unique<Workpiece>(triangles);
    ncProgram.setWorkpieceOffset(workpiece->getOffset());
    workpieceMaxEdge = workpiece->getMaxEdge();
    workpieceOffset = workpiece->getOffset();
    workpiecePosition = { 0, 0, 0 };
    initialWorkpiece = std::make_unique<Workpiece>(*workpiece);
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
    if (workpiece == nullptr) {
        return;
    }

    workpiece->saveToSTLFile(filename);
}

void GeometryMilling::loadNCProgram(const std::string& ncCode)
{
    ncProgram.load(ncCode);
}

bool GeometryMilling::executeNCProgram()
{
    auto startTimeExecute = std::chrono::high_resolution_clock::now();

    std::vector<CutterSweptVolume> cutterSweptVolumes;
    bool hasMoreLines = ncProgram.execute(cutterSweptVolumes);

    auto endTimeExecute = std::chrono::high_resolution_clock::now();
    fout << "Execute time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTimeExecute - startTimeExecute).count() << " ms" << std::endl;

    if (!cutterSweptVolumes.empty()) {
        auto startTimeSubtract = std::chrono::high_resolution_clock::now();
        std::vector<OBB3Df> bboxs;

        // std::vector<std::function<float(const Vector3Df&)>> sdfs;
        // for (const auto& cutterSweptVolume : cutterSweptVolumes) {
        //     auto bs = cutterSweptVolume.getBBoxs();
        //     bboxs.insert(bboxs.end(), bs.begin(), bs.end());
        //     sdfs.emplace_back([&](const Vector3Df& p) {
        //         return cutterSweptVolume.signedDistance(p);
        //     });
        // }
        // workpiece->subtract(bboxs, sdfs);

        std::vector<std::function<float(const Vector3Df&, Posture&)>> newsdfs;
        for (const auto& cutterSweptVolume : cutterSweptVolumes) {
            auto bs = cutterSweptVolume.getBBoxs();
            bboxs.insert(bboxs.end(), bs.begin(), bs.end());
            newsdfs.emplace_back([&](const Vector3Df& p, Posture& posture) {
                return cutterSweptVolume.signedDistanceWithCutterPostion(p, posture);
            });
        }
        workpiece->subtract(bboxs, newsdfs);
        auto endTimeSubtract = std::chrono::high_resolution_clock::now();
        fout << getCurrentLine() << "# Subtract time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTimeSubtract - startTimeSubtract).count() << " ms" << std::endl;

        if (recordMRR) {
            double volume = this->calculateWorkpieceVolume();
            double VolumeDelta = this->getWorkpieceVolume() - volume;
            this->setWorkpieceVolume(volume);

            // std::cout << "WorkpieceVolume: " << this->getWorkpieceVolume() << std::endl;
            // std::cout << "VolumeDelta: " << VolumeDelta << std::endl;
            // double MRR = VolumeDelta / Time;

            // std::cout << "currentMillingTimeCost: " << ncProgram.getCurrentMillingTimeCost() << std::endl;

            double MRR = VolumeDelta / ncProgram.getCurrentMillingTimeCost();
            // std::cout << "MRR: " << MRR << std::endl;
            materialRemovalRate = std::max(0.0, MRR);
        }
    }

    return hasMoreLines;
}

void GeometryMilling::clear()
{
    workpiece.reset();
    initialWorkpiece.reset();
    ncProgram.clear();
}

void GeometryMilling::reset()
{
    workpiece = std::make_unique<Workpiece>(*initialWorkpiece);
    ncProgram.reset();
}

void GeometryMilling::generateWorkpieceMesh(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    if (workpiece == nullptr) {
        return;
    }

    workpiece->generateMesh(centers, sizes, vertices, normals);
}

void GeometryMilling::generateStaticWorkpieceMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals)
{
}

void GeometryMilling::fillStaticWorkpieceDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances)
{
}
void GeometryMilling::fillWorkpieceDistances(std::vector<Vector3Df>& centers, std::vector<float>& sizes, std::vector<float>& distances) const
{
    if (workpiece == nullptr) {
        return;
    }

    workpiece->fillDistances(centers, sizes, distances);
}

bool GeometryMilling::generateCurrentCutterMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    if (ncProgram.isCutterChanged()) {
        vertices.clear();
        normals.clear();
        ncProgram.generateCurrentCutterMesh(vertices, normals);
        std::transform(std::execution::par, vertices.begin(), vertices.end(), vertices.begin(), [&](const auto& p) {
            return (p - workpiecePosition) / halfWorkpieceMaxEdge();
        });
        return true;
    }

    return false;
}
