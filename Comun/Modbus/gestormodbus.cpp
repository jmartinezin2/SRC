/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <QLabel>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QTimer>

#include "gestormodbus.h"


//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

#define TIMEOUT_RESPUESTA   500

class SerialThread: public QThread
{
    QextSerialPort * port;

    protected:
        virtual void run(){
            //QString inStr;
            //port->(inStr.toAscii().constData(), inStr.length());
        }

    public:
        SerialThread(QextSerialPort * port, QObject * parent = 0):
            QThread(parent)
        {
            this->port = port;
        }
};

/** Indica si esta abierto*/
bool GestorModbus::isOpen(){
    return portOpen;
}

GestorModbus::GestorModbus(QObject *parent) :
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
bool GestorModbus::openModbusPort(QString port){

    portOpen = OpenConnection(port);

    return(portOpen);
}

/** Propiedades de COM*/
void GestorModbus::setComPort(int x){
    m_port=x;
}

/** Hace un bind*/
bool GestorModbus::OpenConnection(QString port){

    bool portOK;

    portModbus = new QextSerialPort();
    /*portModbus->setPortName(port);
    portModbus->setBaudRate(BAUD19200);
    portModbus->setQueryMode(QextSerialPort::EventDriven);
    portModbus->setFlowControl(FLOW_OFF);
    portModbus->setParity(PAR_EVEN);
    portModbus->setDataBits(DATA_7);
    portModbus->setStopBits(STOP_1);
    portModbus->setTimeout(250);
    portOK=portModbus->open(QIODevice::ReadWrite);*/
    portModbus = new QextSerialPort();
    portModbus->setPortName(port);
    portModbus->setBaudRate(m_baudRate);
    portModbus->setQueryMode(m_queryMode);
    portModbus->setFlowControl(m_flowControl);
    portModbus->setParity(m_parity);
    portModbus->setDataBits(m_dataBits);
    portModbus->setStopBits(m_stopBits);
    portModbus->setTimeout(m_timeout);
    portOK=portModbus->open(m_mode);

    respuestaRecibida=1;

    Modbuslistener = new puertoSerie(portModbus);
    //Because port and listener are working in different threads
    //Qt will choose queued connection. This may produce clusters of
    //signals waiting in the queue.
    //Modbuslistener->connect(portModbus, SIGNAL(readyRead()), Modbuslistener, SLOT(receive()));
    connect(Modbuslistener, SIGNAL(mensajeModbusRecibido(QByteArray)), this, SLOT(readPendingModbusData(QByteArray)));
    connect(Modbuslistener, SIGNAL(mensajeModbusRecibido()), this, SLOT(readPendingModbusData()));

    return(portOK);
}
/** Establece la velocidad*/
void GestorModbus::setConfig(BaudRateType baudRate,QextSerialBase::QueryMode queryMode,FlowType flowControl,ParityType parity,DataBitsType dataBits,StopBitsType stopBits,int timeout,QIODevice::OpenMode mode){
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
bool GestorModbus::OpenConnection(){
    QString port=QString("COM%1").arg(m_port);

    if (m_port<10)
        port=QString("COM%1").arg(m_port);
    else
        port=QString("\\\\.\\com%1").arg(m_port);



    portModbus = new QextSerialPort();
    portModbus->setPortName(port);
    portModbus->setBaudRate(m_baudRate);
    portModbus->setQueryMode(m_queryMode);
    portModbus->setFlowControl(m_flowControl);
    portModbus->setParity(m_parity);
    portModbus->setDataBits(m_dataBits);
    portModbus->setStopBits(m_stopBits);
    portModbus->setTimeout(m_timeout);
    portOpen=portModbus->open(m_mode);
    /*portModbus->setBaudRate(BAUD19200);
    portModbus->setQueryMode(QextSerialPort::EventDriven);
    portModbus->setFlowControl(FLOW_OFF);
    portModbus->setParity(PAR_EVEN);
    portModbus->setDataBits(DATA_7);
    portModbus->setStopBits(STOP_1);
    portModbus->setTimeout(250);
    portOpen=portModbus->open(QIODevice::ReadWrite);*/
    respuestaRecibida=1;

    Modbuslistener = new puertoSerie(portModbus);

    connect(Modbuslistener, SIGNAL(mensajeModbusRecibido(QByteArray)), this, SLOT(readPendingModbusData(QByteArray)));
    connect(Modbuslistener, SIGNAL(mensajeModbusRecibido()), this, SLOT(readPendingModbusData()));

    return(portOpen);

}




/** Cierra la conexion en curso*/
void GestorModbus::CloseConnection(){
    portModbus->close();
    portOpen=false;
}

bool GestorModbus::getStateConnection(){
    return(portOpen);
}


/** Envía un datagrama con datos*/
void GestorModbus::sendModbusData(QByteArray &d){
    //if (respuestaRecibida==1){
    if (portOpen){
            QByteArray datosEnviar;
            QByteArray datosMostrar;
            ModbusMessage *msg= new ModbusMessage(0,d);
            datosEnviar=msg->CreateMessageWithData(d);
            datosMostrar=datosEnviar.mid(1,datosEnviar.size()-3);
            int bytes=datosEnviar.size();
            if (bytes>1024) bytes=1024;
            portModbus->write(datosEnviar,bytes);
            emit sglModbusDataSent(datosMostrar);
            respuestaRecibida=0;
            portModbus->flush();
            QTimer::singleShot(TIMEOUT_RESPUESTA,this,SLOT(readPendingModbusData()));
    }
    //}
}


/** Lee los datagramas pendientes*/
void GestorModbus::readPendingModbusData(QByteArray dataReceived){

    emit sglModbusDataReceived(dataReceived);

}


void GestorModbus::readPendingModbusData(void){
    QByteArray dataR;
    QByteArray datos;
    char data[1024];
    dataR.clear();
    datos.clear();
    int bytesRead = portModbus->bytesAvailable();
    if (bytesRead==0) return;
    if (bytesRead>1024) bytesRead=1024;
    portModbus->read(data, bytesRead);
    datos=QByteArray(data);
    bytesRead=datos.indexOf("\r\n");
    /*dataR=QByteArray::fromRawData(data,64);
    bytesRead=dataR.indexOf(QString("\r"),0);
    datos=dataR.left(bytesRead);*/
    if (bytesRead>0)
        emit sglModbusDataReceived(datos.left(bytesRead));
}


/** Procesa el datagrama recibido*/
void GestorModbus::processModbusData(const QByteArray &datagram, const quint16 &sender){
    ModbusMessage *msg=new ModbusMessage(0,datagram);
    if (msg->isValidMessage()){
        QByteArray head=msg->head();
       QByteArray data=msg->data();
       //emit newDatagram(head);
       char cType=msg->getID();
       if (cType<0x80)
           emit sglModbusDataReceived(data);
       respuestaRecibida=0;
    }
    else{

    }
}


/*QLabel *GestorModbus::ShowMensaje(QString texto){
    QLabel *InfoLabel=new QLabel(texto);
    Qt::WindowFlags flags=Qt::Window;
    QDesktopWidget d;
    InfoLabel->setWindowFlags(flags|Qt::CustomizeWindowHint);
    InfoLabel->setGeometry((d.width()-AUX_INFO_WIDTH)/2,(d.height()-AUX_INFO_HEIGHT)/2,AUX_INFO_WIDTH,AUX_INFO_HEIGHT);
    InfoLabel->setWindowOpacity(0.75);
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    InfoLabel->setFont(font);
    InfoLabel->setAlignment(Qt::AlignVCenter |Qt:: AlignHCenter);
    InfoLabel->show();
    QCoreApplication::processEvents();

    return InfoLabel;
}*/
