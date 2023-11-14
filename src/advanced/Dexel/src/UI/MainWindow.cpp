#include "MainWindow.h"

#include "ChartMRR.h"
#include "DialogAddCutter.h"
#include "DialogAddWorkpieceManual.h"
#include "DialogAutodiff.h"
#include "DialogSettings.h"
#include "DockWidget.h"
#include "GLWidget.h"
#include "MenuBar.h"
#include "ObjectMilling.h"
#include "StatusBar.h"
#include "TabWidgetProcedure.h"
#include "ToolBarAnimation.h"
#include "ToolBarCreation.h"
#include "ToolBarOthers.h"
#include "ToolBarVisible.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(tr("Milling Simulator"));

    dockWidget = new DockWidget(tr("Procedure"), this);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    glWidget = new GLWidget(this);
    setCentralWidget(glWidget);

    menuBar = new MenuBar(this);
    setMenuBar(menuBar);

    toolBarCreation = new ToolBarCreation(tr("Tool Bar Animation"), this);
    addToolBar(Qt::TopToolBarArea, toolBarCreation);

    toolBarAnimation = new ToolBarAnimation(tr("Tool Bar Creation"), this);
    addToolBar(Qt::TopToolBarArea, toolBarAnimation);

    toolBarVisible = new ToolBarVisible(tr("Tool Bar Visible"), this);
    addToolBar(Qt::TopToolBarArea, toolBarVisible);

    toolBarOthers = new ToolBarOthers(tr("Tool Bar Others"), this);
    addToolBar(Qt::TopToolBarArea, toolBarOthers);

    statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    dialogSettings = new DialogSettings(this);

    chartMRR = new ChartMRR();

    objectMilling = new ObjectMilling(this);
    connect(objectMilling, &ObjectMilling::geometryMillingClear, glWidget, &GLWidget::geometryMillingClear);
    connect(objectMilling, &ObjectMilling::geometryMillingClear, dockWidget->tabWidgetProcedure, &TabWidgetProcedure::clearAll);
    connect(objectMilling, &ObjectMilling::geometryMillingClear, chartMRR, &ChartMRR::clear);
    connect(objectMilling, &ObjectMilling::addWorkpiece, glWidget, &GLWidget::addWorkpiece);
    connect(objectMilling, &ObjectMilling::addWorkpieceFromFile, glWidget, &GLWidget::addWorkpieceFromFile);
    connect(objectMilling, &ObjectMilling::addCutter, glWidget, &GLWidget::addCutter);
    connect(objectMilling, &ObjectMilling::addCutter, dockWidget->tabWidgetProcedure, &TabWidgetProcedure::addCutter);
    connect(objectMilling, &ObjectMilling::addNCProgramFromFile, glWidget, &GLWidget::addNCProgramFromFile);

    connect(menuBar, &MenuBar::loadTestPentagramMilling, objectMilling, &ObjectMilling::loadTestPentagramMilling);
    connect(menuBar, &MenuBar::loadTest5AxisMilling, objectMilling, &ObjectMilling::loadTest5AxisMilling);
    connect(menuBar, &MenuBar::loadTestMultiCuttersMilling, objectMilling, &ObjectMilling::loadTestMultiCuttersMilling);
    connect(menuBar, &MenuBar::loadTestSaddleMilling, objectMilling, &ObjectMilling::loadTestSaddleMilling);
    connect(menuBar, &MenuBar::loadTestBanlei5Milling, objectMilling, &ObjectMilling::loadTestBanlei5Milling);
    connect(menuBar, &MenuBar::loadTestBanleidoublebenlaiMilling, objectMilling, &ObjectMilling::loadTestBanleidoublebenlaiMilling);
    connect(menuBar, &MenuBar::loadTest20220618Milling, objectMilling, &ObjectMilling::loadTest20220618Milling);
    connect(menuBar, &MenuBar::loadTestBDDMilling, objectMilling, &ObjectMilling::loadTestBDDMilling);
    connect(menuBar, &MenuBar::loadTestVaseMilling, objectMilling, &ObjectMilling::loadTestVaseMilling);
    connect(menuBar, &MenuBar::saveWorkpieceToSTLFile, glWidget, &GLWidget::saveWorkpieceToSTLFile);
    connect(menuBar, &MenuBar::clearMilling, objectMilling, &ObjectMilling::geometryMillingClear);
    connect(menuBar, &MenuBar::exit, this, &MainWindow::close);
    connect(menuBar, &MenuBar::settings, dialogSettings, &DialogSettings::show);

    connect(toolBarCreation->dialogAddWorkpieceManual, &DialogAddWorkpieceManual::addWorkpiece, objectMilling, &ObjectMilling::addWorkpiece);
    connect(toolBarCreation->dialogAddCutter, &DialogAddCutter::addCutter, objectMilling, &ObjectMilling::addCutter);
    connect(toolBarCreation, &ToolBarCreation::workpieceFromFileAdded, glWidget, &GLWidget::addWorkpieceFromFile);
    connect(toolBarCreation, &ToolBarCreation::NCProgramFromFileAdded, glWidget, &GLWidget::addNCProgramFromFile);

    connect(toolBarAnimation->actionPlay, &QAction::triggered, glWidget, &GLWidget::animationPlay);
    connect(toolBarAnimation->actionPause, &QAction::triggered, glWidget, &GLWidget::animationPause);
    connect(toolBarAnimation->actionBackToStart, &QAction::triggered, glWidget, &GLWidget::animationBackToStart);
    connect(toolBarAnimation->actionBackToStart, &QAction::triggered, chartMRR, &ChartMRR::clear);

    connect(toolBarVisible->actionWorkpieceWireframeVisible, &QAction::triggered, glWidget, &GLWidget::visibleWorkpieceWireframe);
    connect(toolBarVisible->actionWorkpieceSurfaceVisible, &QAction::triggered, glWidget, &GLWidget::visibleWorkpieceSurface);
    connect(toolBarVisible->actionCutterVisible, &QAction::triggered, glWidget, &GLWidget::visibleCutter);
    connect(toolBarVisible->actionUsePerspective, &QAction::triggered, glWidget, &GLWidget::setUsePerspective);

    connect(toolBarOthers->dialogAutodiff, &DialogAutodiff::autodiff, glWidget, &GLWidget::autodiff);
    connect(toolBarOthers, &ToolBarOthers::showChartMRR, chartMRR, &ChartMRR::show);

    connect(glWidget, &GLWidget::NCProgramAdded, dockWidget->tabWidgetProcedure, &TabWidgetProcedure::setNCCodeLines);
    connect(glWidget, &GLWidget::NCProgramHighlightedLine, dockWidget->tabWidgetProcedure, &TabWidgetProcedure::highlightLine);
    connect(glWidget, &GLWidget::statusMessageChanged, statusBar, &StatusBar::showMessage);
    connect(glWidget, &GLWidget::animationReset, toolBarAnimation, &ToolBarAnimation::reset);
    connect(glWidget, &GLWidget::appendMRRPoint, chartMRR, &ChartMRR::appendPoint);

    connect(dialogSettings, &DialogSettings::renderWorkpieceSurfaceUsingDistanceDirectlyChanged, glWidget, &GLWidget::setRenderWorkpieceSurfaceUsingDistanceDirectly);

    connect(chartMRR, &ChartMRR::setRecordMRR, glWidget, &GLWidget::setRecordMRR);
}
