#include "TabWidgetProcedure.h"

#include "ListViewCutter.h"
#include "ListViewNCProgram.h"

TabWidgetProcedure::TabWidgetProcedure(QWidget* parent)
    : QTabWidget(parent)
{
    listViewCutter = new ListViewCutter(this);
    listViewNCProgram = new ListViewNCProgram(this);

    addTab(listViewNCProgram, tr("NC Program"));
    addTab(listViewCutter, tr("Cutters"));
}

void TabWidgetProcedure::clearAll()
{
    listViewCutter->clear();
    listViewNCProgram->clear();
}

void TabWidgetProcedure::addCutter(float d, float r, float e, float f, float alpha, float beta, float h)
{
    listViewCutter->addCutter(d, r, e, f, alpha, beta, h);
}

void TabWidgetProcedure::removeCutter(size_t i)
{
    listViewCutter->removeCutter(i);
}

void TabWidgetProcedure::setNCCodeLines(const std::vector<std::string>& ncCodeLines)
{
    listViewNCProgram->setNCCodeLines(ncCodeLines);
}

void TabWidgetProcedure::highlightLine(int line)
{
    listViewNCProgram->highlightLine(line);
}
