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
#include "in2gestormantenimiento.h"
#include "MemoryMap.h"
#include "globals.h"
#include "in2log.h"
#include <QDate>

//Columnas
#define COLUMNAS_MANTENIMIENTO 4
#define POS_TIME_SESION 2


In2GestorMantenimiento::In2GestorMantenimiento(QObject *parent) :
    QObject(parent)
{
    m_segundosLampara=0;
    m_qstrFechaCambioFiltro.clear();
    m_bCambioFiltros=false;
    m_bCambioLampara=false;
}
/** Devuelve todos los registros de mantenimiento*/
QStringList In2GestorMantenimiento::getAllRegisters(){
    QString tmp=QString("select * from Mantenimiento");
    In2Log db;
    QStringList rslt=db.execSQL(tmp);
    return rslt;
}
/** Añade un registro*/
bool In2GestorMantenimiento::insertRegister(QString Equipo,QString Fecha,QString tiempo, QString user ){
    QString foo=QString("insert into Mantenimiento ('Equipo','Fecha','TiempoSesion','Usuario') Values ('%1','%2','%3','%4')").arg(Equipo).arg(Fecha).arg(tiempo).arg(user);
    In2Log db;
    bool rslt=db.execSQLNoSave(foo);
    return rslt;
}
/** Guarda los segundos de funcionamiento de la lampara*/
void In2GestorMantenimiento::updateSegundosLampara(long sg){
    QString tmp=QString("update Mantenimiento set TiempoSesion='%1' where Equipo='Lampara'").arg(sg);
    In2Log db;
    db.execSQLNoSave(tmp);

}


/** Devuelve los ssegundos de funcionamiento de la lámpara*/
long In2GestorMantenimiento::checkSegundosLampara(){
    QString tmp=QString("select * from Mantenimiento where Equipo='Lampara'");
    In2Log db;
    QStringList rslt=db.execSQL(tmp);
    if (rslt.count()){
        m_segundosLampara=rslt.at(2).toLong();
    }
    return m_segundosLampara;
}

/** Devuelve la fecha del ultimo cambio de filtros*/
QString In2GestorMantenimiento::checkCambioFiltros(){
    QString tmp=QString("select * from Mantenimiento where Equipo='Filtro'");
    In2Log db;
    QStringList rslt=db.execSQL(tmp);
    if (rslt.count()){
        m_qstrFechaCambioFiltro=rslt.at(1).left(10);
    }
    else{

    }
    MemoryMap *memoryMap=MemoryMap::request(0);
    QDate qdtFechaCambioFiltro=QDate::fromString(m_qstrFechaCambioFiltro,"yyyy-MM-dd");
    QDate qdtNextCambio=qdtFechaCambioFiltro.addDays(memoryMap->m_mapa.m_maquina.DiasMaximosUsoFiltro);

    if (QDate::currentDate()>=qdtNextCambio){
        m_bCambioFiltros=true;
    }

    return qdtNextCambio.toString("yyyy-MM-dd");
}
/** Devuele si es necesario el cambio de filtros*/
bool In2GestorMantenimiento::getCambioFiltros(){
    return m_bCambioFiltros;
}


