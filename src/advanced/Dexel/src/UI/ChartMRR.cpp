#include "ChartMRR.h"

#include <QToolTip>

ChartMRR::ChartMRR(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Chart MRR"));
    setMinimumSize(600, 400);

    chartView = new QChartView(this);
    chart = new QChart();
    series = new QLineSeries(this);
    series->setName(tr("MRR"));

    checkBox = new QCheckBox(tr("Record MRR"), this);
    checkBox->setChecked(false);

    buttonDisplayAll = new QPushButton(tr("Display all"), this);
    connect(buttonDisplayAll, &QPushButton::clicked, [this]() {
        chart->axisX()->setRange(0, maxX);
        chart->axisY()->setRange(minY, maxY);
    });

    layout = new QGridLayout(this);
    layout->addWidget(checkBox, 0, 0);
    layout->addWidget(chartView, 1, 0);
    layout->addWidget(buttonDisplayAll, 2, 0);
    connect(checkBox, &QCheckBox::stateChanged, this, &ChartMRR::setRecordMRR);

    setup();
}

void ChartMRR::appendPoint(double time, double mrr)
{
    filter.append(mrr);
    mrr = filter.getMedian();

    minY = std::min(minY, mrr);
    maxY = std::max(maxY, mrr);
    maxX = std::max(maxX, time);
    chart->axisY()->setRange(minY, maxY);
    chart->axisX()->setRange(0, time);

    series->append(lastTime, mrr);
    series->append(time, mrr);
    lastTime = time;
}

void ChartMRR::clear()
{
    series->clear();
    minY = 0;
    maxY = 1;
    maxX = 1;
    chart->axisY()->setRange(minY, maxY);
    chart->axisX()->setRange(0, 1);
    filter.clear();
}

void ChartMRR::setup()
{
    chart->addSeries(series);
    chart->legend()->hide();
    chart->createDefaultAxes();
    chart->axisX()->setTitleText(tr("Time, s"));
    chart->axisY()->setTitleText(tr("MRR, mm^3/min"));
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
}
