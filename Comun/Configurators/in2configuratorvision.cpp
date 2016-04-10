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
#include "in2configuratorvision.h"
#include "inkjetdebug.h"
#include "in2defines.h"
#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>

#define PAR_VISION                  "VISION"
#define PAR_ENABLED                 "enabled"
#define PAR_FERRORES                "FERRORES"
#define PAR_NUM                     "num"
#define PAR_TIME                    "time"
#define PAR_ITEMOPC                 "itemOPC"
#define PAR_ERRORPATH               "ErrorPath"
#define PAR_GOODPATH               "GoodPath"
#define PAR_VENDOR                  "Vendor"
#define PAR_IP                      "IP"
#define PAR_MAC                      "MAC"
#define PAR_PORT                    "Port"
#define PAR_IMAGE                   "IMAGE"
#define PAR_CX                          "CX"
#define PAR_CY                          "CY"

In2ConfiguratorVision::In2ConfiguratorVision(QObject *parent) :
    QObject(parent)
{
    m_bEnabled=false;
    m_filtros.clear();
    m_expectedFilters=0;
    m_ErrorImagePath.clear();
    m_GoodImagePath.clear();
    m_IP.clear();
    m_MAC.clear();
    m_port.clear();
    m_vendor.clear();

}
/** Lee la configuración*/
void In2ConfiguratorVision::readConfig(){
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    QString str=QString("%1\\Config\\In2Vision.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(str);
    if (!iniFile.exists()){
        infoDebug->ProcessMsg(LOGGER_ERROR,tr("NO HAY FICHERO IN2VISION.INI!"));
        QMessageBox::information(0,"ERROR",tr("No se encuentra el fichero IN2VISION.INI"),QMessageBox::Ok);
        return;
    }
    QSettings FicheroIni(str,QSettings::IniFormat,0);
    FicheroIni.beginGroup(PAR_VISION);
    m_bEnabled=FicheroIni.value(PAR_ENABLED,false).toBool();
    m_expectedFilters=FicheroIni.value(PAR_NUM,0).toInt();
    m_ErrorImagePath=FicheroIni.value(PAR_ERRORPATH,"").toString();
    m_GoodImagePath=FicheroIni.value(PAR_GOODPATH,"").toString();
    m_vendor=FicheroIni.value(PAR_VENDOR,"").toString();
    m_IP=FicheroIni.value(PAR_IP,"").toString();
    m_MAC=FicheroIni.value(PAR_MAC,"").toString();
    m_port=FicheroIni.value(PAR_PORT,"").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,tr("[VISION] m_bEnabled=%1 m_expectedFilters=%2").arg(m_bEnabled).arg(m_expectedFilters));
    infoDebug->ProcessMsg(LOGGER_WARNING,tr("[VISION] errorPath=%1").arg(m_ErrorImagePath));
    infoDebug->ProcessMsg(LOGGER_WARNING,tr("[VISION] goodPath=%1").arg(m_GoodImagePath));
    infoDebug->ProcessMsg(LOGGER_WARNING,tr("[VISION] Vendor=%1 IP=%2 Port=%3").arg(m_vendor).arg(m_IP).arg(m_port));
    FicheroIni.endGroup();
    //Filtro de errores
    QStringList lst=FicheroIni.childGroups();
    for (int x=0;x<m_expectedFilters;x++){
        str=QString("%1%2").arg(PAR_FERRORES).arg(x+1);
        if (lst.contains(str,Qt::CaseInsensitive)){
            FicheroIni.beginGroup(str);
            In2FiltroVision p;
            bool enabled=FicheroIni.value(PAR_ENABLED,false).toBool();
            unsigned short error=FicheroIni.value(PAR_NUM,0).toInt();
            quint64 time=FicheroIni.value(PAR_TIME,0).toInt();
            int itemOPC=FicheroIni.value(PAR_ITEMOPC,0).toInt();
            p.setEnabled(enabled);
            p.setNumErrores(error);
            p.setTime(time);
            p.setItemOPC(itemOPC);
            m_filtros.append(p);
            infoDebug->ProcessMsg(LOGGER_WARNING,tr("[%1] enabled=%2 error=%3 time=%4 itemOPC=5").arg(str).arg(enabled).arg(error).arg(time).arg(itemOPC));
            FicheroIni.endGroup();
        }
    }
    FicheroIni.beginGroup(PAR_IMAGE);
    m_CX=FicheroIni.value(PAR_CX,"1").toFloat();
    infoDebug->ProcessMsg(LOGGER_WARNING,tr("[VISION] CX=%1").arg(m_CX));
    m_CY=FicheroIni.value(PAR_CY,"1").toFloat();
    infoDebug->ProcessMsg(LOGGER_WARNING,tr("[VISION] CY=%1").arg(m_CY));
    FicheroIni.endGroup();

}
/** Devuelve el CX*/
float In2ConfiguratorVision::getCX(){
    return m_CX;
}
/** Devuelve el CY*/
float In2ConfiguratorVision::getCY(){
    return m_CY;
}
/** Guarda la configuracion...solo los errores*/
void In2ConfiguratorVision::saveConfig(QVector<In2FiltroVision> filters){
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    QString str=QString("%1\\Config\\In2Vision.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(str);
    if (!iniFile.exists()){
        infoDebug->ProcessMsg(LOGGER_ERROR,tr("NO HAY FICHERO IN2VISION.INI!"));
        QMessageBox::information(0,"ERROR",tr("No se encuentra el fichero IN2VISION.INI"),QMessageBox::Ok);
        return;
    }
    QSettings FicheroIni(str,QSettings::IniFormat,0);

    for (int x=0;x<filters.count();x++){
        str=QString("%1%2").arg(PAR_FERRORES).arg(x+1);
        FicheroIni.beginGroup(str);
        In2FiltroVision p;
        p=filters.at(x);
        int errores=p.getNumErrores();
        FicheroIni.setValue(PAR_NUM,errores);
        FicheroIni.endGroup();
    }
    FicheroIni.sync();
}


/** Devuelve el fabricante de vision*/
QString In2ConfiguratorVision::getVendor(){
    return m_vendor;
}
/** Devuelve el puerto*/
QString In2ConfiguratorVision::getPort(){
    return m_port;
}
/** Devuelve la MAC*/
QString In2ConfiguratorVision::getMAC(){
    return m_MAC;
}

/** Devuelve la IP*/
QString In2ConfiguratorVision::getIP(){
    return m_IP;
}

/** esta activo*/
bool In2ConfiguratorVision::isEnabled(){
    return m_bEnabled;
}

/** Filtros esperados*/
int In2ConfiguratorVision::expectedFilters(){
    return m_expectedFilters;
}
/** Lista de filtros*/
QVector<In2FiltroVision> In2ConfiguratorVision::getFiltros(){
    return m_filtros;
}

/** Devuelve el path de errores*/
QString In2ConfiguratorVision::getErrorPath(){
    return m_ErrorImagePath;
}
/** Devuelve el path de imagenes ok*/
QString In2ConfiguratorVision::getGoodPath(){
    return m_GoodImagePath;
}
