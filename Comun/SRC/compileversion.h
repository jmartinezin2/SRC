#ifndef COMPILEVERSION_H
#define COMPILEVERSION_H

#include <QDialog>
#include "globals.h"
#include "ui_compileversion.h"

class CompileVersion : public QDialog
{
    Q_OBJECT
    
public:
    CompileVersion(QWidget *parent = 0);
    void setCompileVersion(QString txt);
private:

    ~CompileVersion();
    
private:

    Ui::CompileVersion *ui;

private slots:
    void Exit();
};

#endif // COMPILEVERSION_H
