#pragma once

#include <QListView>

class QStringListModel;

class ListViewCutter : public QListView {
    Q_OBJECT
public:
    explicit ListViewCutter(QWidget* parent = nullptr);

    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h);
    void removeCutter(size_t i);
    void clear();

private:
    QStringListModel* model;
};