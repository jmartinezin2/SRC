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
#include "in2store.h"
#include <QApplication>
#include "In2GestorDB.h"
#include "GestorDB_SQLITE.h"
#include <QMessageBox>

#define  STORE_DB "Database\\In2Store.sqlite"
#define  STORE_MDB "Database\\In2Store.accdb"

#define  STORE_TABLA "In2Store"

In2Store::In2Store(QObject *parent) :
    QObject(parent)
{
    m_nFieldsLastQuery=0;
}
/** Guarda el mensaje en la tabla In2Store*/
bool In2Store::saveMessage(const strctIn2Store & data, QString tabla){
    bool rtn=false;
    struct strctIn2Store mData=data;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        QString foo=QString("insert into '%1' ('Fecha','Hora','Usuario','Evento','Codigo','Tarea') values ('%2','%3','%4','%5','%6','%7')").arg(tabla).arg(mData.fecha).arg(mData.hora).arg(mData.user).arg(mData.evento).arg(mData.codigo).arg(mData.tarea);
        rtn=db.execNoSave(foo);

    }
    return rtn;
}

/** Ejecuta una orden*/
QStringList In2Store::execSql(QString sql){
    bool rtn=false;
    QStringList lst;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        rtn=db.exec(sql);
        if (rtn)
            lst=db.m_Tabla;
    }
    return lst;
}

/** añade una tabla si no existe*/
bool In2Store::createTable(QString tabla){
    bool rtn=false;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        QString foo=QString("create table if not exists '%1' ('Fecha','Hora','Usuario','Evento','Codigo','Tarea')").arg(tabla);
        //QMessageBox::warning(0,"ERROR",foo,QMessageBox::Ok);
        rtn=db.execNoSave(foo);
    }

    return rtn;
}

/** Devuelve todos los datos*/
QStringList In2Store::getAllData(QString tabla){
    QStringList lst;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        QString foo=QString("select * from '%1'").arg(tabla);
        bool rtn=db.exec(foo);
        if (rtn) lst=db.m_Tabla;
        m_nFieldsLastQuery=db.SizeLastQuery();
    }
    return lst;
}

/** Devuelve todos los datos*/
QStringList In2Store::getAllDataFromCode(QString tabla,QString code){
    QStringList lst;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        QString foo=QString("select Evento from '%1' where Codigo='%2'").arg(tabla).arg(code);
        bool rtn=db.exec(foo);
        if (rtn) lst=db.m_Tabla;
        m_nFieldsLastQuery=db.SizeLastQuery();
    }
    return lst;
}



/** Devuelve los metros maquina*/
float In2Store::getMetrosMaquina(QString tabla){
    QStringList lst;
    float rtn=0;
    lst=getDataFiltered(tabla,"evento","%Metros maquina%");
    for (int x=0;x<lst.count();x++){
        QString foo=lst.at(x);
        QStringList lstFoo=foo.split(" ");
        if (lstFoo.count()==3) rtn+=lstFoo.at(2).toInt();
    }
    return rtn;

}

/** Devuelve los metros maquina*/
float In2Store::getMetrosImpresos(QString tabla){
    QStringList lst;
    float rtn=0;
    lst=getDataFiltered(tabla,"evento","%Metros impresos%");
    for (int x=0;x<lst.count();x++){
        QString foo=lst.at(x);
        QStringList lstFoo=foo.split(" ");
        if (lstFoo.count()==3) rtn+=lstFoo.at(2).toFloat();
    }
    return rtn;

}

/** Devuelve segun un flitro*/
QStringList In2Store::getDataFiltered(QString tabla,QString column,QString filter){
    QStringList lst;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        QString foo=QString("select %1 from '%2' where %3 like '%4'").arg(column).arg(tabla).arg(column).arg(filter);
        bool rtn=db.exec(foo);
        if (rtn) lst=db.m_Tabla;
        m_nFieldsLastQuery=db.SizeLastQuery();
    }
    return lst;
}


/** Devuelve el numero de campos de la ultima consulta*/
int In2Store::fieldsLastQuery(){
    return m_nFieldsLastQuery;
}

/** Devuelve la lista de tablas*/
QStringList In2Store::getTables(){
    QString foo=QString("SELECT name FROM sqlite_master WHERE type = \"table\"");
    QStringList lst;
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_DB);
    if (db.open(QString("QSQLITE"),QString("In2Store"),file)){
        bool rtn=db.exec(foo);
        if (rtn) lst=db.m_Tabla;
    }
    return lst;
}
