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
#include "in2reportconfigurator.h"
#include "in2defines.h"
#include <QSettings>
#include <QApplication>
#include <QFile>


#define DEFAULT_CONFIG_FILE "Config\\In2Report.ini"
#define DEFAULT_COMMAND "pdf_sign.exe"
#define DEFAULT_PASSWORD ""
#define DEFAULT_SIGN_FILE "In2ps.pfx"
#define DEFAULT_LOGO_FILE "logo.png"
#define DEFAULT_OUTPATH "D:\\In2Output"
#define DEFAULT_BBDD "Database\\In2Report.sqlite"
#define DEFAULT_WSELECT "select * from Data where OT=%1"

#define TXT_GROUP "CONFIG"
#define TXT_CERTIFICATE "Certificate"
#define TXT_PASSWORD "Password"
#define TXT_COMMAND "Command"
#define TXT_ACTIVE "Active"
#define TXT_LOGO "Logo"
#define TXT_OUTPATH "OutPath"
#define TXT_WSELECT "writeSelect"
#define TXT_AUTO "auto"

In2ReportConfigurator::In2ReportConfigurator(QObject *parent) :
    QObject(parent)
{
}

/** Lee la configuracon*/
void In2ReportConfigurator::readConfig(){
    QString strPath=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);
    QFile dummyFile(strPath);

    if (dummyFile.exists()){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(TXT_GROUP);
        m_signCommand=FicheroIni.value(TXT_COMMAND,DEFAULT_COMMAND).toString();
        m_signFile=FicheroIni.value(TXT_CERTIFICATE,DEFAULT_SIGN_FILE).toString();
        m_auto=FicheroIni.value(TXT_AUTO,true).toBool();
        m_signPassword=FicheroIni.value(TXT_PASSWORD,DEFAULT_PASSWORD).toString();
        m_signActive=FicheroIni.value(TXT_ACTIVE,true).toBool();
        m_Logo=FicheroIni.value(TXT_LOGO,DEFAULT_LOGO_FILE).toString();
        m_outputPath=FicheroIni.value(TXT_OUTPATH,DEFAULT_OUTPATH).toString();
        m_wselect=FicheroIni.value(TXT_WSELECT,DEFAULT_WSELECT).toString();
        m_BBDD=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_BBDD);

    }
    else{
        //emit SendDebugInfo(LOGGER_WARNING,"ReportGenerator:No se encontró fichero de configuración");
        m_signCommand=DEFAULT_COMMAND;
        m_signFile=DEFAULT_SIGN_FILE;
        m_signPassword=DEFAULT_PASSWORD;
        m_Logo=DEFAULT_LOGO_FILE;
        m_signActive=true;
        m_outputPath=DEFAULT_OUTPATH;
        m_BBDD=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_BBDD);
        m_wselect="";
    }
}

/** Carga el comando de firma*/
void In2ReportConfigurator::setSignCommand(QString txt){
    m_signCommand=txt;
}
/** Carga el fichero de firma*/
void In2ReportConfigurator::setSignFile(QString txt){
    m_signFile=txt;
}
/** Carga el password de firma*/
void In2ReportConfigurator::setSignPassword(QString txt){
    m_signPassword=txt;
}
/** Carga el logo*/
void In2ReportConfigurator::setLogo(QString txt){
    m_Logo=txt;
}
/** carga la activacion de firma*/
void In2ReportConfigurator::setSignActive(bool val){
    m_signActive=val;
}
/** Carga el outputPath*/
void In2ReportConfigurator::setOutputPath(QString txt){
    m_outputPath=txt;
}
/** carga la BBDD*/
void In2ReportConfigurator::setBBDD(QString txt){
    m_BBDD=txt;
}
/** carga el wselect*/
void In2ReportConfigurator::setWselect(QString txt){
    m_wselect=txt;
}
/** Devuelve el comando de firma*/
QString In2ReportConfigurator::getSignCommand(){
    return m_signCommand;
}
/** Devuelve el ficherp de firma*/
QString In2ReportConfigurator::getSignFile(){
    return m_signFile;
}
/** Devuelve el passwrod de firma*/
QString In2ReportConfigurator::getSignPassword(){
    return m_signPassword;
}
/** Devuelve el logo*/
QString In2ReportConfigurator::getLogo(){
    return m_Logo;
}
/** Devuelve si esta activa la firma*/
bool In2ReportConfigurator::getSignActive(){
    return m_signActive;
}
/** Devuelve si esta activa el automatico*/
bool In2ReportConfigurator::getAuto(){
    return m_auto;
}

/** Devuelve el path de salida*/
QString In2ReportConfigurator::getOutputPath(){
    return m_outputPath;
}
/** Devuelve la BBDD*/
QString In2ReportConfigurator::getBBDD(){
    return m_BBDD;
}
/** Devuelve el wSelect*/
QString In2ReportConfigurator::getWselect(){
    return m_wselect;
}
