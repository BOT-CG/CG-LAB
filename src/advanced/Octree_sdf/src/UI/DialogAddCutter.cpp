#include "DialogAddCutter.h"

#include "GLWidgetCutter.h"
#include "utils.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

DialogAddCutter::DialogAddCutter(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Add cutter"));
    setMinimumSize(600, 500);

    setupComboBox();

    doubleSpinBoxD = new QDoubleSpinBox(this);
    doubleSpinBoxR = new QDoubleSpinBox(this);
    doubleSpinBoxE = new QDoubleSpinBox(this);
    doubleSpinBoxF = new QDoubleSpinBox(this);
    doubleSpinBoxAlpha = new QDoubleSpinBox(this);
    doubleSpinBoxBeta = new QDoubleSpinBox(this);
    doubleSpinBoxH = new QDoubleSpinBox(this);

    setupDoubleSpinBox(doubleSpinBoxD, 0, 1000, 10, " mm");
    setupDoubleSpinBox(doubleSpinBoxR, 0, 1000, 0, " mm");
    setupDoubleSpinBox(doubleSpinBoxE, -1000, 1000, 0, " mm");
    setupDoubleSpinBox(doubleSpinBoxF, 0, 1000, 0, " mm");
    setupDoubleSpinBox(doubleSpinBoxAlpha, 0, 90, 0, " deg");
    setupDoubleSpinBox(doubleSpinBoxBeta, 0, 90, 0, " deg");
    setupDoubleSpinBox(doubleSpinBoxH, 0, 1000, 100, " mm");

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    labelImage = new QLabel(this);
    QPixmap pixmap(":/images/APT_cutter.png");
    labelImage->setPixmap(pixmap);
    labelImage->setScaledContents(true);
    labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    glWidgetCutter = new GLWidgetCutter(this);

    setupLayout();

    connect(comboBoxType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DialogAddCutter::comboBoxTypeIndexChanged);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DialogAddCutter::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DialogAddCutter::reject);

    connect(this, &DialogAddCutter::accepted, this, [this]() {
        Q_EMIT(addCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });

    connect(doubleSpinBoxD, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });
    connect(doubleSpinBoxE, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });
    connect(doubleSpinBoxF, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });
    connect(doubleSpinBoxAlpha, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });
    connect(doubleSpinBoxBeta, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });
    connect(doubleSpinBoxH, &QDoubleSpinBox::valueChanged, this, [this]() {
        Q_EMIT(setCutter(doubleSpinBoxD->value(), doubleSpinBoxR->value(), doubleSpinBoxE->value(), doubleSpinBoxF->value(), degreeToRadian(doubleSpinBoxAlpha->value()), degreeToRadian(doubleSpinBoxBeta->value()), doubleSpinBoxH->value()));
    });

    connect(this, &DialogAddCutter::setCutter, glWidgetCutter, &GLWidgetCutter::setCutter);
}

void DialogAddCutter::setupComboBox()
{
    comboBoxType = new QComboBox(this);
    comboBoxType->addItem(tr("APT"));
    comboBoxType->addItem(tr("Ball Endmill"));
    comboBoxType->addItem(tr("Fillet Endmill"));
    comboBoxType->addItem(tr("Fillet Taper Endmill"));
    comboBoxType->addItem(tr("Flat Endmill"));
    comboBoxType->addItem(tr("Taper Endmill"));
    comboBoxType->addItem(tr("Taper Fillet Endmill"));
    comboBoxType->addItem(tr("Strange"));
    comboBoxType->setCurrentIndex(0);
}

void DialogAddCutter::setupDoubleSpinBox(QDoubleSpinBox* doubleSpinBox, double min, double max, double value, const QString& suffix)
{
    doubleSpinBox->setRange(min, max);
    doubleSpinBox->setValue(value);
    doubleSpinBox->setSuffix(suffix);
}

void DialogAddCutter::setupLayout()
{
    layoutInput = new QGridLayout();
    layoutInput->addWidget(new QLabel(tr("Type:"), this), 0, 0);
    layoutInput->addWidget(comboBoxType, 0, 1);
    layoutInput->addWidget(new QLabel("d:", this), 1, 0);
    layoutInput->addWidget(doubleSpinBoxD, 1, 1);
    layoutInput->addWidget(new QLabel("r:", this), 2, 0);
    layoutInput->addWidget(doubleSpinBoxR, 2, 1);
    layoutInput->addWidget(new QLabel("e:", this), 3, 0);
    layoutInput->addWidget(doubleSpinBoxE, 3, 1);
    layoutInput->addWidget(new QLabel("f", this), 4, 0);
    layoutInput->addWidget(doubleSpinBoxF, 4, 1);
    layoutInput->addWidget(new QLabel("α:", this), 5, 0);
    layoutInput->addWidget(doubleSpinBoxAlpha, 5, 1);
    layoutInput->addWidget(new QLabel("β:", this), 6, 0);
    layoutInput->addWidget(doubleSpinBoxBeta, 6, 1);
    layoutInput->addWidget(new QLabel("h:", this), 7, 0);
    layoutInput->addWidget(doubleSpinBoxH, 7, 1);
    layoutInput->addWidget(buttonBox, 8, 0, 1, 2);

    layout = new QGridLayout(this);
    layout->addWidget(labelImage, 0, 0);
    layout->addLayout(layoutInput, 1, 0);
    layout->addWidget(glWidgetCutter, 0, 1, 2, 1);
    setLayout(layout);
}

void DialogAddCutter::comboBoxTypeIndexChanged(int index)
{
    doubleSpinBoxE->setRange(-1000, 1000);
    doubleSpinBoxAlpha->setRange(0, 90);
    doubleSpinBoxBeta->setRange(0, 90);

    disconnect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxDValueIsTwiceOfDoubleSpinBoxRValue);
    disconnect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxFValueIsDoubleSpinBoxRValue);
    disconnect(doubleSpinBoxD, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromD);
    disconnect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromR);

    switch (index) {
    case 0:
        comboBoxTypeSelectAPT();
        break;
    case 1:
        comboBoxTypeSelectBallEndmill();
        break;
    case 2:
        comboBoxTypeSelectFilletEndmill();
        break;
    case 3:
        comboBoxTypeSelectFilletTaperEndmill();
        break;
    case 4:
        comboBoxTypeSelectFlatEndmill();
        break;
    case 5:
        comboBoxTypeSelectTaperEndmill();
        break;
    case 6:
        comboBoxTypeSelectTaperFilletEndmill();
        break;
    case 7:
        comboBoxTypeSelectStrange();
        break;
    default:
        break;
    }
}

void DialogAddCutter::comboBoxTypeSelectAPT()
{
    doubleSpinBoxD->setEnabled(true);
    doubleSpinBoxR->setEnabled(true);
    doubleSpinBoxE->setEnabled(true);
    doubleSpinBoxF->setEnabled(true);
    doubleSpinBoxAlpha->setEnabled(true);
    doubleSpinBoxBeta->setEnabled(true);
    doubleSpinBoxH->setEnabled(true);
}

void DialogAddCutter::comboBoxTypeSelectBallEndmill()
{
    doubleSpinBoxD->setEnabled(false);
    doubleSpinBoxD->setValue(doubleSpinBoxR->value() * 2.0);
    doubleSpinBoxR->setEnabled(true);
    doubleSpinBoxE->setEnabled(false);
    doubleSpinBoxE->setValue(0);
    doubleSpinBoxF->setEnabled(false);
    doubleSpinBoxF->setValue(doubleSpinBoxR->value());
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setValue(0);
    doubleSpinBoxBeta->setEnabled(false);
    doubleSpinBoxBeta->setValue(0);
    doubleSpinBoxH->setEnabled(true);
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxDValueIsTwiceOfDoubleSpinBoxRValue);
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxFValueIsDoubleSpinBoxRValue);
}

void DialogAddCutter::comboBoxTypeSelectFilletEndmill()
{
    doubleSpinBoxD->setEnabled(true);
    doubleSpinBoxR->setEnabled(true);
    doubleSpinBoxE->setEnabled(false);
    doubleSpinBoxE->setValue(doubleSpinBoxD->value() / 2.0 - doubleSpinBoxR->value());
    doubleSpinBoxF->setEnabled(false);
    doubleSpinBoxF->setValue(doubleSpinBoxR->value());
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setValue(0);
    doubleSpinBoxBeta->setEnabled(false);
    doubleSpinBoxBeta->setValue(0);
    doubleSpinBoxH->setEnabled(true);
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxFValueIsDoubleSpinBoxRValue);
    connect(doubleSpinBoxD, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromD);
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromR);
}

void DialogAddCutter::comboBoxTypeSelectFilletTaperEndmill()
{
    doubleSpinBoxD->setEnabled(true);
    doubleSpinBoxR->setEnabled(true);
    doubleSpinBoxE->setEnabled(false);
    doubleSpinBoxE->setValue(doubleSpinBoxD->value() / 2.0 - doubleSpinBoxR->value());
    doubleSpinBoxF->setEnabled(false);
    doubleSpinBoxF->setValue(doubleSpinBoxR->value());
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setValue(0);
    doubleSpinBoxBeta->setEnabled(true);
    doubleSpinBoxBeta->setRange(0, 90);
    doubleSpinBoxH->setEnabled(true);
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxFValueIsDoubleSpinBoxRValue);
    connect(doubleSpinBoxD, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromD);
    connect(doubleSpinBoxR, &QDoubleSpinBox::valueChanged, this, &DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromR);
}

void DialogAddCutter::comboBoxTypeSelectFlatEndmill()
{
    doubleSpinBoxD->setEnabled(true);
    doubleSpinBoxR->setEnabled(false);
    doubleSpinBoxR->setValue(0);
    doubleSpinBoxE->setEnabled(false);
    doubleSpinBoxE->setValue(0);
    doubleSpinBoxF->setEnabled(false);
    doubleSpinBoxF->setValue(0);
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setValue(0);
    doubleSpinBoxBeta->setEnabled(false);
    doubleSpinBoxBeta->setValue(0);
    doubleSpinBoxH->setEnabled(true);
}

void DialogAddCutter::comboBoxTypeSelectTaperEndmill()
{
    doubleSpinBoxD->setEnabled(true);
    doubleSpinBoxR->setEnabled(false);
    doubleSpinBoxR->setValue(0);
    doubleSpinBoxE->setEnabled(false);
    doubleSpinBoxE->setValue(0);
    doubleSpinBoxF->setEnabled(false);
    doubleSpinBoxF->setValue(0);
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setValue(0);
    doubleSpinBoxBeta->setEnabled(true);
    doubleSpinBoxBeta->setRange(0, 90);
    doubleSpinBoxH->setEnabled(true);
}

void DialogAddCutter::comboBoxTypeSelectTaperFilletEndmill()
{
    doubleSpinBoxD->setEnabled(true);
    doubleSpinBoxR->setEnabled(true);
    doubleSpinBoxE->setEnabled(true);
    doubleSpinBoxE->setRange(-1000, 0);
    doubleSpinBoxF->setEnabled(true);
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setValue(0);
    doubleSpinBoxBeta->setEnabled(false);
    doubleSpinBoxBeta->setValue(0);
    doubleSpinBoxH->setEnabled(true);
}

void DialogAddCutter::comboBoxTypeSelectStrange()
{
    doubleSpinBoxD->setEnabled(false);
    doubleSpinBoxD->setValue(0);
    doubleSpinBoxR->setEnabled(false);
    doubleSpinBoxR->setValue(0);
    doubleSpinBoxE->setEnabled(false);
    doubleSpinBoxE->setValue(0);
    doubleSpinBoxF->setEnabled(false);
    doubleSpinBoxF->setValue(0);
    doubleSpinBoxAlpha->setEnabled(false);
    doubleSpinBoxAlpha->setRange(-1, 0);
    doubleSpinBoxAlpha->setValue(-1);
    doubleSpinBoxBeta->setEnabled(false);
    doubleSpinBoxBeta->setRange(-1, 0);
    doubleSpinBoxBeta->setValue(-1);
    doubleSpinBoxH->setEnabled(true);
}

void DialogAddCutter::doubleSpinBoxDValueIsTwiceOfDoubleSpinBoxRValue(double value)
{
    doubleSpinBoxD->setValue(value * 2.0);
}

void DialogAddCutter::doubleSpinBoxFValueIsDoubleSpinBoxRValue(double value)
{
    doubleSpinBoxF->setValue(value);
}

void DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromD(double value)
{
    doubleSpinBoxE->setValue(value / 2.0 - doubleSpinBoxR->value());
}

void DialogAddCutter::doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromR(double value)
{
    doubleSpinBoxE->setValue(doubleSpinBoxD->value() / 2.0 - value);
}
