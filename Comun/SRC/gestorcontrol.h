#ifndef GESTORCONTROL_H
#define GESTORCONTROL_H

#include <QObject>
#include <QUdpSocket>

//#include "In2UdpFramework.h"
#include "ethmessage.h"

class GestorControl : public QObject
{
    Q_OBJECT
public:
    explicit GestorControl(QObject *parent = 0);
    void sendDatagram(EthMessage *p,const QHostAddress & sender, quint16 senderPort);
private:
    QUdpSocket *m_ControlSocket;

private slots:
    void readPendingDatagrams();
private:
    void processDatagram(const QByteArray &datagram,const QHostAddress & sender, quint16 senderPort);

signals:
    void rcvMsg(int msg);
    void SendDebugInfo(unsigned char code, const QString &msg);
    
public slots:

    
};

#endif // GESTORCONTROL_H
