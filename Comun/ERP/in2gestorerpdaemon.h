#ifndef IN2GESTORERPDAEMON_H
#define IN2GESTORERPDAEMON_H

#include <QObject>
#include "in2gestorerp.h"
#include <QStringList>

class In2GestorERPDaemon : public QObject
{
    Q_OBJECT
public:
    explicit In2GestorERPDaemon(QObject *parent = 0);
signals:

private:
    In2GestorERP *m_gestorERP;
    bool m_bMonitor;
    QStringList m_data;

private slots:
    void sltMonitor();

signals:
    void sgnERPDaemonDataChanged(const QStringList &data);

public slots:
    void sltSetMonitor(bool val);
    void sltEnableMonitor();
    void sltDisableMonitor();
    void sltCheckERP();
    
};

#endif // IN2GESTORERPDAEMON_H
