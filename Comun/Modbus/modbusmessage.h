#ifndef ModbusMessage_H
#define ModbusMessage_H

#include <QObject>
#include <QByteArray>
#include <QMap>


//Coleccion de errores
enum MODBUS_MESSAGE_ERROR{
    MODBUS_NOT_ERROR,
    MODBUS_ERROR_SYN,
    MODBUS_ERROR_SIZE
};


class ModbusMessage : public QObject
{
    Q_OBJECT
public:
    ModbusMessage(QObject *parent = 0);
    ModbusMessage(QObject *parent = 0,int id=0);
    ModbusMessage(QObject *parent,const QByteArray &p);
    QByteArray CreateMessageWithData(QByteArray &d);
    QByteArray message();
    QByteArray head();
    QByteArray data();
    char getLastError();
    int isValidMessage();
    void loadMessage(const QByteArray & p);
    char getID();
    void SetUnknownMessage();
    unsigned char calculateLRC(QByteArray &trama);
private:
    void SetSyn();


private:
    QByteArray m_data;
    char m_lastError;
signals:
    
public slots:
    
};

#endif // ModbusMessage_H
