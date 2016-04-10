/** @addtogroup in2SystemInformation
 *  @{*/
/** @file in2SystemInformation.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo principal para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: in2SystemInformation.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#include <QStringList>
#include <string.h>
#include <QSettings>
#include <QProcess>
#include <QProcess>
#include <windows.h>
#include <QDebug>
#include <QWidget>
#include <QObject>

#include "in2RegistryInformation.h"

//Constructor 
in2RegistryInformation::in2RegistryInformation(QWidget *parent)
{

}


in2RegistryInformation::~in2RegistryInformation(){

}


QString in2RegistryInformation::getRegistryInfo(){

/*
    QString rtn;
    QString tmp,value;
    // extract Windows Registry settings and save the data we're interested in..Qt method
    QSettings settings( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft", QSettings::NativeFormat );
    tmp.clear();
    value=settings.value("Windows NT/CurrentVersion/ProductId").toString().trimmed();
    tmp.append(QString("Windows NT/CurrentVersion/ProductId=%1\n").arg(value));
    rtn.append(tmp);
    tmp.clear();
    value=settings.value("Internet Explorer/Registration/ProductID").toString().trimmed();
    tmp.append(QString("Internet Explorer/Registration/ProductID=%1\n").arg(value));
    rtn.append(tmp);
    tmp.clear();
    value=settings.value("Cryptography/MachineGuid").toString().trimmed();
    tmp.append(QString("Cryptography/MachineGuid=%1\n").arg(value));
    rtn.append(tmp);*/

    QString rtn;
    QString tmp,value;
    // extract Windows Registry settings and save the data we're interested in..Qt method
    QSettings settings( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft", QSettings::NativeFormat );
    tmp.clear();
    value=settings.value("Windows NT/CurrentVersion/BuildLab").toString().trimmed();
    tmp.append(QString("Windows NT/CurrentVersion/BuildLab=%1\n").arg(value));
    rtn.append(tmp);
    tmp.clear();
    value=settings.value("Windows NT/CurrentVersion/BuildLabEx").toString().trimmed();
    tmp.append(QString("Windows NT/CurrentVersion/BuildLabEx=%1\n").arg(value));
    rtn.append(tmp);
    /*tmp.clear();
    value=settings.value("Cryptography/MachineGuid").toString().trimmed();
    tmp.append(QString("Cryptography/MachineGuid=%1\n").arg(value));
    rtn.append(tmp);*/


    /*settings =QSettings("HKEY_LOCAL_MACHINE",QSettings::NativeFormat);
    QStringList wids;
    wids.append( settings.value( "SOFTWARE/Microsoft/Windows NT/CurrentVersion/ProductId" ).toString().trimmed() );
    QString strHKEY=QString("HKEY_LOCAL_MACHINE");
    QString key=QString("SOFTWARE/Microsoft/Windows NT/CurrentVersion/ProductId");
    value=getValue(strHKEY, key);
    QString key2=QString("SOFTWARE/Microsoft/Windows NT/CurrentVersion/ProductId");
    value2=getValue(ID_HKEY_LOCAL_MACHINE, key2);*/

    return(rtn);


}


QString in2RegistryInformation::getValue(int idHKEY, QString key){

    QString retValue;

    /*
    #define ID_HKEY_CLASSES_ROOT    0
    #define ID_HKEY_CURRENT_USER    1
    #define ID_HKEY_LOCAL_MACHINE   2
    #define ID_HKEY_USERS           3
    #define ID_HKEY_CURRENT_CONFIG  4
     */

    if (idHKEY==ID_HKEY_CLASSES_ROOT){
        QSettings settings0( "HKEY_CLASSES_ROOT", QSettings::NativeFormat );
        retValue =  settings0.value(key).toString().trimmed();
    }
    else if (idHKEY==ID_HKEY_CURRENT_USER){
        QSettings settings1( "HKEY_CURRENT_USER", QSettings::NativeFormat );
        retValue =  settings1.value(key).toString().trimmed();
    }
    else if (idHKEY==ID_HKEY_LOCAL_MACHINE){
        QSettings settings2( "HKEY_LOCAL_MACHINE", QSettings::NativeFormat );
        retValue =  settings2.value(key).toString().trimmed();
    }

    return (retValue);
}


QString in2RegistryInformation::getValue(QString strHKEY, QString key){
    QString currentValue;
    QSettings settings( strHKEY, QSettings::NativeFormat );
    currentValue =  settings.value( key ).toString().trimmed();
    return (currentValue);
}
