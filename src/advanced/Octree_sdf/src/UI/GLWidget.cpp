#include "GLWidget.h"

#include "GeometryMilling.h"
#include "RenderAutodiff.h"
#include "RenderCWE.h"
#include "STLFile.h"
#include "Settings.h"
#include "Triangle3D.h"
#include "Vector3D.h"
#include "utils.h"

#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include <fstream>
#include <iomanip>
#include <sstream>

GLWidget::GLWidget(QWidget* parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    renderWorkpieceSurfaceUsingDistanceDirectly = Settings::renderWorkpieceSurfaceUsingDistanceDirectly();
    use5axis = Settings::use5axis();
    std::cout << "use5axis:" << use5axis << std::endl;
    renderCWE = new RenderCWE();
    renderAutodiff = new RenderAutodiff();
    switchRenderState(RenderState::CWE);
    geometryMilling = new GeometryMilling();

    timerSubtract = new QTimer(this);
    timerRender = new QTimer(this);
    connect(timerSubtract, &QTimer::timeout, this, QOverload<>::of(&GLWidget::updateWorkpiece));
    connect(timerRender, &QTimer::timeout, this, QOverload<>::of(&GLWidget::calculateWorkpiece));
}

GLWidget::~GLWidget()
{
    makeCurrent();
    renderCWE->destroy();
    renderAutodiff->destroy();
    delete renderCWE;
    delete renderAutodiff;
    delete geometryMilling;
    doneCurrent();
}

void GLWidget::geometryMillingClear()
{
    durationForMillingPause = std::chrono::milliseconds(0);
    geometryMilling->clear();
    centers.clear();
    sizes.clear();
    distances.clear();
    vertices.clear();
    normals.clear();
    calculateWorkpiece();
    renderCWE->clearCutter();
    Q_EMIT(animationReset());
}

void GLWidget::addWorkpiece(float length, float width, float height, bool isOriginAtCenter, const QVector3D& position)
{
    use5axis = Settings::use5axis();
    std::cout << "use5axis:" << use5axis << std::endl;
    geometryMilling->initializeWorkpiece(length, width, height, isOriginAtCenter, { position.x(), position.y(), position.z() });
    calculateWorkpiece();
}

void GLWidget::addWorkpieceFromFile(const QString& file)
{
    use5axis = Settings::use5axis();
    std::cout << "use5axis:" << use5axis << std::endl;
    auto timeStart = std::chrono::high_resolution_clock::now();

    std::vector<Triangle3Df> triangles;
    if (!STLFile::Load(file.toStdString(), triangles)) {
        Q_EMIT(statusMessageChanged(tr("Failed to load STL file")));
        return;
    }

    geometryMilling->initializeWorkpiece(triangles);
    calculateWorkpiece();

    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << duration.count() / 1000.0 << "s";
    Q_EMIT(statusMessageChanged(tr("Loaded STL file in %1").arg(oss.str().c_str())));
}

void GLWidget::saveWorkpieceToSTLFile(const QString& file)
{
    geometryMilling->saveWorkpieceToSTLFile(file.toStdString());
}

void GLWidget::addCutter(float d, float r, float e, float f, float alpha, float beta, float h)
{
    geometryMilling->addCutter(d, r, e, f, alpha, beta, h);
}

void GLWidget::addNCProgramFromFile(const QString& file)
{
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Q_EMIT(statusMessageChanged(tr("Open file failed")));
        return;
    }
    QTextStream in(&f);
    QString txt = in.readAll();
    geometryMilling->loadNCProgram(txt.toStdString());

    std::vector<std::string> ncCodeLines = geometryMilling->getNCCodeLines();
    Q_EMIT(NCProgramAdded(ncCodeLines));
}

void GLWidget::animationPlay()
{

    timeMillingStart = std::chrono::high_resolution_clock::now();
    timerSubtract->start(1);
    timerRender->start(1);
    RenderStatic = false;
    // std::cout << "generate static workpiece mesh" << std::endl;
}

void GLWidget::animationPause()
{
    auto timeNow = std::chrono::high_resolution_clock::now();
    durationForMillingPause += std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeMillingStart);
    timerSubtract->stop();
    timerRender->stop();
    calculateWorkpiece();

    RenderStatic = true;
    // geometryMilling->generateStaticWorkpieceMesh(vertices, normals);
    geometryMilling->fillStaticWorkpieceDistances(centers, sizes, distances);
    // std::cout << "generate static workpiece mesh" << std::endl;
}

void GLWidget::animationBackToStart()
{
    durationForMillingPause = std::chrono::milliseconds(0);
    timerSubtract->stop();
    timerRender->stop();
    geometryMilling->reset();
    calculateWorkpiece();
    renderCWE->clearCutter();
}

void GLWidget::visibleWorkpieceWireframe(bool visible)
{
    showWorkpieceWireframe = visible;
    update();
}

void GLWidget::visibleWorkpieceSurface(bool visible)
{
    showWorkpieceSurface = visible;
    update();
}

void GLWidget::visibleCutter(bool visible)
{
    showCutter = visible;
    update();
}

void GLWidget::setUsePerspective(bool usePerspective)
{
    this->renderCWE->setUsePerspective(usePerspective);
    update();
}

void GLWidget::setRenderWorkpieceSurfaceUsingDistanceDirectly(bool value)
{
    renderWorkpieceSurfaceUsingDistanceDirectly = value;
    calculateWorkpiece();
}




void GLWidget::updateWorkpiece()
{

    //     //计算时间
    // auto start = std::chrono::steady_clock::now();

    bool hasNext = geometryMilling->executeNCProgram();

    auto timeNow = std::chrono::high_resolution_clock::now();
    auto duration = durationForMillingPause + std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeMillingStart);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << duration.count() / 1000.0 << "s";
    std::string stringDuration = oss.str();

    std::vector<Vector3Df> verticesCutter, normalsCutter;
    if (geometryMilling->generateCurrentCutterMesh(verticesCutter, normalsCutter)) {
        makeCurrent();
        renderCWE->initializeCutter(verticesCutter, normalsCutter);
        doneCurrent();
    }

    if (!hasNext) {
        timerSubtract->stop();
        timerRender->stop();
        this->calculateWorkpiece();
        if (!RenderStatic) {
            RenderStatic = true;
            // geometryMilling->generateStaticWorkpieceMesh(vertices, normals);
            geometryMilling->fillStaticWorkpieceDistances(centers, sizes, distances);
        }

        Q_EMIT(statusMessageChanged(tr("Milling completed in ") + QString::fromStdString(stringDuration)));
        Q_EMIT(animationReset());
        return;
    }

    auto stringProgramLine = geometryMilling->getCurrentPosture().toString();
    int line = geometryMilling->getCurrentLine();

    Q_EMIT(NCProgramHighlightedLine(line));
    Q_EMIT(statusMessageChanged(tr("Current cutter posture: ") + QString::fromStdString(stringProgramLine) + " " + tr("Time elapsed: ") + QString::fromStdString(stringDuration)));


    // //计算时间
    // auto end_1 = std::chrono::steady_clock::now();
    // std::chrono::duration<double> elapsed_seconds = end_1 - start;
    // std::cout << "time for milling: " << elapsed_seconds * 1000.0 << std::endl;

}

void GLWidget::calculateWorkpiece()
{
    // std::cout << "calculate workpiece" << std::endl;

    generateWorkpieceDataForRender();
    switchRenderState(RenderState::CWE);
    update();
}

void GLWidget::generateWorkpieceDataForRender()
{
    if (renderWorkpieceSurfaceUsingDistanceDirectly) {
        geometryMilling->fillWorkpieceDistances(centers, sizes, distances);
        // std::cout << "fill workpiece distances" << std::endl;

    } else {
        if (RenderStatic) {
            // geometryMilling->generateWorkpieceMesh(centers, sizes, vertices, normals);
            geometryMilling->generateStaticWorkpieceMesh(vertices, normals);
            // std::cout << "generate static workpiece mesh" << std::endl;
        } else {
            geometryMilling->generateWorkpieceMesh(centers, sizes, vertices, normals);
            // std::cout << "generate workpiece mesh" << std::endl;
        }
    }
}

void GLWidget::switchRenderState(RenderState renderState)
{
    this->renderState = renderState;
    switch (renderState) {
    case RenderState::CWE:
        render = renderCWE;
        break;
    case RenderState::Autodiff:
        render = renderAutodiff;
        break;
    default:
        break;
    }
    update();
}

void GLWidget::initializeGL()
{
    renderCWE->initialize();
    renderAutodiff->initialize();
    generateWorkpieceDataForRender();
}

void GLWidget::paintGL()
{
    render->updateWindowSize(width(), height());
    render->updateMatrices();
    render->clearColorBuffer();
    render->drawAxis();

    switch (renderState) {
    case RenderState::CWE: {
        if (showCutter) {
            auto currentPosture = geometryMilling->getCurrentPostureGL();
            renderCWE->drawCutter(currentPosture.center, currentPosture.direction);
        }

        if (showWorkpieceSurface) {
            if (renderWorkpieceSurfaceUsingDistanceDirectly) {
                renderCWE->drawWorkpieceSurfaceUsingDistance(centers, sizes, distances);
                // std::cout << "draw workpiece surface using distance" << std::endl;
            } else {
                renderCWE->drawWorkpieceSurface(vertices, normals);
                // std::cout << "draw workpiece surface" << std::endl;
            }
        }

        if (showWorkpieceWireframe) {
            renderCWE->drawWorkpieceWireframe(centers, sizes);
        }
        break;
    }
    case RenderState::Autodiff:
        renderAutodiff->draw(pointDistance, normals, toleranceGouge, toleranceExcess, colorGouge, colorExcess);
        break;
    default:
        break;
    }
}

void GLWidget::resizeGL(int w, int h)
{
    render->updateWindowSize(width(), height());
    render->updateMatrixProjection();
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    render->keyPressEvent(event);

    if (event->key() == Qt::Key_I) {
        // workpiece->print();
    }

    update();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    render->mousePressEvent(event);
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    render->mouseMoveEvent(event);
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    render->mouseReleaseEvent(event);
    update();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    render->wheelEvent(event);
    update();
}
