#pragma once

#include <QToolBar>

class DialogAddWorkpieceManual;
class DialogAddCutter;
class DialogAddNCProgramManual;

class QAction;
class QMenu;
class QToolButton;

class ToolBarCreation : public QToolBar {
    Q_OBJECT

public:
    explicit ToolBarCreation(const QString& title, QWidget* parent = nullptr);

    DialogAddWorkpieceManual* dialogAddWorkpieceManual;
    DialogAddCutter* dialogAddCutter;
    DialogAddNCProgramManual* dialogAddNCProgramManual;

Q_SIGNALS:
    void workpieceFromFileAdded(const QString& file);
    void NCProgramFromFileAdded(const QString& file);

private:
    QToolButton* toolButtonAddWorkpiece;
    QToolButton* toolButtonAddNCProgram;
    QMenu* menuAddWorkpiece;
    QMenu* menuAddNCProgram;
    QAction* actionAddWorkpieceManual;
    QAction* actionAddWorkpieceFromFile;
    QAction* actionAddCutter;
    QAction* actionAddNCProgramManual;
    QAction* actionAddNCProgramFromFile;

    void setupToolButtonAddWorkpiece();
    void setupToolButtonAddNCProgram();
    void addWorkpieceManual() const;
    void addWorkpieceFromFile();
    void addCutter() const;
    void addNCProgramManual() const;
    void addNCProgramFromFile();
};
