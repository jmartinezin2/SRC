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
#include "in2wenglorcontrol.h"

//QList<unsigned char> mapaHex=(QList<unsigned char>()<<0x00<<0x01<<0x02<<0x03<<0x04<<0x05<<0x06<<0x07<<0x08<<0x09<<0x0A<<0x0B<<0x0C<<0x0D<<0x0E<<0x0F);
//QList<unsigned char> mapaAsc=(QList<unsigned char>()<<0x30<<0x31<<0x32<<0x33<<0x34<<0x35<<0x36<<0x37<<0x38<<0x39<<0x41<<0x42<<0x43<<0x44<<0x45<<0x46);


In2WenglorControl::In2WenglorControl(QObject *parent,int id) :
    QObject(parent)
{
    myID=id+1;

    init();

}

/** Lo inicializa*/
void In2WenglorControl::init(){

    statusFlags=0x0000;
    alarm1Flags=0x0000;
    alarm2Flags=0x0000;
    alarm3Flags=0x0000;

    m_ok=true;

}

/*
void In2WenglorControl::convertDiameter(){
    //dTemperatura=convertToDiameter(sTemperatura);
}



double In2WenglorControl::convertDiameter2double(quint16 registro){
    double ret;
    ret=convertToDiameter(registro);
    return (ret);
}



double In2WenglorControl::convertToDiameter(quint16 registro){
    double dRet;
    dRet=0.01*registro;
    return(dRet);
}
*/

unsigned char In2WenglorControl::getMode(){
    unsigned char iRet=0x00;
    return (iRet);
}


void In2WenglorControl::setMode(unsigned char valor){
    m_mode=valor;
}



