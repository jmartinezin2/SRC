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
#include "in2filtrovision.h"
#include "in2defines.h"
#include "inkjetdebug.h"

/*In2FiltroVision::In2FiltroVision(QObject *parent) :
    QObject(parent)*/
In2FiltroVision::In2FiltroVision()

{
    m_bEnabled=false;
    m_numErrores=0;
    m_time=0;
    m_counter=0;
    m_itemOPC=0;
    m_timeElapsed.invalidate();
    //InkjetDebug *dbg=InkjetDebug::request(0);
    //connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
}
/** Devuelve si esta activo*/
bool In2FiltroVision::isEnabled(){
    return m_bEnabled;
}
/** Cambia el contador y testea si hay alarma*/
bool In2FiltroVision::updateCounter(unsigned short num){
    bool rtn=false;
    if (num!=m_counter){
        if (m_time){
            if (!m_timeElapsed.isValid()) m_timeElapsed.start();
        }
        m_counter=num;
        //Condicion de alarma
        if (m_counter>=m_numErrores){
            //No hay condicion de tiempo...disgusto
            if (!m_time) rtn=true;
            else{

                quint64 mlsg=m_timeElapsed.elapsed();
                //emit SendDebugInfo(LOGGER_WARNING,QString("Vision mslg[%1]- mTime[%2]").arg(mlsg).arg(m_time));
                if (mlsg<=m_time){
                    m_lastElapsed=mlsg;
                    rtn=true;
                }
            }
        }
    }
    return rtn;
}
QString In2FiltroVision::dbgTime(){
    QString txt=QString("m_time=%1 elapsed=%2").arg(m_time).arg(getCurrentElapsed());
    return txt;
}

/** Devuelve el tiempo del ultimo fallo*/
quint64 In2FiltroVision::getLastElapsedTime(){
    return m_lastElapsed;
}

/** Devuelve el tiempo desde el ultimo error*/
quint64 In2FiltroVision::getCurrentElapsed(){
    quint64 val=0;
    if (m_timeElapsed.isValid()) val=m_timeElapsed.elapsed();
    return val;
}

/** establece que item OPC develve el contador*/
void In2FiltroVision::setItemOPC(int item){
    m_itemOPC=item;
}
/** Devuelve el item opc que define este filtro*/
int In2FiltroVision::getItemOPC(){
    return m_itemOPC;
}
/** Inicia el timer del filtro*/
void In2FiltroVision::startTimer(){
    if (m_time) (m_timeElapsed.restart());
}
/** Invalida el timer del filtro*/
void In2FiltroVision::stopTimer(){
    if (m_time) (m_timeElapsed.invalidate());
}

/** habilita*/
void In2FiltroVision::setEnabled(bool enabled){
    m_bEnabled=enabled;
}
/** Devuelve numero de errores*/
unsigned short In2FiltroVision::getNumErrores(){
    return m_numErrores;
}
/** Devuelve el contador actual*/
unsigned short In2FiltroVision::getCounter(){
    return m_counter;
}

/** Establece numero de errores*/
void In2FiltroVision::setNumErrores(unsigned short num){
    m_numErrores=num;
}
/** Devuelve tiempo para alrma*/
quint64 In2FiltroVision::getTime(){
    return m_time;
}

/** Establece tiempo para alrma*/
void In2FiltroVision::setTime(quint64 time){
    m_time=time;
}

