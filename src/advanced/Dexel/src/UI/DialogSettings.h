#pragma once

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QGridLayout;
class QSpinBox;

class DialogSettings : public QDialog {
    Q_OBJECT
public:
    explicit DialogSettings(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

Q_SIGNALS:
    void renderWorkpieceSurfaceUsingDistanceDirectlyChanged(bool value);

public Q_SLOTS:
    void accept() override;

private:
    QGridLayout* layout;
    QCheckBox* checkBoxUseSweptVolume;
    QSpinBox* spinBoxMaxLinePerExecute;
    QComboBox* comboBoxLanguage;
    QDoubleSpinBox* doubleSpinBoxCenterStep;
    QDoubleSpinBox* doubleSpinBoxDirectionStep;
    QCheckBox* checkBoxRenderWorkpieceSurfaceUsingDistanceDirectly;
    QDialogButtonBox* buttonBox;

    void setupLayout();
    static void setupDoubleSpinBox(QDoubleSpinBox* doubleSpinBox, double min, double max, double value, const QString& suffix);
};
