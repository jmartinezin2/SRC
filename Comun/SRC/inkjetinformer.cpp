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
#include "inkjetinformer.h"
//#include "principal.h"
#include "globals.h"
#include <QPainter>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include <QDate>
#include <windows.h>
//Tamaño en pixeles de la aplicación


InkjetInformer::InkjetInformer(QObject *parent)
    : QObject(parent)
{

    Parent=parent;
    NivelInformer=INFORMER_WARNING;
    estado=0x00;
    LeerConfiguracionInformer();

    if (estado==0x01){       
        m_TimerInformer = new QTimer(this);
        connect(m_TimerInformer, SIGNAL(timeout()),this, SLOT(ActualizaDataSlot()));
        Init();
        m_TimerInformer->start(1000);
    }
  
}

InkjetInformer::~InkjetInformer()
{


}

void InkjetInformer::Init(){

    memset(&m_stInformer.codigo,0x00,sizeof(m_stInformer.codigo));

    contador_signal=0;
    contador_tiempo=0;

    QDate hoy=QDate::currentDate();
    diaActual=hoy.dayOfYear();

    archivoActual=(diaActual%numeroArchivos)+1;

    LeerArchivoActual();
}

void InkjetInformer::ActualizaDataSlot(){

    if (guardarPorTiempo==0x01){
        //Implemento el guardado por tiempo
        contador_tiempo++;
        if (contador_tiempo>=tiempoGuardado){
            QTimer::singleShot(0,this,SLOT(SaveLogToFile()));
        }
    }

    int diaActualTemp=QDate::currentDate().dayOfYear();
    if (diaActualTemp>diaActual){
        //Si cambiamos de dia mientras la aplicacion está en marcha
        SaveLogToFile();
        diaActual=diaActualTemp;
        archivoActual=(diaActual%numeroArchivos)+1;
        LeerArchivoActual();
    }


}

void InkjetInformer::LeerConfiguracionInformer(){

    QString dirPath;
    QString dummy_string;

    dirPath=QApplication::applicationDirPath();

    dummy_string=QString("%1\\Config\\In2Informer.ini").arg(dirPath);
    //Testeamos que exista
    QFile iniFile(dummy_string);
    if (!iniFile.exists()){
        return;
    }
    QSettings FicheroIni(dummy_string,QSettings::IniFormat,0);

    FicheroIni.beginGroup(QString("ConfigInformer"));

    dummy_string=FicheroIni.value(QString("Estado"),"Off").toString();
	if (dummy_string=="On") estado=0x01;
	else estado=0x00;
	
    ruta=FicheroIni.value(QString("Ruta"),"C:\\In2Log").toString();
    nombreBase=FicheroIni.value(QString("Nombre"),"In2Informer").toString();
    numeroArchivos=FicheroIni.value(QString("NumeroArchivos"),7).toInt();

    dummy_string=FicheroIni.value(QString("GuardarPorTiempo"),"Off").toString();
    if (dummy_string=="On") guardarPorTiempo=0x01;
    else guardarPorTiempo=0x00;

    dummy_string=FicheroIni.value(QString("GuardarPorNumeroEventos"),"Off").toString();
    if (dummy_string=="On") guardarPorNumeroEventos=0x01;
    else guardarPorNumeroEventos=0x00;

    tiempoGuardado=FicheroIni.value(QString("TiempoGuardado"),120).toInt();
    numeroEventosGuardado=FicheroIni.value(QString("EventosGuardado"),32).toInt();
    if (numeroEventosGuardado>=N_ITEMS_TABLA_INFORMER) numeroEventosGuardado=N_ITEMS_TABLA_INFORMER;

    FicheroIni.endGroup();
}

void InkjetInformer::LeerArchivoActual(){

    NombreRutaArchivoActual=QString("%1\\%2_%3de%4.txt")
            .arg(ruta)
            .arg(nombreBase)
            .arg(archivoActual)
            .arg(numeroArchivos);

    QFile fichero(NombreRutaArchivoActual);
    if (!fichero.exists()){
        //Si no existe, hay que crearlo
        fichero.open(QIODevice::ReadWrite | QIODevice::Text);
    }

    if (!fichero.open(QIODevice::ReadWrite | QIODevice::Text)){
        //Si no se puede abrir, nos salimos
        return;
    }


    QTextStream stream(&fichero);
    QString lectura = stream.readLine(10);
    QDate fechaleida=QDate::fromString(lectura,"dd/MM/yyyy");

    qDebug() << "Valores leidos" << lectura;

    QDate hoy=QDate::currentDate();
    QString hoy_s=hoy.toString("dd/MM/yyyy");
    QDate hoy_f=QDate::fromString(hoy_s,"dd/MM/yyyy");

    if (hoy_f==fechaleida){
        // Continuamos insertando informacion
        qDebug() << "Hoy es: " << hoy_f;
        fichero.close();
    }
    else{
        // Hay que borrar el fichero, es de un dia anterior
        fichero.close();
        fichero.remove();

        QFile ficheronuevo(NombreRutaArchivoActual);
        ficheronuevo.open(QIODevice::ReadWrite | QIODevice::Text);
        ficheronuevo.close();
    }

}


/** Procesa mensaje */
void InkjetInformer::ProcessMsg(unsigned char codigo,const QString & rcv){

    if (estado==0x00) return;

    QString mensaje=QString("%1 %2 %3")
            .arg(QDate::currentDate().toString("dd/MM/yyyy"))
            .arg(QTime::currentTime().toString("hh:mm:ss"))
            .arg(rcv);

    m_stInformer.TablaInformer[contador_signal]=mensaje;
    m_stInformer.codigo[contador_signal]=codigo;
    contador_signal++;
    if ( (guardarPorNumeroEventos==0x01) && (contador_signal>=numeroEventosGuardado)){
        QTimer::singleShot(0,this,SLOT(SaveLogToFile()));
    }
    else if (codigo==INFORMER_WRITE){
        // Con este codigo se fuerza una escritura
        // aunque no se haya cumplido el tiempo o el numero de eventos de guardado
        QTimer::singleShot(0,this,SLOT(SaveLogToFile()));
    }

    if (contador_signal>=N_ITEMS_TABLA_INFORMER) contador_signal=0;
}


void InkjetInformer::SaveLogToFile(){

    if (contador_signal==0x00){
        contador_tiempo=0;
        return;
    }

    QString temp;
    QFile fichero(NombreRutaArchivoActual);

    fichero.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
    QTextStream stream(&fichero);

    for (int i=0;i<contador_signal;i++){
        temp=m_stInformer.TablaInformer[i];
        stream << temp << "\n";
    }

    fichero.close();

    contador_signal=0;
    contador_tiempo=0;

}

