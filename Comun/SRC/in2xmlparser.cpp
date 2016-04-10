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
#include "in2xmlparser.h"

In2XmlParser::In2XmlParser(QObject *parent) :
    QObject(parent)
{
    m_enabled=false;
}
/** Parsea el xml*/
In2XmlParser::In2XmlParser(QObject *parent,QString file) :
    QObject(parent)
{
    m_enabled=false;
    QFile qfileConfig(file);
    if (qfileConfig.exists()){
        qfileConfig.open(QIODevice::ReadWrite);
        m_qdomDocument.setContent(&qfileConfig);
        m_enabled=true;
        qfileConfig.close();
    }
}

/** Escribe el contenido de una key cuay ruta es el QStringList*/
bool In2XmlParser::setValue(QStringList PathKey, QString attribute, QString value){
    bool rtn=false;
    QDomElement prev=m_qdomDocument.firstChildElement(PathKey.at(0));
    QDomElement next=prev;
    if ((!prev.isNull())&&(PathKey.count()>1)){
        for (int x=1;x<PathKey.count();x++){
            next=prev.firstChildElement(PathKey.at(x));
            prev=next;
            if (prev.isNull()) break;
        }
    }
    if (!next.isNull()){
        next.setAttribute(attribute,value);
        rtn=true;
    }
    return rtn;
}

/** Devuelve el contenido de una key*/
QString In2XmlParser::getValue(QStringList PathKey, QString attribute){
    QString rtn="";
    QDomElement prev=m_qdomDocument.firstChildElement(PathKey.at(0));
    QDomElement next=prev;
    if ((!prev.isNull())&&(PathKey.count()>1)){
        for (int x=1;x<PathKey.count();x++){
            next=prev.firstChildElement(PathKey.at(x));
            prev=next;
            if (prev.isNull()) break;
        }
    }
    if (!next.isNull()){
        rtn=next.attribute(attribute,"");
    }
    return rtn;
}



/** Escribe el contenido en el fichero indcado*/
bool In2XmlParser::saveContentsToFile(QString fileName){
    bool rtn=false;
    QString xml=m_qdomDocument.toString();
    QFile qfileConfig(fileName);
    if (qfileConfig.open(QIODevice::WriteOnly)){
        quint64 data=qfileConfig.write(xml.toUtf8().data(),xml.count());
        if (xml.count()==data) rtn=true;
        qfileConfig.close();
    }
    return rtn;
}




