#pragma once

#include <QDialog>

class QDialogButtonBox;
class QDoubleSpinBox;
class QGridLayout;
class QHBoxLayout;
class QLineEdit;
class QPushButton;

class DialogAutodiff : public QDialog {
    Q_OBJECT

public:
    explicit DialogAutodiff(QWidget* parent = nullptr);

Q_SIGNALS:
    void autodiff(const QString& file, double toleranceGouge, double toleranceExcess, const QColor& colorGouge, const QColor& colorExcess);

private:
    QGridLayout* layout;
    QHBoxLayout* layoutFile;
    QLineEdit* lineEditFile;
    QPushButton* buttonBrowse;
    QHBoxLayout* layoutToleranceGouge;
    QDoubleSpinBox* doubleSpinBoxToleranceGouge;
    QPushButton* buttonColorGouge;
    QHBoxLayout* layoutToleranceExcess;
    QDoubleSpinBox* doubleSpinBoxToleranceExcess;
    QPushButton* buttonColorExcess;
    QDialogButtonBox* buttonBox;

    static void setupDoubleSpinBox(QDoubleSpinBox* doubleSpinBox, double min, double max, double value, const QString& suffix);

    void setupLayoutFile();
    void setupLayoutToleranceGouge();
    void setupLayoutToleranceExcess();
    void setupLayout();

private Q_SLOTS:
    void browse();
    void getColorGouge();
    void getColorExcess();
};
