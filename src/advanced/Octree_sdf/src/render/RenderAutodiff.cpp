#include "RenderAutodiff.h"

#include <iostream>

void RenderAutodiff::initialize()
{
    Render::initialize();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

    vao.create();
    vao.bind();

    vbo.create();
    vbo.bind();
    initializeProgram();
    vbo.release();

    vao.release();
}

void RenderAutodiff::destroy()
{
    delete program;
    vbo.destroy();
    vao.destroy();
}

void RenderAutodiff::draw(const std::vector<PointAndDistance>& pointDistance, const std::vector<Vector3Df>& normals, float toleranceGouge, float toleranceExcess, const QVector3D& colorGouge, const QVector3D& colorExcess)
{
    if (pointDistance.size() != normals.size()) {
        throw std::runtime_error("RenderAutodiff::draw: pointDistance.size() != normals.size()");
    }

    int sizeofPointDistance = static_cast<int>(pointDistance.size() * sizeof(PointAndDistance));
    int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));

    vao.bind();

    program->bind();

    vbo.bind();
    vbo.allocate(sizeofPointDistance + sizeofNormals);
    vbo.write(0, pointDistance.data(), sizeofPointDistance);
    vbo.write(sizeofPointDistance, normals.data(), sizeofNormals);

    program->setUniformValue("model", getMatModel());
    program->setUniformValue("view", getMatView());
    program->setUniformValue("projection", getMatProjection());
    program->setUniformValue("normalMatrix", getMatNormalMatrix());
    program->setUniformValue("toleranceGouge", toleranceGouge);
    program->setUniformValue("toleranceExcess", toleranceExcess);
    program->setUniformValue("colorGouge", colorGouge);
    program->setUniformValue("colorExcess", colorExcess);

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 4);

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, sizeofPointDistance, 3);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(pointDistance.size()));

    program->release();

    vbo.release();

    vao.release();
}

void RenderAutodiff::initializeProgram()
{
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/Autodiff.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/Autodiff.frag");
    program->link();
    program->bind();
    program->setUniformValue("model", getMatModel());
    program->setUniformValue("view", getMatView());
    program->setUniformValue("projection", getMatProjection());
    program->setUniformValue("normalMatrix", getMatNormalMatrix());
    program->setUniformValue("colorLight", colorLight);
    program->setUniformValue("colorNormal", colorNormal);
    program->setUniformValue("positionLight", positionLight);
    program->setUniformValue("positionView", getCameraPosition());
    program->release();
}
