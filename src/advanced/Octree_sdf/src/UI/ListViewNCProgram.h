#pragma once

#include <QListView>

class QStringListModel;

class ListViewNCProgram : public QListView {
    Q_OBJECT
public:
    explicit ListViewNCProgram(QWidget* parent = nullptr);

    void setNCCodeLines(const std::vector<std::string>& ncCodeLines);
    void highlightLine(int line);
    void clear();

private:
    QStringListModel* model;
};
