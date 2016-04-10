#ifndef ETHMESSAGE_H
#define ETHMESSAGE_H

#include <QObject>
#include <QByteArray>
#include "In2UdpFramework.h"

#define HEAD_LEN            7

//Posiciones de los bytes de cabecera
#define SYN_POS             0
#define SIZE_POS            2
#define ID_POS              4
#define OPTIONS_POS         5
#define COUNTER_POS         6
#define DATA_POS            7

//Caracter de sincronismo
#define SYN_CHAR            0x5A

//Bits de opciones
#define BIT_REQ_ACK         0
#define BIT_ACK             1
#define BIT_UNKNOWN         2

//Coleccion de errores
enum MESSAGE_ERROR{
    NOT_ERROR,
    ERROR_SYN,
    ERROR_SIZE
};


class EthMessage : public QObject
{
    Q_OBJECT
public:
    EthMessage(QObject *parent = 0);
    EthMessage(QObject *parent = 0,int id=0);
    EthMessage(QObject *parent,const QByteArray &p);
    QByteArray CreateMessage(int id);
    QByteArray CreateMessageNoData(int id);
    QByteArray CreateMessageWithData(int id,QByteArray &d);
    QByteArray message();
    QByteArray head();
    QByteArray data();
    char getLastError();
    int isValidMessage();
    void loadMessage(const QByteArray & p);
    char getID();
    void SetUnknownMessage();
    void SetBitACK();
private:
    void SetSyn();
    void SetReqAck();
private:
    QByteArray m_data;
    char m_lastError;
signals:
    
public slots:
    
};

#endif // ETHMESSAGE_H
