#ifndef CHILLER_H
#define CHILLER_H

#include <QWidget>
#include "ui_Chiller.h"

class Chiller : public QWidget
{
    Q_OBJECT
public:
   Chiller(QWidget *parent = 0);
   ~Chiller();
    
signals:

private slots:
   void sltExit();
   void sltStartChiller();
   void sltStopChiller();
   void sltChangeStatus(int x);
   void sltChangeAlarm1(int x);
   void sltChangeAlarm2(int x);
   void sltChangeAlarm3(int x);
   void sltChangeTemperature(int x);
   void sltChangeSetTemperature(int x);
   void sltChangePresion(int x);


public slots:


private:
    Ui::ChillerUI *ui;
    QWidget *rtnScreen;
    quint16 m_lastStatus;
    quint16 m_lastAlarm1;
    quint16 m_lastAlarm2;
    quint16 m_lastAlarm3;

};

#endif // CHILLER_H
