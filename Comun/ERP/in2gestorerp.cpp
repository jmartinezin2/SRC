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
#include "in2gestorerp.h"
#include "In2GestorDB.h"
#include "in2configuratorerp.h"
#include "in2defines.h"
#include <QMessageBox>
#include "inkjetdebug.h"

//Una sola instancia
In2GestorERP *In2GestorERP::mInstance  = NULL;


/** Lllamda a instancia unica*/
In2GestorERP *In2GestorERP::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new In2GestorERP(parent);
    }
    return mInstance;
}


In2GestorERP::In2GestorERP(QObject *parent) :
    QObject(parent)
{
    In2ConfiguratorERP p;
    p.readConfig();

    m_Connection=p.getConnection();
    m_Driver=p.getDriver();
    m_ODBCEntry=p.getODBCEntry();
    m_User=p.getUser();
    m_Password=p.getPassword();
    m_Cliente=p.getClient();
    m_enabled=p.getEnabled();
    m_BBDD=p.getBBDD();
    m_ReadBBDD=p.getReadBBDD();
    m_WriteBBDD=p.getWriteBBDD();
    m_fieldsIn=p.getFieldsIn();
    m_fieldsOut=p.getFieldsOut();
    m_inTable=p.getInTable();
    m_outTable=p.getOutTable();
    m_bMultipleItems=p.getMultipleItems();
    m_fieldCompo=p.getFieldCompo();
    m_fieldOT=p.getFieldOT();
    m_fieldEstado=p.getFieldEstado();
    m_fieldMetros=p.getFieldMetros();
    m_fieldError=p.getFieldError();
    m_fieldMsg=p.getFieldMsg();
    m_fieldTipoTarea=p.getFieldTipoTarea();
    InkjetDebug *dbg=InkjetDebug::request(0);
    connect(this,SIGNAL(sgnShowInfo()),dbg,SLOT(showLog()));

}
/** Devuelve el indice de metros*/
int In2GestorERP::getMetrosFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldMetros);
    return iField;
}

/** Carga el tipo de tarea*/
void In2GestorERP::setFieldTipoTarea(QString txt){
    m_fieldTipoTarea=txt;
}


/** Devuelve el tipo de tarea*/
QString In2GestorERP::getFieldTipoTarea(){
    return m_fieldTipoTarea;
}

/** Carga el campo de estado*/
void In2GestorERP::setFieldEstado(QString txt){
    m_fieldEstado=txt;
}

/** Devuelve el campo de estado*/
QString In2GestorERP::getFieldEstado(){
    return m_fieldEstado;
}

/** Carga el campo de estado*/
void In2GestorERP::setFieldError(QString txt){
    m_fieldError=txt;
}

/** Devuelve el campo de estado*/
QString In2GestorERP::getFieldError(){
    return m_fieldError;
}
/** Devuelve el campo de estado*/
QString In2GestorERP::getFieldMsg(){
    return m_fieldMsg;
}


/** Devuelve los metros*/
QString In2GestorERP::getMetros(){
    return m_fieldMetros;
}
/** Carga los metros*/
void In2GestorERP::setMetros(QString txt){
    m_fieldMetros=txt;
}

/** Devuelve el indice del campo compo*/
int In2GestorERP::getCompoFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldCompo);
    return iField;
}

/** Devuelve el indice del campo compo*/
int In2GestorERP::getOTFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldOT);
    return iField;
}

/** Carga el field compo*/
void In2GestorERP::setFieldCompo(QString txt){
    m_fieldCompo=txt;
}
/** Devuelve el field compo*/
QString In2GestorERP::getFieldCompo(){
    return m_fieldCompo;
}

/** Carga el field OT*/
void In2GestorERP::setFieldOT(QString txt){
    m_fieldOT=txt;
}
/** Devuelve el field OT*/
QString In2GestorERP::getFieldOT(){
    return m_fieldOT;
}


/** Carga la tabla IN*/
void In2GestorERP::setTableIn(QString txt){
    m_inTable=txt;
}
/** Devuelve la tabla IN*/
QString In2GestorERP::getTableIn(){
    return m_inTable;
}

/** Carga la tabla OUT*/
void In2GestorERP::setTableOut(QString txt){
    m_outTable=txt;
}
/** Devuelve la tabla OUT*/
QString In2GestorERP::getTableOut(){
    return m_outTable;
}


/** carga los campos IN*/
void In2GestorERP::setFieldsIn(QString txt){
    m_fieldsIn=txt;
}

/** Devuelve los campos IN*/
QString In2GestorERP::getFieldsIn(){
    return m_fieldsIn;
}
/** carga los campos OUT*/
void In2GestorERP::setFieldsOut(QString txt){
    m_fieldsOut=txt;
}

/** Devuelve los campos OUT*/
QString In2GestorERP::getFieldsOut(){
    return m_fieldsOut;
}


/** Chequea el ERP*/
QStringList In2GestorERP::checkERP(){
    QStringList data;
    if (!m_ReadBBDD.isNull())
        data=execSQL(m_ReadBBDD,true);
    return data;
}

/** Guarda resultados en el ERP*/
void In2GestorERP::writeResultERP(){

    int countIn=m_fieldsIn.split("@").count();
    if (m_savedRequest.count()==countIn){
        QString foo=QString("insert into %1 ").arg(this->getTableOut());
        foo.append(QString("values ('%1','%2','%3','%4','%5','%6','%7','%8','%9'")
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(1))
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(0))
                   .arg(m_savedRequest.at(0)));


        execSQL(foo,false);
    }

}
/** devuelve el last_query_result*/
bool In2GestorERP::getLastQueryResult(){
    return m_lastQueryResult;
}
/** Escribe sobre un CLOB*/
bool In2GestorERP::execClob(QString txt,QVariantList &lst){
    In2GestorDB p;
    bool rtn=false;
    if (!m_Cliente.compare(ERP_ESTEVE)){
        m_lastQueryResult=p.open(m_Driver,m_Cliente,m_ODBCEntry,m_User,m_Password);
        if (m_lastQueryResult){
            m_lastQueryResult=p.execClob(txt,lst);
            if (!m_lastQueryResult){
                QString error=tr("No se pudo ejecutar %1 en %2").arg(txt).arg(m_BBDD);
                emit SendDebugInfo(LOGGER_ERROR, error);
                //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
                emit sgnShowInfo();
            }
        }
        else{

        }
        rtn=m_lastQueryResult;
    }
    return rtn;
}

/** Chequea los datos sql que se le de*/
QStringList In2GestorERP::execSQL(QString txt,bool save){
    In2GestorDB p;
    QStringList data;
    if (!m_Cliente.compare(ERP_IN2)){
        if ((m_lastQueryResult=p.open(m_Driver,m_Connection,m_BBDD))){
            if ((m_lastQueryResult=p.exec(txt))){
                data=p.m_Tabla;
            }
            else {
                QString error=tr("No se pudo ejecutar %1 en %2").arg(txt).arg(m_BBDD);
                emit SendDebugInfo(LOGGER_ERROR, error);
                //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
                emit sgnShowInfo();
            }
            p.close();
            //QSqlDatabase::removeDatabase(m_Connection);
        }
        else{
            QString error=tr("No se pudo abrir %1").arg(m_BBDD);
            emit SendDebugInfo(LOGGER_ERROR, error);
            emit sgnShowInfo();

            //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
        }
    }
    if (!m_Cliente.compare(ERP_GRAMAR)){
        if ((m_lastQueryResult=p.open(m_Driver,m_Connection,m_BBDD))){
            if ((m_lastQueryResult=p.exec(txt))){
                data=p.m_Tabla;
            }
            else {
                QString error=tr("No se pudo ejecutar %1 en %2").arg(txt).arg(m_BBDD);
                emit SendDebugInfo(LOGGER_ERROR, error);
                //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
                emit sgnShowInfo();
            }
            p.close();
            //QSqlDatabase::removeDatabase(m_Connection);
        }
        else{
            QString error=tr("No se pudo abrir %1").arg(m_BBDD);
            emit SendDebugInfo(LOGGER_ERROR, error);
            emit sgnShowInfo();

            //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
        }
    }
    else if (!m_Cliente.compare(ERP_ESTEVE)){
        if ((m_lastQueryResult=p.open(m_Driver,m_Cliente,m_ODBCEntry,m_User,m_Password))){
            if ((m_lastQueryResult=p.exec(txt))){
                data=p.m_Tabla;
            }
            else{
                QString error=tr("No se pudo ejecutar %1 en %2").arg(txt).arg(m_Driver);
                emit SendDebugInfo(LOGGER_ERROR, error);
                emit sgnShowInfo();
                //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
            }
            p.close();
            //QSqlDatabase::removeDatabase(m_Cliente);
        }
        else{
            QString error=tr("No se pudo abrir %1").arg(m_Driver);
            emit SendDebugInfo(LOGGER_ERROR, error);
            emit sgnShowInfo();
            p.close();
            QSqlDatabase::removeDatabase(m_Cliente);
            //QMessageBox::warning(0,tr("ERROR"),error,QMessageBox::Ok);
        }
    }
    if (save) m_savedRequest=data;
    return data;
}



/** Carga el multiple items*/
void In2GestorERP::setMultipleItems(bool val){
    m_bMultipleItems=val;
}

/** Guarda la instrución para leer la BBDD*/
void In2GestorERP::setReadBBDD(QString txt){
    m_ReadBBDD=txt;
}

/** Guarda la instrución para leer la BBDD*/
void In2GestorERP::setWriteBBDD(QString txt){
    m_WriteBBDD=txt;
}


/** Guarda la BBDD*/
void In2GestorERP::setBBDD(QString txt){
    m_BBDD=txt;
}

/** Guarda la entrada ODBC*/
void In2GestorERP::setODBCEntry(QString txt){
    m_ODBCEntry=txt;
}

/** Guarda el driver*/
void In2GestorERP::setDriver(QString txt){
   m_Driver=txt;
}

/** Guarda la conexion*/
void In2GestorERP::setConnection(QString txt){
    m_Connection=txt;
}
/** Guarda el user*/
void In2GestorERP::setUser(QString txt){
    m_User=txt;
}
/** Guarda el password*/
void In2GestorERP::setPassword(QString txt){
    m_Password=txt;
}
/** Guarda el cliente*/
void In2GestorERP::setCliente(QString txt){
    m_Cliente=txt;
}
/** Guarda el enabled*/
void In2GestorERP::setEnabled(bool val){
    m_enabled=val;
}

/** Devuelve la conexion*/
QString In2GestorERP::getConnection(){
    return m_Connection;
}

/** Devuelve el user*/
QString In2GestorERP::getUser(){
    return m_User;
}

/** Devuelve el password*/
QString In2GestorERP::getPassword(){
    return m_Password;
}

/** Devuelve el cliente*/
QString In2GestorERP::getCliente(){
    return m_Cliente;
}

/** Devuelve el enabled*/
bool In2GestorERP::getEnabled(){
    return m_enabled;
}

/** Devuelve el driver*/
QString In2GestorERP::getDriver(){
    return m_Driver;
}
/** Devuelve la entrada ODBC*/
QString In2GestorERP::getODBCEntry(){
    return m_ODBCEntry;
}
/** Devuelve la BBDD*/
QString In2GestorERP::getBBDD(){
    return m_BBDD;
}

/** Devuelve el read_BBDD*/
QString In2GestorERP::getReadBBDD(){
    return m_ReadBBDD;
}

/** Devuelve el read_BBDD*/
QString In2GestorERP::getWriteBBDD(){
    return m_WriteBBDD;
}

/** Devuelve el multiple items*/
bool In2GestorERP::getMultipleItems(){
    return m_bMultipleItems;
}

/** Devuelve resultado de la ultima consulta*/
QStringList In2GestorERP::getLastRequest(){
    return m_savedRequest;
}
