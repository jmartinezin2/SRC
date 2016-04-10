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
#include "in2configuratorpermisos.h"
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include "MemoryMap.h"
#include "In2GestorDB.h"


#define PERMISO_UNKNOWN 0xFF

#define DB_PERMISOS "In2Permisos.sqlite"

In2ConfiguratorPermisos::In2ConfiguratorPermisos()
{
}
/** Carga los permisos de pantalla principal*/
bool In2ConfiguratorPermisos::loadPermisos(QString pantalla){
    bool rtn=false;
    In2GestorDB db;
    QString file=QString("%1\\Database\\%2").arg(QApplication::applicationDirPath()).arg(DB_PERMISOS);
    m_listaPermisos.clear();
    if (db.open(QString("QSQLITE"),QString("In2Permisos"),file)){
        QString foo=QString("select * from '%1'").arg(pantalla);
        rtn=db.exec(foo);
        if (rtn){
            int counter=0;
            int items=db.m_Tabla.count();
            if ((items)&&((items%2)==0)){
                while (counter<db.m_Tabla.count()){
                    QString item=db.m_Tabla.at(counter);
                    counter++;
                    bool ok;
                    int value=db.m_Tabla.at(counter).toInt(&ok);
                    if (ok){
                        m_listaPermisos.insert(item,value);
                    }
                    counter++;
                }
            }
        }
        db.close();
    }
    return rtn;
}


/** Devuelve el permiso de pantalla principal*/
unsigned char In2ConfiguratorPermisos::_searchPermiso(QString key){
    unsigned char value=m_listaPermisos.value(key,PERMISO_UNKNOWN);
    return value;
}

/** Devuelve permiso de pantalla principal */
bool In2ConfiguratorPermisos::permiso(QString key){
    bool rtn=false;
    MemoryMap *memoryMap=MemoryMap::request(0);
    int rights=memoryMap->getUserRights();
    int val=_searchPermiso(key);
    if ((rights!=PERMISO_UNKNOWN)&&(rights)){
        if (val&(1<<(rights-1))) rtn=true;
    }
    else{
        //Avisamos que hay un permiso sin definir
        QMessageBox::warning(0,"ERROR",QObject::tr("No se ha deficido permiso para %1").arg(key),QMessageBox::Ok);
    }
    return rtn;
}
