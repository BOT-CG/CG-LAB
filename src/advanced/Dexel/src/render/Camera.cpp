#include "Camera.h"

Camera::Camera()
{
#if 0
    pos = QVector3D(0.0f, 1.5f, 2.0f);
    front = QVector3D(0.0f, -0.5f, -1.0f);
    up = QVector3D(0.0f, 1.0f, 0.0f);
#endif

#if 0
    pos = QVector3D(1.2f, 3.2f, 2.2f);
    front = QVector3D(-1.2f, -2.7f, -2.2f);
    up = QVector3D(0.0f, 1.0f, 0.0f);
#endif
}

void Camera::keyPressEvent(QKeyEvent* event)
{
    float Speed = 0.1f;
    if (event->key() == Qt::Key_W) {
        position += Speed * front;
    }
    if (event->key() == Qt::Key_A) {
        position -= Speed * QVector3D::crossProduct(front, up).normalized();
    }
    if (event->key() == Qt::Key_S) {
        position -= Speed * front;
    }
    if (event->key() == Qt::Key_D) {
        position += Speed * QVector3D::crossProduct(front, up).normalized();
    }
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (isFirstMousePress) {
            mousePressPosition = QVector2D(event->position());
            isFirstMousePress = false;
        }

        qreal angularSpeed = 2;
        QVector2D diff = QVector2D(event->position()) - mousePressPosition;
        mousePressPosition = QVector2D(event->position());
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        rotationAxis = (rotationAxis * angularSpeed + n).normalized();
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
    }
}

void Camera::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        fov -= (float)numSteps.y();
    }

    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;

    event->accept();
}
