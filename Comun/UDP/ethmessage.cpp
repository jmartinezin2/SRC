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

#include "ethmessage.h"


/** Constructor*/
EthMessage::EthMessage(QObject *parent) :
    QObject(parent)
{
    m_lastError=NOT_ERROR;
    m_data.resize(HEAD_LEN);
}
/** Constructor sobrecargado*/
EthMessage::EthMessage(QObject *parent,int id) :
    QObject(parent)
{
    m_lastError=NOT_ERROR;
    m_data.resize(HEAD_LEN);
    CreateMessage(id);
}


/** Constructor sobrecargado*/
EthMessage::EthMessage(QObject *parent,const QByteArray &p) :
    QObject(parent)
{
    m_lastError=NOT_ERROR;
    m_data=p;
}
/** Crea mensaje*/
QByteArray EthMessage::CreateMessage(int id){
    this->m_data.fill(0x00);
    SetSyn();
    SetReqAck();
    m_data[ID_POS]=id;
    m_data[SIZE_POS]=(char)(HEAD_LEN/256);
    m_data[SIZE_POS+1]=(char)(HEAD_LEN%256);
    return m_data;
}
/** Crea mensaje con datos*/
QByteArray EthMessage::CreateMessageWithData(int id,QByteArray &d){
    this->m_data.fill(0x00);
    SetSyn();
    //SetReqAck();
    m_data[ID_POS]=id;
    m_data.append(d);
    m_data[SIZE_POS]=(char)(m_data.length());
    //m_data[SIZE_POS]=(char)(m_data.length()/256);
    //m_data[SIZE_POS+1]=(char)(m_data.length()%256);
    return m_data;
}


/** Crea mensaje sin datos*/
QByteArray EthMessage::CreateMessageNoData(int id){
    m_data.resize(HEAD_LEN);
    this->m_data.fill(0x00);
    SetSyn();
    SetReqAck();
    m_data[ID_POS]=id;
    m_data[SIZE_POS]=(char)(HEAD_LEN/256);
    m_data[SIZE_POS+1]=(char)(HEAD_LEN%256);
    return m_data;
}
/** Devuelve el head del mensaje*/
QByteArray EthMessage::head(){
    QByteArray rtn=m_data.left(HEAD_LEN);
    return rtn;
}

/** Carga los bytes de sincronismo*/
void EthMessage::SetSyn(){
    m_data[SYN_POS]=SYN_CHAR;
    m_data[SYN_POS+1]=SYN_CHAR;
}
/** Carga bit de req-Ack*/
void EthMessage::SetReqAck(){
   char dummy=m_data[OPTIONS_POS]|(1<<BIT_REQ_ACK);
    m_data[OPTIONS_POS]=dummy;
}
/** Carga bit de Ack*/
void EthMessage::SetBitACK(){
   char dummy=m_data[OPTIONS_POS]|(1<<BIT_ACK);
   m_data[OPTIONS_POS]=dummy;
}
/** Carga bit de no reconocido..pone resto de opciones a cero*/
void EthMessage::SetUnknownMessage(){
   char dummy=(1<<BIT_UNKNOWN);
    m_data[OPTIONS_POS]=dummy;
}
/** Devuelve el ID del ultimo error detectado y borra el registro*/
char EthMessage::getLastError(){
    char rtn=m_lastError;
    m_lastError=NOT_ERROR;
    return rtn;
}
/** Devuelve 1 si es mensaje en memoria valido*/
int EthMessage::isValidMessage(){
    int rtn=0;
    if ((m_data[SYN_POS]==(char)SYN_CHAR)&&(m_data[SYN_POS+1]==(char)SYN_CHAR)){
        if ((m_data[SIZE_POS]*256+m_data[SIZE_POS+1])==m_data.size()){
            rtn=1;
        }
        else m_lastError=ERROR_SIZE;
    }
    else m_lastError=ERROR_SYN;

    return rtn;
}
/** Copia el mensaje*/
void EthMessage::loadMessage(const QByteArray &p){
    m_data=p;
}
/** Devuelve el ID*/
char EthMessage::getID(){
   return m_data.at(ID_POS);
}
/** Devuelve el mensaje interno*/
QByteArray EthMessage::message(){
    return m_data;
}
/** Devuelve un array con los datos*/
QByteArray EthMessage::data(){
    QByteArray p;
    p.clear();
    if (m_data.size()>HEAD_LEN) p=m_data.right(m_data.size()-HEAD_LEN);
    return p;
}
