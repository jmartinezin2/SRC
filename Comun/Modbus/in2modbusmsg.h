#ifndef IN2MODBUSMSG_H
#define IN2MODBUSMSG_H

#include <QObject>

class In2ModbusMsg : public QObject
{
    Q_OBJECT
public:
    explicit In2ModbusMsg(QObject *parent = 0);
    QByteArray getMsg(char code,QByteArray data);

private:
    QByteArray m_msg;
    
signals:
    
public slots:
    
};

#endif // IN2MODBUSMSG_H
