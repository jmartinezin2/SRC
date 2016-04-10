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
#include "in2char.h"
#include "modbusmessage.h"

//extern QList<unsigned char> mapaHex;
//extern QList<unsigned char> mapaAsc;

#define MODBUS_HEAD_LEN            1
//Posiciones de los bytes de cabecera
#define MODBUS_SYN_POS             0
#define MODBUS_SLV_ADD_POS         1
#define MODBUS_FUNCTION_POS        3
#define MODBUS_DATA_POS            4

#define MODBUS_SIZE_POS            2
#define MODBUS_ID_POS              3
#define MODBUS_OPTIONS_POS         4
#define MODBUS_COUNTER_POS         4


//Caracter de sincronismo
#define MODBUS_SYN_CHAR            0x3A
#define MODBUS_STOP1_CHAR          0x0D
#define MODBUS_STOP2_CHAR          0x0A

//Bits de opciones
#define MODBUS_BIT_REQ_ACK         0
#define MODBUS_BIT_ACK             1
#define MODBUS_BIT_UNKNOWN         2



/** Constructor*/
ModbusMessage::ModbusMessage(QObject *parent) :
    QObject(parent)
{
    m_lastError=MODBUS_NOT_ERROR;
    m_data.resize(MODBUS_HEAD_LEN);
}


/** Constructor sobrecargado*/
ModbusMessage::ModbusMessage(QObject *parent,int id) :
    QObject(parent)
{
    m_lastError=MODBUS_NOT_ERROR;
    m_data.resize(MODBUS_HEAD_LEN);
    //CreateMessage(id);
}


/** Constructor sobrecargado*/
ModbusMessage::ModbusMessage(QObject *parent,const QByteArray &p) :
    QObject(parent)
{
    m_lastError=MODBUS_NOT_ERROR;
    m_data=p;
}


/** Crea mensaje con datos*/
QByteArray ModbusMessage::CreateMessageWithData(QByteArray &d){
    m_data.clear();
    SetSyn();
    m_data.append(d);
    In2Char z;
    unsigned char dummy=calculateLRC(d);
    unsigned char clrc1=(dummy&0xF0)>>4;
    unsigned char clrc2=(dummy&0x0F);
    unsigned char lrc1=z.fromHex(clrc1);
    unsigned char lrc2=z.fromHex(clrc2);
    m_data.append(lrc1);
    m_data.append(lrc2);
    m_data.append("\r");
    m_data.append("\n");
    return m_data;
}


/** Devuelve el head del mensaje*/
QByteArray ModbusMessage::head(){
    QByteArray rtn=m_data.left(MODBUS_HEAD_LEN);
    return rtn;
}

/** Carga los bytes de sincronismo*/
void ModbusMessage::SetSyn(){
    m_data[MODBUS_SYN_POS]=MODBUS_SYN_CHAR;
}


/** Carga bit de no reconocido..pone resto de opciones a cero*/
void ModbusMessage::SetUnknownMessage(){
   char dummy=(1<<MODBUS_BIT_UNKNOWN);
    m_data[MODBUS_OPTIONS_POS]=dummy;
}

/** Devuelve el ID del ultimo error detectado y borra el registro*/
char ModbusMessage::getLastError(){
    char rtn=m_lastError;
    m_lastError=MODBUS_NOT_ERROR;
    return rtn;
}

/** Devuelve 1 si es mensaje en memoria valido*/
int ModbusMessage::isValidMessage(){
    int rtn=0;
    if (m_data[MODBUS_SYN_POS]==(char)MODBUS_SYN_CHAR){
        if ((int)m_data[MODBUS_SIZE_POS]==m_data.size()){
            rtn=1;
        }
        else m_lastError=MODBUS_ERROR_SIZE;
    }
    else m_lastError=MODBUS_ERROR_SYN;

    return rtn;
}

/** Copia el mensaje*/
void ModbusMessage::loadMessage(const QByteArray &p){
    m_data=p;
}

/** Devuelve el ID*/
char ModbusMessage::getID(){
   char cRet;
   /*unsigned char c1=m_data.at(MODBUS_FUNCTION_POS);
   unsigned char b1=mapaHex.at(mapaAsc.indexOf(c1));
   unsigned char c2=m_data.at(MODBUS_FUNCTION_POS+1);
   unsigned char b2=mapaHex.at(mapaAsc.indexOf(c2));*/
   In2Char z;
   unsigned char b1=z.fromAscii(m_data.at(MODBUS_FUNCTION_POS));
   unsigned char b2=z.fromAscii(m_data.at(MODBUS_FUNCTION_POS+1));
   cRet=(b1<<4)&b2;
   return (cRet);
}

/** Devuelve el mensaje interno*/
QByteArray ModbusMessage::message(){
    return m_data;
}

/** Devuelve un array con los datos*/
QByteArray ModbusMessage::data(){
    QByteArray p;
    p.clear();
    if (m_data.size()>MODBUS_HEAD_LEN)
        p=m_data.right(m_data.size()-MODBUS_HEAD_LEN);
    return p;
}

unsigned char ModbusMessage::calculateLRC(QByteArray &trama){
    unsigned int suma=0;
    unsigned int sumando=0;
    char dummy;
    bool ok=false;
    unsigned char uSuma,uSuma2;
    for (int i=0;i<trama.size();i++){
        dummy=trama.at(i);
        sumando=QString("%1").arg(dummy).toUtf8().toInt(&ok,16);
        suma+=sumando;
    }
    uSuma=(unsigned char) (suma&0xFF);
    uSuma2=(unsigned char) ~uSuma;
    uSuma=uSuma2+1;
    return (uSuma);

}
