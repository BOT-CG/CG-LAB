#include "GLCompute.h"

#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

#include <array>
#include <mutex>

class GLComputeInstance : public QOffscreenSurface, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    explicit GLComputeInstance();
    ~GLComputeInstance() override;

    void computeSTL2UDF(const std::vector<float>& triangleVertices, std::vector<PointAndDistance>& pointsAndDistances);
    void computeSTL2SDF(const std::vector<float>& triangleVertices, const std::vector<float>& triangleNormals, std::vector<PointAndDistance>& pointsAndDistances);

private:
    QOpenGLContext* context = nullptr;

    QOpenGLBuffer vboSTLTriangleVertices;
    QOpenGLBuffer vboSTLPointsAndDistances;

    QOpenGLBuffer vboSTL2SDFTriangleVertices;
    QOpenGLBuffer vboSTL2SDFTriangleNormals;
    QOpenGLBuffer vboSTL2SDFPointsAndDistances;

    QOpenGLShaderProgram* programSTL2UDF;
    QOpenGLShaderProgram* programSTL2SDF;

    void initializeGL();
    void initializeProgramSTL2UDF();
    void initializeProgramSTL2SDF();
    void getNumGroups(size_t numElements, std::array<GLuint, 3>& numGroups);
};
#include "GLCompute.moc"

GLComputeInstance::GLComputeInstance()
    : QOffscreenSurface()
{
    setFormat(QSurfaceFormat::defaultFormat());
    create();
    context = new QOpenGLContext();
    context->setFormat(requestedFormat());
    context->create();
    context->makeCurrent(this);
    initializeGL();
}

GLComputeInstance::~GLComputeInstance()
{
    delete programSTL2UDF;
    delete programSTL2SDF;
    delete context;
}

void GLComputeInstance::computeSTL2UDF(const std::vector<float>& triangleVertices, std::vector<PointAndDistance>& pointsAndDistances)
{
    context->makeCurrent(this);

    if (triangleVertices.size() % 9 != 0) {
        throw std::runtime_error("Render::computeSTL2UDF: triangleVertices.size() % 9 != 0");
    }

    int sizeofTriangleVertices = static_cast<int>(triangleVertices.size() * sizeof(float));
    int sizeofPointsAndDistances = static_cast<int>(pointsAndDistances.size() * sizeof(PointAndDistance));

    programSTL2UDF->bind();

    vboSTLTriangleVertices.bind();
    vboSTLTriangleVertices.allocate(triangleVertices.data(), sizeofTriangleVertices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vboSTLTriangleVertices.bufferId());

    vboSTLPointsAndDistances.bind();
    vboSTLPointsAndDistances.allocate(pointsAndDistances.data(), sizeofPointsAndDistances);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vboSTLPointsAndDistances.bufferId());

    std::array<GLuint, 3> numGroups;
    getNumGroups(pointsAndDistances.size(), numGroups);

    glDispatchCompute(numGroups[0], numGroups[1], numGroups[2]);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    vboSTLPointsAndDistances.read(0, pointsAndDistances.data(), sizeofPointsAndDistances);

    vboSTLPointsAndDistances.release();
    vboSTLTriangleVertices.release();

    programSTL2UDF->release();

    context->doneCurrent();
}

void GLComputeInstance::computeSTL2SDF(const std::vector<float>& triangleVertices, const std::vector<float>& triangleNormals, std::vector<PointAndDistance>& pointsAndDistances)
{
    context->makeCurrent(this);

    if (triangleVertices.size() % 9 != 0) {
        throw std::runtime_error("Render::computeSTL2SDF: triangleVertices.size() % 9 != 0");
    }

    if (triangleNormals.size() % 3 != 0) {
        throw std::runtime_error("Render::computeSTL2SDF: triangleNormals.size() % 3 != 0");
    }

    int sizeofTriangleVertices = static_cast<int>(triangleVertices.size() * sizeof(float));
    int sizeofTriangleNormals = static_cast<int>(triangleNormals.size() * sizeof(float));
    int sizeofPointsAndDistances = static_cast<int>(pointsAndDistances.size() * sizeof(PointAndDistance));

    programSTL2SDF->bind();

    vboSTL2SDFTriangleVertices.bind();
    vboSTL2SDFTriangleVertices.allocate(triangleVertices.data(), sizeofTriangleVertices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vboSTL2SDFTriangleVertices.bufferId());

    vboSTL2SDFTriangleNormals.bind();
    vboSTL2SDFTriangleNormals.allocate(triangleNormals.data(), sizeofTriangleNormals);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vboSTL2SDFTriangleNormals.bufferId());

    vboSTL2SDFPointsAndDistances.bind();
    vboSTL2SDFPointsAndDistances.allocate(pointsAndDistances.data(), sizeofPointsAndDistances);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vboSTL2SDFPointsAndDistances.bufferId());

    std::array<GLuint, 3> numGroups;
    getNumGroups(pointsAndDistances.size(), numGroups);

    glDispatchCompute(numGroups[0], numGroups[1], numGroups[2]);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    vboSTL2SDFPointsAndDistances.read(0, pointsAndDistances.data(), sizeofPointsAndDistances);

    vboSTL2SDFPointsAndDistances.release();
    vboSTL2SDFTriangleNormals.release();
    vboSTL2SDFTriangleVertices.release();

    programSTL2SDF->release();

    context->doneCurrent();
}

void GLComputeInstance::initializeGL()
{
    context->makeCurrent(this);
    initializeOpenGLFunctions();

    vboSTLTriangleVertices.create();
    vboSTLPointsAndDistances.create();
    vboSTLTriangleVertices.bind();
    vboSTLPointsAndDistances.bind();
    initializeProgramSTL2UDF();
    vboSTLTriangleVertices.release();
    vboSTLPointsAndDistances.release();

    vboSTL2SDFTriangleVertices.create();
    vboSTL2SDFTriangleNormals.create();
    vboSTL2SDFPointsAndDistances.create();
    vboSTL2SDFTriangleVertices.bind();
    vboSTL2SDFTriangleNormals.bind();
    vboSTL2SDFPointsAndDistances.bind();
    initializeProgramSTL2SDF();
    vboSTL2SDFTriangleVertices.release();
    vboSTL2SDFTriangleNormals.release();
    vboSTL2SDFPointsAndDistances.release();

    context->doneCurrent();
}

void GLComputeInstance::initializeProgramSTL2UDF()
{
    programSTL2UDF = new QOpenGLShaderProgram();
    programSTL2UDF->addShaderFromSourceFile(QOpenGLShader::Compute, ":/computeShaders/STL2UDF.comp");
    programSTL2UDF->link();
}

void GLComputeInstance::initializeProgramSTL2SDF()
{
    programSTL2SDF = new QOpenGLShaderProgram();
    programSTL2SDF->addShaderFromSourceFile(QOpenGLShader::Compute, ":/computeShaders/STL2SDF.comp");
    programSTL2SDF->link();
}

void GLComputeInstance::getNumGroups(size_t numElements, std::array<GLuint, 3>& numGroups)
{
    GLint maxComputeWorkGroupCount[3] = { 65535 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, maxComputeWorkGroupCount);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, maxComputeWorkGroupCount + 1);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, maxComputeWorkGroupCount + 2);
    numGroups[0] = static_cast<GLuint>(numElements);
    numGroups[1] = 1;
    numGroups[2] = 1;
    if (numGroups[0] > maxComputeWorkGroupCount[0]) {
        numGroups[0] = maxComputeWorkGroupCount[0];
        numGroups[1] = static_cast<GLuint>(numElements) / maxComputeWorkGroupCount[0] + 1;
        if (numGroups[1] > maxComputeWorkGroupCount[1]) {
            numGroups[1] = maxComputeWorkGroupCount[1];
            numGroups[2] = static_cast<GLuint>(numElements) / (maxComputeWorkGroupCount[0] * maxComputeWorkGroupCount[1]) + 1;
        }
    }
}

GLComputeInstance* GLCompute::instance = nullptr;

GLComputeInstance* GLCompute::getInstance()
{
    static std::once_flag flag;
    std::call_once(flag, []() {
        instance = new GLComputeInstance();
    });
    return instance;
}

void GLCompute::computeSTL2UDF(const std::vector<float>& triangleVertices, std::vector<PointAndDistance>& pointsAndDistances)
{
    getInstance()->computeSTL2UDF(triangleVertices, pointsAndDistances);
}

void GLCompute::computeSTL2SDF(const std::vector<float>& triangleVertices, const std::vector<float>& triangleNormals, std::vector<PointAndDistance>& pointsAndDistances)
{
    getInstance()->computeSTL2SDF(triangleVertices, triangleNormals, pointsAndDistances);
}
