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
#include "in2aisvisionobject.h"

In2AISVisionObject::In2AISVisionObject()

{
    m_x=0;
    m_y=0;
    m_height=0;
    m_width=0;
    m_type=0;
    m_angle=0;
    m_string.clear();
    m_name.clear();
    m_strAngle.clear();
    m_strHeight.clear();
    m_strWidth.clear();
    m_strX.clear();
    m_strY.clear();
}
/** Carga datos*/
void In2AISVisionObject::setData(int type, QString x, QString y, QString h, QString w, QString str,QString name){
    m_type=type;
    m_string=str;
    m_strX=x;
    m_strY=y;
    m_strHeight=h;
    m_strWidth=w;
    m_x=x.toFloat();
    m_y=y.toFloat();
    m_height=h.toFloat();
    m_width=w.toFloat();
    m_name=name;
}
/** Devuelve el  angulo*/
QString In2AISVisionObject::getAngle(){
    return m_strAngle;
}
/** Carga el  angulo*/
void In2AISVisionObject::setAngle(QString val){
    m_strAngle=val;
    m_angle=val.toFloat();
}

/** Devuelve el tipo*/
int In2AISVisionObject::getType(){
    return m_type;
}
/** Devuelve el string*/
QString In2AISVisionObject::getString(){
    return m_string;
}
/** Carga el string*/
void In2AISVisionObject::setString(QString str){
    m_string=str;
}
/** Devuelve el name*/
QString In2AISVisionObject::getName(){
    return m_name;
}
/** Carga el string*/
void In2AISVisionObject::setName(QString str){
    m_name=str;
}


/** Carga el tipo*/
void In2AISVisionObject::setType(int val){
    m_type=val;
}

/** Devuelve la posicion X*/
QString In2AISVisionObject::getX(){
    return m_strX;
}
/** Devuelve la posicion Y*/
QString In2AISVisionObject::getY(){
    return m_strY;
}
/** Devuelve el H*/
QString In2AISVisionObject::getH(){
    return m_strHeight;
}
/** Devuelve el W*/
QString In2AISVisionObject::getW(){
    return m_strWidth;
}
/** Carga el X*/
void In2AISVisionObject::setX(QString val){
    m_strX=val;
    m_x=val.toFloat();
}
/** carga el Y*/
void In2AISVisionObject::setY(QString val){
    m_strY=val;
    m_y=val.toFloat();
}
/** Carga el H*/
void In2AISVisionObject::setH(QString val){
    m_strHeight=val;
    m_height=val.toFloat();
}
/** Carga el W*/
void In2AISVisionObject::setW(QString val){
    m_strWidth=val;
    m_width=val.toFloat();
}

