#pragma once

#include <QDockWidget>

class TabWidgetProcedure;

class DockWidget : public QDockWidget {
    Q_OBJECT
public:
    explicit DockWidget(const QString& title, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    TabWidgetProcedure* tabWidgetProcedure;
};
