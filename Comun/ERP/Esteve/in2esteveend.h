#ifndef IN2ESTEVEEND_H
#define IN2ESTEVEEND_H

#include <QWidget>
#include "ui_In2SGLClose.h"

class In2EsteveEnd : public QDialog
{
    Q_OBJECT
public:
    explicit In2EsteveEnd(QWidget *parent = 0);
    void setSGLUpdate(bool val);

private slots:
    void sltExit();
    void sltSaveAndExit();
signals:
    void sgnERPWorkClosed();
private:
    Ui::In2SGLClose *ui;
public slots:
    
};

#endif // IN2ESTEVEEND_H
