#include "ToolBarAnimation.h"

#include <QIcon>

ToolBarAnimation::ToolBarAnimation(const QString& title, QWidget* parent)
    : QToolBar(title, parent)
{
    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    setMovable(true);
    setFloatable(false);
    setIconSize(QSize(48, 48));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    actionPlay = new QAction(QIcon(":/icons/play-circle.svg"), tr("Play"), this);
    actionPause = new QAction(QIcon(":/icons/pause-circle.svg"), tr("Pause"), this);
    actionBackToStart = new QAction(QIcon(":/icons/skip-previous-circle.svg"), tr("Back to start"), this);
    actionPlay->setEnabled(true);
    actionPause->setEnabled(false);
    actionBackToStart->setEnabled(true);
    connect(actionPlay, &QAction::triggered, this, &ToolBarAnimation::play);
    connect(actionPause, &QAction::triggered, this, &ToolBarAnimation::pause);
    connect(actionBackToStart, &QAction::triggered, this, &ToolBarAnimation::backToStart);
    addAction(actionPlay);
    addAction(actionPause);
    addAction(actionBackToStart);
}

void ToolBarAnimation::reset() const
{
    actionPlay->setEnabled(true);
    actionPause->setEnabled(false);
}

void ToolBarAnimation::play() const
{
    actionPlay->setEnabled(false);
    actionPause->setEnabled(true);
}

void ToolBarAnimation::pause() const
{
    actionPlay->setEnabled(true);
    actionPause->setEnabled(false);
}

void ToolBarAnimation::backToStart() const
{
    actionPlay->setEnabled(true);
    actionPause->setEnabled(false);
}
