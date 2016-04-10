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
#include "in2saicontrol.h"
#include <QFile>
#include <QSettings>
#include <QTimer>
#include <QApplication>
#include "in2gestorfluidica.h"
#include "clienteopc.h"
#include "in2gestormantenimiento.h"
#include "in2defines.h"
#include <QHostInfo>
#include "in2ping.h"
#include "in2gestorfluidica.h"

#define TIMEOUT_TEST_SAI 10000
#define TIMEOUT_TEST_SAI_FAST 3000
#define TIMEOUT_SAVE    999

#define NET_WARNING 2
#define NET_ERROR   5

#define IP_CONTROL "192.168.0.1"

In2SaiControl::In2SaiControl(QObject *parent) :
    QObject(parent)
{
    m_bMonitor=false;
    m_bEnabled=false;
    m_strFile.clear();
    QString path=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    if (QFile::exists(path)){
        QSettings FicheroIni(path,QSettings::IniFormat,0);
        FicheroIni.beginGroup(QString("SAI"));
        m_bEnabled=FicheroIni.value(QString("SAIControl"),false).toBool();
        m_strFile=FicheroIni.value(QString("SAIFile"),"").toString();
        QFile::remove(m_strFile);
        FicheroIni.endGroup();
        QTimer::singleShot(5000,this,SLOT(sltStartMonitor()));

    }
}
/** Está habilitada o no*/
bool In2SaiControl::isEnabled(){
    return m_bEnabled;
}
/** Comienza a monitorizar*/
void In2SaiControl::sltStartMonitor(){
    if (!m_bMonitor){
        m_bMonitor=true;
        sltMonitor();
    }
}

/** Termina de monitorizar*/
void In2SaiControl::sltStopMonitor(){
    m_bMonitor=false;
}

/** Monitoriza*/
void In2SaiControl::sltMonitor(){
    bool bSaiActive=false;
    if (m_bMonitor){
        if (testCondicionFichero()){
            bSaiActive=true;
        }
        if (testCondicionRed()){
            bSaiActive=true;
        }

        if (bSaiActive){
            shutdown();
        }
        else{
            QTimer::singleShot(m_timer,this,SLOT(sltMonitor()));
        }
    }
}

/** Apagado de maquina*/
void In2SaiControl::shutdown(){
    ClienteOPC *opc=ClienteOPC::request(0);
    opc->setEnabled(false); //Deshabilito el cliente OPC
    In2GestorFluidica *gestor=In2GestorFluidica::request(0);
    for (int x=0;x<gestor->numEquipos();x++){
        gestor->ShutdownEquipo(x);
        emit SendDebugInfo(LOGGER_ERROR,QString("Apagando equipo %1").arg(x));
    }
    m_bMonitor=false; //Ya no monitorizamos mas
    emit sgnSAIActive();
}

/** Condicion de fichero*/
bool In2SaiControl::testCondicionFichero(){
    bool rtn=false;
    if (QFile::exists(m_strFile)) {
        QFile::remove(m_strFile);
        emit SendDebugInfo(LOGGER_ERROR,QString("SAIControl->Condicion de fichero"));
        rtn=true;
    }
    return rtn;
}

/** Condiciones de control*/
bool In2SaiControl::testCondicionRed(){
    bool rtn=false;
    static int counter=0;

    In2Ping foo;
    if (!foo.ping(IP_CONTROL)){
        emit SendDebugInfo(LOGGER_ERROR,QString("SAIControl->Condicion de red"));
        counter++;
        m_timer=TIMEOUT_TEST_SAI_FAST;
        if (counter==NET_WARNING){
            rtn=true;
        }
    }
    else{
        counter=0;
        m_timer=TIMEOUT_TEST_SAI;
    }
    QTime time=QTime::currentTime();
    int marca=time.msec();
    if (marca==TIMEOUT_SAVE) emit sgnError(0xFF);

    return rtn;
}

void In2SaiControl::lookedUp(const QHostInfo & host){
    if (host.error() != QHostInfo::NoError) {
             qDebug() << "Lookup failed:" << host.errorString();
             return;
         }

         foreach (const QHostAddress &address, host.addresses())
             qDebug() << "Found address:" << address.toString();

}

