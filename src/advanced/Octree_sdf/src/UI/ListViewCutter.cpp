#include "ListViewCutter.h"

#include "CutterFactory.h"

#include <QStringListModel>

ListViewCutter::ListViewCutter(QWidget* parent)
    : QListView(parent)
{
    model = new QStringListModel(this);
    setModel(model);
}

void ListViewCutter::addCutter(float d, float r, float e, float f, float alpha, float beta, float h)
{
    static const std::map<CutterFactory::Type, QString> cutterTypeMap = {
        { CutterFactory::Type::APT, tr("APT") },
        { CutterFactory::Type::BallEndmill, tr("Ball Endmill") },
        { CutterFactory::Type::FilletEndmill, tr("Fillet Endmill") },
        { CutterFactory::Type::FilletTaperEndmill, tr("Fillet Taper Endmill") },
        { CutterFactory::Type::FlatEndmill, tr("Flat Endmill") },
        { CutterFactory::Type::TaperEndmill, tr("Taper Endmill") },
        { CutterFactory::Type::TaperFilletEndmill, tr("Taper Fillet Endmill") },
        { CutterFactory::Type::Strange, tr("Strange") },
    };

    CutterFactory::Type t = CutterFactory::getType(d, r, e, f, alpha, beta, h);
    QStringList list = model->stringList();
    list.append(cutterTypeMap.at(t));
    model->setStringList(list);
}

void ListViewCutter::removeCutter(size_t i)
{
    QStringList list = model->stringList();
    list.removeAt(i);
    model->setStringList(list);
}

void ListViewCutter::clear()
{
    model->setStringList(QStringList());
}
