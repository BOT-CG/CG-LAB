#include "Render.h"

#include "utils.h"

#include <cmath>

std::string Render::openGLInfo = "";

void Render::initialize()
{
    initializeOpenGLFunctions();

    openGLInfo = "OpenGL Version: " + std::string((char*)glGetString(GL_VERSION))
        + "\nGLSL Version: " + std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION))
        + "\nVendor: " + std::string((char*)glGetString(GL_VENDOR))
        + "\nRenderer: " + std::string((char*)glGetString(GL_RENDERER));

    initializeProgramAxis();

    matRotationModel.setToIdentity();
    matRotationModel.rotate(-90.0f, QVector3D(1.0f, 0, 0));
    matRotationModel.rotate(-90.0f, QVector3D(0, 0, 1.0f));

    matViewAxis.lookAt(camera.position, camera.position + camera.front, camera.up);
    initialCameraDistance = camera.position.length();
}

void Render::drawAxis()
{
    constexpr QVector3D axisCenter = { -1.5f, -1.5f, 0.0f };
    constexpr float axisSize = 0.7f;
    constexpr QColor colorAxisX = { 0xF4, 0x43, 0x36 };
    constexpr QColor colorAxisY = { 0x4C, 0xAF, 0x50 };
    constexpr QColor colorAxisZ = { 0x21, 0x96, 0xF3 };

    programAxis->bind();

    QMatrix4x4 matModelAxis;
    matModelAxis.translate(axisCenter);
    matModelAxis = matModelAxis * matRotationDrag * matRotationModel;

    programAxis->setUniformValue("model", matModelAxis);
    programAxis->setUniformValue("view", matViewAxis);
    programAxis->setUniformValue("projection", matProjectionAxis);
    programAxis->setUniformValue("colorAxisX", colorAxisX);
    programAxis->setUniformValue("colorAxisY", colorAxisY);
    programAxis->setUniformValue("colorAxisZ", colorAxisZ);
    programAxis->setUniformValue("axisSize", axisSize);
    glDrawArrays(GL_POINTS, 0, 1);
    programAxis->release();
}

void Render::updateWindowSize(int w, int h)
{
    windowWidth = static_cast<float>(w);
    windowHeight = static_cast<float>(h);
}

void Render::updateMatrices()
{
    updateMatrixModel();
    updateMatrixView();
    updateMatrixProjection();
}

void Render::updateMatrixModel()
{
    matModel = matTranslationDrag * matTranslationModel * matRotationDrag * matRotationModel;
    // matModel.rotate(-90.0f, QVector3D(1.0f, 0, 0));
    // matModel.rotate(-90.0f, QVector3D(0, 0, 1.0f));

    matNormalMatrix = matModel.normalMatrix();
}

void Render::updateMatrixView()
{
    matView.setToIdentity();
    matView.lookAt(camera.position, camera.position + camera.front, camera.up);
}

void Render::updateMatrixProjection()
{
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 100.0f;

    matProjection.setToIdentity();
    matProjectionAxis.setToIdentity();
    if (usePerspective) {
        matProjection.perspective(camera.fov, aspectRatio(), nearPlane, farPlane);
        matProjectionAxis.perspective(camera.fov, aspectRatio(), nearPlane, farPlane);
    } else {
        matProjection.ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);
        float top = std::tan(degreeToRadian(camera.fov * 0.5f)) * initialCameraDistance;
        float bottom = -top;
        float left = aspectRatio() * bottom;
        float right = aspectRatio() * top;
        matProjectionAxis.ortho(left, right, bottom, top, nearPlane, farPlane);
    }
}

void Render::setUsePerspective(bool usePerspective)
{
    this->usePerspective = usePerspective;
    if (!usePerspective) {
        orthoTop = std::tan(degreeToRadian(camera.fov * 0.5f)) * camera.position.length();
        orthoBottom = -orthoTop;
        orthoLeft = aspectRatio() * orthoBottom;
        orthoRight = aspectRatio() * orthoTop;
    }
}

void Render::keyPressEvent(QKeyEvent* event)
{
    camera.keyPressEvent(event);
}

void Render::mousePressEvent(QMouseEvent* event)
{
    switch (event->button()) {
    case Qt::LeftButton:
        break;
    case Qt::MiddleButton:
        isMouseMiddleButtonPressed = true;
        positionLastMousePressMiddle = QVector2D(event->position());
        break;
    case Qt::RightButton:
        isMouseRightButtonPressed = true;
        positionLastMousePressRight = QVector2D(event->position());
        break;
    default:
        break;
    }
}

void Render::mouseMoveEvent(QMouseEvent* event)
{
    QMatrix4x4 matToWorld = (matProjection * matView).inverted();
    if (isMouseMiddleButtonPressed) {
        QVector2D directionMouseDrag = QVector2D(event->position()) - positionLastMousePressMiddle;
        QVector3D axis = (matToWorld.map(QVector3D(directionMouseDrag.y(), directionMouseDrag.x(), 0))).normalized();
        float angle = directionMouseDrag.length() / windowHeight * 90.0f;
        matRotationDrag.setToIdentity();
        matRotationDrag.rotate(angle, axis);
    } else if (isMouseRightButtonPressed) {
        QVector2D directionMouseDrag = QVector2D(event->position()) - positionLastMousePressRight;
        QVector3D delta = matToWorld.map(QVector3D(directionMouseDrag.x(), -directionMouseDrag.y(), 0)).normalized() * directionMouseDrag.length() / windowHeight * 2.0f;
        matTranslationDrag.setToIdentity();
        matTranslationDrag.translate(delta);
    }
}

void Render::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button()) {
    case Qt::LeftButton:
        break;
    case Qt::MiddleButton:
        isMouseMiddleButtonPressed = false;
        matRotationModel = matRotationDrag * matRotationModel;
        matRotationDrag.setToIdentity();
        break;
    case Qt::RightButton:
        isMouseRightButtonPressed = false;
        matTranslationModel = matTranslationDrag * matTranslationModel;
        matTranslationDrag.setToIdentity();
        break;
    default:
        break;
    }
}

void Render::wheelEvent(QWheelEvent* event)
{
    float numSteps = event->angleDelta().y() / 1200.0f;
    float x = event->position().x() / static_cast<float>(windowWidth) * 2.0f - 1.0f;
    float y = 1.0f - event->position().y() / static_cast<float>(windowHeight) * 2.0f;

    QMatrix4x4 matToWorld = (matProjection * matView).inverted();
    QVector3D direction = matToWorld.map(QVector3D(x, y, 1)).normalized();

    camera.position += direction * numSteps;
    float top = std::tan(degreeToRadian(camera.fov * 0.5f)) * camera.position.length();
    orthoTop = top + y * numSteps;
    orthoBottom = -top + y * numSteps;
    orthoLeft = orthoBottom * aspectRatio() + x * numSteps;
    orthoRight = orthoTop * aspectRatio() + x * numSteps;

    updateMatrixView();
    updateMatrixProjection();
}

void Render::initializeProgramAxis()
{
    programAxis = new QOpenGLShaderProgram();
    programAxis->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/Axis.vert");
    programAxis->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/Axis.geom");
    programAxis->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/Axis.frag");
    programAxis->link();
}
