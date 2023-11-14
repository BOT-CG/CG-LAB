#include "ToolBarVisible.h"

#include <QAction>

ToolBarVisible::ToolBarVisible(const QString& title, QWidget* parent)
    : QToolBar(title, parent)
{
    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    setMovable(true);
    setFloatable(false);
    setIconSize(QSize(48, 48));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    actionWorkpieceWireframeVisible = new QAction(this);
    setupAction(actionWorkpieceWireframeVisible, ":/icons/cube-outline.svg", ":/icons/cube-off-outline.svg", tr("Workpiece wireframe visible"));
    actionWorkpieceSurfaceVisible = new QAction(this);
    setupAction(actionWorkpieceSurfaceVisible, ":/icons/cube.svg", ":/icons/cube-off.svg", tr("Workpiece surface visible"));
    actionCutterVisible = new QAction(this);
    setupAction(actionCutterVisible, ":/icons/box-cutter.svg", ":/icons/box-cutter-off.svg", tr("Cutter visible"));
    actionUsePerspective = new QAction(this);
    setupAction(actionUsePerspective, ":/icons/road-variant.svg", ":/icons/road-variant.svg", tr("Use perspective"));
}

void ToolBarVisible::setupAction(QAction* action, const QString& iconOn, const QString& iconOff, const QString& text)
{
    action->setCheckable(true);
    action->setChecked(true);
    action->setIcon(QIcon(iconOn));
    action->setText(text);
    addAction(action);
    connect(action, &QAction::triggered, this, [action, iconOn, iconOff](bool checked) {
        if (checked) {
            action->setIcon(QIcon(iconOn));
        } else {
            action->setIcon(QIcon(iconOff));
        }
    });
}
