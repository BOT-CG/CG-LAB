#include "MenuBar.h"

#include "Render.h"

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

MenuBar::MenuBar(QWidget* parent)
    : QMenuBar(parent)
{
    menuFile = addMenu(tr("&File"));
    menuLoadTestMilling = menuFile->addMenu(tr("&Load test milling"));
    actionLoadTestPentagramMilling = menuLoadTestMilling->addAction(tr("Pentagram milling"));
    actionLoadTest5AxisMilling = menuLoadTestMilling->addAction(tr("5-Axis milling"));
    actionLoadTestMultiCuttersMilling = menuLoadTestMilling->addAction(tr("Multi-cutters milling"));
    actionLoadTestSaddleMilling = menuLoadTestMilling->addAction(tr("Saddle milling"));
    actionLoadTestBanlei5Milling = menuLoadTestMilling->addAction(tr("Banlei5 milling"));
    actionLoadTestBanleidoublebenlaiMilling = menuLoadTestMilling->addAction(tr("Banleidoublebenlai milling"));
    actionLoadTest20220618Milling = menuLoadTestMilling->addAction(tr("20220618 milling"));
    actionLoadTestBDDMilling = menuLoadTestMilling->addAction(tr("BDD milling"));
    actionLoadTestVaseMilling = menuLoadTestMilling->addAction(tr("Vase milling"));

    actionSaveWorkpieceToSTLFile = menuFile->addAction(tr("&Save workpiece to STL file"));
    actionClearMilling = menuFile->addAction(tr("&Clear milling"));
    actionExit = menuFile->addAction(tr("E&xit"));

    menuEdit = addMenu(tr("&Edit"));
    actionSettings = menuEdit->addAction(tr("&Settings"));

    menuHelp = addMenu(tr("&Help"));
    actionAboutOpenGL = menuHelp->addAction(tr("About OpenGL"));
    actionAbout = menuHelp->addAction(tr("&About"));

    connect(actionLoadTestPentagramMilling, &QAction::triggered, this, &MenuBar::loadTestPentagramMilling);
    connect(actionLoadTest5AxisMilling, &QAction::triggered, this, &MenuBar::loadTest5AxisMilling);
    connect(actionLoadTestMultiCuttersMilling, &QAction::triggered, this, &MenuBar::loadTestMultiCuttersMilling);
    connect(actionLoadTestSaddleMilling, &QAction::triggered, this, &MenuBar::loadTestSaddleMilling);
    connect(actionLoadTestBanlei5Milling, &QAction::triggered, this, &MenuBar::loadTestBanlei5Milling);
    connect(actionLoadTestBanleidoublebenlaiMilling, &QAction::triggered, this, &MenuBar::loadTestBanleidoublebenlaiMilling);
    connect(actionLoadTest20220618Milling, &QAction::triggered, this, &MenuBar::loadTest20220618Milling);
    connect(actionLoadTestBDDMilling, &QAction::triggered, this, &MenuBar::loadTestBDDMilling);
    connect(actionLoadTestVaseMilling, &QAction::triggered, this, &MenuBar::loadTestVaseMilling);

    connect(actionSaveWorkpieceToSTLFile, &QAction::triggered, this, [this]() {
        QString file = QFileDialog::getSaveFileName(this, tr("Save workpiece to STL file"), QDir::homePath(), tr("STL files (*.stl)"));
        if (!file.isEmpty()) {
            Q_EMIT(saveWorkpieceToSTLFile(file));
        }
    });

    connect(actionClearMilling, &QAction::triggered, this, &MenuBar::clearMilling);

    connect(actionExit, &QAction::triggered, this, &MenuBar::exit);
    connect(actionSettings, &QAction::triggered, this, &MenuBar::settings);

    connect(actionAboutOpenGL, &QAction::triggered, this, &MenuBar::actionAboutOpenGLTriggered);
    connect(actionAbout, &QAction::triggered, this, &MenuBar::actionAboutTriggered);
}

void MenuBar::actionAboutOpenGLTriggered()
{
    QMessageBox::about(this, tr("About OpenGL"), QString::fromStdString(Render::getOpenGLInfo()));
}

void MenuBar::actionAboutTriggered()
{
    QMessageBox::aboutQt(this);
}
