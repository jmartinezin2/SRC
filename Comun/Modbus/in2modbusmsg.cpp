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
#include "in2modbusmsg.h"

#define DEFAULT_LEN 8

#define SIZE_HIGH 4
#define SIZE_LOW 5
#define CODE 7
In2ModbusMsg::In2ModbusMsg(QObject *parent) :
    QObject(parent)
{
    m_msg.resize(DEFAULT_LEN);
    m_msg.fill(0x00);
    m_msg[SIZE_LOW]=0x02;
}

QByteArray In2ModbusMsg::getMsg(char code,QByteArray data){
    m_msg.append(data);
    m_msg[CODE]=code;
    return m_msg;

}


