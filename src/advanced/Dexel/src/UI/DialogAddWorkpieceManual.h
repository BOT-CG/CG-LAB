#pragma once

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QGridLayout;
class QDoubleSpinBox;
class QSpinBox;

class DialogAddWorkpieceManual : public QDialog {
    Q_OBJECT
public:
    explicit DialogAddWorkpieceManual(QWidget* parent = nullptr);

Q_SIGNALS:
    void addWorkpiece(float length, float width, float height, bool isOriginAtCenter, const QVector3D& position);

private:
    QGridLayout* layout;
    QComboBox* comboBoxShape;
    QCheckBox* checkBoxOriginCenter;
    QSpinBox* spinBoxLength;
    QSpinBox* spinBoxWidth;
    QSpinBox* spinBoxHeight;
    QDoubleSpinBox* doubleSpinBoxPositionX;
    QDoubleSpinBox* doubleSpinBoxPositionY;
    QDoubleSpinBox* doubleSpinBoxPositionZ;
    QDialogButtonBox* buttonBox;

    static void setupSpinBox(QSpinBox* spinBox, int min, int max, int value);
    static void setupDoubleSpinBox(QDoubleSpinBox* spinBox, double min, double max, double value);
    void setupLayout();

    void comboBoxShapeIndexChanged(int index);

private Q_SLOTS:
    void spinBoxWidthValueIsSpinBoxLengthValue(int value);
    void spinBoxHeightValueIsSpinBoxLengthValue(int value);
};
