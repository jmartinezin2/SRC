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

#include "gestorserie.h"
#include "in2gestordiametro.h"

//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

#define TIMEOUT_RESPUESTA   2500

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




GestorSerie::GestorSerie(QObject *parent) :
    QObject(parent)
{
    portOpen=false;

}

/** Hace un bind*/
bool GestorSerie::openPort(QString port){

    portOpen = OpenConnection(port);

    return(portOpen);
}


/** Hace un bind*/
bool GestorSerie::OpenConnection(QString port){

    bool portOK;

    m_port = new QextSerialPort();
    m_port->setPortName(port);
    m_port->setBaudRate(BAUD38400);
    //portModbus->setQueryMode(QextSerialPort::Polling);
    m_port->setQueryMode(QextSerialPort::EventDriven);
    m_port->setFlowControl(FLOW_OFF);
    m_port->setParity(PAR_NONE);
    m_port->setDataBits(DATA_8);
    m_port->setStopBits(STOP_1);
    m_port->setTimeout(1000);
    portOK=m_port->open(QIODevice::ReadWrite);
    respuestaRecibida=1;

    connect(m_port, SIGNAL(readyRead()), this, SLOT(sltReceive()));

    SerialThread * thread = new SerialThread(m_port);
    //connect(thread, SIGNAL(finished()), this, SLOT(quit()));
    thread->start();

    return(portOK);

}

/** Recibe datos*/
void GestorSerie::sltReceive(){
    QByteArray dataReceived;
    QByteArray dataR;
    char data[64];
    dataReceived.clear();
    dataR.clear();
    qint64 bytesRead = m_port->bytesAvailable();
    if (bytesRead<14) return;
    if (bytesRead>64) bytesRead=64;

    memset(&data[0],0x00,64);
    m_port->read(data, bytesRead);
    dataReceived=QByteArray::fromRawData(data,bytesRead);

    int indice_barra=dataReceived.lastIndexOf("/");
    int indice_punto=dataReceived.lastIndexOf(".");

    if ((indice_barra==-1) || (indice_punto==-1)) return;

    dataR=dataReceived.mid(indice_barra+5,indice_punto-indice_barra-1);
    emit sglDataReceived(dataR);

}

/** Cierra la conexion en curso*/
void GestorSerie::CloseConnection(){
    m_port->close();
    portOpen=false;
}

bool GestorSerie::getStateConnection(){
    return(portOpen);
}


/** Envía un datagrama con datos*/
void GestorSerie::sendData(QByteArray &d){
    //if (respuestaRecibida==1){
        int bytes=d.size();
        if (bytes>1024) bytes=1024;
        m_port->write(d,bytes);
        //if (rtn!=-1) emit sglDataSent(datosMostrar);
        respuestaRecibida=0;
        m_port->flush();
        //QTimer::singleShot(TIMEOUT_RESPUESTA,this,SLOT(readPendingModbusData()));
    //}
}
