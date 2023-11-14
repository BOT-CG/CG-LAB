#include "DialogAutodiff.h"

#include <QColorDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

DialogAutodiff::DialogAutodiff(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Autodiff"));
    setMinimumSize(150, 200);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    setupLayout();

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DialogAutodiff::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DialogAutodiff::reject);

    connect(this, &DialogAutodiff::accepted, this, [this]() {
        Q_EMIT(autodiff(lineEditFile->text(), doubleSpinBoxToleranceGouge->value(), doubleSpinBoxToleranceExcess->value(), buttonColorGouge->palette().color(QPalette::Button), buttonColorExcess->palette().color(QPalette::Button)));
    });
}

void DialogAutodiff::setupDoubleSpinBox(QDoubleSpinBox* doubleSpinBox, double min, double max, double value, const QString& suffix)
{
    doubleSpinBox->setRange(min, max);
    doubleSpinBox->setSuffix(suffix);
    doubleSpinBox->setSingleStep(0.001);
    doubleSpinBox->setDecimals(5);
    doubleSpinBox->setValue(value);
}

void DialogAutodiff::setupLayoutFile()
{
    layoutFile = new QHBoxLayout();
    lineEditFile = new QLineEdit(this);
    buttonBrowse = new QPushButton(tr("Browse"), this);
    layoutFile->addWidget(new QLabel(tr("Design model:"), this));
    layoutFile->addWidget(lineEditFile);
    layoutFile->addWidget(buttonBrowse);
    connect(buttonBrowse, &QPushButton::clicked, this, &DialogAutodiff::browse);
}

void DialogAutodiff::setupLayoutToleranceGouge()
{
    layoutToleranceGouge = new QHBoxLayout();
    doubleSpinBoxToleranceGouge = new QDoubleSpinBox(this);
    setupDoubleSpinBox(doubleSpinBoxToleranceGouge, 0.0, 10.0, 0.001, " mm");
    buttonColorGouge = new QPushButton(this);
    buttonColorGouge->setStyleSheet("background-color: red;");
    layoutToleranceGouge->addWidget(new QLabel(tr("Tolerance gouge:"), this));
    layoutToleranceGouge->addWidget(doubleSpinBoxToleranceGouge);
    layoutToleranceGouge->addWidget(buttonColorGouge);
    connect(buttonColorGouge, &QPushButton::clicked, this, &DialogAutodiff::getColorGouge);
}

void DialogAutodiff::setupLayoutToleranceExcess()
{
    layoutToleranceExcess = new QHBoxLayout();
    doubleSpinBoxToleranceExcess = new QDoubleSpinBox(this);
    setupDoubleSpinBox(doubleSpinBoxToleranceExcess, 0.0, 10.0, 0.001, " mm");
    buttonColorExcess = new QPushButton(this);
    buttonColorExcess->setStyleSheet("background-color: blue;");
    layoutToleranceExcess->addWidget(new QLabel(tr("Tolerance excess:"), this));
    layoutToleranceExcess->addWidget(doubleSpinBoxToleranceExcess);
    layoutToleranceExcess->addWidget(buttonColorExcess);
    connect(buttonColorExcess, &QPushButton::clicked, this, &DialogAutodiff::getColorExcess);
}

void DialogAutodiff::setupLayout()
{
    setupLayoutFile();
    setupLayoutToleranceGouge();
    setupLayoutToleranceExcess();
    layout = new QGridLayout(this);
    layout->addLayout(layoutFile, 0, 0, 1, 2);
    layout->addLayout(layoutToleranceGouge, 1, 0, 1, 2);
    layout->addLayout(layoutToleranceExcess, 2, 0, 1, 2);
    layout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(layout);
}

void DialogAutodiff::browse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open design model"), QDir::homePath(), tr("Design model (*.stl)"));
    if (!fileName.isEmpty()) {
        lineEditFile->setText(fileName);
    }
}

void DialogAutodiff::getColorGouge()
{
    QColor color = QColorDialog::getColor(Qt::red, this, tr("Select color"));
    if (color.isValid()) {
        buttonColorGouge->setStyleSheet(QString("background-color: %1").arg(color.name()));
    }
}

void DialogAutodiff::getColorExcess()
{
    QColor color = QColorDialog::getColor(Qt::blue, this, tr("Select color"));
    if (color.isValid()) {
        buttonColorExcess->setStyleSheet(QString("background-color: %1").arg(color.name()));
    }
}
