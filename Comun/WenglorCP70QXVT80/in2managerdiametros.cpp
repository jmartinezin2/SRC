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
#include "in2managerdiametros.h"
#include <QApplication>
#include "in2defines.h"

In2ManagerDiametros *In2ManagerDiametros::mInstance=NULL;

In2ManagerDiametros::In2ManagerDiametros(QObject *parent) :
    QObject(parent)
{
    m_MonitorIn=NULL;
    m_MonitorOut=NULL;
}
/** request*/
In2ManagerDiametros* In2ManagerDiametros::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new In2ManagerDiametros(parent);
    }
    return mInstance;
}

void In2ManagerDiametros::initMonitor(int index){
    if (index==IN2MANAGER_DIN){
        m_MonitorIn=new MonitorDiametro(this);
        QString path=QString("%1\\Config\\In2DiameterIN.ini").arg(QApplication::applicationDirPath());
        m_MonitorIn->setIniFile(path);
        m_MonitorIn->init();
    }
    else if (index==IN2MANAGER_DOUT){
        m_MonitorOut=new MonitorDiametro(this);
        QString path=QString("%1\\Config\\In2DiameterOUT.ini").arg(QApplication::applicationDirPath());
        m_MonitorOut->setIniFile(path);
        m_MonitorOut->init();
    }
}

/** Devuelve punteros a los gestores*/
MonitorDiametro* In2ManagerDiametros::getMonitor(int index){
    if (index==IN2MANAGER_DIN) return m_MonitorIn;
    else if (index==IN2MANAGER_DOUT) return m_MonitorOut;
    else return NULL;
}
/** Guarda un valor del PLC*/
void In2ManagerDiametros::storeValueFromPLC(int index,int value){
    if (index==IN2MANAGER_DIN) m_MonitorIn->storeValueFromPLC(value);
    else if (index==IN2MANAGER_DOUT) m_MonitorOut->storeValueFromPLC(value);

}
