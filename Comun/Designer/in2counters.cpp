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
#include "in2counters.h"
#include "in2defines.h"
#include <QSqlError>
#include <QApplication>

#define DEFAULT_COUNTERS_FILE "In2Counter.sqlite"
#define TABLA_IN2TRABAJOS "Trabajos"
In2Counters::In2Counters(QObject *parent) :
    QObject(parent)
{

}
//Destructor
In2Counters::~In2Counters(){
    m_db.close();
}

/** Añade un record*/
bool In2Counters::addRecord(QString  path,int time,int meters,int count,QString  txt,QString OT){
    bool rtn=false;
    struct In2counterRecord record;
    QString strPath=QString("%1\\Database\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_COUNTERS_FILE);
    if (m_db.open(QString("QSQLITE"),QString("In2Counter"),strPath)){
        /*QString tmp=QString("select * from Trabajos");
        m_db.exec(tmp.toAscii().data());*/
        QDateTime dateTime=QDateTime::currentDateTime();
        record.m_path=path;
        record.m_meters=QString("%1").arg(meters);
        record.m_count=QString("%1").arg(count);
        record.m_finishComment=txt;
        record.m_elapsedTime=QString("%1").arg(time);
        record.m_PrintDate=dateTime.toString("yyyy-MM-dd hh:mm:ss");
        record.m_InitDate=dateTime.addMSecs((-1)*time).toString("yyyy-MM-dd hh:mm:ss");
        record.m_OT=OT;
        QString tmp=QString("insert into %1 ('Path','Inicio','Fin','Metros','Impresiones','Comentario','OT') Values ('%2','%3','%4','%5','%6','%7','%8')")
                .arg(TABLA_IN2TRABAJOS)
                .arg(record.m_path)
                .arg(record.m_InitDate)
                .arg(record.m_PrintDate)
                .arg(record.m_meters)
                .arg(record.m_count)
                .arg(record.m_finishComment)
                .arg(record.m_OT);
        emit SendDebugInfo(LOGGER_WARNING,tmp);
        if (m_db.exec(tmp)) rtn=true;
        else {
            QString error=m_db.getLastError();
            emit SendDebugInfo(LOGGER_ERROR,error);

        }
        m_db.close();
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,"No se pudo abrir In2Counter.sqlite");
    }
    return rtn;
}
/** Devuelve datos*/
QStringList In2Counters::getRecord(QString query){
    QStringList data;
    QString strPath=QString("%1\\Database\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_COUNTERS_FILE);
    if (m_db.open(QString("QSQLITE"),QString("In2Counter"),strPath)){
        QString tmp=QString("%1").arg(query);
        emit SendDebugInfo(LOGGER_WARNING,tmp);
        if (m_db.exec(tmp)){
            data=m_db.m_Tabla;
        }
        else {
            QString error=m_db.getLastError();
            emit SendDebugInfo(LOGGER_ERROR,error);

        }
        m_db.close();
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,"No se pudo abrir In2Counter.sqlite");
    }
    return data;
}



/** Devuelve todos los metros impresos*/
int In2Counters::getTotalMeters(){
    QString req=QString("select Metros from Trabajos");
    QStringList rslt=getRecord(req);
    int total=0;
    for (int x=0;x<rslt.count();x++){
        total+=rslt.at(x).toInt();
    }
    return total;
}

/** Devuelve numero de trabajos impresos*/
int In2Counters::getTotalPrinted(){
    QString req=QString("select Metros from Trabajos");
    QStringList rslt=getRecord(req);
    return rslt.count();
}

