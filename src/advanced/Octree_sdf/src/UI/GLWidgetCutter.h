#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class RenderCutter;

class GLWidgetCutter : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    explicit GLWidgetCutter(QWidget* parent = nullptr);
    ~GLWidgetCutter() override;

public Q_SLOTS:
    void setCutter(float d, float r, float e, float f, float alpha, float beta, float h);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    RenderCutter* renderCutter;
};
