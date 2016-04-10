#include "in2indukitconfigurator.h"
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QFileInfo>

#define FILE "In2Indukit.ini"
#define CONFIG  "CONFIG"
#define OFFSET  "Offset"
#define IMGW    "TotalWidth"
#define IMGCUT  "CutWidth"

#define DEFAULT_CUT 250
#define DEFAULT_OFFSET  0
#define DEFAULT_IMAGE   1000
In2IndukitConfigurator::In2IndukitConfigurator()
{
    m_cutWidth=DEFAULT_CUT;
    m_offset=DEFAULT_OFFSET;
    m_totalWidth=DEFAULT_IMAGE;
    loadConfig();
}
/** Carga la configuracion*/
void In2IndukitConfigurator::loadConfig(){
    QString file=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(FILE);
    if (QFile::exists(file)){
        QSettings FicheroIni(file,QSettings::IniFormat,0);
        FicheroIni.beginGroup(CONFIG);
        m_cutWidth=FicheroIni.value(IMGCUT,DEFAULT_CUT).toInt();
        m_offset=FicheroIni.value(OFFSET,DEFAULT_OFFSET).toInt();
        m_totalWidth=FicheroIni.value(IMGW,DEFAULT_IMAGE).toInt();
        FicheroIni.endGroup();
    }
}

/** Devuelve el ancho del corte*/
int In2IndukitConfigurator::cutWidth(){
    return m_cutWidth;
}

/** Devuelve el offset*/
int In2IndukitConfigurator::offset(){
    return m_offset;
}

/** Devielve el corte total de imagen*/
int In2IndukitConfigurator::totalWidth(){
    return m_totalWidth;
}

/** Establece el ancho del corte*/
void In2IndukitConfigurator::setCutWidth(int value){
    m_cutWidth=value;
}

/** Establece el offset*/
void In2IndukitConfigurator::setOffset(int value){
    m_offset=value;
}

/** Establece el corte total de imagen*/
void In2IndukitConfigurator::setTotalWidth(int value){
    m_totalWidth=value;
}

/** Guarda la configuracion*/
bool In2IndukitConfigurator::saveConfig(){
    bool  rtn=false;
    QString file=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(FILE);
    if (QFile::exists(file)){
        QSettings FicheroIni(file,QSettings::IniFormat,0);
        FicheroIni.beginGroup(CONFIG);
        FicheroIni.setValue(IMGCUT,m_cutWidth);
        FicheroIni.setValue(OFFSET,m_offset);
        FicheroIni.setValue(IMGW,m_totalWidth);
        FicheroIni.endGroup();
        FicheroIni.sync();
        rtn=true;
    }
    return rtn;
}


