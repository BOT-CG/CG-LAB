#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>
#include <QWheelEvent>

class Camera {
public:
    Camera();
    virtual ~Camera() = default;

    float fov = 45.0f;
    float scale = 1.0f;
    QVector3D position = QVector3D(0.0f, 0.0f, 5.0f);
    QVector3D front = QVector3D(0.0f, 0.0f, -1.0f);
    QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);
    QQuaternion rotation;

    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    bool isFirstMousePress = true;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
};
