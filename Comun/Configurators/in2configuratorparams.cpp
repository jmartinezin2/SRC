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
#include "in2configuratorparams.h"
#include <QFile>
#include <QSettings>
#include <QMessageBox>
#include "Memorymap.h"
#include "inkjetdebug.h"
#include "in2defines.h"

In2ConfiguratorParams::In2ConfiguratorParams(QObject *parent) : QObject(parent)
{

}

In2ConfiguratorParams::~In2ConfiguratorParams()
{

}


void In2ConfiguratorParams::readConfig(){

    QString dummy_string;
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    dummy_string=QString("%1\\Config\\In2Parameters.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(dummy_string);
    if (!iniFile.exists()){
        infoDebug->ProcessMsg(LOGGER_ERROR,"NO HAY FICHERO IN2PARAMETERS.INI!");
        
        QMessageBox::information(0,"ERROR",tr("No se encuentra el fichero de parámetros.\nCierre la aplicación y pongase en contacto con In2"),QMessageBox::Ok);
        return;
    }
    QSettings FicheroIni(dummy_string,QSettings::IniFormat,0);
    MemoryMap *memoryMap=MemoryMap::request(0);
    FicheroIni.beginGroup(QString("XUSB"));
    for (unsigned int i=1;i<MAXSCORPIONS+1;i++){
        dummy_string=FicheroIni.value(QString("SerialXUSB%1").arg(i),"000000000000").toString();

    }
    for (unsigned int i=1;i<MAXSCORPIONS+1;i++){
        dummy_string=FicheroIni.value(QString("VolumenGota%1").arg(i),"0@0@0@0@0@0@0@0").toString();
        QStringList tmp=dummy_string.split('@');
        if (tmp.count()>8){
            dummy_string=QString("%1@%2@%3@%4@%5@%6@%7@%8").arg(tmp.at(0)).arg(tmp.at(1)).arg(tmp.at(2)).arg(tmp.at(3)).arg(tmp.at(4)).arg(tmp.at(5)).arg(tmp.at(6)).arg(tmp.at(7));
        }
        else if (tmp.count()<8){
            while (tmp.count()<8) tmp.append("0");
            dummy_string=QString("%1@%2@%3@%4@%5@%6@%7@%8").arg(tmp.at(0)).arg(tmp.at(1)).arg(tmp.at(2)).arg(tmp.at(3)).arg(tmp.at(4)).arg(tmp.at(5)).arg(tmp.at(6)).arg(tmp.at(7));
        }
        infoDebug->ProcessMsg(LOGGER_WARNING,QString("VolumenGota%1=%1").arg(i).arg(dummy_string));

    }
    FicheroIni.endGroup();
    FicheroIni.beginGroup(QString("TablaDiametroVsFreno"));
    QStringList qstrListkeys=FicheroIni.childKeys();
    foreach (QString str,qstrListkeys){
        QString value=FicheroIni.value(str,DEFAULT_FRENO).toString();
        memoryMap->m_mapa.m_maquina.qmDiamPresion.insert(str.toInt(),value.toInt());
    }
    //Si la cuenta es cero, metemos un valor por defecto
    if ( memoryMap->m_mapa.m_maquina.qmDiamPresion.count()==0){
        memoryMap->m_mapa.m_maquina.qmDiamPresion.insert(DEFAULT_ANCHO,DEFAULT_FRENO);
    }
    memoryMap->m_mapa.m_maquina.AnchoBobina=memoryMap->m_mapa.m_maquina.qmDiamPresion.end().key();
    FicheroIni.endGroup();
}
