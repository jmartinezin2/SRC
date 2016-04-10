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
#include "in2configuratorlampara.h"
#include "inkjetdebug.h"
#include <QFile>
#include <QMessageBox>
#include <QSettings>

#define VENDOR "Vendor"
#define ENABLED "Enabled"
#define POSDIN "PosDin"
#define INTDIN "IntDin"

#define STR_PHOSEON "PHOSEON"
#define STR_HONLE   "HONLE"

In2ConfiguratorLampara::In2ConfiguratorLampara()
{
    m_strVendor=STR_PHOSEON;
    m_Vendor=PHOSEON;
    m_PosDinamico=false;
    m_IntDinamico=true;
    m_enabled=false;

    InkjetDebug *infoDebug=InkjetDebug::request(0);
    QString str=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(str);
    if (!iniFile.exists()){
        QString txt=QString("No In2.ini");
        infoDebug->ProcessMsg(LOGGER_ERROR,txt);
        QMessageBox::information(0,"ERROR",txt,QMessageBox::Ok);
        return;
    }
    QSettings FicheroIni(str,QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("LAMPARA"));
    m_strVendor=FicheroIni.value(VENDOR,STR_PHOSEON).toString();

    if (!m_strVendor.compare(STR_PHOSEON,Qt::CaseInsensitive)) m_Vendor=PHOSEON;
    else if (!m_strVendor.compare(STR_HONLE,Qt::CaseInsensitive)) m_Vendor=HONLE;

    //m_Vendor=FicheroIni.value(VENDOR,PHOSEON).toInt();
    infoDebug->ProcessMsg(LOGGER_STORE,QString("Lampara=%1").arg(QString(m_strVendor)));
    m_enabled=FicheroIni.value(ENABLED,false).toBool();
    infoDebug->ProcessMsg(LOGGER_STORE,QString("Enabled=%1").arg(m_enabled));
    m_PosDinamico=FicheroIni.value(POSDIN,false).toBool();
    infoDebug->ProcessMsg(LOGGER_STORE,QString("Posdin=%1").arg(m_PosDinamico));
    m_IntDinamico=FicheroIni.value(INTDIN,true).toBool();
    infoDebug->ProcessMsg(LOGGER_STORE,QString("Intdin=%1").arg(m_IntDinamico));
    FicheroIni.endGroup();
}
/** El fabricante de lampara*/
QString In2ConfiguratorLampara::getStrVendor(){
    return m_strVendor;
}
/** El fabricante de lampara*/
int In2ConfiguratorLampara::getVendor(){
    return m_Vendor;
}
/** Si existe posicionamiento dinamico*/
bool In2ConfiguratorLampara::getPosDinamico(){
    return m_PosDinamico;
}
/** Si existe intesidad dinamica*/
bool In2ConfiguratorLampara::getIntDinamico(){
    return m_IntDinamico;
}
/** Si esa habilitado el gestor*/
bool In2ConfiguratorLampara::getEnabled(){
    return m_enabled;
}
