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
#include "in2aisconfigurator.h"
#include <QSettings>
#include <QFile>
#include "in2aisstringgen.h"


#define VISION_TAG      "VISION_AIS"
#define VISION_VENDOR   "VENDOR"
#define FIELD           "FIELD"
#define TYPE            "TYPE"
#define N               "n"
#define X               "X"
#define Y               "Y"
#define W               "W"
#define H               "H"
#define STRING          "STR"
#define NAME            "NAME"
#define IMAGE           "IMAGE"


In2AISConfigurator::In2AISConfigurator(QObject *parent) :
    QObject(parent)
{
    m_count=0;
}


/** Abrimos el fichero ini*/
bool In2AISConfigurator::readConfig(QString file){
    bool rtn=false;
    if (QFile::exists(file)){
        rtn=true;
        QSettings fichIni(file,QSettings::IniFormat,0);
        fichIni.beginGroup(VISION_TAG);
        m_count=fichIni.value(N,0).toInt();
        for (int z=0;z<m_count;z++){
            //QString foo=QString("%1%2").arg(FIELD).arg(x);
                //fichIni.beginGroup(foo);
                QString foo=QString("%1%2").arg(X).arg(z);
                QString x=fichIni.value(foo,"0").toString();
                foo=QString("%1%2").arg(Y).arg(z);
                QString y=fichIni.value(foo,"0").toString();
                foo=QString("%1%2").arg(W).arg(z);
                QString h=fichIni.value(foo,"0").toString();
                foo=QString("%1%2").arg(H).arg(z);
                QString w=fichIni.value(foo,"0").toString();
                foo=QString("%1%2").arg(TYPE).arg(z);
                int type=fichIni.value(foo,0).toInt();
                foo=QString("%1%2").arg(STRING).arg(z);
                QString str=fichIni.value(foo,"").toString();
                foo=QString("%1%2").arg(NAME).arg(z);
                QString name=fichIni.value(foo,"").toString();
                In2AISVisionObject p;
                p.setData(type,x,y,w,h,str,name);
                m_lista.append(p);
            }
        fichIni.endGroup();                               
    }
    return rtn;

}

/** Devuelve la lista de objetos AIS*/
QVector<In2AISVisionObject> In2AISConfigurator::getAISVisionObjects(){
    return m_lista;
}
/** Guarda la lista de objetos AIS*/
bool In2AISConfigurator::saveConfig(QString file,QVector<In2AISVisionObject> & lista){
    bool rtn=false;
    m_lista=lista;
    if (QFile::exists(file)){
        rtn=true;
        QSettings fichIni(file,QSettings::IniFormat,0);
        fichIni.beginGroup(VISION_TAG);
        QVector<In2AISVisionObject> foo=lista;
        int count=foo.count();
        fichIni.setValue(N,count);
        for (int x=0;x<count;x++){
            In2AISVisionObject p=lista.at(x);
            QString foo=QString("%1%2").arg(X).arg(x);
            fichIni.setValue(foo,QString("%1").arg(p.getX()));
            foo=QString("%1%2").arg(Y).arg(x);
            fichIni.setValue(foo,QString("%1").arg(p.getY()));
            foo=QString("%1%2").arg(H).arg(x);
            fichIni.setValue(foo,QString("%1").arg(p.getH()));
            foo=QString("%1%2").arg(W).arg(x);
            fichIni.setValue(foo,QString("%1").arg(p.getW()));
            foo=QString("%1%2").arg(STRING).arg(x);
            fichIni.setValue(foo,p.getString());
            foo=QString("%1%2").arg(NAME).arg(x);
            fichIni.setValue(foo,p.getName());
            foo=QString("%1%2").arg(TYPE).arg(x);
            fichIni.setValue(foo,QString("%1").arg(p.getType()));
        }
        fichIni.endGroup();
        fichIni.sync();
    }
    return rtn;
}


/** Devuelve el string de AIS*/
QString In2AISConfigurator::getStringAIS(){
    In2AISStringGen p;
    return p.getString(m_lista);
}

/** Devuelve el string de AIS de una lista*/
QString In2AISConfigurator::getStringAISFromList(QVector<In2AISVisionObject> & lista){
    In2AISStringGen p;
    QVector<In2AISVisionObject>  foo=lista;
    return p.getString(foo);
}

/** Devuelve el string de load*/
QString In2AISConfigurator::getStringLoad(QString txt){
    In2AISStringGen p;
    return p.getStringLoadFile(txt);
}
/** Devuelve el string de load*/
QString In2AISConfigurator::getStringConfig(QString txt){
    In2AISStringGen p;
    return p.getStringConfig(txt);
}

/** Devuelve el string de save*/
QString In2AISConfigurator::getStringSave(QString txt){
    In2AISStringGen p;
    return p.getStringSaveFile(txt);
}
/** Devuelve el string de stop*/
QString In2AISConfigurator::getStringStop(){
    In2AISStringGen p;
    return p.getStringStop();
}



