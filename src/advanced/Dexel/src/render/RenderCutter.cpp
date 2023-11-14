#include "RenderCutter.h"

void RenderCutter::initialize()
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

void RenderCutter::destroy()
{
    delete program;
    vbo.destroy();
    vao.destroy();
}

void RenderCutter::setCutter(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals)
{
    int sizeofVertices = static_cast<int>(vertices.size() * sizeof(Vector3Df));
    int sizeofNormals = static_cast<int>(normals.size() * sizeof(Vector3Df));
    vao.bind();
    vbo.bind();
    vbo.allocate(sizeofVertices + sizeofNormals);
    vbo.write(0, vertices.data(), sizeofVertices);
    vbo.write(sizeofVertices, normals.data(), sizeofNormals);
    vbo.release();
    vao.release();
    isInitialized = true;
}

void RenderCutter::drawCutter()
{
    if (!isInitialized) {
        return;
    }

    vao.bind();
    vbo.bind();

    program->bind();

    QMatrix4x4 matModelCutter;
    program->setUniformValue("modelCutter", matModelCutter);
    program->setUniformValue("model", getMatModel());
    program->setUniformValue("view", getMatView());
    program->setUniformValue("projection", getMatProjection());
    program->setUniformValue("normalMatrix", getMatNormalMatrix());

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, static_cast<int>(vbo.size() / 2), 3);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vbo.size() / 2 / sizeof(Vector3Df)));

    program->release();

    vbo.release();
    vao.release();
}

void RenderCutter::clearCutter()
{
    isInitialized = false;
}

void RenderCutter::initializeProgram()
{
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/Cutter.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/Cutter.frag");
    program->link();
    program->bind();
    program->setUniformValue("model", getMatModel());
    program->setUniformValue("view", getMatView());
    program->setUniformValue("projection", getMatProjection());
    program->setUniformValue("normalMatrix", getMatNormalMatrix());
    program->setUniformValue("colorLight", colorLight);
    program->setUniformValue("colorObject", colorCutter);
    program->setUniformValue("positionLight", positionLight);
    program->setUniformValue("positionView", getCameraPosition());
    program->release();
}
