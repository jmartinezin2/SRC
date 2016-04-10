#ifndef GestorSerie_H
#define GestorSerie_H

#include <QObject>
#include <QLabel>
#include <QThread>
#include "qextserialport.h"


class GestorSerie : public QObject
{
    Q_OBJECT
public:
    explicit GestorSerie(QObject *parent = 0);
    unsigned char respuestaRecibida;
    bool portOpen;
public slots:
    bool openPort(QString port);
    bool OpenConnection(QString port);
    void CloseConnection();
    void sendData(QByteArray &d);
    bool getStateConnection();
signals:
    void DebugInfo(int type,const QString & txt);
    void newDatagram(const QByteArray & head);
    void sglDataReceived(QByteArray data);
    void SendDebugInfo(unsigned char,QString);
    void sglDataSent(QByteArray data);


private slots:
    void sltReceive();

private:
    QextSerialPort * m_port;

public slots:

};

#endif // GestorSerie_H
