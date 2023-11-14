#include "ToolBarOthers.h"

#include "ChartMRR.h"
#include "DialogAutodiff.h"

#include <QAction>
#include <QIcon>

ToolBarOthers::ToolBarOthers(const QString& title, QWidget* parent)
    : QToolBar(title, parent)
{
    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    setMovable(true);
    setFloatable(false);
    setIconSize(QSize(32, 32));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    dialogAutodiff = new DialogAutodiff(this);

    actionAutodiff = new QAction(QIcon(":/icons/file-multiple.svg"), tr("Autodiff"), this);
    addAction(actionAutodiff);
    connect(actionAutodiff, &QAction::triggered, this, [this]() { dialogAutodiff->exec(); });

    actionChartMRR = new QAction(QIcon(":/icons/chart-bell-curve.svg"), tr("Chart MRR"), this);
    addAction(actionChartMRR);
    connect(actionChartMRR, &QAction::triggered, this, [this]() { Q_EMIT showChartMRR(); });
}
