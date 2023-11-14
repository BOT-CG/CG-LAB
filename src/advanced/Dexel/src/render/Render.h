#pragma once

#include "Camera.h"

#include <QColor>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuaternion>
#include <QVector3D>
#include <QWheelEvent>

#include <string>

class Render : protected QOpenGLExtraFunctions {
public:
    Render() = default;
    virtual ~Render() = default;

    inline void clearColorBuffer()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    virtual void initialize();
    void drawAxis();
    void updateWindowSize(int w, int h);
    void updateMatrices();
    void updateMatrixModel();
    void updateMatrixView();
    void updateMatrixProjection();
    void setUsePerspective(bool usePerspective);

    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    inline static std::string getOpenGLInfo() { return openGLInfo; }

protected:
    QMatrix4x4 getMatModel() const { return matModel; }
    QMatrix4x4 getMatView() const { return matView; }
    QMatrix4x4 getMatProjection() const { return matProjection; }
    QMatrix3x3 getMatNormalMatrix() const { return matNormalMatrix; }
    QVector3D getCameraPosition() const { return camera.position; }

private:
    bool isMouseMiddleButtonPressed = false;
    bool isMouseRightButtonPressed = false;
    bool usePerspective = true;
    float windowWidth = 0.0f;
    float windowHeight = 0.0f;

    float orthoTop = 0.0f;
    float orthoBottom = 0.0f;
    float orthoLeft = 0.0f;
    float orthoRight = 0.0f;
    float initialCameraDistance = 0.0f;

    QMatrix4x4 matModel;
    QMatrix4x4 matView;
    QMatrix4x4 matProjection;
    QMatrix3x3 matNormalMatrix;

    QVector2D positionLastMousePressMiddle;
    QVector2D positionLastMousePressRight;
    QMatrix4x4 matRotationModel;
    QMatrix4x4 matRotationDrag;
    QMatrix4x4 matTranslationModel;
    QMatrix4x4 matTranslationDrag;

    QMatrix4x4 matViewAxis;
    QMatrix4x4 matProjectionAxis;
    QOpenGLShaderProgram* programAxis;

    Camera camera;

    static std::string openGLInfo;

    void initializeProgramAxis();
    inline float aspectRatio() const { return windowWidth / windowHeight; }
};
