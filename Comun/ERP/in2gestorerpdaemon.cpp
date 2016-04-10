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
#include "in2gestorerpdaemon.h"
#include "in2gestorerp.h"
#include <QTimer>


#define UPDATE_TIME 10000

In2GestorERPDaemon::In2GestorERPDaemon(QObject *parent) :
    QObject(parent)
{
    m_bMonitor=false;
    m_gestorERP=In2GestorERP::request(0);
}
/** Des/Activa el monitor*/
void In2GestorERPDaemon::sltSetMonitor(bool val){
    if (val){
        if (!m_bMonitor) QTimer::singleShot(0,this,SLOT(sltMonitor()));
    }
    m_bMonitor=val;
}
/** Activa el monitor*/
void In2GestorERPDaemon::sltEnableMonitor(){
    sltSetMonitor(true);
}
/** Desactiva el monitor*/
void In2GestorERPDaemon::sltDisableMonitor(){
    sltSetMonitor(false);
}


/** Monitoriza el ERP*/
void In2GestorERPDaemon::sltMonitor(){
    sltCheckERP();
    if (m_bMonitor) QTimer::singleShot(UPDATE_TIME,this,SLOT(sltMonitor()));
}

/** Chequea el ERP*/
void In2GestorERPDaemon::sltCheckERP(){
    QStringList data=m_gestorERP->checkERP();
    if (m_data!=data){
        m_data=data;
        emit sgnERPDaemonDataChanged(data);
    }
}
