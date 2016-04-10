#ifndef GESTORETHERNET_H
#define GESTORETHERNET_H

#include <QObject>
#include "gestorcontrol.h"

class GestorEthernet : public QObject
{
    Q_OBJECT
public:
    explicit GestorEthernet(QObject *parent = 0);
    void SendControlDatagram(EthMessage *p,const QHostAddress & sender, quint16 senderPort);

signals:
    void SendDebugInfo(unsigned char code, const QString &msg);

public slots:

private slots:
    void sltDebugInfo(unsigned char code, const QString &msg);

private:
    GestorControl *m_gestorControl;
};

#endif // GESTORETHERNET_H
