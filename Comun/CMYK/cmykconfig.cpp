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
#include "cmykconfig.h"
#include "in2defines.h"

//#include "globals.h"


#include <QFile>
#include <QSettings>
#include <QProcess>
#include <QImage>
#include <QApplication>
#include <QMessageBox>

#define DEFAULT_CONFIG_FILE "In2CMYK.ini"
#define TXT_CONFIG "CONFIG"
#define TXT_GSPATH "GsPath"
#define TXT_CYANINDEX "CyanIndex"
#define TXT_MAGENTAINDEX "MagentaIndex"
#define TXT_YELLOWINDEX "YellowIndex"
#define TXT_BLACKINDEX "BlackIndex"
#define TXT_BLACK2INDEX "Black2Index"
#define TXT_WHITEINDEX "WhiteIndex"
#define TXT_QTCONVERSIONFLAGS "QtConversionFlag"
#define TXT_CALCULATION_FORMAT "CalcFormat"
#define TXT_ALG_MEJORA_BLANCOS "AlgMejoraBlancos"
#define TXT_ALG_SABRE_ANGLE "AlgSabreAngle"

#define DEFAULT_CYAN_INDEX 1
#define DEFAULT_MAGENTA_INDEX 2
#define DEFAULT_YELLOW_INDEX 3
#define DEFAULT_BLACK_INDEX 4
#define DEFAULT_BLACK2_INDEX 5
#define DEFAULT_WHITE_INDEX 6
#define DEFAULT_CONVERSION_FLAG 0
#define DEFAULT_CALCULATION_FORMAT QImage::Format_RGB32
#define DEFAULT_ALG_MEJORA_BLANCOS 0

CMYKConfig::CMYKConfig(QObject *parent) :
    QObject(parent)
{
    m_qstrEfectiveCall.clear();
    m_qstrIccFile.clear();
    m_qstrInputFile.clear();
    m_qstrgspath.clear();
    m_qstrOutFile.clear();
    //Por defecto damos este orden
    m_CyanIndex=DEFAULT_CYAN_INDEX;
    m_MagentaIndex=DEFAULT_MAGENTA_INDEX;
    m_YellowIndex=DEFAULT_YELLOW_INDEX;
    m_BlackIndex=DEFAULT_BLACK_INDEX;
    m_WhiteIndex=DEFAULT_WHITE_INDEX;
    m_QtConversionFlag=DEFAULT_CONVERSION_FLAG;
    m_CalculationFormat=DEFAULT_CALCULATION_FORMAT;
    m_bAlgMejoraBlancos=false;
    m_sabreAngle=0;

    QString strPath=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);
    QFile dummyFile(strPath);
    if (dummyFile.exists()){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(TXT_CONFIG);
        m_qstrgspath=FicheroIni.value(TXT_GSPATH,"").toString();
        m_CyanIndex=FicheroIni.value(TXT_CYANINDEX,DEFAULT_CYAN_INDEX).toInt();
        m_MagentaIndex=FicheroIni.value(TXT_MAGENTAINDEX,DEFAULT_MAGENTA_INDEX).toInt();
        m_YellowIndex=FicheroIni.value(TXT_YELLOWINDEX,DEFAULT_YELLOW_INDEX).toInt();
        m_BlackIndex=FicheroIni.value(TXT_BLACKINDEX,DEFAULT_BLACK_INDEX).toInt();
        m_Black2Index=FicheroIni.value(TXT_BLACK2INDEX,DEFAULT_BLACK2_INDEX).toInt();
        m_WhiteIndex=FicheroIni.value(TXT_WHITEINDEX,DEFAULT_WHITE_INDEX).toInt();
        m_QtConversionFlag=FicheroIni.value(TXT_QTCONVERSIONFLAGS,DEFAULT_CONVERSION_FLAG).toInt();
        m_CalculationFormat=FicheroIni.value(TXT_CALCULATION_FORMAT,DEFAULT_CALCULATION_FORMAT).toInt();
        m_bAlgMejoraBlancos=FicheroIni.value(TXT_ALG_MEJORA_BLANCOS,DEFAULT_ALG_MEJORA_BLANCOS).toBool();
        m_sabreAngle=FicheroIni.value(TXT_ALG_SABRE_ANGLE,0).toInt();
    }
}
/** Guarda el fichero*/

void CMYKConfig::saveConfigFile(){
    QString strPath=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_CONFIG_FILE);
    if (QFile::exists(strPath)){
        QSettings FicheroIni(strPath,QSettings::IniFormat,0);
        FicheroIni.beginGroup(TXT_CONFIG);
        FicheroIni.setValue(TXT_GSPATH,QString("%1").arg(m_qstrgspath));
        FicheroIni.setValue(TXT_CYANINDEX,QString("%1").arg(m_CyanIndex));
        FicheroIni.setValue(TXT_MAGENTAINDEX,QString("%1").arg(m_MagentaIndex));
        FicheroIni.setValue(TXT_YELLOWINDEX,QString("%1").arg(m_YellowIndex));
        FicheroIni.setValue(TXT_BLACKINDEX,QString("%1").arg(m_BlackIndex));
        FicheroIni.setValue(TXT_BLACK2INDEX,QString("%1").arg(m_Black2Index));
        FicheroIni.setValue(TXT_WHITEINDEX,QString("%1").arg(m_WhiteIndex));
        FicheroIni.setValue(TXT_QTCONVERSIONFLAGS,QString("%1").arg(m_QtConversionFlag));
        FicheroIni.setValue(TXT_CALCULATION_FORMAT,QString("%1").arg(m_CalculationFormat));
        FicheroIni.setValue(TXT_ALG_MEJORA_BLANCOS,QString("%1").arg(m_bAlgMejoraBlancos));
        FicheroIni.setValue(TXT_ALG_SABRE_ANGLE,QString("%1").arg(m_sabreAngle));
        FicheroIni.endGroup();
        FicheroIni.sync();
    }
    else QMessageBox::warning(0,"ERROR",tr("No existe %1").arg(strPath));
}
/** Guarda el sabre angle*/
void CMYKConfig::setSabreAngle(int angle){
    m_sabreAngle=angle;
}
/** Devuelve el sabre angle*/
int CMYKConfig::getSabreAngle(){
    return m_sabreAngle;
}

/** Carga el fichero ICC*/
void CMYKConfig::setIccFile(const QString & qstr){
    this->m_qstrIccFile=qstr;
}
/** Devuelve el fichero ICC actual*/
QString CMYKConfig::getIccFile(){
    return this->m_qstrIccFile;
}
/** Establece el fichero de  entrada*/
void CMYKConfig::setInputFile(const QString & qstr){
    this->m_qstrInputFile=qstr;
}
/** Devuelve el fichero actual*/
QString CMYKConfig::getInputFile(){
    return this->m_qstrInputFile;
}
/** Establece la llamada efectiva con los parámetros actuales*/
void CMYKConfig::setEfectiveCall(){
    setOutPutFile();
    m_qstrEfectiveCall=QString("\"%1\" -sOutputFile=\"%2\" -r%3x%4 -sDEVICE=tiffsep -dUseCIEColor -sOutputICCProfile=\"%5\" -dSAFER -dNOPAUSE -dBATCH \"%6\"") \
            .arg(m_qstrgspath) \
            .arg(m_qstrOutFile) \
            .arg(this->m_width) \
            .arg(this->m_height) \
            .arg(m_qstrIccFile) \
            .arg(m_qstrInputFile);


}
/** Ejecuta la llamada*/
int CMYKConfig::execute(){
    QProcess convertgs;
    int error=convertgs.execute(m_qstrEfectiveCall);
    emit SendDebugInfo(LOGGER_WARNING,QString("CMYKConfig:%1").arg(m_qstrEfectiveCall));
    return error;
}

/** Carga una llamada efectiva */
void CMYKConfig::setEfectiveCall(const QString & qstr){
    this->m_qstrEfectiveCall=qstr;
}
/** Devuelve la llamada efectiva actual*/
QString CMYKConfig::getEfectiveCall(){
    return this->m_qstrEfectiveCall;
}
/** Establece la anchura en dpi*/
void CMYKConfig::setDpiWidth(int i){
    this->m_width=i;
}
/** Devuelve la anchura en dpi*/
int CMYKConfig::getDpiWidth(){
    return this->m_width;
}
/** Establece la altura en dpi*/
void CMYKConfig::setDpiHeight(int i){
    this->m_height=i;
}
/** Devuelve la altura en dpi*/
int CMYKConfig::getDpiHeight(){
    return this->m_height;
}
/** Establece fichero de salida*/
void CMYKConfig::setOutPutFile(){
    this->m_qstrOutFile=this->m_qstrInputFile.left(this->m_qstrInputFile.lastIndexOf('.'));
    this->m_qstrOutFile.append(".tif");
}
/** Devuelve fichero de salida*/
QString CMYKConfig::getOutPutFile(){
    return this->m_qstrOutFile;
}
/** Devuelve el índice para el magenta*/
int CMYKConfig::getMagentaIndex(){
    return m_MagentaIndex;
}
/** Devuelve el índice para el magenta*/
int CMYKConfig::getCyanIndex(){
    return m_CyanIndex;
}
/** Devuelve el índice para el magenta*/
int CMYKConfig::getYellowIndex(){
    return m_YellowIndex;
}
/** Devuelve el índice para el magenta*/
int CMYKConfig::getBlackIndex(){
    return m_BlackIndex;
}
/** Devuelve el índice para el magenta*/
int CMYKConfig::getBlack2Index(){
    return m_Black2Index;
}
/** Devuelve el índice para el magenta*/
int CMYKConfig::getWhiteIndex(){
    return m_WhiteIndex;
}
/** Cambia el índice para el Blanco*/
void CMYKConfig::setWhiteIndex(int index){
    m_WhiteIndex=index;
}
/** Cambia el índice para el magenta*/
void CMYKConfig::setMagentaIndex(int index){
    m_MagentaIndex=index;
}
/** Cambia el índice para el Cyan*/
void CMYKConfig::setCyanIndex(int index){
    m_CyanIndex=index;
}
/** Cambia el índice para el Yellow*/
void CMYKConfig::setYellowIndex(int index){
    m_YellowIndex=index;
}
/** Cambia el índice para el Black*/
void CMYKConfig::setBlackIndex(int index){
    m_BlackIndex=index;
}
/** Cambia el índice para el Black*/
void CMYKConfig::setBlack2Index(int index){
    m_Black2Index=index;
}
/** Devuelve el flag de conversion por defecto*/
int CMYKConfig::getQtConversionFlag(){
    return m_QtConversionFlag;
}
/** Devuelve el flag de conversion por defecto*/
void CMYKConfig::setQtConversionFlag(int flags){
    m_QtConversionFlag=flags;
}
/** Devuelve el flag de conversion por defecto*/
int CMYKConfig::getCalcFormat(){
    return m_CalculationFormat;
}
/** Devuelve el flag de conversion por defecto*/
void CMYKConfig::setCalcFormat(int flags){
    m_CalculationFormat=flags;
}
/** Devuelve el flag de algoritmo de mejora de blancos*/
bool CMYKConfig::getAlgMejoraBlancos(){
    return m_bAlgMejoraBlancos;
}
/** Establece el algoritmo de mejora de blancos*/
void CMYKConfig::setAlgMejoraBlancos(bool val){
    m_bAlgMejoraBlancos=val;
}


