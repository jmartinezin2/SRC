#ifndef GESTORLEC_H
#define GESTORLEC_H

#include <QObject>
#include <QLabel>
#include "Lecmessage.h"
#include "puertoserie.h"
#include "gestormodbus.h"
#include "qextserialport.h"

class GestorLec : public QObject
{
    Q_OBJECT
public:
    explicit GestorLec(QObject *parent = 0);
    void setConfig(BaudRateType baudRate,QextSerialBase::QueryMode queryMode,FlowType flowControl,ParityType parity,DataBitsType dataBits,StopBitsType stopBits,int timeout,QIODevice::OpenMode mode);
    bool isOpen();

private:
    unsigned char respuestaRecibida;
    bool portOpen;
public slots:
    bool openLecPort(QString port);
    bool OpenConnection(QString port);
    bool OpenConnection();
    void CloseConnection();
    void sendLecData(QByteArray &d);
    bool getStateConnection();
    void setComPort(int x);
signals:
    void DebugInfo(int type,const QString & txt);
    void newDatagram(const QByteArray & head);
    void sglLecDataReceived(QByteArray data);
    void SendDebugInfo(unsigned char,QString);
    void sglLecDataSent(QByteArray data);


private slots:
    void processLecData(const QByteArray &datagram, const quint16 & sender);
    void readPendingLecData(QByteArray dataReceived);
    void readPendingLecData(void);
    //QLabel *ShowMensaje(QString texto);

private:
    QextSerialPort * portLec;
    puertoSerie *Leclistener;
    int m_port;
    BaudRateType m_baudRate;
    QextSerialBase::QueryMode m_queryMode;
    FlowType m_flowControl;
    ParityType m_parity;
    DataBitsType m_dataBits;
    StopBitsType m_stopBits;
    int m_timeout;
    QIODevice::OpenMode m_mode;

public slots:

};


#endif // GESTORLEC_H
