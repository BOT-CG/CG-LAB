#pragma once

#include "Render.h"
#include "Vector3D.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <vector>

class RenderCWE : public Render {
public:
    RenderCWE() = default;
    ~RenderCWE() override = default;

    void initialize() override;
    void destroy();
    void drawWorkpieceWireframe(const std::vector<Vector3Df>& centers, const std::vector<float>& sizes);
    void drawWorkpieceSurfaceUsingDistance(const std::vector<Vector3Df>& centers, const std::vector<float>& sizes, const std::vector<float>& distances);
    void drawWorkpieceSurface(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals);
    void initializeCutter(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals);
    void drawCutter(const Vector3Df& position, const Vector3Df& direction);
    void clearCutter();

private:
    QOpenGLVertexArrayObject vaoWorkpiece;
    QOpenGLVertexArrayObject vaoCutter;

    QOpenGLBuffer vboWorkpieceSurface;
    QOpenGLBuffer vboWorkpieceSurfaceUsingDistance;
    QOpenGLBuffer vboWorkpieceWireframe;
    QOpenGLBuffer vboTriangleTable;
    QOpenGLBuffer vboCutter;

    QOpenGLShaderProgram* programWorkpieceSurface;
    QOpenGLShaderProgram* programWorkpieceSurfaceUsingDistance;
    QOpenGLShaderProgram* programWorkpieceWireframe;
    QOpenGLShaderProgram* programCutter;

    bool isCutterInitialized = false;

    void initializeProgramWorkpieceSurface();
    void initializeProgramWorkpieceSurfaceUsingDistance();
    void initializeProgramWorkpieceWireframe();
    void initializeProgramCutter();

    static constexpr QVector3D colorLight = QVector3D(1.0f, 1.0f, 1.0f);
    static constexpr QVector3D colorWorkpieceSurface = QVector3D(0.0f, 0.737f, 0.813f);
    static constexpr QVector3D colorWorkpieceWireframe = QVector3D(0.914f, 0.118f, 0.388f);
    static constexpr QVector3D colorCutter = QVector3D(0.988f, 0.788f, 0.0f);
    static constexpr QVector3D positionLight = QVector3D(1.0f, 2.0f, 10.0f);
};
