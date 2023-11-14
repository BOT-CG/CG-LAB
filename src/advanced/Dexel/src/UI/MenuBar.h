#pragma once

#include <QMenuBar>

class QMenu;
class QAction;

class MenuBar : public QMenuBar {
    Q_OBJECT
public:
    explicit MenuBar(QWidget* parent = nullptr);

Q_SIGNALS:
    void exit();
    void clearMilling();
    void saveWorkpieceToSTLFile(const QString& file);
    void loadTestPentagramMilling();
    void loadTest5AxisMilling();
    void loadTestMultiCuttersMilling();
    void loadTestSaddleMilling();
    void loadTestBanlei5Milling();
    void loadTestBanleidoublebenlaiMilling();
    void loadTest20220618Milling();
    void loadTestBDDMilling();
    void loadTestVaseMilling();
    void settings();

private:
    QMenu* menuFile;
    QMenu* menuLoadTestMilling;
    QMenu* menuEdit;
    QMenu* menuHelp;

    QAction* actionClearMilling;
    QAction* actionSaveWorkpieceToSTLFile;
    QAction* actionLoadTestPentagramMilling;
    QAction* actionLoadTest5AxisMilling;
    QAction* actionLoadTestMultiCuttersMilling;
    QAction* actionLoadTestSaddleMilling;
    QAction* actionLoadTestBanlei5Milling;
    QAction* actionLoadTestBanleidoublebenlaiMilling;
    QAction* actionLoadTest20220618Milling;
    QAction* actionLoadTestBDDMilling;
    QAction* actionLoadTestVaseMilling;
    QAction* actionExit;
    QAction* actionSettings;
    QAction* actionAboutOpenGL;
    QAction* actionAbout;

private Q_SLOTS:
    void actionAboutOpenGLTriggered();
    void actionAboutTriggered();
};
