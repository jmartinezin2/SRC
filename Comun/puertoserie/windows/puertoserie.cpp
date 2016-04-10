
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

    port->read(data, 128);
    dataReceived=QByteArray::fromRawData(data,128);

    int indiceCR=dataReceived.indexOf("\r");
    dataR=dataReceived.left(indiceCR-1);

    if (indiceCR>6)  emit mensajeModbusRecibido(dataR);

    //emit mensajeModbusRecibido();
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
