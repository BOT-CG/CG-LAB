#pragma once

#include <QToolBar>

class QAction;

class ToolBarVisible : public QToolBar {
    Q_OBJECT
public:
    explicit ToolBarVisible(const QString& title, QWidget* parent = nullptr);

    QAction* actionWorkpieceWireframeVisible;
    QAction* actionWorkpieceSurfaceVisible;
    QAction* actionCutterVisible;
    QAction* actionUsePerspective;

private:
    void setupAction(QAction* action, const QString& iconOn, const QString& iconOff, const QString& text);
};
