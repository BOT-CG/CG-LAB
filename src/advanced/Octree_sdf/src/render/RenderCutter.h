#pragma once

#include "Render.h"
#include "Vector3D.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <vector>

class RenderCutter : public Render {
public:
    RenderCutter() = default;
    ~RenderCutter() override = default;

    void initialize() override;
    void destroy();
    void setCutter(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals);
    void drawCutter();
    void clearCutter();

private:
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLShaderProgram* program;

    bool isInitialized = false;

    void initializeProgram();

    static constexpr QVector3D colorLight = QVector3D(1.0f, 1.0f, 1.0f);
    static constexpr QVector3D colorCutter = QVector3D(0.988f, 0.788f, 0.0f);
    static constexpr QVector3D positionLight = QVector3D(100.0f, 200.0f, 1000.0f);
};
