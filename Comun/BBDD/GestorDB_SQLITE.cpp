﻿/** @addtogroup GestorDB_SQLITE
 *  @{*/
/** @file GestorDB_SQLITE.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Gestor de la base de datos
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: GestorDB_SQLITE.cpp
 *    @li Versión: x.0
 *    @li Fecha: 30/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "GestorDB_SQLITE.h"
#include <QFileDialog>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

//#include "MemoryMap.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <qsqlresult.h>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlError>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>


int GuardaResultado(void* paque,int n, char** re,char** nomcol);

GestorDBSQLITE::GestorDBSQLITE(void)
{
}

GestorDBSQLITE::~GestorDBSQLITE(void)
{
}

//Cierra la base de datos
void GestorDBSQLITE::close(){
    /*m_db.close();
    QSqlDatabase::removeDatabase(m_name);*/
}

//Abre la base de datos
bool GestorDBSQLITE::open(QString cDataBase,QString conexion){
    QSqlDatabase db;
    QString conexion_sql=QString(conexion);
    if (QSqlDatabase::contains(conexion_sql)){
        db=QSqlDatabase::database(conexion_sql);
    }
    else{
        db = QSqlDatabase::addDatabase("QSQLITE",conexion_sql);
        db.setDatabaseName(m_name);
    }
    m_name=conexion;

    bool ok =true;
    if (!db.isOpen())
        ok=db.open();
    return ok;
}

//Cierra la base de datos
void GestorDBSQLITE::close(QString conexion){
    /*m_db.close();
    QSqlDatabase::removeDatabase(conexion);*/
}


//Ejecuta comandos SQL
bool GestorDBSQLITE::exec(QString sql){
    bool okp=false;
    //QSqlQuery *q=new QSqlQuery(m_db);
    QSqlDatabase db=QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(db);
    QString dummy;
    unsigned char i;
    int counter=0;
    dummy=QString(sql);
    if (q.prepare(dummy)){
        okp=q.exec();
        if (okp==true){
            m_Tabla.clear();
            QSqlRecord rec = q.record();
            CamposDatabase=rec.count();
            while (q.next()){
                for(i=0;i<CamposDatabase;i++){
                    dummy=q.value(i).toString();
                    m_Tabla.append(dummy);
                    counter++;
                }
            }
        }
        else{
            qDebug () << q.lastError();
            m_error=q.lastError();
        }
    }
    else{
        qDebug () << q.lastError();
        m_error=q.lastError();
    }
    q.clear();
    //delete (q);
    return okp;
}

/** Devuelve el ultimo error*/
QSqlError GestorDBSQLITE::getLastError(){
    return m_error;
}

//Ejecuta comandos SQL sin alterar m_Tabla
bool GestorDBSQLITE::execNoSave(char *sql){
    return execNoSave(QString(sql));
}

//Ejecuta comandos SQL sin alterar m_Tabla
bool GestorDBSQLITE::execNoSave(const QString & sql){
    QSqlDatabase db=QSqlDatabase::database(m_name,false);
    QSqlQuery *q=new QSqlQuery(db);
    QString dummy=QString("%1").arg(sql);
    bool rtn=q->prepare(dummy);
    if (rtn){
        rtn=q->exec();
    }
    q->finish();
    delete (q);
    return rtn;
}


//Callback del select
int GuardaResultado(void *paque, int n, char **re, char **nomcol){
    GestorDBSQLITE *tmp;
    tmp=(GestorDBSQLITE*)paque;
	int index=0;
	for (index=0;index<n;index++) tmp->m_Tabla.append(re[index]);
	return 0;
}
