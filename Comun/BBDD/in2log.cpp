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
#include "in2log.h"
#include "In2GestorDB.h"
#include <QApplication>

#define  STORE_DB "Database\\log.sqlite"


In2Log::In2Log(QObject *parent) :
    QObject(parent)
{
}

//Guarda el mensaje en la tabla Log
QStringList In2Log::execSQL(QString message)
{
    In2GestorDB db;
    QStringList rtn;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Log"),file)){
        if (db.exec(message))
            rtn=db.m_Tabla;                
    }
    return rtn;
}

//Guarda el mensaje en la tabla Log
bool In2Log::execSQLNoSave(QString message)
{
    In2GestorDB db;
    bool rtn=false;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Log"),file)){
        rtn=db.execNoSave(message);                
    }
    return rtn;
}
