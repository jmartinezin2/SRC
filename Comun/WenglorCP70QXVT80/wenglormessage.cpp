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
#include <QMap>

#include "wenglormessage.h"
#include "in2char.h"

#define HEAD_LEN            1
//Posiciones de los bytes de cabecera
#define SYN_POS             0
#define SLV_ADD_POS         1
#define FUNCTION_POS        3
#define DATA_POS            4
#define SIZE_POS            2
#define ID_POS              3
#define OPTIONS_POS         4
#define COUNTER_POS         4
//Caracter de sincronismo
#define SYN_CHAR           0x2F
#define STOP_CHAR          0x2E

//Bits de opciones
#define BIT_REQ_ACK         0
#define BIT_ACK             1
#define BIT_UNKNOWN         2


//extern QList<unsigned char> mapaHex;
//extern QList<unsigned char> mapaAsc;

/** Constructor*/
WenglorMessage::WenglorMessage(QObject *parent) :
    QObject(parent)
{
    m_lastError=MONDIAMETRO_NOT_ERROR;
    m_data.resize(HEAD_LEN);
}

/** Constructor sobrecargado*/
WenglorMessage::WenglorMessage(QObject *parent,const QByteArray &p) :
    QObject(parent)
{
    m_lastError=MONDIAMETRO_NOT_ERROR;
    m_data=p;
}


/** Crea mensaje con datos*/
QByteArray WenglorMessage::CreateMessageWithData(QByteArray &d){
    m_data.clear();
    SetSyn();
    m_data.append(d);
    m_data.append(".");
    return m_data;
}


/** Devuelve el head del mensaje*/
QByteArray WenglorMessage::head(){
    QByteArray rtn=m_data.left(HEAD_LEN);
    return rtn;
}

/** Carga los bytes de sincronismo*/
void WenglorMessage::SetSyn(){
    m_data[SYN_POS]=SYN_CHAR;
}


/** Carga bit de no reconocido..pone resto de opciones a cero*/
void WenglorMessage::SetUnknownMessage(){
   char dummy=(1<<BIT_UNKNOWN);
    m_data[OPTIONS_POS]=dummy;
}

/** Devuelve el ID del ultimo error detectado y borra el registro*/
char WenglorMessage::getLastError(){
    char rtn=m_lastError;
    m_lastError=MONDIAMETRO_NOT_ERROR;
    return rtn;
}

/** Devuelve 1 si es mensaje en memoria valido*/
int WenglorMessage::isValidMessage(){
    int rtn=0;
    if (m_data[SYN_POS]==(char)SYN_CHAR){
        if ((int)m_data[SIZE_POS]==m_data.size()){
            rtn=1;
        }
        else m_lastError=MONDIAMETRO_ERROR_SIZE;
    }
    else m_lastError=MONDIAMETRO_ERROR_SYN;

    return rtn;
}

/** Copia el mensaje*/
void WenglorMessage::loadMessage(const QByteArray &p){
    m_data=p;
}

/** Devuelve el ID*/
char WenglorMessage::getID(){
   char cRet;
   In2Char z;
   /*unsigned char c1=m_data.at(FUNCTION_POS);
   unsigned char b1=mapaHex.at(mapaAsc.indexOf(c1));
   unsigned char c2=m_data.at(FUNCTION_POS+1);
   unsigned char b2=mapaHex.at(mapaAsc.indexOf(c2));*/
   unsigned char b1=z.fromAscii(m_data.at(FUNCTION_POS));
   unsigned char b2=z.fromAscii(m_data.at(FUNCTION_POS+1));
   cRet=(b1<<4)&b2;
   return (cRet);
}

/** Devuelve el mensaje interno*/
QByteArray WenglorMessage::message(){
    return m_data;
}

/** Devuelve un array con los datos*/
QByteArray WenglorMessage::data(){
    QByteArray p;
    p.clear();
    if (m_data.size()>HEAD_LEN)
        p=m_data.right(m_data.size()-HEAD_LEN);
    return p;
}

