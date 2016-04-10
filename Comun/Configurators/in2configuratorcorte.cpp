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
#include "in2configuratorcorte.h"
#include <QSettings>
#include <QFile>
#include "inkjetdebug.h"

#define ENABLED "Enabled"

In2ConfiguratorCorte::In2ConfiguratorCorte()
{
    m_bEnabled=false;
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    QString str=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(str);
    if (!iniFile.exists()){
        QString txt=QString("No In2.ini");
        infoDebug->ProcessMsg(LOGGER_ERROR,txt);
        return;
    }
    QSettings FicheroIni(str,QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("CORTE"));
    m_bEnabled=FicheroIni.value(ENABLED,false).toBool();
    infoDebug->ProcessMsg(LOGGER_STORE,QString("CorteEnabled=%1").arg(m_bEnabled));
    FicheroIni.endGroup();

}

/** Devuelve si esta activa o no*/
bool In2ConfiguratorCorte::isEnabled(){
    return m_bEnabled;
}
