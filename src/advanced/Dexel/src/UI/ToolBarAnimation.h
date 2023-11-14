#pragma once

#include <QToolBar>

class QAction;

class ToolBarAnimation : public QToolBar {
    Q_OBJECT
public:
    explicit ToolBarAnimation(const QString& title, QWidget* parent = nullptr);

    QAction* actionPlay;
    QAction* actionPause;
    QAction* actionBackToStart;

public Q_SLOTS:
    void reset() const;

private:
    void play() const;
    void pause() const;
    void backToStart() const;
};
