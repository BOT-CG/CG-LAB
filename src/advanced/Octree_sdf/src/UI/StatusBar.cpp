#include "StatusBar.h"

StatusBar::StatusBar(QWidget* parent)
    : QStatusBar(parent)
{
    showMessage(tr("Ready"));
}
