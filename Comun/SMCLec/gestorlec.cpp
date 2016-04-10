#include <QDesktopWidget>
#include <QCoreApplication>
#include <QTimer>

#include "gestorLec.h"

#define TIMEOUT_RESPUESTA   500

class SerialThread: public QThread
{
    QextSerialPort * port;

    protected:
        virtual void run(){
        }

    public:
        SerialThread(QextSerialPort * port, QObject * parent = 0):
            QThread(parent)
        {
            this->port = port;
        }
};

/** Indica si esta abierto*/
bool GestorLec::isOpen(){
    return portOpen;
}

GestorLec::GestorLec(QObject *parent) :
    QObject(parent)
{
    portOpen=false;
    m_port=0;

    m_baudRate=BAUD19200;
    m_queryMode=QextSerialPort::EventDriven;
    m_flowControl=FLOW_OFF;
    m_parity=PAR_EVEN;
    m_dataBits=DATA_7;
    m_stopBits=STOP_1;
    m_timeout=250;
    m_mode=QIODevice::ReadWrite;

}

/** Hace un bind*/
bool GestorLec::openLecPort(QString port){

    portOpen = OpenConnection(port);

    return(portOpen);
}

/** Propiedades de COM*/
void GestorLec::setComPort(int x){
    m_port=x;
}

/** Hace un bind*/
bool GestorLec::OpenConnection(QString port){

    bool portOK;

    portLec = new QextSerialPort();
    portLec = new QextSerialPort();
    portLec->setPortName(port);
    portLec->setBaudRate(m_baudRate);
    portLec->setQueryMode(m_queryMode);
    portLec->setFlowControl(m_flowControl);
    portLec->setParity(m_parity);
    portLec->setDataBits(m_dataBits);
    portLec->setStopBits(m_stopBits);
    portLec->setTimeout(m_timeout);
    portOK=portLec->open(m_mode);

    respuestaRecibida=1;

    Leclistener = new puertoSerie(portLec);
    //Because port and listener are working in different threads
    //Qt will choose queued connection. This may produce clusters of
    //signals waiting in the queue.
    //Modbuslistener->connect(portModbus, SIGNAL(readyRead()), Modbuslistener, SLOT(receive()));
    connect(Leclistener, SIGNAL(mensajeLecRecibido(QByteArray)), this, SLOT(readPendingLecData(QByteArray)));
    connect(Leclistener, SIGNAL(mensajeLecRecibido()), this, SLOT(readPendingLecData()));

    return(portOK);
}
/** Establece la velocidad*/
void GestorLec::setConfig(BaudRateType baudRate,QextSerialBase::QueryMode queryMode,FlowType flowControl,ParityType parity,DataBitsType dataBits,StopBitsType stopBits,int timeout,QIODevice::OpenMode mode){
    m_baudRate=baudRate;
    m_queryMode=queryMode;
    m_flowControl=flowControl;
    m_parity=parity;
    m_dataBits=dataBits;
    m_stopBits=stopBits;
    m_timeout=timeout;
    m_mode=mode;
}

/** Hace un bind*/
bool GestorLec::OpenConnection(){
    QString port=QString("COM%1").arg(m_port);

    if (m_port<10)
        port=QString("COM%1").arg(m_port);
    else
        port=QString("\\\\.\\com%1").arg(m_port);



    portLec = new QextSerialPort();
    portLec->setPortName(port);
    portLec->setBaudRate(m_baudRate);
    portLec->setQueryMode(m_queryMode);
    portLec->setFlowControl(m_flowControl);
    portLec->setParity(m_parity);
    portLec->setDataBits(m_dataBits);
    portLec->setStopBits(m_stopBits);
    portLec->setTimeout(m_timeout);
    portOpen=portLec->open(m_mode);
    /*portModbus->setBaudRate(BAUD19200);
    portModbus->setQueryMode(QextSerialPort::EventDriven);
    portModbus->setFlowControl(FLOW_OFF);
    portModbus->setParity(PAR_EVEN);
    portModbus->setDataBits(DATA_7);
    portModbus->setStopBits(STOP_1);
    portModbus->setTimeout(250);
    portOpen=portModbus->open(QIODevice::ReadWrite);*/
    respuestaRecibida=1;

    Leclistener = new puertoSerie(portLec);

    connect(Leclistener, SIGNAL(mensajeLecRecibido(QByteArray)), this, SLOT(readPendingLecData(QByteArray)));
    connect(Leclistener, SIGNAL(mensajeLecRecibido()), this, SLOT(readPendingLecData()));

    return(portOpen);

}




/** Cierra la conexion en curso*/
void GestorLec::CloseConnection(){
    portLec->close();
    portOpen=false;
}

bool GestorLec::getStateConnection(){
    return(portOpen);
}


/** Envía un datagrama con datos*/
void GestorLec::sendLecData(QByteArray &d){
    //if (respuestaRecibida==1){
    if (portOpen){
            QByteArray datosEnviar;
            LecMessage *msg= new LecMessage();
            datosEnviar=msg->CreateMessageWithData(d);
            portLec->write(datosEnviar,datosEnviar.count());
            emit sglLecDataSent(datosEnviar);
            respuestaRecibida=0;
            portLec->flush();
            QTimer::singleShot(TIMEOUT_RESPUESTA,this,SLOT(readPendingLecData()));
    }
}


/** Lee los datagramas pendientes*/
void GestorLec::readPendingLecData(QByteArray dataReceived){

    emit sglLecDataReceived(dataReceived);

}


void GestorLec::readPendingLecData(void){
    QByteArray dataR;
    QByteArray datos;
    char data[1024];
    dataR.clear();
    datos.clear();
    int bytesRead = portLec->bytesAvailable();
    if (bytesRead==0) return;
    portLec->read(data, bytesRead);
    datos=QByteArray(data);
    if (bytesRead>0)
        emit sglLecDataReceived(datos);
}


/** Procesa el datagrama recibido*/
void GestorLec::processLecData(const QByteArray &datagram, const quint16 &sender){
    /*LecMessage *msg=new LecMessage(0,datagram);
    if (msg->isValidMessage()){
       QByteArray head=msg->head();
       QByteArray data=msg->data();
       //emit newDatagram(head);
       char cType=msg->getID();
       if (cType<0x80)
           emit sglLecDataReceived(data);
       respuestaRecibida=0;
    }
    else{

    }*/
}
