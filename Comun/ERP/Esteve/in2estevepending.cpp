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
#include "in2estevepending.h"
#include <QFile>
#include <QApplication>
#include <QSettings>

#define ESTEVE_PENDING "In2EstevePending.ini"
#define SEPARATOR "<@>"
In2EstevePending::In2EstevePending(QObject *parent) :
    QObject(parent)
{

}
/** dice si hay una orden pendiente*/
bool In2EstevePending::orderPending(){
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(ESTEVE_PENDING);
    return QFile::exists(txt);
}

/** Elimina una orden pendiente*/
void In2EstevePending::deletePending(){
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(ESTEVE_PENDING);
    QFile::remove(txt);
}

/** Guarda una orden pendiente*/
void In2EstevePending::saveOrder(QStringList &msg){
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(ESTEVE_PENDING);
    QFile::remove(txt);
    QFile file(txt);
    if (file.open(QIODevice::WriteOnly)){
        QString toSave=msg.join(SEPARATOR);
#if QT_VERSION>=0x050000
        file.write(toSave.toLatin1());
#else
        file.write(toSave.toAscii());
#endif

    }
    file.close();
}

/** Devuelve una orden*/
QStringList In2EstevePending::getOrder(){
    QStringList rtn;
    QString txt=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(ESTEVE_PENDING);
    QFile file(txt);
    if (file.open(QIODevice::ReadOnly)){
        QByteArray byteArray=file.readAll();
        QString txt=QString(byteArray);
        rtn=txt.split(SEPARATOR);
    }
    return rtn;
}
