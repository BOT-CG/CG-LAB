#include "GLWidgetCutter.h"

#include "CutterFactory.h"
#include "RenderCutter.h"

#include <memory>

GLWidgetCutter::GLWidgetCutter(QWidget* parent)
    : QOpenGLWidget(parent)
{
    renderCutter = new RenderCutter();
}

GLWidgetCutter::~GLWidgetCutter()
{
    makeCurrent();
    renderCutter->destroy();
    delete renderCutter;
    doneCurrent();
}

void GLWidgetCutter::setCutter(float d, float r, float e, float f, float alpha, float beta, float h)
{
    std::unique_ptr<Cutter> cutter = CutterFactory::create(d, r, e, f, alpha, beta, h);
    std::vector<Vector3Df> vertices, normals;
    cutter->generateMesh(vertices, normals);
    renderCutter->setCutter(vertices, normals);
    update();
}

void GLWidgetCutter::initializeGL()
{
    initializeOpenGLFunctions();

    renderCutter->initialize();
}

void GLWidgetCutter::resizeGL(int w, int h)
{
    renderCutter->updateWindowSize(width(), height());
    renderCutter->updateMatrixProjection();
}

void GLWidgetCutter::paintGL()
{
    renderCutter->updateWindowSize(width(), height());
    renderCutter->updateMatrices();
    renderCutter->clearColorBuffer();
    renderCutter->drawCutter();
}

void GLWidgetCutter::keyPressEvent(QKeyEvent* event)
{
    renderCutter->keyPressEvent(event);
    update();
}

void GLWidgetCutter::mousePressEvent(QMouseEvent* event)
{
    renderCutter->mousePressEvent(event);
}

void GLWidgetCutter::mouseMoveEvent(QMouseEvent* event)
{
    renderCutter->mouseMoveEvent(event);
    update();
}

void GLWidgetCutter::mouseReleaseEvent(QMouseEvent* event)
{
    renderCutter->mouseReleaseEvent(event);
    update();
}

void GLWidgetCutter::wheelEvent(QWheelEvent* event)
{
    renderCutter->wheelEvent(event);
    update();
}
