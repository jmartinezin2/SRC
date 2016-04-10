#ifndef IN2MANAGERDIAMETROS_H
#define IN2MANAGERDIAMETROS_H

#include <QObject>
#include "monitordiametro.h"


class In2ManagerDiametros : public QObject
{
    Q_OBJECT
public:
    static In2ManagerDiametros *request(QObject *parent=0);
    MonitorDiametro* getMonitor(int index);
    void initMonitor(int index);
    void storeValueFromPLC(int index,int value);

private:
    In2ManagerDiametros(QObject *parent = 0);

private:
    static In2ManagerDiametros *mInstance;
signals:

private:
    MonitorDiametro *m_MonitorIn;
    MonitorDiametro *m_MonitorOut;

    
public slots:
    
};

#endif // IN2MANAGERDIAMETROS_H
