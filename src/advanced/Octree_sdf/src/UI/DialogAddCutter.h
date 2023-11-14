#pragma once

#include <QDialog>

class GLWidgetCutter;

class QComboBox;
class QDialogButtonBox;
class QGridLayout;
class QDoubleSpinBox;
class QLabel;

class DialogAddCutter : public QDialog {
    Q_OBJECT

public:
    explicit DialogAddCutter(QWidget* parent = nullptr);

Q_SIGNALS:
    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h);
    void setCutter(float d, float r, float e, float f, float alpha, float beta, float h);

private:
    GLWidgetCutter* glWidgetCutter;

    QGridLayout* layout;
    QGridLayout* layoutInput;
    QComboBox* comboBoxType;
    QDoubleSpinBox* doubleSpinBoxD;
    QDoubleSpinBox* doubleSpinBoxR;
    QDoubleSpinBox* doubleSpinBoxE;
    QDoubleSpinBox* doubleSpinBoxF;
    QDoubleSpinBox* doubleSpinBoxAlpha;
    QDoubleSpinBox* doubleSpinBoxBeta;
    QDoubleSpinBox* doubleSpinBoxH;
    QDialogButtonBox* buttonBox;
    QLabel* labelImage;

    void setupComboBox();
    static void setupDoubleSpinBox(QDoubleSpinBox* doubleSpinBox, double min, double max, double value, const QString& suffix);
    void setupLayout();

    void comboBoxTypeIndexChanged(int index);
    void comboBoxTypeSelectAPT();
    void comboBoxTypeSelectBallEndmill();
    void comboBoxTypeSelectFilletEndmill();
    void comboBoxTypeSelectFilletTaperEndmill();
    void comboBoxTypeSelectFlatEndmill();
    void comboBoxTypeSelectTaperEndmill();
    void comboBoxTypeSelectTaperFilletEndmill();
    void comboBoxTypeSelectStrange();

private Q_SLOTS:
    void doubleSpinBoxDValueIsTwiceOfDoubleSpinBoxRValue(double value);
    void doubleSpinBoxFValueIsDoubleSpinBoxRValue(double value);
    void doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromD(double value);
    void doubleSpinBoxEValueIsHalfOfDoubleSpinBoxDValueMinusDoubleSpinBoxRValueFromR(double value);
};
