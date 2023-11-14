#pragma once

#include <QMainWindow>

class DockWidget;
class GLWidget;
class MenuBar;
class ToolBarCreation;
class ToolBarAnimation;
class ToolBarVisible;
class ToolBarOthers;
class StatusBar;
class DialogSettings;
class ObjectMilling;
class ChartMRR;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

private:
    DockWidget* dockWidget;
    GLWidget* glWidget;
    MenuBar* menuBar;
    ToolBarCreation* toolBarCreation;
    ToolBarAnimation* toolBarAnimation;
    ToolBarVisible* toolBarVisible;
    ToolBarOthers* toolBarOthers;
    StatusBar* statusBar;
    DialogSettings* dialogSettings;
    ObjectMilling* objectMilling;
    ChartMRR* chartMRR;
};
