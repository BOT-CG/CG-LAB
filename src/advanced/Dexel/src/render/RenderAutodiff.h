#pragma once

#include "Render.h"
#include "Vector3D.h"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <vector>

class RenderAutodiff : public Render {
public:
    RenderAutodiff() = default;
    ~RenderAutodiff() override = default;

    void initialize() override;
    void destroy();
    void draw(const std::vector<PointAndDistance>& pointDistance, const std::vector<Vector3Df>& normals, float toleranceGouge, float toleranceExcess, const QVector3D& colorGouge, const QVector3D& colorExcess);

private:
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLShaderProgram* program;

    void initializeProgram();

    static constexpr QVector3D colorLight = QVector3D(1.0f, 1.0f, 1.0f);
    static constexpr QVector3D colorNormal = QVector3D(0.0f, 0.737f, 0.813f);
    static constexpr QVector3D positionLight = QVector3D(1.0f, 2.0f, 10.0f);
};
