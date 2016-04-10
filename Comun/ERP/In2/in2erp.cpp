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
#include "in2erp.h"
#include "MemoryMap.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QDate>


//Una sola instancia
In2ERP *In2ERP::mInstance  = NULL;


/** Lllamda a instancia unica*/
In2ERP *In2ERP::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new In2ERP(parent);
    }
    return mInstance;
}


In2ERP::In2ERP(QObject *parent) :
    QObject(parent)
{
    m_gestorErp=In2GestorERP::request(this);

}

/** Chequea el ERP*/
QStringList In2ERP::checkERP(){
    QStringList data=m_gestorErp->checkERP();
    return data;
}
/** Inserta el trabajo actual en la cola de impresion*/
bool In2ERP::insertCurrentWork(){
    bool rtn=true;
    MemoryMap *memoryMap=MemoryMap::request(0);
    int resp=QMessageBox::question(0,tr("ATENCION"),tr("Desea incluir el trabajo actual en la cola de maquina?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (!resp){
        QString filePath=memoryMap->m_FilePath;
        QFileInfo info(filePath);
        QString name=info.fileName();
        QString path=info.path();
        QString foo=QString("select * from ColaImpresion where Composicion='%1' and Path='%2'")
                .arg(name)
                .arg(path);
        QStringList data=m_gestorErp->execSQL(foo,false);
        if (!data.count()){
            QString fechahora=QString("%1 %2").arg(QDate::currentDate().toString("yyyy-MM-dd")).arg(QTime::currentTime().toString("hh:mm:ss"));
            foo=QString("insert into ColaImpresion ('Composicion','Path','Fecha','OrdenFabricacion') Values ('%1','%2','%3','%4')")
                                .arg(name)
                                .arg(path)
                                .arg(fechahora)
                                .arg(memoryMap->m_mapa.m_variables.OrdenFabricacion);

            m_gestorErp->execSQL(foo,false);
            emit SendDebugInfo(LOGGER_STORE,foo);
        }
        else{
            QString fechahora=QString("%1 %2").arg(QDate::currentDate().toString("yyyy-MM-dd")).arg(QTime::currentTime().toString("hh:mm:ss"));
            foo=QString("update ColaImpresion set Fecha='%1' where Composicion='%2' AND Path='%3'")
                    .arg(fechahora)
                    .arg(name)
                    .arg(path);
            m_gestorErp->execSQL(foo,false);
            emit SendDebugInfo(LOGGER_STORE,foo);
        }
        //memoryMap->ReturnToTempWorld();
    }
    return rtn;
}


/** Ejecuta el sql*/
QStringList In2ERP::execSQL(QString txt, bool save){
    return m_gestorErp->execSQL(txt,save);
}

