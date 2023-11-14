#pragma once

#include "Filter.h"

#include <QChart>
#include <QChartView>
#include <QCheckBox>
#include <QGridLayout>
#include <QLineSeries>
#include <QPushButton>
#include <QWidget>

#include <vector>

class ChartMRR : public QWidget {
    Q_OBJECT

public:
    explicit ChartMRR(QWidget* parent = nullptr);

public Q_SLOTS:
    void appendPoint(double time, double mrr);
    void clear();

Q_SIGNALS:
    void setRecordMRR(bool value);

private:
    void setup();

    QChartView* chartView;
    QChart* chart;
    QLineSeries* series;
    QCheckBox* checkBox;
    QGridLayout* layout;
    QPushButton* buttonDisplayAll;

    double minY = 0, maxY = 1, maxX = 1;
    double lastTime = 0;
    Filter<double> filter = Filter<double>(10, true);
};
