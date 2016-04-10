#ifndef IN2MAGICPACKET_H
#define IN2MAGICPACKET_H

#include <QObject>

class In2MagicPacket : public QObject
{
    Q_OBJECT
public:
    explicit In2MagicPacket(QObject *parent = 0);
    void SendPacket(QString strMAC);
    
signals:

private:


public slots:
    
};

#endif // IN2MAGICPACKET_H
