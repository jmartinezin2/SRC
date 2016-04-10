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
#include "in2gestorais.h"
#include "in2configuratorvision.h"
#include "in2defines.h"
#include "in2magicpacket.h"
#include "in2aisstringgen.h"
#include <QApplication>
#include "in2ping.h"

//Timeout de reconexion
#define TIMEOUT_RECON 30000
#define DEFAULT_FILE "In2Launchers.ini"


In2GestorAIS::In2GestorAIS(QObject *parent) :
    QObject(parent)
{

    QObject::connect(&m_socket,SIGNAL(readyRead()),this,SLOT(sltReadSocket()));
    QObject::connect(&m_socket,SIGNAL(disconnected()),this,SIGNAL(sgnSocketDisconnected()));
    QObject::connect(&m_socket,SIGNAL(disconnected()),this,SLOT(sltSocketDisConnected()));
    QObject::connect(&m_socket,SIGNAL(connected()),this,SLOT(sltSocketConnected()));
    QObject::connect(&m_socket,SIGNAL(connected()),this,SIGNAL(sgnSocketConnected()));
    QObject::connect(&m_socket,SIGNAL(connected()),this,SIGNAL(sgnRemoteAlive()));
    m_bConnected=false;
    m_bWaitAnswer=false;

}

/*In2GestorAIS::~In2GestorAIS(){
    m_socket.close();
}*/

bool In2GestorAIS::connect(){
    //m_socket.connectToHost("192.168.0.29",2000,QIODevice::ReadWrite);
    In2ConfiguratorVision config;
    config.readConfig();
    m_ip=config.getIP();
    m_port=config.getPort();

    m_tryRecon=true;


    m_socket.connectToHost(m_ip,m_port.toInt(),QIODevice::ReadWrite);

    QString txt=QString("AIS Device, connecting [%1:%2]").arg(m_ip).arg(m_port);
    emit SendDebugInfo(LOGGER_STORE,txt);
    bool rtn=m_socket.waitForConnected(3000);
    if (!rtn) QTimer::singleShot(0,this,SLOT(sltSocketDisConnected()));
    emit SendDebugInfo(LOGGER_STORE,QString("AIS Device connection %1").arg(rtn));
    return rtn;

}



void In2GestorAIS::wakeUp(){
    In2ConfiguratorVision config;
    config.readConfig();
    QString strMAC=config.getMAC();
    if (!strMAC.isEmpty()){
        In2MagicPacket p;
        p.SendPacket(strMAC);
        emit SendDebugInfo(LOGGER_STORE,QString("In2GestorAIS: WakeUP->%1").arg(strMAC));
    }
    else emit SendDebugInfo(LOGGER_STORE,QString("In2GestorAIS: NO WakeUP MAC configured"));
}

void In2GestorAIS::setTryReconn(bool val){
    if ((!m_tryRecon)&&(val)) QTimer::singleShot(TIMEOUT_RECON,this,SLOT(connect()));
    m_tryRecon=val;
}

bool In2GestorAIS::isConnected(){
    return m_bConnected;
}

void In2GestorAIS::sltSocketConnected(){
    m_bConnected=true;
    emit SendDebugInfo(LOGGER_STORE,QString("AIS Device connected"));
    //Segun se conecta, que intente mapear
    sltMapearUnidad();
}
/** Mapea la unidad de disco*/
bool In2GestorAIS::sltMapearUnidad(){
    bool  rtn=false;
    QSettings FicheroIni(QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_FILE),QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("Launchers"));
    QString strMapLauncher=FicheroIni.value(QString("In2AISMap"),"").toString();
    FicheroIni.endGroup();
    if (!QFile::exists(strMapLauncher)){
        return false;
    }

    if (!strMapLauncher.isEmpty()){
        QProcess foo;
        if (!foo.startDetached("cmd.exe",QStringList()<<"/c"<<strMapLauncher)){
            emit SendDebugInfo(LOGGER_STORE,QString("Vision->No se pudo mapear %1").arg(strMapLauncher));
        }
        else{
            emit SendDebugInfo(LOGGER_STORE,QString("Vision->Mapeado %1").arg(strMapLauncher));
            rtn=true;
        }
    }
    else{
        emit SendDebugInfo(LOGGER_STORE,QString("Vision->No hay unidad de mapeo configurada"));
    }
    return rtn;
}

void In2GestorAIS::sltSocketDisConnected(){
    m_bConnected=false;
    if (m_tryRecon){
        In2Ping foo;
        bool rtn=foo.ping(m_ip);
        if (rtn){
            emit SendDebugInfo(LOGGER_STORE,tr("Vision->IP to %1 OK").arg(m_ip));
            emit sgnRemoteAlive();
        }
        else{
            emit SendDebugInfo(LOGGER_STORE,tr("Vision->IP to %1 NOK").arg(m_ip));
            emit sgnRemoteDead();
        }

        QTimer::singleShot(TIMEOUT_RECON,this,SLOT(connect()));
    }
}

/** Escribe el string*/
bool In2GestorAIS::sendString(QString txt){
#if QT_VERSION>=0x050000
    m_socket.write(txt.toLatin1());
#else
    m_socket.write(txt.toAscii());
#endif
    emit SendDebugInfo(LOGGER_STORE,QString("Vision->%1").arg(txt));
    if (txt.at(0)!='%'){
        m_bWaitAnswer=true;
        QTimer::singleShot(5000,this,SLOT(sltTestAnswer()));
    }else{
        m_bWaitAnswer=false;
    }
    return m_socket.waitForBytesWritten(2000);

}
/** Testea que hay respuesta de AIS*/
void In2GestorAIS::sltTestAnswer(){
    if (m_bWaitAnswer) {
        emit sgnAISNoComm();
        emit SendDebugInfo(LOGGER_ERROR,"ERROR:NO HAY RESPUESTA DE EQUIPO DE VISION");
    }
}

/** Recibe datos*/
void In2GestorAIS::sltReadSocket(){
   m_bWaitAnswer=false;
   QByteArray data=m_socket.readAll();
   //data.clear();
   QString rcv=QString(data);
   //Corrijo desviaciones
   if (rcv.contains("##")){
       //Es un string de configuracion..corrijo si no se ha recibido OK
       if (rcv.at(0)!='%'){
           rcv.prepend('%');
           In2AISStringGen gen;
           //Mandamos autoimaticamente el string reconfigurado
           QString foo=gen.completeOCRString(rcv);
           sendString(foo);
       }
       else{ //Es de configuración pero viene bien formado
           In2AISStringGen gen;
           //Mandamos autoimaticamente el string reconfigurado
           QString foo=gen.completeOCRString(rcv);
           sendString(foo);
       }
   }

   emit sgnAISReceived(rcv);
}
