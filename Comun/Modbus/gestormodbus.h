#ifndef GestorModbus_H
#define GestorModbus_H

#include <QObject>
#include <QLabel>
#include "modbusmessage.h"
#include "puertoserie.h"
#include "gestormodbus.h"
#include "qextserialport.h"

class GestorModbus : public QObject
{
    Q_OBJECT
public:
    explicit GestorModbus(QObject *parent = 0);
    void setConfig(BaudRateType baudRate,QextSerialBase::QueryMode queryMode,FlowType flowControl,ParityType parity,DataBitsType dataBits,StopBitsType stopBits,int timeout,QIODevice::OpenMode mode);
    bool isOpen();

private:
    unsigned char respuestaRecibida;
    bool portOpen;
public slots:
    bool openModbusPort(QString port);
    bool OpenConnection(QString port);
    bool OpenConnection();
    void CloseConnection();
    void sendModbusData(QByteArray &d);
    bool getStateConnection();
    void setComPort(int x);
signals:
    void DebugInfo(int type,const QString & txt);
    void newDatagram(const QByteArray & head);
    void sglModbusDataReceived(QByteArray data);
    void SendDebugInfo(unsigned char,QString);
    void sglModbusDataSent(QByteArray data);


private slots:
    void processModbusData(const QByteArray &datagram, const quint16 & sender);
    void readPendingModbusData(QByteArray dataReceived);
    void readPendingModbusData(void);
    //QLabel *ShowMensaje(QString texto);

private:
    QextSerialPort * portModbus;
    puertoSerie *Modbuslistener;
    int m_port;
    BaudRateType m_baudRate;
    QextSerialBase::QueryMode m_queryMode;
    FlowType m_flowControl;
    ParityType m_parity;
    DataBitsType m_dataBits;
    StopBitsType m_stopBits;
    int m_timeout;
    QIODevice::OpenMode m_mode;




    //short m_add;

public slots:

};

#endif // GestorModbus_H
