#include "ObjectMilling.h"

#include "utils.h"

#include <QTemporaryDir>

ObjectMilling::ObjectMilling(QObject* parent)
    : QObject(parent)
{
}

void ObjectMilling::loadTestPentagramMilling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(100, 100, 100, true, { 0, 0, 0 }));
    Q_EMIT(addCutter(6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/pentagram.nc"));
}

void ObjectMilling::loadTest5AxisMilling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(100, 100, 100, true, { 0, 0, 0 }));
    Q_EMIT(addCutter(10.0f, 3.0f, 2.0f, 3.0f, 0.0f, 0.0f, 20.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/5axis.nc"));
}

void ObjectMilling::loadTestMultiCuttersMilling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(120, 120, 120, false, { 0, 0, 0 }));
    Q_EMIT(addCutter(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f));
    Q_EMIT(addCutter(10.0f, 5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 20.0f));
    Q_EMIT(addCutter(10.0f, 3.0f, 2.0f, 3.0f, 0.0f, 0.0f, 20.0f));
    Q_EMIT(addCutter(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, degreeToRadian(10.0f), 20.0f));
    Q_EMIT(addCutter(10.0f, 3.0f, 2.0f, 3.0f, 0.0f, degreeToRadian(10.0f), 20.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/multicutter.nc"));
}

void ObjectMilling::loadTestSaddleMilling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(130.0f, 90.0f, 40.0f, false, { 0, 0, 0 }));
    Q_EMIT(addCutter(6.0f, 3.0f, 0.0f, 3.0f, 0.0f, 0.0f, 200.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/saddle.nc"));
}

void ObjectMilling::loadTestBanlei5Milling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(200.0f, 118.0f, 19.0f, false, { -100, -59, -18.9f }));
    Q_EMIT(addCutter(6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 88.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/banlei5.nc"));
}

void ObjectMilling::loadTestBanleidoublebenlaiMilling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(130, 70, 30, false, { -65, -35, -29.5f }));
    Q_EMIT(addCutter(6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 88.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/banleidoublebenlai.nc"));
}

void ObjectMilling::loadTest20220618Milling()
{
    Q_EMIT(geometryMillingClear());

    Q_EMIT(addWorkpiece(155, 380, 11, false, { -75, -185, 11.0f }));
    Q_EMIT(addCutter(6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 88.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/20220618.nc"));
}

void ObjectMilling::loadTestBDDMilling()
{
    Q_EMIT(geometryMillingClear());

    QTemporaryDir tempDir;
    if (tempDir.isValid()) {
        const QString tempFile = tempDir.path() + "/BDD-blank.stl";
        if (QFile::copy(":/stls/BDD-blank.stl", tempFile)) {
            Q_EMIT(addWorkpieceFromFile(tempFile));
        }
    }

    Q_EMIT(addCutter(10.0f, 5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 10.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/BDD.nc"));
}

void ObjectMilling::loadTestVaseMilling()
{
    Q_EMIT(geometryMillingClear());

    QTemporaryDir tempDir;
    if (tempDir.isValid()) {
        const QString tempFile = tempDir.path() + "/vase-blank.stl";
        if (QFile::copy(":/stls/vase-blank.stl", tempFile)) {
            Q_EMIT(addWorkpieceFromFile(tempFile));
        }
    }

    Q_EMIT(addCutter(10.0f, 5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 100.0f));
    Q_EMIT(addNCProgramFromFile(":/ncs/vase.nc"));
}
