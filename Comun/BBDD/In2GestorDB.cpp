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
#include "In2GestorDB.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <qsqlresult.h> 
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlError>
#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

/** Constructor*/
In2GestorDB::In2GestorDB(void)
{
}

In2GestorDB::~In2GestorDB(void)
{
    //QSqlDatabase::removeDatabase(m_name);
}

/** Abre la base de datos*/
bool In2GestorDB::open(QString driver,QString conexion,QString name){
    bool rtn=true;
    //m_sqlDataBase = QSqlDatabase::addDatabase(driver,conexion);
    QSqlDatabase sqlDataBase;
    if (QSqlDatabase::contains(conexion)){
        //sqlDataBase=QSqlDatabase::database(conexion);
        sqlDataBase = QSqlDatabase::database(conexion,true);
    }
    else
        sqlDataBase = QSqlDatabase::addDatabase(driver,conexion);

    sqlDataBase.setDatabaseName(name);
    m_name=conexion;
    if (!sqlDataBase.isOpen()){

        rtn=sqlDataBase.open();
    }
    if (!rtn){
        m_lastError=QString("DBError:%1 - DriverError:%2").arg(sqlDataBase.lastError().databaseText()).arg(sqlDataBase.lastError().driverText());
        qDebug () << m_lastError;

    }
    return rtn;
}

/** Abre la base de datos.
 *@conexion: Damos el origen de datos ODBC configurado en el PC
 **/
bool In2GestorDB::open(QString driver,QString conexion,QString name,QString user, QString password){
    bool rtn=true;
    QSqlDatabase sqlDataBase;
    if (QSqlDatabase::contains(conexion)){
        sqlDataBase=QSqlDatabase::database(conexion,false);
    }
    else{
        sqlDataBase = QSqlDatabase::addDatabase(driver,conexion);
        sqlDataBase.setDatabaseName(name);
        sqlDataBase.setUserName(user);
        sqlDataBase.setPassword(password);
    }
        if (!sqlDataBase.isOpen())
            rtn=sqlDataBase.open();
        if (!rtn){
            m_lastError=QString("DBError:%1 - DriverError:%2").arg(sqlDataBase.lastError().databaseText()).arg(sqlDataBase.lastError().driverText());
            qDebug () << m_lastError;
        }

    m_name=conexion;
    return rtn;

}

//Abre la base de datos
void In2GestorDB::ValoresDB(QString sql){

    unsigned char i;
    QSqlDatabase db = QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(db);

    bool okp=q.prepare(sql);
    if (okp){
        okp=q.exec();
        if (okp){
            QSqlRecord rec = q.record();
            int CDatabase=rec.count();
            for(i=0;i<CDatabase;i++) NombreCampo[i]=rec.fieldName(i);
        }
        else{
            m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
            qDebug () << m_lastError;
        }
    }
    else{
        m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
        qDebug () << m_lastError;

    }
    q.finish();

}

//Cierra la base de datos
void In2GestorDB::close(){
    /*QSqlDatabase db = QSqlDatabase::database(m_name,false);
    if (db.isOpen())
        db.close();*/
}

//Ejecuta comandos SQL
bool In2GestorDB::exec(char *sql){
    return exec(QString(sql));
}

//Ejecuta comandos SQL
bool In2GestorDB::exec(const QString & sql){
    QSqlDatabase db = QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(db);
    QString dummy=sql;
    unsigned char i;    
    bool okp=q.prepare(dummy);
    if (okp){
        okp=q.exec();
        if (okp){
            m_Tabla.clear();
            QSqlRecord rec = q.record();
            CamposDatabase=rec.count();
            while (q.next()){
                for(i=0;i<CamposDatabase;i++){
                    dummy=q.value(i).toString();
                    m_Tabla.append(dummy);
                }
            }
        }
        else{
            m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
            qDebug () << m_lastError;
        }
    }
    else{
        m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
        qDebug () << m_lastError;
    }

    //q.finish();
    q.clear();
    q.finish();
    return okp;
    //return false;

}


int In2GestorDB::SizeLastQuery(){
	return CamposDatabase;
}

void In2GestorDB::setNumCamposDataBase(int num){
	CamposDatabase=num;
}

//Ejecuta comandos SQL
bool In2GestorDB::execNoParse(char *sql){
    QSqlDatabase db = QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(/*m_sqlDataBase*/db);
	QString dummy;
    dummy=QString("%1").arg(sql);
    bool okp=q.prepare(dummy);
    if (okp){
        okp=q.exec();
        if (okp){
            m_Tabla.clear();
            QStringList fieldNames;
            QSqlRecord rec = q.record();
            for( int i=0; i<rec.count(); ++i )
                fieldNames << rec.fieldName( i );
            foreach( QString str, fieldNames ){
                while( q.next() ){
                    m_Tabla << q.value(rec.indexOf( str )).toString();
                }
                q.seek(-1);
            }
        }
        else{
            m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
            qDebug () << m_lastError;
        }
    }
    else{
        m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
        qDebug () << m_lastError;
    }

    q.finish();
    return (okp);
}
//Ejecuta comandos SQL sin alterar m_Tabla
bool In2GestorDB::execNoSave(char *sql){
    return exec(QString(sql));
}

//Ejecuta comandos SQL sin alterar m_Tabla
bool In2GestorDB::execNoSave(const QString & sql){
    QSqlDatabase db = QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(db);
    QString dummy=sql;
    bool okp=q.prepare(dummy);
    if (okp){
        okp=q.exec();
        if (!okp){
            m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
            qDebug () << m_lastError;
        }
    }
    else{
        m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
        qDebug () << m_lastError;
    }


    q.clear();
    q.finish();
    return okp;
}

QString In2GestorDB::getLastError(){
    return m_lastError;
}

bool In2GestorDB::execClob(const QString &sql,QVariantList &lst){
   /* QSqlDatabase db = QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(db);
    QByteArray array;
    for (int x=0;x<10000;x++){
        array.append(QString("-%1").arg(x));
    }
    bool rtn=q.prepare("INSERT INTO IN2_TO_SGL_LOG (COD_ORDEN,DATOS) VALUES ('ENS2',?)");
    QVariant clob(array);
    q.bindValue(0,clob);
    rtn=q.exec();
    return rtn;*/

    QSqlDatabase db = QSqlDatabase::database(m_name,false);
    QSqlQuery q=QSqlQuery(db);
    bool rtn=q.prepare(sql);
    if (rtn){
        int count=lst.count();
        for (int x=0;x<count;x++){
            QVariant clob=lst.at(x);
            q.bindValue(x,clob);
        }
        rtn=q.exec();
        if (!rtn){
            m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
            qDebug () << m_lastError;
        }
    }
    else{
        m_lastError=QString("DBError:%1 - DriverError:%2").arg(q.lastError().databaseText()).arg(q.lastError().driverText());
        qDebug () << m_lastError;
    }
    return rtn;


}
