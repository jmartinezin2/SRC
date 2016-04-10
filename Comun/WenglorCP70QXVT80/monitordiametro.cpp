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
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QTextCursor>
#include <QByteArray>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QApplication>

#include "monitordiametro.h"
#include "in2gestordiametro.h"

#define TIMEOUT_UPDATE TIMEOUT_MONITORING/*1000*/

//Por defecto 10s de delay
#define DEFAULT_LOG_SAVE_DELAY 10000

//Tiempo en segundos para limpiar automaticamente los logs
#define TIME_s_TO_CLEAR      120

//Tiempo en segundos para guardar datos en fichero
#define TIME_s_TO_SAVE      60
//Numero de veces de guardado que implica un nuevo fichero
#define TIMES_TO_NEWFILE    5


//Una sola instancia
//MonitorDiametro *MonitorDiametro::mInstance  = NULL;


/** Lllamda a instancia unica*/
/*MonitorDiametro *MonitorDiametro::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new MonitorDiametro(parent);
    }
    return mInstance;
}*/


MonitorDiametro::MonitorDiametro(QObject *parent)
{
    isPortOpen=false;
    numeroEquipos=1;
    monitorEnabled=false;
    m_valueFromPLC=0;
    m_rawValueFromPLC=0;
    /*abrirFicheroIni();
    init();*/
}


MonitorDiametro::~MonitorDiametro(){
    delete (miGestorDiametro);
}

bool MonitorDiametro::isEnabled(){
    return monitorEnabled;
}

bool MonitorDiametro::isPLCEnabled(){
    return PLCEnabled;
}


/** Carga un fichero de configuracion*/
void MonitorDiametro::setIniFile(QString file){
    m_iniFile=file;
}

/** Lo inicializa*/
void MonitorDiametro::init(){

    primeravez=true;

    contadorMonitor=0;
    abrirFicheroIni();
    if (monitorEnabled){
        //Llamamos al gestor para coger datos*/
        miGestorDiametro = new In2GestorDiametro(this,1);
        if (miGestorDiametro->numEquipos()>0){
            connect(miGestorDiametro,SIGNAL(In2GDError(QString)),this,SLOT(sltError(QString)));
            connect(miGestorDiametro,SIGNAL(sglCambioActual(int)),this,SLOT(sltCambioActual(int)));
            connect(miGestorDiametro,SIGNAL(sglCambioMaximo(int)),this,SLOT(sltCambioMaximo(int)));
            connect(miGestorDiametro,SIGNAL(sglCambioMinimo(int)),this,SLOT(sltCambioMinimo(int)));
            connect(miGestorDiametro,SIGNAL(sglCambioFiltrado(int)),this,SLOT(sltCambioFiltrado(int)));
            connect(this,SIGNAL(sglReadWenglorDiameter()),miGestorDiametro,SLOT(sltReadWenglorDiameter()));
            connect(miGestorDiametro,SIGNAL(sglEstadoPuerto(bool)),this,SLOT(sltEstadoPuerto(bool)));
            connect(this,SIGNAL(sglDatosCalibracion(double,double,double,double)),miGestorDiametro,SLOT(sltDatosCalibracion(double,double,double,double)));
            QTimer::singleShot(0,this,SLOT(sltActualizaDatos_1()));
            emit sglDatosCalibracion(cal_s1,cal_db1,cal_s2,cal_db2);
        }
        openPort();
    }

}


void MonitorDiametro::sltActualizaDatos_1(){

    if (isPortOpen){
        emit sglReadWenglorDiameter();
    }

    QTimer::singleShot(TIMEOUT_UPDATE,this,SLOT(sltActualizaDatos_1()));
}



void MonitorDiametro::sltError(const QString &txt){
    //QString dummyStr=QString("Error:%1").arg(txt);
    //write2Log(dummyStr);
}


void MonitorDiametro::sltCambioActual(int equipo){
    double dummyDouble;
    dummyDouble=miGestorDiametro->getCurrentDiameter(equipo);
    emit sgnWenglorDiameter(dummyDouble);
}

void MonitorDiametro::sltCambioMaximo(int equipo){
    /*double dummyDouble;
    dummyDouble=miGestorDiametro->getMaxDiameter(equipo);*/
}


void MonitorDiametro::sltCambioMinimo(int equipo){
    /*double dummyDouble;
    dummyDouble=miGestorDiametro->getMinDiameter(equipo);*/
}

void MonitorDiametro::sltCambioFiltrado(int equipo){
    /*double dummyDouble;
    dummyDouble=miGestorDiametro->getFilteredDiameter(equipo);*/
}

void MonitorDiametro::sltCambioReady(int equipo){
    bool dummy;
    //int indiceEquipo=ui->m_Equipo->currentIndex();
    //if (indiceEquipo==equipo){
        dummy=miGestorDiametro->getReady(equipo);
        if (dummy){
            //ui->lbl_Ready->setText("Ready");
        }
        else{
            //ui->lbl_Ready->setText("Error");
        }
    //}
}

void MonitorDiametro::openPort(){
    //emit sglOpenMosbusPort(puerto);
    miGestorDiametro->sltOpenPort(puerto);
}


void MonitorDiametro::closePort(){
    if (!isPortOpen) return;
    emit sglClosePort();
}

/** Abre el fichero de configuración*/
void MonitorDiametro::abrirFicheroIni(){

    QString dummy_string;

    //QString path=QString("%1\\Config\\In2WenglorCP70QXVT80.ini").arg(QApplication::applicationDirPath());
    QString path=m_iniFile;
    QSettings FicheroIni(path,QSettings::IniFormat,0);

    FicheroIni.beginGroup("SensorDiametro");

    iPuerto=FicheroIni.value("Puerto",5).toInt();

    if (iPuerto<10)
        puerto=QString("COM%1").arg(iPuerto);
    else
        puerto=QString("\\\\.\\com%1").arg(iPuerto);   

    dummy_string=FicheroIni.value("Estado","Off").toString();
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

void MonitorDiametro::sltEstadoPuerto(bool estado){
    isPortOpen=estado;
    monitorEnabled=estado;
}
/** Devuelve el diametro en raw*/
QString MonitorDiametro::getRawDiameter(){
    QString rtn;
    if (monitorEnabled)
        rtn=miGestorDiametro->getRawDiameter();
    else{
        if (PLCEnabled){
            rtn=QString("%1").arg(m_rawValueFromPLC);
        }
    }
    return rtn;
}

/** Devuelve el diametro actual calibrado*/
double MonitorDiametro::getCurrentDiameter(int index){
    double rtn=0;
    if (monitorEnabled)
        rtn=miGestorDiametro->getCurrentDiameter(index);
    else{
        if (PLCEnabled){
            //rtn=m_valueFromPLC;
            rtn=m_rawValueFromPLC;
        }
    }
    return rtn;
}

/** Calcula y guarda el valor actual de diametro*/
void MonitorDiametro::storeValueFromPLC(int value){    
    if (m_rawValueFromPLC!=value){
        m_rawValueFromPLC=value;
        /*double valorLeido_mm=0.01*value;
        m_valueFromPLC=cal_db1+(((cal_db2-cal_db1)*(valorLeido_mm-cal_s1))/(cal_s2-cal_s1));*/
    }
}
