#pragma once

#include <QToolBar>

class DialogAutodiff;

class QAction;

class ToolBarOthers : public QToolBar {
    Q_OBJECT
public:
    explicit ToolBarOthers(const QString& title, QWidget* parent = nullptr);

    DialogAutodiff* dialogAutodiff;

Q_SIGNALS:
    void showChartMRR();

private:
    QAction* actionAutodiff;
    QAction* actionChartMRR;
};
