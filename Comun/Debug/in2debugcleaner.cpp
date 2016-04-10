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
#include "in2debugcleaner.h"
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QApplication>
#include <QDate>

#define DEFAULT_FILE "Config\\In2Launchers.ini"
#define DEFAULT_EXECUTABLE ""

In2DebugCleaner::In2DebugCleaner(QObject *parent) :
    QObject(parent)
{
    QSettings FicheroIni(QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_FILE),QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("Launchers"));
    m_strExecutable=FicheroIni.value(QString("In2DebugCleaner"),DEFAULT_EXECUTABLE).toString();
    FicheroIni.endGroup();
}

In2DebugCleaner::~In2DebugCleaner()
{

}
/** Devuelve si existe*/
bool In2DebugCleaner::exists()
{
   bool rtn=false;
   if (!m_strExecutable.isEmpty()){
    rtn=QFile::exists(m_strExecutable);
   }
   return rtn;
}
/** Lanza el proceso con el fichero que se le diga*/
void In2DebugCleaner::start(){
    if (!testCurrentMonth()){
        QProcess foo;
        int month=QDate::currentDate().month();
        int year=QDate::currentDate().year();
        QString folder=QString("%1_%2").arg(year).arg(month);
        QString debugFolder=QString("%1/Output").arg(QApplication::applicationDirPath());
        debugFolder.replace("/","\\");
        //QString debugFolder=QString("D:/Output");
        QString command=QString("%1").arg(m_strExecutable);
        foo.start("cmd.exe",QStringList()<<"/c"<<command<<debugFolder<<folder);
        if (foo.waitForStarted()){
            if (foo.waitForFinished()){
                emit SendDebugInfo(LOGGER_STORE,QString("In2DebugCleaner->%1").arg(folder));
            }
        }
    }
}

/** Testeamos si existe la carpeta de compresion*/
bool In2DebugCleaner::testCurrentMonth(){
    bool rtn=false;
    int month=QDate::currentDate().month();
    int year=QDate::currentDate().year();
    QString file=QString("%1\\Output\\%2_%3\\%4_%5.rar").arg(QApplication::applicationDirPath()).arg(year).arg(month).arg(year).arg(month);
    if (QFile::exists(file)){
        rtn=true;
    }
    return rtn;
}
