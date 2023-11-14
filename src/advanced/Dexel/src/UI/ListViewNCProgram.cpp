#include "ListViewNCProgram.h"

#include <QStringListModel>

ListViewNCProgram::ListViewNCProgram(QWidget* parent)
    : QListView(parent)
{
    model = new QStringListModel(this);
    setModel(model);
}

void ListViewNCProgram::setNCCodeLines(const std::vector<std::string>& ncCodeLines)
{
    QStringList list;
    for (const auto& line : ncCodeLines) {
        list << QString::fromStdString(line);
    }
    model->setStringList(list);
}

void ListViewNCProgram::highlightLine(int line)
{
    QModelIndex index = model->index(line);
    scrollTo(index);
    selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void ListViewNCProgram::clear()
{
    model->setStringList(QStringList());
}
