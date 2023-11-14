#include "DockWidget.h"

#include "TabWidgetProcedure.h"

DockWidget::DockWidget(const QString& title, QWidget* parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags)
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable);

    tabWidgetProcedure = new TabWidgetProcedure(this);
    setWidget(tabWidgetProcedure);
}
