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
#include "In2ConfiguratorPrinter.h"
#include "MemoryMap.h"
#include "in2defines.h"
#include <QFile>
#include "inkjetdebug.h"
#include <QSettings>
#include <QMessageBox>

In2ConfiguratorPrinter::In2ConfiguratorPrinter(QObject *parent) : QObject(parent)
{

}

In2ConfiguratorPrinter::~In2ConfiguratorPrinter()
{

}
/** Devuelve si existe el fichero de config*/
bool In2ConfiguratorPrinter::fileExists(){
    QString foo=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    return QFile::exists(foo);
}
/** Devuelve si existe el fichero de config*/
QString In2ConfiguratorPrinter::configFile(){
    QString foo=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    return foo;
}


/** Abre y lee la configuración*/
void In2ConfiguratorPrinter::readConfig(){

    QString dummy_string;
    int dummy_int=0;
    double dummy_double=0;
    dummy_string=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(dummy_string);
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    if (!iniFile.exists()){
        infoDebug->ProcessMsg(LOGGER_ERROR,"NO HAY FICHERO IN2.INI!");
        //((Principal*)GlobalGetMainWindow())->ReceiveDebugInfo(LOGGER_ERROR,"NO HAY FICHERO IN2.INI!");
        QMessageBox::information(0,"ERROR",tr("No se encuentra el fichero de configuracion.\nCierre la aplicacion y pongase en contacto con In2"),QMessageBox::Ok);
        return;
    }
    QSettings FicheroIni(dummy_string,QSettings::IniFormat,0);
    MemoryMap *memoryMap=MemoryMap::request(0);
    FicheroIni.beginGroup(QString("APLICACION"));
    dummy_string=FicheroIni.value(QString("Lan"),"es").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Languaje=%1").arg(dummy_string));
    //((Principal*)GlobalGetMainWindow())->ReceiveDebugInfo(LOGGER_WARNING,QString("Languaje=%1").arg(dummy_string));
    dummy_string=FicheroIni.value(QString("Cliente"),CLIENT_IN2).toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Cliente=%1").arg(dummy_string));
    memoryMap->setClient(dummy_string);
    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("OPC"));
    dummy_string=FicheroIni.value(QString("NombreServidor"),"None").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("NombreServidor=%1").arg(dummy_string));
    memoryMap->m_mapa.m_maquina.m_opc.Nombre=dummy_string;

    dummy_string=FicheroIni.value(QString("Estado"),"Off").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Estado=%1").arg(dummy_string));
    if (!dummy_string.compare("On",Qt::CaseInsensitive)) memoryMap->m_mapa.m_maquina.m_opc.estado=1;
    else memoryMap->m_mapa.m_maquina.m_opc.estado=0;


    FicheroIni.beginGroup(QString("LDAP"));
    bool dummy_bool=FicheroIni.value(QString("Active"),0).toBool();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("LDAP=%1").arg(dummy_bool));
    memoryMap->m_mapa.m_config.m_LDAPActive=dummy_bool;
    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("SAI"));
    dummy_bool=FicheroIni.value(QString("SAIControl"),0).toBool();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("SAIControl=%1").arg(dummy_bool));
    memoryMap->m_mapa.m_sai.testSai=dummy_bool;
    dummy_string=FicheroIni.value(QString("SAIFile"),"").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("SAIFile=%1").arg(dummy_string));
    memoryMap->m_mapa.m_sai.tokenFile=dummy_string;
    FicheroIni.endGroup();


    FicheroIni.beginGroup(QString("NET"));
    dummy_bool=FicheroIni.value(QString("UseLocalCopy"),0).toBool();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("UseLocalCopy=%1").arg(dummy_bool));
    memoryMap->m_mapa.m_variables.bUseLocalCopy=dummy_bool;
    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("Fluidica"));
    dummy_string=FicheroIni.value(QString("Tipo"),"None").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Tipo=%1").arg(dummy_string));
    if (dummy_string=="Hydra") memoryMap->m_mapa.m_maquina.m_fluidica.Tipo=FLUIDICA_HYDRA;
    if (dummy_string=="EIK") memoryMap->m_mapa.m_maquina.m_fluidica.Tipo=FLUIDICA_EIK;
    dummy_string=FicheroIni.value(QString("Conectado"),"Off").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Conectado=%1").arg(dummy_string));
    if (dummy_string=="On") memoryMap->m_mapa.m_maquina.m_fluidica.Conectado=1;
    else memoryMap->m_mapa.m_maquina.m_fluidica.Conectado=0;
    dummy_int=FicheroIni.value(QString("Numero"),1).toInt();
    memoryMap->m_mapa.m_maquina.m_fluidica.NumConfigured=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Numero=%1").arg(dummy_int));
    for (int x=0;x<dummy_int;x++){
        QString tmp=QString("Autostart%1").arg(x);
        if (FicheroIni.value(tmp,0).toInt()!=0){
            memoryMap->m_mapa.m_maquina.m_fluidica.bAutostart[x]=true;
            infoDebug->ProcessMsg(LOGGER_WARNING,QString("Autostart%1=true").arg(x));
        }
        else
            infoDebug->ProcessMsg(LOGGER_WARNING,QString("Autostart%1=false").arg(x));

        tmp=QString("AutoShutdown%1").arg(x);
        if (FicheroIni.value(tmp,0).toInt()!=0){
            memoryMap->m_mapa.m_maquina.m_fluidica.bAutoShutdown[x]=true;
            infoDebug->ProcessMsg(LOGGER_WARNING,QString("AutoShutdown%1=true").arg(x));
        }
        else
            infoDebug->ProcessMsg(LOGGER_WARNING,QString("AutoShutdown%1=false").arg(x));
    }

    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("Mantenimiento"));
    dummy_int=FicheroIni.value(QString("HorasUsoLampara"),HORAS_USO_LAMPARA_DEFAULT).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("HorasUsoLampara=%1").arg(dummy_int));
    memoryMap->m_mapa.m_maquina.HorasMaximasUsoLampara=dummy_int;
    dummy_int=FicheroIni.value(QString("DiasUsoFiltro"),DIAS_USO_FILTRO_DEFAULT).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DiasUsoFiltro=%1").arg(dummy_int));
    memoryMap->m_mapa.m_maquina.DiasMaximosUsoFiltro=dummy_int;
    dummy_int=FicheroIni.value(QString("TemperaturaMaximaLampara"),TEMPERATURA_MAXIMA_LAMPARA_DEFAULT).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("TemperaturaMaximaLampara=%1").arg(dummy_int));
    memoryMap->m_mapa.m_maquina.TemperaturaMaximaLampara=dummy_int;
    dummy_int=FicheroIni.value(QString("DiasLimpiezaLampara"),DIAS_MAX_LIMPIEZA_LAMPARA_DEFAULT).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DiasLimpiezaLampara=%1").arg(dummy_int));
    memoryMap->m_mapa.m_maquina.DiasMaximosLimpiezaLampara=dummy_int;
    FicheroIni.endGroup();


    FicheroIni.beginGroup(QString("XUSB"));
    dummy_string=FicheroIni.value(QString("NombreColaImpresion"),"Unknown").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("NombreColaImpresion=%1").arg(dummy_string));
    memoryMap->m_mapa.m_database.NombreColaImpresion=QString("%1").arg(dummy_string);

    dummy_string=FicheroIni.value(QString("RutaColaImpresion"),"Unknown").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("RutaColaImpresion=%1").arg(dummy_string));
    memoryMap->m_mapa.m_database.RutaColaImpresion=QString("%1").arg(dummy_string);

    dummy_string=FicheroIni.value(QString("DefaultImagePath"),QApplication::applicationDirPath()).toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DefaultImagePath=%1").arg(dummy_string));
    memoryMap->m_mapa.m_variables.defaultImagePath=QString("%1").arg(dummy_string);

    dummy_int=FicheroIni.value(QString("DefaultNumHeads"),8).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DefaultNumHeads=%1").arg(dummy_int));
    memoryMap->m_mapa.m_variables.DefaultNumHeads=dummy_int;

    //Lo pasamos a una sola variable
    memoryMap->m_mapa.m_database.RutaColaImpresion=QString("%1\\%2").arg(memoryMap->m_mapa.m_database.RutaColaImpresion).arg(memoryMap->m_mapa.m_database.NombreColaImpresion);
    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("POST"));
    dummy_string=FicheroIni.value(QString("ArchivoPost"),"").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("ArchivoPost=%1").arg(dummy_string));
    memoryMap->m_mapa.m_variables.NombreArchivoPost=QString("%1").arg(dummy_string);
    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("DEFAULTS"));
    dummy_double=FicheroIni.value(QString("WebWidth"),DEFAULT_WEB_WIDTH).toDouble();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("WebWidth=%1").arg(dummy_double));
    memoryMap->m_mapa.m_machinedefaults.WebWidth=dummy_double;
    memoryMap->m_mapa.m_variables.WebWidth=dummy_double;

    dummy_double=FicheroIni.value(QString("WebHeight"),DEFAULT_WEB_HEIGHT).toDouble();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("WebHeight=%1").arg(dummy_double));
    memoryMap->m_mapa.m_machinedefaults.WebHeight=dummy_double;
    memoryMap->m_mapa.m_variables.WebHeight=dummy_double;


    dummy_bool=FicheroIni.value(QString("MaculaTrasPrint"),1).toBool();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("MaculaTrasPrint=%1").arg(dummy_bool));
    memoryMap->m_mapa.m_machinedefaults.MaculaTrasPrint=dummy_bool;
    dummy_int=FicheroIni.value(QString("DistanceToStop"),DEFAULT_DISTANCIA_STOP).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DistanceToStop=%1").arg(dummy_int));
    memoryMap->m_mapa.m_machinedefaults.DistanciaStop=dummy_int;

    dummy_int=FicheroIni.value(QString("VelocidadArranque"),VEL_MIN_ARRANQUE).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("VelocidadArranque=%1").arg(dummy_int));
    memoryMap->m_mapa.m_machinedefaults.VelArranque=dummy_int;
    dummy_int=FicheroIni.value(QString("TiempoArranque"),TIEMPO_ARRANQUE).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("TiempoArranque=%1").arg(dummy_int));
    memoryMap->m_mapa.m_machinedefaults.TiempoArranque=dummy_int;

    dummy_int=FicheroIni.value(QString("TiempoEmpalme"),TIEMPO_ARRANQUE_EMPALME).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("TiempoEmpalme=%1").arg(dummy_int));
    memoryMap->m_mapa.m_machinedefaults.JointTime=dummy_int;

    dummy_int=FicheroIni.value(QString("ControlHead"),DEFAULT_CONTROL_HEAD).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("ControlHead=%1").arg(dummy_int));
    memoryMap->m_mapa.m_machinedefaults.ControlHead=dummy_int;

    dummy_bool=FicheroIni.value(QString("FiltroMaculas"),false).toBool();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("FiltroMaculas=%1").arg(dummy_bool));
    memoryMap->m_mapa.m_machinedefaults.filtroMaculas=dummy_bool;
    memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula=dummy_bool;

    dummy_double=FicheroIni.value(QString("ErrorFiltroMacula"),0).toDouble();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("ErrorFiltroMacula=%1").arg(dummy_double));
    memoryMap->m_mapa.m_impresion.m_fltrMacula.fError=dummy_double;

    dummy_double=FicheroIni.value(QString("DistanciaFiltroMacula"),0).toDouble();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DistanciaFiltroMacula=%1").arg(dummy_double));
    memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia=dummy_double;
    //Limites
    memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax=memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia+memoryMap->m_mapa.m_impresion.m_fltrMacula.fError;
    memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax=memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia-memoryMap->m_mapa.m_impresion.m_fltrMacula.fError;

    dummy_int=FicheroIni.value(QString("TiempoDelayedMonitor"),START_TIMED).toInt();
    memoryMap->m_mapa.m_machinedefaults.delayedMonitorTime=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("TiempoDelayedMonitor=%1").arg(dummy_int));

    dummy_int=FicheroIni.value(QString("DMaxBobinaEntrada"),DIAMETRO_BOBINA_ENTRADA_MAX).toInt();
    memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaEntrada=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DMaxBobinaEntrada=%1").arg(dummy_int));

    dummy_int=FicheroIni.value(QString("DMaxBobinaSalida"),DIAMETRO_BOBINA_SALIDA_MAX).toInt();
    memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaSalida=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DMaxBobinaSalida=%1").arg(dummy_int));

    dummy_int=FicheroIni.value(QString("DMinBobinaEntrada"),DIAMETRO_BOBINA_ENTRADA_MIN).toInt();
    memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaEntrada=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DMinBobinaEntrada=%1").arg(dummy_int));

    dummy_int=FicheroIni.value(QString("DMinBobinaSalida"),DIAMETRO_BOBINA_SALIDA_MIN).toInt();
    memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaSalida=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DMinBobinaSalida=%1").arg(dummy_int));

    dummy_int=FicheroIni.value(QString("DMinBultoSalida"),DIAMETRO_MINIMO_BULTO_SALIDA).toInt();
    memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DMinBultoSalida=%1").arg(dummy_int));

    dummy_int=FicheroIni.value(QString("DMinBultoEntrada"),DIAMETRO_MINIMO_BULTO_ENTRADA).toInt();
    memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoEntrada=dummy_int;
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("DMinBultoEntrada=%1").arg(dummy_int));



    FicheroIni.endGroup();


    memoryMap->m_mapa.m_variables.DisablePrinting=0;
}

