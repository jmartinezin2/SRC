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
#include "in2gestorvision.h"
#include "in2configuratorvision.h"
#include "inkjetdebug.h"
#include "in2defines.h"
#include "in2aisconfigurator.h"
#define AIS_VENDOR "AIS"


In2GestorVision *In2GestorVision::mInstance  = NULL;


In2GestorVision::In2GestorVision(QObject *parent) :
    QObject(parent)
{
    In2ConfiguratorVision config;
    m_bEquipoConnected=false;
    m_bSocketConnected=false;
    config.readConfig();
    m_vendor=config.getVendor();
    m_enabled=config.isEnabled();
    if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
        m_gestorAIS= new In2GestorAIS(this);
        QObject::connect(m_gestorAIS,SIGNAL(sgnAISReceived(QString)),this,SLOT(sltVisionReceived(QString)));
        QObject::connect(m_gestorAIS,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SIGNAL(SendDebugInfo(unsigned char,QString)));
        QObject::connect(m_gestorAIS,SIGNAL(sgnAISNoComm()),this,SIGNAL(sgnVisionNoComm()));
        QObject::connect(m_gestorAIS,SIGNAL(sgnSocketDisconnected()),this,SLOT(sltSocketDisconnected()));
        QObject::connect(m_gestorAIS,SIGNAL(sgnSocketConnected()),this,SLOT(sltSocketConnected()));
        QObject::connect(m_gestorAIS,SIGNAL(sgnRemoteAlive()),this,SLOT(sltVisionRemoteAlive()));
        QObject::connect(m_gestorAIS,SIGNAL(sgnRemoteDead()),this,SLOT(sltVisionRemoteDead()));


    }
    InkjetDebug *dbg=InkjetDebug::request(0);
    QObject::connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
    m_lastCommand=NO_COMMAND;
}

/** Estado socjket vision*/
void In2GestorVision::sltSocketDisconnected(){
    m_bSocketConnected=false;
    emit sgnSocketDisconnected();
}
/** Estado socket vision*/
void In2GestorVision::sltSocketConnected(){
    m_bSocketConnected=true;
    emit sgnSocketConnected();
}
/** Estado equipo vision*/
void In2GestorVision::sltVisionRemoteAlive(){
    m_bEquipoConnected=true;
    emit sgnVisionRemoteAlive();
}
/** Estado equipo vision*/
void In2GestorVision::sltVisionRemoteDead(){
    m_bEquipoConnected=false;
    emit sgnVisionRemoteDead();
}

/** Indica si el socket esta conectado*/
bool In2GestorVision::socketConnected(){
    return m_bSocketConnected;
}
/** Indica si la aplicacion esta conectada*/
bool In2GestorVision::equipoConnected(){
    return m_bEquipoConnected;
}


/** Mapea una unidad*/
bool In2GestorVision::sltMapearUnidad(){
    bool rtn=false;
    if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
        rtn=m_gestorAIS->sltMapearUnidad();
    }
    return rtn;
}


bool In2GestorVision::isConnected(){
    bool rtn=false;
    if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
        rtn=m_gestorAIS->isConnected();
    }
    return rtn;
}

/** Singleton*/
In2GestorVision *In2GestorVision::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new In2GestorVision(parent);
    }
    return mInstance;

}
/** Activado o no*/
bool In2GestorVision::isEnabled(){
    return m_enabled;
}

/** Despierta por red*/
void In2GestorVision::wakeUp(){
    if (m_enabled){
        if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
            m_gestorAIS->wakeUp();
        }
    }
}
/** Permite reconectar*/
void In2GestorVision::setTryReconn(bool val){
    if (m_enabled){
        if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
            m_gestorAIS->setTryReconn(val);
        }
    }
}

/** Conecta*/
bool In2GestorVision::connect(){
    bool rtn=false;
    if (m_enabled){
        if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
            rtn=m_gestorAIS->connect();
        }
    }
    //emit SendDebugInfo(LOGGER_STORE,tr("In2Vision: Connect %1").arg(rtn));
    return rtn;
}
/** Manda un comando*/
bool In2GestorVision::sendCommand(int command, QString txt){
    m_lastCommand=command;
    return sendString(txt);
}

/** Manda un string*/
bool In2GestorVision::sendString(QString txt){
    bool  rtn=false;
    if (m_enabled){
        if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
            rtn=m_gestorAIS->sendString(txt);
            m_strLastCommand=txt;

        }
    }
    emit SendDebugInfo(LOGGER_STORE,tr("In2Vision:Send[%1] %2").arg(rtn).arg(txt));
    return rtn;
}

/** Ha recibido vision*/
void In2GestorVision::sltVisionReceived(QString txt){
    emit sgnVisionReceived(txt);
    if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
        //Informacion de Setup
        QString foo=QString(txt.at(0));
        if (!foo.compare(AIS_CONFIG)){
            emit sgnVisionCommandAnswer(VISION_CONFIG,m_lastCommand);
        }
        else if (!txt.compare(AIS_ACK)){
            emit sgnVisionCommandAnswer(VISION_ACK,m_lastCommand);
        }
        else {
            if (!txt.compare(AIS_NACK)){
                emit sgnVisionCommandAnswer(VISION_NACK,m_lastCommand);
            }
            else{
                emit sgnVisionAlarm(txt);
            }
        }
    }
    emit SendDebugInfo(LOGGER_STORE,tr("In2Vision:RCV %1").arg(txt));
}
/** Carga un trabajo completo*/
bool In2GestorVision::sltLoadWork(QString txt){
    bool rtn=false;
    if (!m_vendor.compare(AIS_VENDOR,Qt::CaseInsensitive)){
        QString visionFile=txt;
        In2AISConfigurator p;
        QString str=p.getStringLoad(visionFile);
        m_lastCommand=LOAD_FILE;
        rtn=this->sendString(str);
    }
    return rtn;

}

/** Devuelve el ultimo comando enviado*/
QString In2GestorVision::lastCommandSent(){
    return m_strLastCommand;
}
