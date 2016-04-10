#include "in2gestorsmclec.h"

In2GestorSMCLec::In2GestorSMCLec(QObject *parent) :
    QObject(parent)
{
    m_gsLec=new GestorLec();
    connect(m_gsLec,SIGNAL(sglLecDataReceived(QByteArray)),this,SLOT(sltLecDataReceived(QByteArray)));
    connect(m_gsLec,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
}
/** Abre la conexion*/
bool In2GestorSMCLec::open(){
    bool  rtn=false;
    if (m_gsLec){
        m_gsLec->setComPort(16);
        m_gsLec->setConfig(BAUD38400,QextSerialPort::EventDriven,FLOW_OFF,PAR_NONE,DATA_7,STOP_1,250,QIODevice::ReadWrite);
        rtn=m_gsLec->OpenConnection();
    }
    return rtn;
}

/** Recibe datos del puerto serie*/
void In2GestorSMCLec::sltLecDataReceived(QByteArray byteArray){
    emit SendDebugInfo(LOGGER_WARNING,QString(byteArray));
}

/** Manda datos*/
void In2GestorSMCLec::SendData(QByteArray data){
    if (m_gsLec){
        m_gsLec->sendLecData(data);
    }
}

