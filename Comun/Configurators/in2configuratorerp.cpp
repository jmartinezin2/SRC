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
#include "in2configuratorerp.h"
#include <QSettings>
#include <QApplication>
#include <QFile>
#include <QDir>
#include "inkjetdebug.h"
#include "in2defines.h"


In2ConfiguratorERP::In2ConfiguratorERP(QObject *parent) :
    QObject(parent)
{
    m_connection="";
    m_user="";
    m_password="";
    m_cliente="";
    m_Driver="";
    m_ODBCEntry="";
    m_BBDD="";
    m_readBBDD="";
    m_fieldsIn="";
    m_fieldsOut="";
    m_inTable="";
    m_outTable="";
    m_fieldCompo="";
    m_fieldOT="";
    m_enabled=false;
    m_bMultipleItems=false;
    m_ID="";
    m_interFile="";
    m_interFieldsIn="";
}
/** Lee la configuracion*/
bool In2ConfiguratorERP::readConfig(){
    bool rtn=false;
    QString txt;
    txt=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(ERP_FILE_ERP);
    //Testeamos que exista
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    if (QFile::exists(txt)){
        QSettings FicheroIni(txt,QSettings::IniFormat,0);
        FicheroIni.beginGroup(QString(ERP_ERP));
        m_connection=FicheroIni.value(ERP_CONNECTION,"").toString();
        m_user=FicheroIni.value(ERP_USER,"").toString();
        m_password=FicheroIni.value(ERP_PASSWORD,"").toString();
        m_cliente=FicheroIni.value(ERP_CLIENTE,"").toString();
        m_enabled=FicheroIni.value(ERP_ENABLED,false).toBool();
        m_Driver=FicheroIni.value(ERP_DRIVER,"").toString();
        m_ODBCEntry=FicheroIni.value(ERP_ODBC_ENTRY,"").toString();
        QString foo=FicheroIni.value(ERP_BBDD,"").toString();
        if (foo.count()){
            if (foo.at(0)=='.') {
                QString str=foo.mid(1,foo.count()-1);
                m_BBDD=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(str);
            }
            else m_BBDD=foo;
        }
        else
            m_BBDD=foo;

        m_readBBDD=FicheroIni.value(ERP_READ_BBDD,"").toString();
        m_writeBBDD=FicheroIni.value(ERP_WRITE_BBDD,"").toString();
        m_fieldsIn=FicheroIni.value(ERP_FIELDS_BBDD_IN,"").toString();
        m_fieldsOut=FicheroIni.value(ERP_FIELDS_BBDD_OUT,"").toString();
        m_inTable=FicheroIni.value(ERP_TABLE_IN,"").toString();
        m_outTable=FicheroIni.value(ERP_TABLE_OUT,"").toString();
        m_bMultipleItems=FicheroIni.value(ERP_MULTIPLE_ITEMS,false).toBool();
        m_fieldCompo=FicheroIni.value(ERP_FIELD_COMPO,"").toString();
        m_fieldOT=FicheroIni.value(ERP_FIELD_OT,"").toString();
        m_fieldMetros=FicheroIni.value(ERP_FIELD_METROS,"").toString();
        m_fieldEstado=FicheroIni.value(ERP_FIELD_ESTADO,"").toString();
        m_fieldError=FicheroIni.value(ERP_FIELD_ERROR,"").toString();
        m_fieldTipoTarea=FicheroIni.value(ERP_FIELD_TIPO_TAREA,"").toString();
        m_fieldLote=FicheroIni.value(ERP_FIELD_LOTE,"").toString();
        m_fieldFchCad=FicheroIni.value(ERP_FIELD_FCHCAD,"").toString();
        m_fieldFchFab=FicheroIni.value(ERP_FIELD_FCHFAB,"").toString();
        m_fieldMsg=FicheroIni.value(ERP_FIELD_MSG,"").toString();
        m_interFile=FicheroIni.value(ERP_INTERFILE,"").toString();        
        m_interFieldsIn=FicheroIni.value(ERP_INTERFIELDS_IN,"").toString();
        FicheroIni.endGroup();
        rtn=true;
    }
    else{
        infoDebug->ProcessMsg(LOGGER_WARNING,tr("No se pudo leer %1").arg(txt));
    }
    return rtn;
}
/** Devuelve el ID*/
QString In2ConfiguratorERP::getID(){
    return m_ID;
}

/** Devuelve el indice del campo compo*/
int In2ConfiguratorERP::getCompoFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldCompo);
    return iField;
}

/** Devuelve el indice del campo compo*/
int In2ConfiguratorERP::getOTFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldOT);
    return iField;
}

/** Devuelve el indice de metros*/
int In2ConfiguratorERP::getMetrosFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldMetros);
    return iField;
}

/** Devuelve el indice de metros*/
int In2ConfiguratorERP::getLoteFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldLote);
    return iField;
}
/** Devuelve el indice de metros*/
int In2ConfiguratorERP::getFchCadFieldIndex(){
    QStringList lst=m_fieldsIn.split("@");
    int iField=lst.indexOf(m_fieldFchCad);
    return iField;
}


/** Devuelve el tipo de tarea*/
QString In2ConfiguratorERP::getFieldTipoTarea(){
    return m_fieldError;
}

/** Devuelve el error*/
QString In2ConfiguratorERP::getFieldError(){
    return m_fieldError;
}
/** Devuelve el campo Msg*/
QString In2ConfiguratorERP::getFieldMsg(){
    return m_fieldMsg;
}

/** Devuelve el estado*/
QString In2ConfiguratorERP::getFieldEstado(){
    return m_fieldEstado;
}

/** Devuelve los metros*/
QString In2ConfiguratorERP::getFieldMetros(){
    return m_fieldMetros;
}

/** Devuelve el campo de compo*/
QString In2ConfiguratorERP::getFieldOT(){
    return m_fieldOT;
}

/** Devuelve el campo de compo*/
QString In2ConfiguratorERP::getFieldCompo(){
    return m_fieldCompo;
}

/** Devuelve el campo Lote*/
QString In2ConfiguratorERP::getFieldLote(){
    return m_fieldLote;
}

/** Devuelve el campo fecha de caducidad*/
QString In2ConfiguratorERP::getFieldFchCad(){
    return m_fieldFchCad;
}

/** Devuelve el campo fecha de fabricacion*/
QString In2ConfiguratorERP::getFieldFchFab(){
    return m_fieldFchFab;

}




/** Devuelve si es mutiple items*/
bool In2ConfiguratorERP::getMultipleItems(){
    return m_bMultipleItems;
}

/** Devuelve la tabla IN*/
QString In2ConfiguratorERP::getInTable(){
    return m_inTable;
}
/** Devuelve la tabla OUT*/
QString In2ConfiguratorERP::getOutTable(){
    return m_outTable;
}

/** Devuelve los campos IN*/
QString In2ConfiguratorERP::getFieldsIn(){
    return m_fieldsIn;
}
/** Devuelve los campos OUT*/
QString In2ConfiguratorERP::getFieldsOut(){
    return m_fieldsOut;
}

/** Devuelve la instruccion de lectura*/
QString In2ConfiguratorERP::getReadBBDD(){
    return m_readBBDD;
}
/** Devuelve la instruccion de lectura*/
QString In2ConfiguratorERP::getWriteBBDD(){
    return m_writeBBDD;
}


/** Devuelve la BBDD*/
QString In2ConfiguratorERP::getBBDD(){
   return m_BBDD;
}

/** Devuelve la entrada ODBC*/
QString In2ConfiguratorERP::getODBCEntry(){
    return m_ODBCEntry;
}
/** Devuelve el driver*/
QString In2ConfiguratorERP::getDriver(){
    return m_Driver;
}

/** Devuelve si esta enabled*/
bool In2ConfiguratorERP::getEnabled(){
    return m_enabled;
}
/** Devuelve la conexion*/
QString In2ConfiguratorERP::getConnection(){
    return m_connection;
}
/** Devuelve el usuario*/
QString In2ConfiguratorERP::getUser(){
    return m_user;
}
/** Devuelve el password*/
QString In2ConfiguratorERP::getPassword(){
    return m_password;
}
/** Devuelve el cliente*/
QString In2ConfiguratorERP::getClient(){
    return m_cliente;
}
/** Devuelve el fichero a testear intermedio*/
QString In2ConfiguratorERP::getInterFile(){
    return m_interFile;
}
/** Devuelve los campos a testear intermedio*/
QString In2ConfiguratorERP::getInterFields(){
    return m_interFieldsIn;
}

