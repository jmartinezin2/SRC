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
#include "in2filedialog.h"
#include <QApplication>

In2FileDialog::In2FileDialog(QWidget *parent) :
    QFileDialog(parent)
{
}
/** Guarda los settings de un FileDialog*/
void In2FileDialog::saveSettings(){
    QString path=QString("%1\\Config\\In2History.ini").arg(QApplication::applicationDirPath());
    QSettings FicheroIni(path,QSettings::IniFormat,0);
    FicheroIni.beginGroup("Settings");
    QString strList=FicheroIni.value("Settings").toString();
    QStringList paths=strList.split(";");
    QString curPath=this->directory().currentPath();
    if (!paths.contains(curPath,Qt::CaseInsensitive)){
        paths.append(curPath);
        strList=paths.join(";");
    }
    FicheroIni.setValue("Settings",strList);
    FicheroIni.endGroup();
}

/** Restaura los settings de un fileDialog*/
void In2FileDialog::restoreSettings(){
    QString path=QString("%1\\Config\\In2History.ini").arg(QApplication::applicationDirPath());
    QSettings FicheroIni(path,QSettings::IniFormat,0);
    FicheroIni.beginGroup("Settings");
    QString strList=FicheroIni.value("Settings").toString();
    QStringList paths=strList.split(";");
    this->setHistory(paths);
}
