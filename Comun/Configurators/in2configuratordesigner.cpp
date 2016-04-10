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
#include "in2configuratordesigner.h"
#include <QFile>
#include <QSettings>
#include <QApplication>

#define FILE "In2Designer.ini"
#define IN2_DESIGNER "DESIGNER"
#define SAVE_CAMPOS "SaveCampos"

#define COLOR1 "Color1"
#define COLOR2 "Color2"
#define COLOR3 "Color3"
#define COLOR4 "Color4"
#define NUMCOLORS "NumColors"

#define DEFCOLOR1   0x000000
#define DEFCOLOR2   0x00FFFF
#define DEFCOLOR3   0xFF00FF
#define DEFCOLOR4   0xFFFF00


In2ConfiguratorDesigner::In2ConfiguratorDesigner()
{
    m_bSaveCampos=false;
    QString file=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(FILE);
    if (QFile::exists(file)){
        QSettings fichIni(file,QSettings::IniFormat,0);
        fichIni.beginGroup(IN2_DESIGNER);
        m_bSaveCampos=fichIni.value(SAVE_CAMPOS,false).toBool();
        m_color1=fichIni.value(COLOR1,DEFCOLOR1).toInt();
        m_color2=fichIni.value(COLOR2,0x00FFFF).toInt();
        m_color3=fichIni.value(COLOR3,0xFF00FF).toInt();
        m_color4=fichIni.value(COLOR4,DEFCOLOR4).toInt();
        m_numColors=fichIni.value(NUMCOLORS,1).toInt();
        fichIni.endGroup();
    }
}

/** Devuelve el numero de colores*/
int In2ConfiguratorDesigner::getNumColors(){
    return m_numColors;
}

/** Devuelve el color 1*/
unsigned int In2ConfiguratorDesigner::getColor1(){
    return m_color1;
}
/** Devuelve el color 2*/
unsigned int In2ConfiguratorDesigner::getColor2(){
    return m_color2;
}
/** Devuelve el color 3*/
unsigned int In2ConfiguratorDesigner::getColor3(){
    return m_color3;
}
/** Devuelve el color 4*/
unsigned int In2ConfiguratorDesigner::getColor4(){
    return m_color4;
}


/** Guarda el designer*/
void In2ConfiguratorDesigner::saveConfig(){
    QString file=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(FILE);
    if (QFile::exists(file)){
        QSettings fichIni(file,QSettings::IniFormat,0);
        fichIni.beginGroup(IN2_DESIGNER);
        fichIni.setValue(SAVE_CAMPOS,m_bSaveCampos);
        fichIni.endGroup();
        fichIni.sync();
    }
}

In2ConfiguratorDesigner::~In2ConfiguratorDesigner()
{

}
/** Si se salvan los campos o no*/
bool In2ConfiguratorDesigner::saveCampos(){
    return m_bSaveCampos;
}
/** Si se salvan campos o no*/
void In2ConfiguratorDesigner::setSaveCampos(bool val){
    m_bSaveCampos=val;
}

