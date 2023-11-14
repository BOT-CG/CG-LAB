#pragma once

#include <QTabWidget>

class ListViewCutter;
class ListViewNCProgram;

class TabWidgetProcedure : public QTabWidget {
    Q_OBJECT
public:
    explicit TabWidgetProcedure(QWidget* parent = nullptr);

    void clearAll();
    void addCutter(float d, float r, float e, float f, float alpha, float beta, float h);
    void removeCutter(size_t i);
    void setNCCodeLines(const std::vector<std::string>& ncCodeLines);
    void highlightLine(int line);

private:
    ListViewCutter* listViewCutter;
    ListViewNCProgram* listViewNCProgram;
};
