#ifndef IN2MESSAGE_H
#define IN2MESSAGE_H

#include <QWidget>
#include "ui_In2MessageUI.h"

class In2Message : public QWidget
{
    Q_OBJECT
public:
    explicit In2Message(QWidget *parent = 0,QString txt="",int buttons=0);
    ~In2Message(void);

public slots:
    void sltUpdateProgressBar(int x);
    void sltUpdateMessage(QString txt);
    void sltDelete();
    void sltEnableAutomaticPB(int time);
    void sltDeleteLater(int msg);
signals:

private slots:
    void sltOK();
    void sltCancel();
    void sltAutoUpdatePB();

private:
    Ui::In2Message *ui;
    int m_autoPBTime;
    int m_autoPBStep;
    
public slots:
    
};

#endif // IN2MESSAGE_H
