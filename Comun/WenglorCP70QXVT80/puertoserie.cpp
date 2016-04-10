/**
 * @file puertoserie.cpp
 * @brief puertoserie Implementation.
 * @see puertoserie.h
 */

#include <QLabel>
#include <qextserialport.h>
#include "puertoserie.h"
#include <QDesktopWidget>
#include <QCoreApplication>

//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

//Una sola instancia
puertoSerie *puertoSerie::mInstance  = NULL;

/** Lllamda a instancia unica*/
puertoSerie *puertoSerie::request(QextSerialPort * port, QObject * parent)
{
    if(mInstance == NULL){
        mInstance = new puertoSerie(port,parent);
    }
    return mInstance;
}


puertoSerie::puertoSerie(QextSerialPort * port, QObject * parent):
    QObject(parent)
{
    this->port = port;

    BufferRx.clear();
    BufferTx.clear();

}

void puertoSerie::receive(){

    QByteArray dataReceived;
    QByteArray dataR;
    char data[1024];
    dataReceived.clear();
    dataR.clear();
    qint64 bytesRead = port->bytesAvailable();
    if (bytesRead>1024) bytesRead=1024;

    port->read(data, bytesRead);
    dataReceived=QByteArray::fromRawData(data,bytesRead);

    int indice_barra=dataReceived.lastIndexOf("/");
    int indice_punto=dataReceived.lastIndexOf(".");

    if ((indice_barra==-1) || (indice_punto==-1)) return;

    dataR=dataReceived.mid(indice_barra+5,indice_punto-indice_barra-1);

    emit mensajeModbusRecibido(dataR);

}


QLabel *puertoSerie::ShowMensaje(QString texto){
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
}
