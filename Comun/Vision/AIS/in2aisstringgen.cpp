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
#include "in2aisstringgen.h"
#include <QStringList>
#include "MemoryMap.h"
#include "in2configuratorvision.h"
#include "in2defines.h"

/*#define OFFSETX 400
#define OFFSETY 0*/
In2AISStringGen::In2AISStringGen(QObject *parent) :
    QObject(parent)
{
    m_totalOCR=0;
    m_totalTemplate1=0;
    m_totalTemplate2=0;    
}


void In2AISStringGen::setItems(QVector<In2AISVisionObject> &lst){
    m_lista=lst;
}

void In2AISStringGen::_getTypes(){
    int count=m_lista.count();
    m_totalOCR=0;
    m_totalTemplate1=0;
    m_totalTemplate2=0;
    for (int x=0;x<count;x++){
        In2AISVisionObject p=m_lista.at(x);
        if (p.getType()==AIS_OCR) m_totalOCR++;
        else if (p.getType()==AIS_IMG1) m_totalTemplate1++;
        else if (p.getType()==AIS_IMG2) m_totalTemplate2++;
    }
}
/** String de guardar fichero*/
QString In2AISStringGen::getStringSaveFile(QString file){
    QString txt=QString("&##%1").arg(file);
    return txt;
}
/** String de cargar fichero*/
QString In2AISStringGen::getStringLoadFile(QString file){
    QString txt=QString("@##%1").arg(file);
    return txt;
}
/** String de cargar fichero*/
QString In2AISStringGen::getStringConfig(QString file){
    QString txt=QString("$");
    return txt;
}

/** String de cargar fichero*/
QString In2AISStringGen::getStringStop(){
    QString txt=QString("@##STOP");
    return txt;
}

/** Devuelve el template general*/
In2AISVisionObject In2AISStringGen::getTemplateGeneral(QString txt){
    In2AISVisionObject obj;
    QStringList lst=txt.split("##");
    if (lst.count()>=3){
        QString foo=lst.at(0);
        QStringList items=foo.split(" ");
        if (items.count()==5){
            //Quitamos el % inicial
            QString tmp=items.at(0).mid(1,items.at(0).count()-1);
            obj.setX(tmp);
            obj.setY(items.at(1));
            obj.setAngle(items.at(2));
            obj.setW(items.at(3));
            obj.setH(items.at(4));
            obj.setType(AIS_IMG1);
        }
    }
    return obj;

}
/** Devuelve el template de cabezal*/
In2AISVisionObject In2AISStringGen::getTemplateCabezal(QString txt){
    In2AISVisionObject obj;
    QStringList lst=txt.split("##");
    if (lst.count()>=3){
        QString foo=lst.at(1);
        QStringList items=foo.split(" ");
        if (items.count()==5){
            obj.setX(items.at(0));
            obj.setY(items.at(1));
            obj.setAngle(items.at(2));
            obj.setW(items.at(3));
            obj.setH(items.at(4));
            obj.setType(AIS_IMG2);
        }
    }
    return obj;

}
/** Devuelve la lista de OCRS*/
QVector<In2AISVisionObject> In2AISStringGen::getOCRList(QString txt){
    bool valid=false;
    QVector<In2AISVisionObject> vector;
    QStringList lst=txt.split("##");
    if (lst.count()>=3){
        //int x=lst.at(1).toInt();
        //vector.resize(x);
        QString ocrs=lst.at(2);
        QStringList items=ocrs.split(";");
        //if (x==items.count()){
        valid=false;
        for (int index=0;index<items.count();index++){
                valid=false;
                QString item=items.at(index);
                QStringList params=item.split(",");
                if (params.count()==2){
                    In2AISVisionObject obj;
                    obj.setString(params.at(1));
                    QStringList values=params.at(0).split(" ");
                    if (values.count()==5){
                        obj.setX(values.at(0));
                        obj.setY(values.at(1));
                        obj.setAngle(values.at(2));
                        obj.setW(values.at(3));
                        obj.setH(values.at(4));
                        valid=true;
                    }
                    if (valid){
                        obj.setType(AIS_OCR);
                        vector.append(obj);
                    }
                }
                else if (params.count()==1){
                    In2AISVisionObject obj;
                    obj.setString("");
                    QStringList values=params.at(0).split(" ");
                    if (values.count()==5){
                        obj.setX(values.at(0));
                        obj.setY(values.at(1));
                        obj.setAngle(values.at(2));
                        obj.setW(values.at(3));
                        obj.setH(values.at(4));
                        valid=true;
                    }
                    if (valid){
                        obj.setType(AIS_OCR);
                        vector.append(obj);
                    }
                }
            }
        //}
    }
    return vector;

}
/** Devuelve el tempplate de cabezal*/
/*QVector<In2AISVisionObject> In2AISStringGen::getTemplateCabezalObjects(QString txt){

}*/


/** Devuelve el string*/
QString In2AISStringGen::getString(QVector<In2AISVisionObject> &lst){
    setItems(lst);
    _getTypes();
    //Cabecera

/*    QString foo=QString("%##%1##%2##")
                .arg(m_totalOCR)
                .arg(m_totalTemplate1+m_totalTemplate2);*/
    QString foo;
    QString strOCR;
    QString strTemplate1;


    strOCR.clear();
    strTemplate1.clear();

    for (int x=0;x<m_lista.count();x++){
        In2AISVisionObject p=m_lista.at(x);
        QString tmp;
        QString width=p.getW();
        QString height=p.getH();
        QString x0=p.getX();
        QString y0=p.getY();
        QString angle=p.getAngle();
        //if ((x0+width)>4096) width=4096-x0;
        if ((p.getType()==AIS_IMG1)){
            tmp=QString("%1,%2 %3 %4 %5##")
                    .arg(QString("%1").arg(x0))
                    .arg(QString("%1").arg(y0))
                    .arg(QString("%1").arg(angle))
                    .arg(QString("%1").arg(width))
                    .arg(QString("%1").arg(height));

           strTemplate1.append(tmp);
        }
        else if ((p.getType()==AIS_IMG2)){
            tmp=QString("%1,%2 %3 %4 %5##")
                    .arg(QString("%1").arg(x0))
                    .arg(QString("%1").arg(y0))
                    .arg(QString("%1").arg(angle))
                    .arg(QString("%1").arg(width))
                    .arg(QString("%1").arg(height));

           strTemplate1.append(tmp);
        }
        else if (p.getType()==AIS_OCR){
            tmp=QString("%1 %2 %3 %4 %5,%6;")
                    .arg(QString("%1").arg(x0))
                    .arg(QString("%1").arg(y0))
                    .arg(QString("%1").arg(angle))
                    .arg(QString("%1").arg(width))
                    .arg(QString("%1").arg(height))
                    .arg(QString("%1").arg(p.getString()));
            if (p.getString().isEmpty()) tmp.chop(1); //Quitamos la coma si no hay nada que buscar
            strOCR.append(tmp);
        }
    }

    if (!strOCR.isNull()) strOCR.chop(1);
    //if (!strTemplate1.isNull()) strTemplate1.chop(1);

    foo.append(QString("%%1%2").arg(strTemplate1).arg(strOCR));
    return foo;

}

/** Genera un nuevo string con los nuevos valores OCR*/
QString In2AISStringGen::completeOCRString(QString txt) {

    In2ConfiguratorVision config;
    config.readConfig();

    float cx=config.getCX();
    float cy=config.getCY();

    if (cx==0) cx=1;
    if (cy==0) cy=1;

    MemoryMap *memoryMap=MemoryMap::request(0);
    QVector<In2AISVisionObject> ocrs=getOCRList(txt);
    In2AISVisionObject templateGeneral=getTemplateGeneral(txt);
    In2AISVisionObject templateCabezal=getTemplateCabezal(txt);
    float offsetX=-templateGeneral.getX().toFloat();
    float offsetY=-templateGeneral.getY().toFloat();
    for (int x=0;x<ocrs.count();x++){
        In2AISVisionObject ocr=ocrs.at(x);
        float xPos=((ocr.getX().toFloat()+offsetX)/cx)/FACTOR_PIXELS_MM;
        float yPos=((ocr.getY().toFloat()+offsetY)/cy)/FACTOR_PIXELS_MM;
        int pos=memoryMap->getClosestFixedField(xPos,yPos);
        QString val=QString();
        if (pos!=-1){
            val=(memoryMap->m_mapa.m_campos[pos].OnOffCampo)?memoryMap->m_mapa.m_campos[pos].ValorActualCampo:QString();
            ocr.setString(val);
            ocrs.replace(x,ocr);
        }
    }
    QVector<In2AISVisionObject> newList;
    newList.append(templateGeneral);
    newList.append(templateCabezal);
    newList+=ocrs;
    QString rtn=getString(newList);
    return rtn;

}




