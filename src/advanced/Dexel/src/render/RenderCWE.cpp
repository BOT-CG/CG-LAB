#include "RenderCWE.h"

#include "MarchingCubes.h"

#include <cmath>
#include <iostream>
void RenderCWE::initialize()
{
    Render::initialize();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

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

    vboWorkpieceDexelSurface.create();
    vboWorkpieceDexelSurface.bind();
    initializeProgramWorkpieceDexelSurface();
    vboWorkpieceDexelSurface.release();

    vaoWorkpiece.release();

    vaoCutter.create();
    vaoCutter.bind();

    vboCutter.create();
    vboCutter.bind();
    initializeProgramCutter();
    vboCutter.release();

    vaoCutter.release();

    vaoTessllation.create();
    vaoTessllation.bind();
    vboTessllation.create();
    vboTessllation.bind();
    initializeProgramTessllation();
    vboTessllation.release();
    vaoTessllation.release();
    // glDisable(GL_CULL_FACE);
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
    programCutter->setUniformValue("BoxSize", BoxSize);
    
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

void RenderCWE::drawWorkpieceDexelSurface(const CutterParm& cutterPara, const std::vector<Vector3Df>& CubeCenter, const std::vector<Vector3Df>& CubeNormals, const std::vector<float>& PointHeigth, float CubeSize)
{

    if (CubeCenter.size() != CubeNormals.size()) {
        throw std::runtime_error("drawWorkpieceDexelSurface: CubeCenter.size() != CubeNormals.size()");
    }

    if (CubeCenter.size() * 8 != PointHeigth.size()) {
        throw std::runtime_error("drawWorkpieceDexelSurface: CubeCenter.size() * 8 != PointHeigth.size()");
    }

    BoxSize = QVector3D(cutterPara.BoxSize.x, cutterPara.BoxSize.y, cutterPara.BoxSize.z);
    int sizeofCenter = static_cast<int>(CubeCenter.size() * sizeof(Vector3Df));
    int sizeofNormal = static_cast<int>(CubeNormals.size() * sizeof(Vector3Df));
    int sizeofPoint = static_cast<int>(PointHeigth.size() * sizeof(float));

    vaoWorkpiece.bind();
    programWorkpieceDexelSurface->bind();

    vboWorkpieceDexelSurface.bind();
    vboWorkpieceDexelSurface.allocate(sizeofCenter + sizeofNormal + sizeofPoint);
    //第一个参数为数据写入的起始位置，第二个参数为源数据被拷贝的起始地址，最后一个为写入
    vboWorkpieceDexelSurface.write(0, CubeCenter.data(), sizeofCenter);
    vboWorkpieceDexelSurface.write(sizeofCenter, CubeNormals.data(), sizeofNormal);
    vboWorkpieceDexelSurface.write(sizeofCenter + sizeofNormal, PointHeigth.data(), sizeofPoint);

    programWorkpieceDexelSurface->setUniformValue("model", getMatModel());
    programWorkpieceDexelSurface->setUniformValue("view", getMatView());
    programWorkpieceDexelSurface->setUniformValue("projection", getMatProjection());
    programWorkpieceDexelSurface->setUniformValue("normalMatrix", getMatNormalMatrix());

    //dexel分割大小
    programWorkpieceDexelSurface->setUniformValue("size", CubeSize * 2.0f);
    //坐标变换
    programWorkpieceDexelSurface->setUniformValue("BoxSize", BoxSize);

    programWorkpieceDexelSurface->enableAttributeArray(0);
    programWorkpieceDexelSurface->setAttributeBuffer(0, GL_FLOAT, 0, 3); //由于CubeCenter这里的数据是紧密相连的，因此第五个参数不用填，默认为0，
    programWorkpieceDexelSurface->enableAttributeArray(1);
    programWorkpieceDexelSurface->setAttributeBuffer(1, GL_FLOAT, sizeofCenter, 3);

    for (int i = 0; i < 8; ++i) {
        programWorkpieceDexelSurface->enableAttributeArray(i + 2);
        //激活顶点属性i+1. 该顶点属性对应的数据在vbo中的起始位置为sizeofCenter + i * sizeof(float)，数据类型为GL_FLOAT，每个数据的大小为1，步长为8 * sizeof(float)，即下一个数据的位置为sizeofCenter + i * sizeof(float) + 8 * sizeof(float)
        programWorkpieceDexelSurface->setAttributeBuffer(i + 2, GL_FLOAT, sizeofCenter + sizeofNormal + i * sizeof(float), 1, 8 * sizeof(float));
    }

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(CubeCenter.size()));
    // std::cout << "drawWorkpieceDexelSurface" << std::endl;
    programWorkpieceDexelSurface->release();
    vboWorkpieceDexelSurface.release();
    vaoWorkpiece.release();
}

void RenderCWE::drawWorkpieceDexelLine(const CutterParm& cutterPara, const std::vector<Vector3Df>& CubeCenter, const std::vector<Vector3Df>& CubeNormals, const std::vector<float>& PointHeigth, float CubeSize)
{
    if (CubeCenter.size() != CubeNormals.size()) {
        throw std::runtime_error("drawWorkpieceDexelSurface: CubeCenter.size() != CubeNormals.size()");
    }

    if (CubeCenter.size() * 8 != PointHeigth.size()) {
        throw std::runtime_error("drawWorkpieceDexelSurface: CubeCenter.size() * 8 != PointHeigth.size()");
    }

    QVector3D BoxSize = QVector3D(cutterPara.BoxSize.x, cutterPara.BoxSize.y, cutterPara.BoxSize.z);
    int sizeofCenter = static_cast<int>(CubeCenter.size() * sizeof(Vector3Df));
    int sizeofNormal = static_cast<int>(CubeNormals.size() * sizeof(Vector3Df));
    int sizeofPoint = static_cast<int>(PointHeigth.size() * sizeof(float));
    // std::cout << "sizeofCenter: " << sizeofCenter << std::endl;
    // std::cout << "sizeofNormal: " << sizeofNormal << std::endl;
    // std::cout << "sizeofCenter: " << sizeofCenter << std::endl;
    // std::cout << "sizeofNormal: " << sizeofNormal << std::endl;
    // std::cout << "sizeofPoint: " << sizeofPoint << std::endl;
    // std::cout << "CubeCenter.size(): " << CubeCenter.size() << std::endl;
    vaoWorkpiece.bind();
    programWorkpieceDexelLine->bind();

    vboWorkpieceDexelSurface.bind();
    vboWorkpieceDexelSurface.allocate(sizeofCenter + sizeofNormal + sizeofPoint);
    //第一个参数为数据写入的起始位置，第二个参数为源数据被拷贝的起始地址，最后一个为写入
    vboWorkpieceDexelSurface.write(0, CubeCenter.data(), sizeofCenter);
    vboWorkpieceDexelSurface.write(sizeofCenter, CubeNormals.data(), sizeofNormal);
    vboWorkpieceDexelSurface.write(sizeofCenter + sizeofNormal, PointHeigth.data(), sizeofPoint);

    programWorkpieceDexelLine->setUniformValue("model", getMatModel());
    programWorkpieceDexelLine->setUniformValue("view", getMatView());
    programWorkpieceDexelLine->setUniformValue("projection", getMatProjection());
    programWorkpieceDexelLine->setUniformValue("normalMatrix", getMatNormalMatrix());

    //dexel分割大小
    programWorkpieceDexelLine->setUniformValue("size", CubeSize);
    //坐标变换
    programWorkpieceDexelLine->setUniformValue("BoxSize", BoxSize);
    programWorkpieceDexelLine->enableAttributeArray(0);
    programWorkpieceDexelLine->setAttributeBuffer(0, GL_FLOAT, 0, 3); //由于CubeCenter这里的数据是紧密相连的，因此第五个参数不用填，默认为0，
    programWorkpieceDexelLine->enableAttributeArray(1);
    programWorkpieceDexelLine->setAttributeBuffer(1, GL_FLOAT, sizeofCenter, 3);

    for (int i = 0; i < 8; ++i) {
        programWorkpieceDexelLine->enableAttributeArray(i + 2);
        //激活顶点属性i+1. 该顶点属性对应的数据在vbo中的起始位置为sizeofCenter + i * sizeof(float)，数据类型为GL_FLOAT，每个数据的大小为1，步长为8 * sizeof(float)，即下一个数据的位置为sizeofCenter + i * sizeof(float) + 8 * sizeof(float)
        programWorkpieceDexelLine->setAttributeBuffer(i + 2, GL_FLOAT, sizeofCenter + sizeofNormal + i * sizeof(float), 1, 8 * sizeof(float));
    }

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(CubeCenter.size()));
    // std::cout << "drawWorkpieceDexelSurface" << std::endl;
    programWorkpieceDexelLine->release();
    vboWorkpieceDexelSurface.release();
    vaoWorkpiece.release();
}

//DrawDexelSurfaceUseGemo
void RenderCWE::initializeProgramWorkpieceDexelSurface()
{
    programWorkpieceDexelSurface = new QOpenGLShaderProgram();
    programWorkpieceDexelSurface->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/WorkpieceDexelSurface.vert");
    programWorkpieceDexelSurface->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/WorkpieceDexelSurface.geom");
    programWorkpieceDexelSurface->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/WorkpieceDexelSurface.frag");

    programWorkpieceDexelSurface->link();
    programWorkpieceDexelSurface->bind();
    programWorkpieceDexelSurface->setUniformValue("model", getMatModel());
    programWorkpieceDexelSurface->setUniformValue("view", getMatView());
    programWorkpieceDexelSurface->setUniformValue("projection", getMatProjection());
    programWorkpieceDexelSurface->setUniformValue("normalMatrix", getMatNormalMatrix());
    programWorkpieceDexelSurface->setUniformValue("colorLight", colorLight);
    programWorkpieceDexelSurface->setUniformValue("colorObject", colorWorkpieceSurface);
    programWorkpieceDexelSurface->setUniformValue("positionLight", positionLight);
    programWorkpieceDexelSurface->setUniformValue("positionView", getCameraPosition());
    programWorkpieceDexelSurface->release();

    programWorkpieceDexelLine = new QOpenGLShaderProgram();
    programWorkpieceDexelLine->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/WorkpieceDexelSurface.vert");
    programWorkpieceDexelLine->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/WorkpieceDexelLine.geom");
    programWorkpieceDexelLine->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/WorkpieceDexelLine.frag");
    programWorkpieceDexelLine->link();
    programWorkpieceDexelLine->bind();
    programWorkpieceDexelLine->setUniformValue("model", getMatModel());
    programWorkpieceDexelLine->setUniformValue("view", getMatView());
    programWorkpieceDexelLine->setUniformValue("projection", getMatProjection());
    programWorkpieceDexelLine->setUniformValue("normalMatrix", getMatNormalMatrix());
    programWorkpieceDexelLine->setUniformValue("colorLight", colorLight);
    programWorkpieceDexelLine->setUniformValue("colorObject", colorWorkpieceWireframe);
    programWorkpieceDexelLine->setUniformValue("positionLight", positionLight);
    programWorkpieceDexelLine->setUniformValue("positionView", getCameraPosition());
    programWorkpieceDexelLine->release();
}

void RenderCWE::initializeProgramTessllation()
{
    programTessllation = new QOpenGLShaderProgram();
    programTessllation->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/DexelTessllation.vert");
    programTessllation->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/DexelTessllation.frag");
    programTessllation->addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shaders/DexelTessllation.tcs");
    programTessllation->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shaders/DexelTessllation.tes");
    programTessllation->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/DexelTessllation.geom");

    programTessllation->link();
    programTessllation->bind();

    programTessllation->setUniformValue("model", getMatModel());
    programTessllation->setUniformValue("view", getMatView());
    programTessllation->setUniformValue("projection", getMatProjection());
    programTessllation->setUniformValue("normalMatrix", getMatNormalMatrix());

    programTessllation->setUniformValue("colorLight", colorLight);
    programTessllation->setUniformValue("colorObject", colorWorkpieceSurface);
    programTessllation->setUniformValue("positionLight", positionLight);
    programTessllation->setUniformValue("positionView", getCameraPosition());
    programTessllation->release();
}

void RenderCWE::InitializeTessllationArray(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, const std::vector<Vector3Df>& CutterPostions)
{

    int sizeofVectices = static_cast<int>(vertices.size() * sizeof(Vector3Df));
    int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));
    int sizeofCutterPoints = static_cast<int>(CutterPostions.size() * sizeof(Vector3Df));

    // std::cout << "sizeofVectices: " << sizeofVectices << std::endl;
    // std::cout << "sizeofNormals: " << sizeofNormals << std::endl;
    // std::cout << "sizeofCutterPoints: " << sizeofCutterPoints << std::endl;

    programTessllation->bind();
    vaoTessllation.bind();
    vboTessllation.bind();
    vboTessllation.allocate(sizeofVectices + sizeofNormals + sizeofCutterPoints);
    vboTessllation.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vboTessllation.write(0, &vertices[0], sizeofVectices);
    vboTessllation.write(sizeofVectices, &normals[0], sizeofNormals);
    vboTessllation.write(sizeofVectices + sizeofNormals, &CutterPostions[0], sizeofCutterPoints);
    //设置顶点属性
    programTessllation->enableAttributeArray(0);
    programTessllation->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    //设置法线属性
    programTessllation->enableAttributeArray(1);
    programTessllation->setAttributeBuffer(1, GL_FLOAT, sizeofVectices, 3);
    //设置刀具位置属性
    programTessllation->enableAttributeArray(2);
    programTessllation->setAttributeBuffer(2, GL_FLOAT, sizeofVectices + sizeofNormals, 3);
    glPatchParameteri(GL_PATCH_VERTICES, 3); //设置每个patch的顶点数量为3，即每个patch包含3个顶点
    // glBindBuffer(GL_ARRAY_BUFFER, 0); //注意查看是否要解绑，使用OpenGL原生函数解绑是否会出错
    // glBindVertexArray(0);
    // programTessllation->release();
    // vboTessllation.release();
    // vaoTessllation.release();
}

void RenderCWE::SetTessllationArray(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, const std::vector<Vector3Df>& CutterPostions)
{

    int sizeofVectices = static_cast<int>(vertices.size() * sizeof(Vector3Df));
    int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));
    int sizeofCutterPoints = static_cast<int>(CutterPostions.size() * sizeof(Vector3Df));

    // vaoTessllation.create();
    // vboTessllation.create();

    vaoTessllation.bind();
    vboTessllation.bind();

    vboTessllation.allocate(sizeofVectices + sizeofNormals + sizeofCutterPoints);
    vboTessllation.setUsagePattern(QOpenGLBuffer::StaticDraw);

    vboTessllation.write(0, &vertices[0], sizeofVectices);
    vboTessllation.write(sizeofVectices, &normals[0], sizeofNormals);
    vboTessllation.write(sizeofVectices + sizeofNormals, &CutterPostions[0], sizeofCutterPoints);
    //设置顶点属性
    programTessllation->enableAttributeArray(0);
    programTessllation->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    //设置法线属性
    programTessllation->enableAttributeArray(1);
    programTessllation->setAttributeBuffer(1, GL_FLOAT, sizeofVectices, 3);
    //设置刀具位置属性
    programTessllation->enableAttributeArray(2);
    programTessllation->setAttributeBuffer(2, GL_FLOAT, sizeofVectices + sizeofNormals, 3);

    glPatchParameteri(GL_PATCH_VERTICES, 3); //设置每个patch的顶点数量为3，即每个patch包含3个顶点
    // glBindBuffer(GL_ARRAY_BUFFER, 0); //注意查看是否要解绑，使用OpenGL原生函数解绑是否会出错
    // glBindVertexArray(0);
}

void RenderCWE::drawTessllation(const CutterParm& cutterPara, const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, const std::vector<Vector3Df>& CutterPostions)
{
    // glDisable(GL_CULL_FACE);
    SetTessllationArray(vertices, normals, CutterPostions);
    QVector3D BoxSize = QVector3D(cutterPara.BoxSize.x, cutterPara.BoxSize.y, cutterPara.BoxSize.z);
    float CutterR = cutterPara.d / (2.0f);
    unsigned int CutterType = cutterPara.cutterType;
    // std::cout << "CutterType: " << CutterType << std::endl;
    // std::cout << "CutterR: " << CutterR << std::endl;
    // int sizeofVectices = static_cast<int>(vertices.size() * sizeof(Vector3Df));
    // int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));
    // int sizeofCutterPoints = static_cast<int>(CutterPostions.size() * sizeof(Vector3Df));
    // std::cout << "sizeofVectices: " << sizeofVectices << std::endl;
    // std::cout << "sizeofNormals: " << sizeofNormals << std::endl;
    // std::cout << "sizeofCutterPoints: " << sizeofCutterPoints << std::endl;

    programTessllation->bind();

    programTessllation->setUniformValue("model", getMatModel());
    programTessllation->setUniformValue("view", getMatView());
    programTessllation->setUniformValue("projection", getMatProjection());
    programTessllation->setUniformValue("normalMatrix", getMatNormalMatrix());
    programTessllation->setUniformValue("CutterR", CutterR);
    programTessllation->setUniformValue("CutterType", CutterType);
    programTessllation->setUniformValue("BoxSize", BoxSize);

    vaoTessllation.bind(); //绑定顶点数组对象

    glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(vertices.size())); //绘制,需要绘制的顶点数量为vertices.size()

    glBindVertexArray(0); //注意查看能否使用OpenGL原生函数解绑

    // programTessllation->release();
    // vaoTessllation.release();

    // programTessllation->release();
    // vboTessllation.release();
    // vaoTessllation.release();
    // vboTessllation.release();
}