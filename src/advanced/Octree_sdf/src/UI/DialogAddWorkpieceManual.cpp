#include "DialogAddWorkpieceManual.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVector3D>

#include <iostream>

DialogAddWorkpieceManual::DialogAddWorkpieceManual(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Add workpiece"));
    setMinimumSize(150, 200);

    comboBoxShape = new QComboBox(this);
    comboBoxShape->addItem(tr("Cuboid"));
    comboBoxShape->addItem(tr("Cube"));
    comboBoxShape->setCurrentIndex(0);

    checkBoxOriginCenter = new QCheckBox(this);

    spinBoxLength = new QSpinBox(this);
    spinBoxWidth = new QSpinBox(this);
    spinBoxHeight = new QSpinBox(this);
    setupSpinBox(spinBoxLength, 0, std::numeric_limits<int>::max(), 100);
    setupSpinBox(spinBoxWidth, 0, std::numeric_limits<int>::max(), 100);
    setupSpinBox(spinBoxHeight, 0, std::numeric_limits<int>::max(), 100);

    doubleSpinBoxPositionX = new QDoubleSpinBox(this);
    doubleSpinBoxPositionY = new QDoubleSpinBox(this);
    doubleSpinBoxPositionZ = new QDoubleSpinBox(this);
    setupDoubleSpinBox(doubleSpinBoxPositionX, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), 0);
    setupDoubleSpinBox(doubleSpinBoxPositionY, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), 0);
    setupDoubleSpinBox(doubleSpinBoxPositionZ, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), 0);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    setupLayout();

    connect(comboBoxShape, &QComboBox::currentIndexChanged, this, &DialogAddWorkpieceManual::comboBoxShapeIndexChanged);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DialogAddWorkpieceManual::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DialogAddWorkpieceManual::reject);

    connect(this, &DialogAddWorkpieceManual::accepted, this, [this]() {
        Q_EMIT(addWorkpiece(spinBoxLength->value(), spinBoxWidth->value(), spinBoxHeight->value(), checkBoxOriginCenter->isChecked(), QVector3D(doubleSpinBoxPositionX->value(), doubleSpinBoxPositionY->value(), doubleSpinBoxPositionZ->value())));
    });
}

void DialogAddWorkpieceManual::setupSpinBox(QSpinBox* spinBox, int min, int max, int value)
{
    spinBox->setRange(min, max);
    spinBox->setValue(value);
    spinBox->setSuffix(" mm");
}

void DialogAddWorkpieceManual::setupDoubleSpinBox(QDoubleSpinBox* spinBox, double min, double max, double value)
{
    spinBox->setRange(min, max);
    spinBox->setValue(value);
    spinBox->setSuffix(" mm");
    spinBox->setDecimals(2);
}

void DialogAddWorkpieceManual::setupLayout()
{
    layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Shape:"), this), 0, 0, 1, 1);
    layout->addWidget(comboBoxShape, 0, 1, 1, 3);
    layout->addWidget(new QLabel(tr("Length:"), this), 1, 0, 1, 1);
    layout->addWidget(spinBoxLength, 1, 1, 1, 3);
    layout->addWidget(new QLabel(tr("Width:"), this), 2, 0, 1, 1);
    layout->addWidget(spinBoxWidth, 2, 1, 1, 3);
    layout->addWidget(new QLabel(tr("Height:"), this), 3, 0, 1, 1);
    layout->addWidget(spinBoxHeight, 3, 1, 1, 3);
    layout->addWidget(new QLabel(tr("Origin at center:"), this), 4, 0, 1, 1);
    layout->addWidget(checkBoxOriginCenter, 4, 1, 1, 3);
    layout->addWidget(new QLabel(tr("Position:"), this), 5, 0, 1, 1);
    layout->addWidget(doubleSpinBoxPositionX, 5, 1, 1, 1);
    layout->addWidget(doubleSpinBoxPositionY, 5, 2, 1, 1);
    layout->addWidget(doubleSpinBoxPositionZ, 5, 3, 1, 1);
    layout->addWidget(buttonBox, 6, 0, 1, 4);
    setLayout(layout);
}

void DialogAddWorkpieceManual::comboBoxShapeIndexChanged(int index)
{
    switch (index) {
    case 0:
        spinBoxWidth->setEnabled(true);
        spinBoxHeight->setEnabled(true);
        disconnect(spinBoxLength, &QSpinBox::valueChanged, this, &DialogAddWorkpieceManual::spinBoxWidthValueIsSpinBoxLengthValue);
        disconnect(spinBoxLength, &QSpinBox::valueChanged, this, &DialogAddWorkpieceManual::spinBoxHeightValueIsSpinBoxLengthValue);
        break;
    case 1:
        spinBoxWidth->setEnabled(false);
        spinBoxHeight->setEnabled(false);
        spinBoxWidth->setValue(spinBoxLength->value());
        spinBoxHeight->setValue(spinBoxLength->value());
        connect(spinBoxLength, &QSpinBox::valueChanged, this, &DialogAddWorkpieceManual::spinBoxWidthValueIsSpinBoxLengthValue);
        connect(spinBoxLength, &QSpinBox::valueChanged, this, &DialogAddWorkpieceManual::spinBoxHeightValueIsSpinBoxLengthValue);
        break;
    default:
        break;
    }
}

void DialogAddWorkpieceManual::spinBoxWidthValueIsSpinBoxLengthValue(int value)
{
    spinBoxWidth->setValue(value);
}

void DialogAddWorkpieceManual::spinBoxHeightValueIsSpinBoxLengthValue(int value)
{
    spinBoxHeight->setValue(value);
}
