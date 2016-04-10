#ifndef ADDTRABAJO_H
#define ADDTRABAJO_H

#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QPushButton>
#include "inkjetcommon.h"
#include "ui_AddTrabajoUI.h"

class AddTrabajo : public QWidget
{
    Q_OBJECT
public:
    AddTrabajo(QWidget *parent,struct In2Trabajo  t);

private slots:
    void sltPb0();
    void sltPb1();
    void sltPb2();
    void sltPb3();
    void sltPb4();
    void sltPb5();
    void sltPb6();
    void sltPb7();
    void sltPb8();
    void sltPb9();
    void sltPbBack();
    void sltPbOk();
    void sltPbCancel();
    void sltSelectLabel(int,int);
    bool eventFilter(QObject *target, QEvent *event);
private:
    void addChar(QString c);
private:
    Ui::AddTrabajoUI *ui;
    struct In2Trabajo m_structTrabajo;
    QString m_buffer;
    QWidget *m_WidgetSelected;
    bool m_selected;
signals:
    void newTrabajo(const In2Trabajo &t);

public slots:
    
};

#endif // ADDTRABAJO_H
