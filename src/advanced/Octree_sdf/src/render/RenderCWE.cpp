#include "RenderCWE.h"

#include "MarchingCubes.h"

#include <cmath>

void RenderCWE::initialize()
{
    Render::initialize();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    // glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    vaoWorkpiece.create();
    vaoWorkpiece.bind();

    vboWorkpieceSurface.create();
    vboWorkpieceSurface.bind();
    initializeProgramWorkpieceSurface();
    vboWorkpieceSurface.release();

    vboWorkpieceSurfaceUsingDistance.create();
    vboTriangleTable.create();
    vboWorkpieceSurfaceUsingDistance.bind();
    vboTriangleTable.bind();
    initializeProgramWorkpieceSurfaceUsingDistance();
    vboWorkpieceSurfaceUsingDistance.release();
    vboTriangleTable.release();

    vboWorkpieceWireframe.create();
    vboWorkpieceWireframe.bind();
    initializeProgramWorkpieceWireframe();
    vboWorkpieceWireframe.release();

    vaoWorkpiece.release();

    vaoCutter.create();
    vaoCutter.bind();

    vboCutter.create();
    vboCutter.bind();
    initializeProgramCutter();
    vboCutter.release();

    vaoCutter.release();
}

void RenderCWE::destroy()
{
    delete programWorkpieceSurface;
    delete programWorkpieceSurfaceUsingDistance;
    delete programWorkpieceWireframe;
    delete programCutter;

    vboWorkpieceSurface.destroy();
    vboWorkpieceSurfaceUsingDistance.destroy();
    vboWorkpieceWireframe.destroy();
    vboCutter.destroy();

    vaoWorkpiece.destroy();
    vaoCutter.destroy();
}

void RenderCWE::drawWorkpieceWireframe(const std::vector<Vector3Df>& centers, const std::vector<float>& sizes)
{
    if (centers.size() != sizes.size()) {
        throw std::runtime_error("RenderCWE::drawWorkpieceWireframe: centers.size() != sizes.size()");
    }

    int sizeofCenters = static_cast<int>(centers.size() * sizeof(Vector3Df));
    int sizeofSizes = static_cast<int>(sizes.size() * sizeof(float));

    vaoWorkpiece.bind();

    programWorkpieceWireframe->bind();

    vboWorkpieceWireframe.bind();
    vboWorkpieceWireframe.allocate(sizeofCenters + sizeofSizes);
    vboWorkpieceWireframe.write(0, centers.data(), sizeofCenters);
    vboWorkpieceWireframe.write(sizeofCenters, sizes.data(), sizeofSizes);

    programWorkpieceWireframe->setUniformValue("model", getMatModel());
    programWorkpieceWireframe->setUniformValue("view", getMatView());
    programWorkpieceWireframe->setUniformValue("projection", getMatProjection());

    programWorkpieceWireframe->enableAttributeArray(0);
    programWorkpieceWireframe->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    programWorkpieceWireframe->enableAttributeArray(1);
    programWorkpieceWireframe->setAttributeBuffer(1, GL_FLOAT, sizeofCenters, 1);

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(centers.size()));

    programWorkpieceWireframe->release();

    vboWorkpieceWireframe.release();

    vaoWorkpiece.release();
}

void RenderCWE::drawWorkpieceSurfaceUsingDistance(const std::vector<Vector3Df>& centers, const std::vector<float>& sizes, const std::vector<float>& distances)
{
    glDisable(GL_CULL_FACE);
    //glEnable(GL_CULL_FACE);
    if (centers.size() != sizes.size()) {
        throw std::runtime_error("RenderCWE::drawWorkpieceSurfaceUsingDistance: centers.size() != sizes.size()");
    }

    if (centers.size() * 8 != distances.size()) {
        throw std::runtime_error("RenderCWE::drawWorkpieceSurfaceUsingDistance: centers.size() * 8 != distances.size()");
    }

    int sizeofCenters = static_cast<int>(centers.size() * sizeof(Vector3Df));
    int sizeofSizes = static_cast<int>(sizes.size() * sizeof(float));
    int sizeofDistances = static_cast<int>(distances.size() * sizeof(float));

    vaoWorkpiece.bind();

    programWorkpieceSurfaceUsingDistance->bind();

    vboWorkpieceSurfaceUsingDistance.bind();
    vboWorkpieceSurfaceUsingDistance.allocate(sizeofCenters + sizeofSizes + sizeofDistances);
    vboWorkpieceSurfaceUsingDistance.write(0, centers.data(), sizeofCenters);
    vboWorkpieceSurfaceUsingDistance.write(sizeofCenters, sizes.data(), sizeofSizes);
    vboWorkpieceSurfaceUsingDistance.write(sizeofCenters + sizeofSizes, distances.data(), sizeofDistances);

    programWorkpieceSurfaceUsingDistance->setUniformValue("model", getMatModel());
    programWorkpieceSurfaceUsingDistance->setUniformValue("view", getMatView());
    programWorkpieceSurfaceUsingDistance->setUniformValue("projection", getMatProjection());
    programWorkpieceSurfaceUsingDistance->setUniformValue("normalMatrix", getMatNormalMatrix());

    programWorkpieceSurfaceUsingDistance->enableAttributeArray(0);
    programWorkpieceSurfaceUsingDistance->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    programWorkpieceSurfaceUsingDistance->enableAttributeArray(1);
    programWorkpieceSurfaceUsingDistance->setAttributeBuffer(1, GL_FLOAT, sizeofCenters, 1);

    for (int i = 0; i < 8; ++i) {
        programWorkpieceSurfaceUsingDistance->enableAttributeArray(i + 2);
        programWorkpieceSurfaceUsingDistance->setAttributeBuffer(i + 2, GL_FLOAT, sizeofCenters + sizeofSizes + i * sizeof(float), 1, 8 * sizeof(float));
    }

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(centers.size()));

    programWorkpieceSurfaceUsingDistance->release();

    vboWorkpieceSurfaceUsingDistance.release();

    vaoWorkpiece.release();
}

void RenderCWE::drawWorkpieceSurface(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals)
{
    glDisable(GL_CULL_FACE);
    if (vertices.size() != normals.size()) {
        throw std::runtime_error("RenderCWE::drawWorkpieceSurface: vertices.size() != normals.size()");
    }

    int sizeofVertices = static_cast<int>(vertices.size() * sizeof(Vector3Df));
    int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));

    vaoWorkpiece.bind();

    programWorkpieceSurface->bind();

    vboWorkpieceSurface.bind();
    vboWorkpieceSurface.allocate(sizeofVertices + sizeofNormals);
    vboWorkpieceSurface.write(0, vertices.data(), sizeofVertices);
    vboWorkpieceSurface.write(sizeofVertices, normals.data(), sizeofNormals);

    programWorkpieceSurface->setUniformValue("model", getMatModel());
    programWorkpieceSurface->setUniformValue("view", getMatView());
    programWorkpieceSurface->setUniformValue("projection", getMatProjection());
    programWorkpieceSurface->setUniformValue("normalMatrix", getMatNormalMatrix());

    programWorkpieceSurface->enableAttributeArray(0);
    programWorkpieceSurface->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    programWorkpieceSurface->enableAttributeArray(1);
    programWorkpieceSurface->setAttributeBuffer(1, GL_FLOAT, sizeofVertices, 3);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    programWorkpieceSurface->release();

    vboWorkpieceSurface.release();

    vaoWorkpiece.release();
}

void RenderCWE::initializeCutter(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals)
{
    int sizeofVertices = static_cast<int>(vertices.size() * sizeof(Vector3Df));
    int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));
    vaoCutter.bind();
    vboCutter.bind();
    vboCutter.allocate(sizeofVertices + sizeofNormals);
    vboCutter.write(0, vertices.data(), sizeofVertices);
    vboCutter.write(sizeofVertices, normals.data(), sizeofNormals);
    vboCutter.release();
    vaoCutter.release();

    isCutterInitialized = true;
}

void RenderCWE::drawCutter(const Vector3Df& position, const Vector3Df& direction)
{
    if (!isCutterInitialized) {
        return;
    }

    vaoCutter.bind();
    vboCutter.bind();

    programCutter->bind();

    QMatrix4x4 matModelCutterTranslation;
    matModelCutterTranslation.setToIdentity();
    matModelCutterTranslation.translate(position.x, position.y, position.z);
    QMatrix4x4 matModelCutterRotation;
    matModelCutterRotation.setToIdentity();
    matModelCutterRotation.rotate(QQuaternion::fromDirection(QVector3D(direction.x, direction.y, direction.z), QVector3D(0, 0, 1)));
    QMatrix4x4 matModelCutter = matModelCutterTranslation * matModelCutterRotation;

    programCutter->setUniformValue("modelCutter", matModelCutter);
    programCutter->setUniformValue("model", getMatModel());
    programCutter->setUniformValue("view", getMatView());
    programCutter->setUniformValue("projection", getMatProjection());
    programCutter->setUniformValue("normalMatrix", getMatNormalMatrix());

    programCutter->enableAttributeArray(0);
    programCutter->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    programCutter->enableAttributeArray(1);
    programCutter->setAttributeBuffer(1, GL_FLOAT, static_cast<int>(vboCutter.size() / 2), 3);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vboCutter.size() / 2 / sizeof(Vector3Df)));

    programCutter->release();

    vboCutter.release();
    vaoCutter.release();
}

void RenderCWE::clearCutter()
{
    isCutterInitialized = false;
}

void RenderCWE::initializeProgramWorkpieceSurface()
{
    programWorkpieceSurface = new QOpenGLShaderProgram();

    programWorkpieceSurface->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/WorkpieceSurface.vert");
    programWorkpieceSurface->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/WorkpieceSurface.frag");

    programWorkpieceSurface->link();
    programWorkpieceSurface->bind();
    programWorkpieceSurface->setUniformValue("model", getMatModel());
    programWorkpieceSurface->setUniformValue("view", getMatView());
    programWorkpieceSurface->setUniformValue("projection", getMatProjection());
    programWorkpieceSurface->setUniformValue("normalMatrix", getMatNormalMatrix());
    programWorkpieceSurface->setUniformValue("colorLight", colorLight);
    programWorkpieceSurface->setUniformValue("colorObject", colorWorkpieceSurface);
    programWorkpieceSurface->setUniformValue("positionLight", positionLight);
    programWorkpieceSurface->setUniformValue("positionView", getCameraPosition());
    programWorkpieceSurface->release();
}

void RenderCWE::initializeProgramWorkpieceSurfaceUsingDistance()
{
    programWorkpieceSurfaceUsingDistance = new QOpenGLShaderProgram();

    programWorkpieceSurfaceUsingDistance->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/WorkpieceSurfaceUsingDistance.vert");
    programWorkpieceSurfaceUsingDistance->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/WorkpieceSurfaceUsingDistance.geom");
    programWorkpieceSurfaceUsingDistance->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/WorkpieceSurfaceUsingDistance.frag");

    programWorkpieceSurfaceUsingDistance->link();
    programWorkpieceSurfaceUsingDistance->bind();
    programWorkpieceSurfaceUsingDistance->setUniformValue("model", getMatModel());
    programWorkpieceSurfaceUsingDistance->setUniformValue("view", getMatView());
    programWorkpieceSurfaceUsingDistance->setUniformValue("projection", getMatProjection());
    programWorkpieceSurfaceUsingDistance->setUniformValue("normalMatrix", getMatNormalMatrix());
    programWorkpieceSurfaceUsingDistance->setUniformValue("colorLight", colorLight);
    programWorkpieceSurfaceUsingDistance->setUniformValue("colorObject", colorWorkpieceSurface);
    programWorkpieceSurfaceUsingDistance->setUniformValue("positionLight", positionLight);
    programWorkpieceSurfaceUsingDistance->setUniformValue("positionView", getCameraPosition());

    vboTriangleTable.bind();
    vboTriangleTable.allocate(MarchingCubes::TriangleTable.data(), sizeof(MarchingCubes::TriangleTable));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vboTriangleTable.bufferId());

    programWorkpieceSurfaceUsingDistance->release();
}

void RenderCWE::initializeProgramWorkpieceWireframe()
{
    programWorkpieceWireframe = new QOpenGLShaderProgram();
    programWorkpieceWireframe->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/WorkpieceWireframe.vert");
    programWorkpieceWireframe->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/WorkpieceWireframe.geom");
    programWorkpieceWireframe->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/WorkpieceWireframe.frag");
    programWorkpieceWireframe->link();
    programWorkpieceWireframe->bind();
    programWorkpieceWireframe->setUniformValue("model", getMatModel());
    programWorkpieceWireframe->setUniformValue("view", getMatView());
    programWorkpieceWireframe->setUniformValue("projection", getMatProjection());
    programWorkpieceWireframe->setUniformValue("colorObject", colorWorkpieceWireframe);
    programWorkpieceWireframe->release();
}

void RenderCWE::initializeProgramCutter()
{
    programCutter = new QOpenGLShaderProgram();
    programCutter->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/Cutter.vert");
    programCutter->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/Cutter.frag");
    programCutter->link();
    programCutter->bind();
    programCutter->setUniformValue("model", getMatModel());
    programCutter->setUniformValue("view", getMatView());
    programCutter->setUniformValue("projection", getMatProjection());
    programCutter->setUniformValue("normalMatrix", getMatNormalMatrix());
    programCutter->setUniformValue("colorLight", colorLight);
    programCutter->setUniformValue("colorObject", colorCutter);
    programCutter->setUniformValue("positionLight", positionLight);
    programCutter->setUniformValue("positionView", getCameraPosition());
    programCutter->release();
}
