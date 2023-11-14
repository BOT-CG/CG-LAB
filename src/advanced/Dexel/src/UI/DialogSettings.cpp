#include "DialogSettings.h"

#include "ObjectLanguage.h"
#include "Parameter.h"
#include "Settings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

DialogSettings::DialogSettings(QWidget* parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(200, 150);
    setModal(true);

    const float centerStep = Settings::centerStep();
    const float directionStep = Settings::directionStep();
    const bool useSweptVolume = Settings::useSweptVolume();
    const int maxLinePerExecute = Settings::maxLinePerExecute();
    Parameter::getInstance()->setCenterStep(centerStep);
    Parameter::getInstance()->setDirectionStep(directionStep);
    Parameter::getInstance()->setUseSweptVolume(useSweptVolume);
    Parameter::getInstance()->setMaxLinePerExecute(maxLinePerExecute);

    const QStringList qmFiles = ObjectLanguage::findQmFiles();
    const QString currentLanguage = Settings::language();

    comboBoxLanguage = new QComboBox(this);
    for (const QString& qmFile : qmFiles) {
        const QString languageName = ObjectLanguage::name(qmFile);
        const QString languageCode = ObjectLanguage::code(qmFile);
        comboBoxLanguage->addItem(languageName, languageCode);
        if (ObjectLanguage::match(currentLanguage, qmFile)) {
            comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
        }
    }

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    checkBoxUseSweptVolume = new QCheckBox(this);
    checkBoxUseSweptVolume->setCheckState(useSweptVolume ? Qt::Checked : Qt::Unchecked);

    doubleSpinBoxCenterStep = new QDoubleSpinBox(this);
    doubleSpinBoxDirectionStep = new QDoubleSpinBox(this);

    setupDoubleSpinBox(doubleSpinBoxCenterStep, 0.1, 100.0, Settings::centerStep(), " mm");
    setupDoubleSpinBox(doubleSpinBoxDirectionStep, 0.1, 100.0, Settings::directionStep(), " deg");

    spinBoxMaxLinePerExecute = new QSpinBox(this);
    spinBoxMaxLinePerExecute->setMinimum(1);
    spinBoxMaxLinePerExecute->setMaximum(1024);
    spinBoxMaxLinePerExecute->setValue(maxLinePerExecute);

    checkBoxRenderWorkpieceSurfaceUsingDistanceDirectly = new QCheckBox(this);
    checkBoxRenderWorkpieceSurfaceUsingDistanceDirectly->setCheckState(Settings::renderWorkpieceSurfaceUsingDistanceDirectly() ? Qt::Checked : Qt::Unchecked);

    setupLayout();

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DialogSettings::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DialogSettings::reject);
}

void DialogSettings::accept()
{
    const QString language = comboBoxLanguage->currentData().toString();
    const float centerStep = doubleSpinBoxCenterStep->value();
    const float directionStep = doubleSpinBoxDirectionStep->value();
    const int maxLinePerExecute = spinBoxMaxLinePerExecute->value();
    const bool renderWorkpieceSurfaceUsingDistanceDirectly = checkBoxRenderWorkpieceSurfaceUsingDistanceDirectly->checkState() == Qt::Checked;
    const bool useSweptVolume = checkBoxUseSweptVolume->checkState() == Qt::Checked;

    Settings::setLanguage(language);
    Settings::setUseSweptVolume(useSweptVolume);
    Settings::setCenterStep(centerStep);
    Settings::setDirectionStep(directionStep);
    Settings::setMaxLinePerExecute(maxLinePerExecute);
    Settings::setRenderWorkpieceSurfaceUsingDistanceDirectly(renderWorkpieceSurfaceUsingDistanceDirectly);

    Parameter::getInstance()->setUseSweptVolume(useSweptVolume);
    Parameter::getInstance()->setCenterStep(centerStep);
    Parameter::getInstance()->setDirectionStep(directionStep);
    Parameter::getInstance()->setMaxLinePerExecute(maxLinePerExecute);
    Q_EMIT(renderWorkpieceSurfaceUsingDistanceDirectlyChanged(renderWorkpieceSurfaceUsingDistanceDirectly));

    QDialog::accept();
}

void DialogSettings::setupLayout()
{
    layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Changing the language requires a restart of the application"), this), 0, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Language:"), this), 1, 0);
    layout->addWidget(comboBoxLanguage, 1, 1);
    layout->addWidget(new QLabel(tr("Use swept volume:"), this), 2, 0);
    layout->addWidget(checkBoxUseSweptVolume, 2, 1);
    layout->addWidget(new QLabel(tr("Center step:"), this), 3, 0);
    layout->addWidget(doubleSpinBoxCenterStep, 3, 1);
    layout->addWidget(new QLabel(tr("Direction step:"), this), 4, 0);
    layout->addWidget(doubleSpinBoxDirectionStep, 4, 1);
    layout->addWidget(new QLabel(tr("Max line per execute:"), this), 5, 0);
    layout->addWidget(spinBoxMaxLinePerExecute, 5, 1);
    layout->addWidget(new QLabel(tr("Render workpiece surface using distance directly:"), this), 6, 0);
    layout->addWidget(checkBoxRenderWorkpieceSurfaceUsingDistanceDirectly, 6, 1);
    layout->addWidget(buttonBox, 7, 0, 1, 2);
    setLayout(layout);
}

void DialogSettings::setupDoubleSpinBox(QDoubleSpinBox* doubleSpinBox, double min, double max, double value, const QString& suffix)
{
    doubleSpinBox->setMinimum(min);
    doubleSpinBox->setMaximum(max);
    doubleSpinBox->setValue(value);
    doubleSpinBox->setSuffix(suffix);
}
