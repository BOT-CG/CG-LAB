#include "ToolBarCreation.h"

#include "DialogAddCutter.h"
#include "DialogAddNCProgramManual.h"
#include "DialogAddWorkpieceManual.h"

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QToolButton>

ToolBarCreation::ToolBarCreation(const QString& title, QWidget* parent)
    : QToolBar(title, parent)
{
    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    setMovable(true);
    setFloatable(false);
    setIconSize(QSize(32, 32));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    dialogAddWorkpieceManual = new DialogAddWorkpieceManual(this);
    dialogAddCutter = new DialogAddCutter(this);
    dialogAddNCProgramManual = new DialogAddNCProgramManual(this);

    setupToolButtonAddWorkpiece();

    actionAddCutter = new QAction(QIcon(":/icons/pencil.svg"), tr("Add cutter"), this);
    addAction(actionAddCutter);
    connect(actionAddCutter, &QAction::triggered, this, &ToolBarCreation::addCutter);

    setupToolButtonAddNCProgram();
}

void ToolBarCreation::setupToolButtonAddWorkpiece()
{
    toolButtonAddWorkpiece = new QToolButton(this);
    menuAddWorkpiece = new QMenu(this);
    actionAddWorkpieceManual = new QAction(tr("Manual"), this);
    actionAddWorkpieceFromFile = new QAction(tr("From file"), this);
    menuAddWorkpiece->addAction(actionAddWorkpieceManual);
    menuAddWorkpiece->addAction(actionAddWorkpieceFromFile);
    toolButtonAddWorkpiece->setMenu(menuAddWorkpiece);
    toolButtonAddWorkpiece->setPopupMode(QToolButton::MenuButtonPopup);
    toolButtonAddWorkpiece->setDefaultAction(actionAddWorkpieceManual);
    toolButtonAddWorkpiece->setIcon(QIcon(":/icons/pyramid.svg"));
    toolButtonAddWorkpiece->setText(tr("Add workpiece"));
    toolButtonAddWorkpiece->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addWidget(toolButtonAddWorkpiece);
    connect(actionAddWorkpieceManual, &QAction::triggered, this, &ToolBarCreation::addWorkpieceManual);
    connect(actionAddWorkpieceFromFile, &QAction::triggered, this, &ToolBarCreation::addWorkpieceFromFile);
}

void ToolBarCreation::setupToolButtonAddNCProgram()
{
    toolButtonAddNCProgram = new QToolButton(this);
    menuAddNCProgram = new QMenu(this);
    actionAddNCProgramManual = new QAction(tr("Manual"), this);
    actionAddNCProgramFromFile = new QAction(tr("From file"), this);
    menuAddNCProgram->addAction(actionAddNCProgramManual);
    menuAddNCProgram->addAction(actionAddNCProgramFromFile);
    toolButtonAddNCProgram->setMenu(menuAddNCProgram);
    toolButtonAddNCProgram->setPopupMode(QToolButton::MenuButtonPopup);
    toolButtonAddNCProgram->setDefaultAction(actionAddNCProgramFromFile);
    toolButtonAddNCProgram->setIcon(QIcon(":/icons/application-brackets.svg"));
    toolButtonAddNCProgram->setText(tr("Add NC program"));
    toolButtonAddNCProgram->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addWidget(toolButtonAddNCProgram);
    connect(actionAddNCProgramManual, &QAction::triggered, this, &ToolBarCreation::addNCProgramManual);
    connect(actionAddNCProgramFromFile, &QAction::triggered, this, &ToolBarCreation::addNCProgramFromFile);
}

void ToolBarCreation::addWorkpieceManual() const
{
    dialogAddWorkpieceManual->exec();
}

void ToolBarCreation::addWorkpieceFromFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open STL file"), QDir::homePath(), tr("STL files (*.stl)"));
    if (!file.isEmpty()) {
        Q_EMIT(workpieceFromFileAdded(file));
    }
}

void ToolBarCreation::addCutter() const
{
    dialogAddCutter->exec();
}

void ToolBarCreation::addNCProgramManual() const
{
    dialogAddNCProgramManual->exec();
}

void ToolBarCreation::addNCProgramFromFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Add NC Program from File"), QDir::homePath(), tr("NC Program (*.nc *.txt)"));
    if (!file.isEmpty()) {
        Q_EMIT(NCProgramFromFileAdded(file));
    }
}
