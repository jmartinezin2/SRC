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
#include "gsrenderengine.h"
#include "in2defines.h"
#include <QProcess>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QApplication>
#include <QPixmap>
#include "MemoryMap.h"
#include <QMessageBox>
#include <qDebug>

#define CONF_FILE "In2Render.ini"
#define DEFAULT_DEVICE "bmp16m"
#define DEFAULT_RESOLUTION "360x360"
#define DEFAULT_EXECUTABLE "C:\\Program Files\\gs\\gs9.06\\bin\\gswin64c.exe"
#define DEFAULT_COMMAND "\"%1\" -sOutputFile=\"%2\" -r%3 -sDEVICE=%4 -dNOPAUSE -dBATCH \"%5\""
#define DEFAULT_OPTIONS ""
#define DEFAULT_INSTALLER ".\\Installers\\gs918w64.exe"
#define DEFAULT_ICC ""
#define TEMP_GS_FILE "tmpGsFile.bmp"

GsRenderEngine::GsRenderEngine(QObject *parent) :
    QObject(parent)
{
    QString path=getConfigFile();
    QSettings FicheroIni(path,QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("RENDER"));
    m_strExecutable=FicheroIni.value(QString("Application"),DEFAULT_EXECUTABLE).toString();
    if (m_strExecutable.length()){
        if (m_strExecutable.at(0)=='.'){ //Suponemos directorio local
            QString foo=m_strExecutable;
            foo.remove(0,1);
            m_strExecutable=QString("%1%2").arg(QApplication::applicationDirPath()).arg(foo);
        }
    }
    m_strDevice=FicheroIni.value(QString("Device"),DEFAULT_DEVICE).toString();
    m_strResolution=FicheroIni.value(QString("Resolution"),DEFAULT_RESOLUTION).toString();
    m_strExtraOptions=FicheroIni.value(QString("Options"),DEFAULT_OPTIONS).toString();
    m_strICCFile=FicheroIni.value(QString("ICC"),DEFAULT_ICC).toString();
    QString foo=FicheroIni.value(QString("Installer"),DEFAULT_INSTALLER).toString();
    if (!foo.isEmpty()) {
        if (foo.at(0)=='.') m_strInstaller=QString("%1%2").arg(QApplication::applicationDirPath()).arg(foo.mid(1,foo.count()-1));
        else m_strInstaller=QString("%1\\Installers\\%2").arg(QApplication::applicationDirPath()).arg(foo);
    }
    m_command="";
    m_doubleRes=false;
    FicheroIni.endGroup();
}

/** Guarda fichero de configuracion*/
void GsRenderEngine::saveConfigFile(){
    QString path=getConfigFile();
    if (QFile::exists(path)){
        QSettings FicheroIni(path,QSettings::IniFormat,0);
        FicheroIni.beginGroup(QString("RENDER"));
        FicheroIni.setValue(QString("Application"),QString("%1").arg(m_strExecutable));
        FicheroIni.setValue(QString("Device"),QString("%1").arg(m_strDevice));
        FicheroIni.setValue(QString("Resolution"),QString("%1").arg(m_strResolution));
        FicheroIni.setValue(QString("Options"),QString("%1").arg(m_strExtraOptions));
        FicheroIni.setValue(QString("ICC"),QString("%1").arg(m_strICCFile));
        FicheroIni.endGroup();
        FicheroIni.sync();
    }
}
/** Cambia los bits de dithering*/
void GsRenderEngine::setDitheringBits(int value){
    QRegExp regDithering("-dGraphicsAlphaBits=(\\d)");
    QString newregDithering=QString("-dGraphicsAlphaBits=%1").arg(value);
    if (m_strExtraOptions.isEmpty()) m_strExtraOptions=newregDithering;
    else m_strExtraOptions.replace(regDithering,newregDithering);
}

/** Fichero de configuracion*/
QString GsRenderEngine::getConfigFile(){
    QString path=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(CONF_FILE);
    return path;
}

/** Devuelve el ultimo commando*/
QString GsRenderEngine::getLastCommand(){
    return m_command;
}

/** Testea que existe el ghostscript*/
bool GsRenderEngine::testGsInstallation(){
    QFile gsFile(m_strExecutable);
    return gsFile.exists();
}
/** Ejecuta la linea que le pasemos*/
int GsRenderEngine::executeCommand(const QString &d){
    int rtn=-1;
    QProcess convertgs;
    if (testGsInstallation()){
        m_command=QString("%1 %2").arg(m_strExecutable).arg(d);
        rtn=convertgs.execute(m_command);
    }
    return rtn;
}
/** Device*/
void GsRenderEngine::setDevice(const QString &d){
    m_strDevice=d;
}
/** Output file*/
void GsRenderEngine::setResolution(const QString &d){
    m_strResolution=d;
}
/** Output resolution*/
QString GsRenderEngine::getResolution(){
    return m_strResolution;
}
/** Output file ICC*/
void GsRenderEngine::setICCFile(const QString &d){
    m_strICCFile=d;
}
/** Output file ICC*/
QString GsRenderEngine::getICCFile(){
    return m_strICCFile;
}
/** Executable*/
void GsRenderEngine::setExecutable(const QString &d){
    m_strExecutable=d;
}
/** Executable*/
QString GsRenderEngine::getExecutable(){
    return m_strExecutable;
}
/** Device*/
QString GsRenderEngine::getDevice(){
    return m_strDevice;
}
/** ExtraOptions*/
void GsRenderEngine::setExtraOptions(const QString &d){
    m_strExtraOptions=d;
}
/** ExtraOptions*/
QString GsRenderEngine::getExtraOptions(){
    return m_strExtraOptions;
}
/** Input file*/
void GsRenderEngine::setInputFile(const QString &d){
    m_strInputFile=d;
}
/** Mete doble resolucion transversal*/
void GsRenderEngine::setDoubleResolution(){
    QStringList lst=m_strResolution.split("x",QString::SkipEmptyParts,Qt::CaseInsensitive);
    if (lst.count()==2){
        int newRes=lst.at(0).toInt()*2;
        lst.replace(0,QString("%1").arg(newRes));
        /*newRes=lst.at(1).toInt()*2;
        lst.replace(1,QString("%1").arg(newRes));*/
        m_strResolution=lst.join("x");
        m_doubleRes=true;
    }
}

/** Output file*/
void GsRenderEngine::setOutputFile(const QString &d){
    m_strOutputFile=d;
}
/** Genear y ejecuta el comando*/
int GsRenderEngine::render(){
    int rtn=-1;
    QFileInfo info(m_strInputFile);
    QString suffix=info.completeSuffix();
    //Si es un pdf procedo como siempre a renderizar
    if (!suffix.compare("pdf",Qt::CaseInsensitive)){
        if (testGsInstallation()){
            QString tmp=m_strOutputFile;
            if (m_doubleRes){ //Si es con doble resolucion, lo hago en dos pasos
                QFile::remove(TEMP_GS_FILE);
                m_strOutputFile=TEMP_GS_FILE;
            }
            m_command=QString("\"%1\" -sOutputFile=\"%2\" -r%3 -sDEVICE=%4  -sOutputICCProfile=\"%5\" -dNOPAUSE -dBATCH  %6 \"%7\"").arg(m_strExecutable).arg(m_strOutputFile).arg(m_strResolution).arg(m_strDevice).arg(m_strICCFile).arg(m_strExtraOptions).arg(m_strInputFile);
            //m_command=QString("\"%1\" -sOutputFile=\"%2\" -r%3 -sDEVICE=%4  -sOutputICCProfile=\"%5\" -dNOPAUSE -dBATCH  %6 \"%7\"").arg(m_strExecutable).arg(outputFoo).arg(m_strResolution).arg(m_strDevice).arg(m_strICCFile).arg(m_strExtraOptions).arg(localFile);
            //emit SendDebugInfo(LOGGER_ERROR,tr("%1 - %2").arg(localFile).arg(outputFoo));

            QProcess convertgs;
            rtn=convertgs.execute(m_command);
            qDebug()<<m_command;
            emit SendDebugInfo(LOGGER_WARNING,m_command);
            if (m_doubleRes){
                QImage *tmpImage=new QImage(m_strOutputFile);
                m_strOutputFile=tmp;
                int width=tmpImage->width();
                int height=tmpImage->height();
                QImage *dest=new QImage(width/2,height,tmpImage->format());
                dest->fill(Qt::white);
                for (int x=0;x<width;x=x+2){
                    for (int y=0;y<height;y++){
                        dest->setPixel(x/2,y,tmpImage->pixel(x,y));
                    }
                }
                dest->save(m_strOutputFile);
                delete (dest);
                delete (tmpImage);
            }

        }
    }
    //Detecto que es una imagen
    else if ((!suffix.compare("png",Qt::CaseInsensitive))|| (!suffix.compare("jpeg",Qt::CaseInsensitive))||(!suffix.compare("jpg",Qt::CaseInsensitive))||(!suffix.compare("bmp",Qt::CaseInsensitive))||(!suffix.compare("tif",Qt::CaseInsensitive))){ //Si es un archivo de imagen
        QPixmap dummy=QPixmap(m_strInputFile);
        if (!dummy.isNull()){
            dummy.save(QString("%1").arg(m_strOutputFile),"bmp");
            rtn=1;
        }
    }
    return rtn;
}
/** Trato de instalarla automaticamente el ghostscript*/
void GsRenderEngine::install(){
    QProcess proc;
    QString txt=tr("No se enconcuentra %1. Proceda instalar manualmente").arg(m_strInstaller);

    if (!QFile::exists(m_strInstaller)){
        QMessageBox::information(0,"ERROR",txt,QMessageBox::Ok);
        return;
    }
    proc.start(m_strInstaller);
    if (proc.waitForStarted()){
        if (proc.waitForFinished()){
            txt=tr("Instalado %1").arg(m_strInstaller);
            QMessageBox::information(0,"INFO",txt,QMessageBox::Ok);
            emit SendDebugInfo(LOGGER_DEBUG,txt);
        }
        else{
            txt=tr("No pudo finalizar %1").arg(m_strInstaller);
            QMessageBox::information(0,"ERROR",txt,QMessageBox::Ok);
            emit SendDebugInfo(LOGGER_DEBUG,txt);
        }
    }
    else{
        txt=tr("No pudo iniciarse %1").arg(m_strInstaller);
        QMessageBox::information(0,"ERROR",txt,QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_DEBUG,txt);
    }
}
