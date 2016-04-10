#ifndef IN2GESTORSMCLEC_H
#define IN2GESTORSMCLEC_H

#include <QObject>
#include "gestorLec.h"
#include "in2defines.h"

class In2GestorSMCLec : public QObject
{
    Q_OBJECT
public:
    explicit In2GestorSMCLec(QObject *parent = 0);
    bool open();
    void SendData(QByteArray data);

private:
    GestorLec *m_gsLec;

signals:
    void SendDebugInfo(unsigned char value, QString txt);

private slots:
    void sltLecDataReceived(QByteArray byteArray);
public slots:
    
};

#endif // IN2GESTORSMCLEC_H
