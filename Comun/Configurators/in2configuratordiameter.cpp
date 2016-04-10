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
#include "in2configuratordiameter.h"
#include "in2defines.h"
#include <QSettings>
#include <QApplication>

In2ConfiguratorDiameter::In2ConfiguratorDiameter(int sensor)
{
    QString file;
    if (sensor==IN2MANAGER_DIN) file=QString("%1\\Config\\In2DiameterIN.ini").arg(QApplication::applicationDirPath());
    else if (sensor==IN2MANAGER_DOUT) file=QString("%1\\Config\\In2DiameterOUT.ini").arg(QApplication::applicationDirPath());
    QSettings FicheroIni(file,QSettings::IniFormat,0);
    FicheroIni.beginGroup("SensorDiametro");
    iPuerto=FicheroIni.value("Puerto",5).toInt();

    QString dummy_string=FicheroIni.value("Estado","Off").toString();
    if (dummy_string.compare("On",Qt::CaseInsensitive)==0) monitorEnabled=true;
    else monitorEnabled=false;
    //Solo si no esta habilitado el puerto serie, chequeo si lo hace el PLC
    if (!monitorEnabled){
        PLCEnabled=FicheroIni.value("PLCEnabled",false).toBool();
    }
    FicheroIni.endGroup();
    FicheroIni.beginGroup("Calibracion");
    cal_db1=FicheroIni.value("DiametroBobina_1",200.0).toDouble();
    cal_s1=FicheroIni.value("ValorLeido_1",250.0).toDouble();
    cal_db2=FicheroIni.value("DiametroBobina_2",300.0).toDouble();
    cal_s2=FicheroIni.value("ValorLeido_2",50.0).toDouble();
    FicheroIni.endGroup();
}

bool In2ConfiguratorDiameter::getEnabled(){
    return monitorEnabled;
}

bool In2ConfiguratorDiameter::getPLCEnabled(){
    return PLCEnabled;
}
