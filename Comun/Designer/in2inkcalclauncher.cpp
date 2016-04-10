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

#include "in2inkcalclauncher.h"
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QApplication>


#define DEFAULT_FILE "Config\\In2Launchers.ini"
#define DEFAULT_EXECUTABLE ""

In2InkCalcLauncher::In2InkCalcLauncher(QObject *parent) : QObject(parent)
{

    QSettings FicheroIni(QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_FILE),QSettings::IniFormat,0);
    FicheroIni.beginGroup(QString("Launchers"));
    m_strExecutable=FicheroIni.value(QString("In2InkCalculator"),DEFAULT_EXECUTABLE).toString();
    FicheroIni.endGroup();
}

In2InkCalcLauncher::~In2InkCalcLauncher()
{

}
/** Devuelve si existe*/
bool In2InkCalcLauncher::exists()
{
   bool rtn=false;
   if (!m_strExecutable.isEmpty()){
    rtn=QFile::exists(m_strExecutable);
   }
   return rtn;
}
/** Lanza el proceso con el fichero que se le diga*/
void In2InkCalcLauncher::start(QString file){
    QProcess inkCal;
//    QString strPath=QString("%1\\%2").arg(m_strExecutable).arg(file);
    QString command=QString("%1 %2").arg(m_strExecutable).arg(file);
    inkCal.startDetached(command);
}
