#include "in2printerconfigurator.h"
#include "MemoryMap.h"
#include <QFile>
#include "inkjetdebug.h"
#include <QSettings>

In2PrinterConfigurator::In2PrinterConfigurator(QObject *parent) : QObject(parent)
{

}

In2PrinterConfigurator::~In2PrinterConfigurator()
{

}
/** Abre y lee la configuración*/
void In2PrinterConfigurator::readConfig(){

    QString dummy_string;
    int dummy_int=0;
    dummy_string=QString("%1\\Config\\In2.ini").arg(QApplication::applicationDirPath());
    //Testeamos que exista
    QFile iniFile(dummy_string);
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    if (!iniFile.exists()){
        infoDebug->ProcessMsg(LOGGER_ERROR,"NO HAY FICHERO IN2.INI!");
        //((Principal*)GlobalGetMainWindow())->ReceiveDebugInfo(LOGGER_ERROR,"NO HAY FICHERO IN2.INI!");
        QMessageBox::information(0,"ERROR",QString("No se encuentra el fichero de configuración.\nCierre la aplicación y pongase en contacto con In2"),QMessageBox::Ok);
        return;
    }
    QSettings FicheroIni(dummy_string,QSettings::IniFormat,0);
    MemoryMap *memoryMap=MemoryMap::request(0);
    FicheroIni.beginGroup(QString("APLICACION"));
    dummy_string=FicheroIni.value(QString("Lan"),"es").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Languaje=%1").arg(dummy_string));
    //((Principal*)GlobalGetMainWindow())->ReceiveDebugInfo(LOGGER_WARNING,QString("Languaje=%1").arg(dummy_string));
    FicheroIni.endGroup();

    FicheroIni.beginGroup(QString("OPC"));
    dummy_string=FicheroIni.value(QString("NombreServidor"),"None").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("NombreServidor=%1").arg(dummy_string));
    memoryMap->m_mapa.m_maquina.m_opc.Nombre=dummy_string;

    dummy_string=FicheroIni.value(QString("Estado"),"Off").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Estado=%1").arg(dummy_string));
    if (!dummy_string.compare("On",Qt::CaseInsensitive)) memoryMap->m_mapa.m_maquina.m_opc.estado=1;
    else memoryMap->m_mapa.m_maquina.m_opc.estado=0;

    dummy_int=FicheroIni.value(QString("VersionMaquina"),1).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("VersionMaquina=%1").arg(dummy_int));
    memoryMap->m_mapa.m_maquina.VersionMaquina=dummy_int;
    FicheroIni.endGroup();

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
    if (dummy_string=="Cemitec") memoryMap->m_mapa.m_maquina.m_fluidica.Tipo=FLUIDICA_CEMITEC;
    dummy_string=FicheroIni.value(QString("Conectado"),"Off").toString();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Conectado=%1").arg(dummy_string));
    if (dummy_string=="On") memoryMap->m_mapa.m_maquina.m_fluidica.Conectado=1;
    else memoryMap->m_mapa.m_maquina.m_fluidica.Conectado=0;
    dummy_int=FicheroIni.value(QString("Numero"),1).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("Numero=%1").arg(dummy_int));
    for (int x=0;x<dummy_int;x++){
        QString tmp=QString("Autostart%1").arg(x);
        if (FicheroIni.value(tmp,0).toInt()!=0){
            memoryMap->m_mapa.m_maquina.m_fluidica.bAutostart[x]=true;
            infoDebug->ProcessMsg(LOGGER_WARNING,QString("Autostart%1=true").arg(x));
        }
        else
            infoDebug->ProcessMsg(LOGGER_WARNING,QString("Autostart%1=false").arg(x));


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
    dummy_int=FicheroIni.value(QString("WebWidth"),DEFAULT_WEB_WIDTH).toInt();
    infoDebug->ProcessMsg(LOGGER_WARNING,QString("WebWidth=%1").arg(dummy_int));
    memoryMap->m_mapa.m_machinedefaults.WebWidth=dummy_int;
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


    FicheroIni.endGroup();


    memoryMap->m_mapa.m_variables.DisablePrinting=0;
}
