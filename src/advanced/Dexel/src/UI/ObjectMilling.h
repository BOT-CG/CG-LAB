#pragma once

#include <QObject>
#include <QVector3D>

class ObjectMilling : public QObject {
    Q_OBJECT

public:
    explicit ObjectMilling(QObject* parent = nullptr);

public Q_SLOTS:
    void loadTestPentagramMilling();
    void loadTest5AxisMilling();
    void loadTestMultiCuttersMilling();
    void loadTestSaddleMilling();
    void loadTestBanlei5Milling();
    void loadTestBanleidoublebenlaiMilling();
    void loadTest20220618Milling();
    void loadTestBDDMilling();
    void loadTestVaseMilling();

Q_SIGNALS:
    void geometryMillingClear();
    void addWorkpiece(float length, float width, float height, bool isOriginAtCenter, const QVector3D& position);
    void addWorkpieceFromFile(const QString& file);
    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h);
    void addNCProgramFromFile(const QString& file);
};
