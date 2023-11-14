#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QVector3D>

#include <chrono>
#include <vector>

struct PointAndDistance;

class Render;
class RenderCWE;
class RenderAutodiff;
class GeometryMilling;
class NCProgram;

template <class T>
class Vector3D;
using Vector3Df = Vector3D<float>;

class QKeyEvent;
class QMouseEvent;
class QTimer;
class QWheelEvent;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~GLWidget() override;

public Q_SLOTS:
    void geometryMillingClear();
    void addWorkpiece(float length, float width, float height, bool isOriginAtCenter, const QVector3D& position);
    void addWorkpieceFromFile(const QString& file);
    void saveWorkpieceToSTLFile(const QString& file);
    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h);
    void addNCProgramFromFile(const QString& file);
    void animationPlay();
    void animationPause();
    void animationBackToStart();
    void visibleWorkpieceWireframe(bool visible);
    void visibleWorkpieceSurface(bool visible);
    void visibleCutter(bool visible);
    void setUsePerspective(bool usePerspective);
    void setRenderWorkpieceSurfaceUsingDistanceDirectly(bool value);
    void autodiff(const QString& file, double toleranceGouge, double toleranceExcess, const QColor& colorGouge, const QColor& colorExcess);
    void setRecordMRR(bool value);

Q_SIGNALS:
    void NCProgramAdded(const std::vector<std::string>& ncCodeLines);
    void NCProgramHighlightedLine(int line);
    void statusMessageChanged(const QString& text, int timeout = 0);
    void animationReset();
    void appendMRRPoint(double time, double mrr);

protected:
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QTimer* timerSubtract;
    QTimer* timerRender;

    std::chrono::time_point<std::chrono::high_resolution_clock> timeMillingStart;
    std::chrono::milliseconds durationForMillingPause = std::chrono::milliseconds(0);

    RenderCWE* renderCWE;
    RenderAutodiff* renderAutodiff;
    GeometryMilling* geometryMilling;

    // std::vector<Vector3Df> centers;
    // std::vector<float> sizes;
    std::vector<float> distances;
    std::vector<Vector3Df> vertices;
    std::vector<Vector3Df> normals;

    std::vector<PointAndDistance> pointDistance;
    float toleranceGouge;
    float toleranceExcess;
    QVector3D colorGouge;
    QVector3D colorExcess;

    bool showWorkpieceWireframe = true;
    bool showWorkpieceSurface = true;
    bool showCutter = true;
    bool renderWorkpieceSurfaceUsingDistanceDirectly = false;

    enum RenderState {
        None,
        CWE,
        Autodiff
    };
    RenderState renderState = RenderState::CWE;
    Render* render;

    void generateWorkpieceDataForRender();
    void updateWorkpiece();
    void calculateWorkpiece();
    void switchRenderState(RenderState renderState);
};
