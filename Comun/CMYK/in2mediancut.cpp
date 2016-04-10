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
#include "in2mediancut.h"
#include <QVector>
#include <QColor>
#include <QMap>
#include <QDebug>
//#include <QtMath>
#include <QtCore>

In2MedianCut::In2MedianCut()
{
    m_bCoorDinamica=false;
}
/** Algoritmo de median cut*/
QVector<QRgb> In2MedianCut::medianCut(QVector<QRgb> palette,int colors,bool corr){

    //return sortPaletteIn2(palette);
    //Vamos a suponer que de momento el algoritmo es para 4 colores
    QVector<QRgb> c1;
    QVector<QRgb> c2;
    QVector<QRgb> c3;
    QVector<QRgb> c4;

    struct strMCPal strct;

    m_bCoorDinamica=corr;

    strct=sortPalette(palette);
    QVector<QRgb> foo=strct.pal;
    int index=strct.index;
    //En rtn esta la paleta ordenada
    c1=foo.mid(0,index);
    c2=foo.mid(index-1,foo.count()-index);

    strct=sortPalette(c1);
    foo=strct.pal;
    index=strct.index;
    c3=foo.mid(0,index);
    c4=foo.mid(index-1,foo.count()-index);


    strct=sortPalette(c2);
    foo=strct.pal;
    index=strct.index;
    c1=foo.mid(0,index);
    c2=foo.mid(index-1,foo.count()-index);



    //Hacemos las medias

    QVector<QRgb> rtn;
    unsigned long r=0;
    unsigned long g=0;
    unsigned long b=0;
    for (int x=0;x<c1.count();x++){
        QColor color(c1.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();
        //foo1+=c1.at(x);
    }
    r=r/c1.count();
    g=g/c1.count();
    b=b/c1.count();
    QColor color=QColor(r,g,b);
    rtn.append(color.rgb());
    r=0;
    g=0;
    b=0;
    for (int x=0;x<c2.count();x++){
        QColor color(c2.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();
        //foo1+=c2.at(x);
    }
    r=r/c2.count();
    g=g/c2.count();
    b=b/c2.count();
    color=QColor(r,g,b);
    rtn.append(color.rgb());

    r=0;
    g=0;
    b=0;
    for (int x=0;x<c3.count();x++){
        QColor color(c3.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();

        //foo1+=c3.at(x);
    }
    r=r/c3.count();
    g=g/c3.count();
    b=b/c3.count();
    color=QColor(r,g,b);
    rtn.append(color.rgb());

    r=0;
    g=0;
    b=0;
    for (int x=0;x<c4.count();x++){
        QColor color(c4.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();
        //foo1+=c4.at(x);
    }
    r=r/c4.count();
    g=g/c4.count();
    b=b/c4.count();
    color=QColor(r,g,b);
    rtn.append(color.rgb());




    //std::sort(rtn.begin(), rtn.end(), std::greater<int>());
    qSort(rtn);

    return rtn;
}

/** Ordena una subpaleta*/
/*struct strMCPal In2MedianCut::sortPalette(QVector<QRgb> palette){
    //Hay que ver que canal es el predominante en rango
    int colors=palette.count();
    QMap<int,int> redMap;
    QMap<int,int> greenMap;
    QMap<int,int> blueMap;


    unsigned int medianR=0;
    unsigned int medianG=0;
    unsigned int medianB=0;

    QRgb c=palette.at(0);
    int r = (c >> 16) & 0xFF;
    int g = (c >> 8) & 0xFF;
    int b = c & 0xFF;

    int rangeR=0;
    int rangeG=0;
    int rangeB=0;

    int r1=r;
    int g1=g;
    int b1=b;

    for (int x=0;x<colors;x++){
        c=palette.at(x);
        int r = (c >> 16) & 0xFF;
        int g = (c >> 8) & 0xFF;
        int b = c & 0xFF;
        redMap.insertMulti(r,x);
        greenMap.insertMulti(g,x);
        blueMap.insertMulti(b,x);

        //Rangos
        int tmpRangeR=qAbs(r-r1);
        int tmpRangeG=qAbs(g-g1);
        int tmpRangeB=qAbs(b-b1);
        if (tmpRangeR>rangeR) rangeR=tmpRangeR;
        if (tmpRangeG>rangeG) rangeG=tmpRangeG;
        if (tmpRangeB>rangeB) rangeB=tmpRangeB;
        //medias
        medianR+=r;
        medianG+=g;
        medianB+=b;
    }

    medianR=medianR/colors;
    medianG=medianG/colors;
    medianB=medianB/colors;


    QList<int> order;
    int index=0;    
    if ((rangeR>=rangeG)&&(rangeR>=rangeB)) {
        order=redMap.values();
        QMap<int,int>::iterator foo=redMap.lowerBound(medianR);
        index=order.count()-order.indexOf(foo.value());
        if (index<=order.count()/2){
            index=index/2;
        }
        else{
            int remain=order.count()-index;
            index=index+remain/2;
        }


    }
    else if ((rangeG>=rangeR)&&(rangeG>=rangeB)) {
        order=greenMap.values();
        QMap<int,int>::iterator foo=greenMap.lowerBound(medianG);
        index=order.count()-order.indexOf(foo.value());
        if (index<=order.count()/2){
            index=index/2;
        }
        else{
            int remain=order.count()-index;
            index=index+remain/2;
        }


    }
    else if ((rangeB>=rangeR)&&(rangeB>=rangeG)) {
        order=blueMap.values();
        QMap<int,int>::iterator foo=blueMap.lowerBound(medianB);
        index=order.count()-order.indexOf(foo.value());

        if (index<=order.count()/2){
            index=index/2;
        }
        else{
            int remain=order.count()-index;
            index=index+remain/2;
        }

    }


    QVector<QRgb> rtn;
    int outColors=order.count();
    //esta ordenado de menor a mayor...recorremos la tabla al reves
    for(int x=0;x<outColors;x++){
        rtn.append(palette.at(order.at(outColors-1-x)));
    }

    struct strMCPal strct;
    strct.pal=rtn;
    strct.index=index;

    return strct;
}*/

/** Ordena una subpaleta*/
struct strMCPal In2MedianCut::sortPalette(QVector<QRgb> palette){
    //Hay que ver que canal es el predominante en rango
    int colors=palette.count();
    unsigned int redMap[256];
    unsigned int greenMap[256];
    unsigned int blueMap[256];
    
    memset(&redMap[0],0x00,sizeof(redMap));
    memset(&greenMap[0],0x00,sizeof(greenMap));
    memset(&blueMap[0],0x00,sizeof(blueMap));

    quint64 medianR=0;
    quint64 medianG=0;
    quint64 medianB=0;


    QVector< QVector<QRgb> > redIndex(256);
    QVector< QVector<QRgb> > greenIndex(256);
    QVector< QVector<QRgb> > blueIndex(256);

    for (int x=0;x<colors;x++){
        QRgb c=palette.at(x);
        int r = (c >> 16) & 0xFF;
        int g = (c >> 8) & 0xFF;
        int b = c & 0xFF;
        redIndex[r].append(c);
        greenIndex[g].append(c);
        blueIndex[b].append(c);
    }
    
    int rMin=0xFF;
    int rMax=0;
    int gMin=0xFF;
    int gMax=0;
    int bMin=0xFF;
    int bMax=0;

    for (int x=0;x<256;x++){
        if (redMap[x]){
            medianR+=redMap[x]*x;

        }
        if (greenMap[x]){
            medianG+=greenMap[x]*x;
        }
        if (blueMap[x]){
            medianB+=blueMap[x]*x;
        }       
    }

    medianR=medianR/colors;
    medianG=medianG/colors;
    medianB=medianB/colors;

    //Hacemos calculos sobre los histogramas
    quint64 rangeR=(rMax-rMin+1)*medianR;
    quint64 rangeG=(gMax-gMin+1)*medianG;
    quint64 rangeB=(bMax-bMin+1)*medianB;

    QVector<QRgb> rtn;
    unsigned int index=0;
    unsigned long foo=0;
    QVector<unsigned int> vector;
    if ((rangeR>=rangeG)&&(rangeR>=rangeB)) {
        for (int x=255;x>=0;x--){
            vector.append(redMap[x]);
            QVector<QRgb> v=redIndex.at(x);
            rtn+=v;
            if (x==medianR) index=rtn.count();
            //if ((rtn.count()>colors/2)&&(index==0)) index=rtn.count();
        }
    }
    else if ((rangeG>=rangeR)&&(rangeG>=rangeB)) {
        for (int x=255;x>=0;x--){
            vector.append(greenMap[x]);
            QVector<QRgb> v=greenIndex.at(x);
            rtn+=v;
            if (x==medianG) index=rtn.count();
            //if ((rtn.count()>colors/2)&&(index==0)) index=rtn.count();
        }
    }
    else if ((rangeB>=rangeR)&&(rangeB>=rangeG)) {
        for (int x=255;x>=0;x--){
            vector.append(blueMap[x]);
            QVector<QRgb> v=blueIndex[x];
            rtn+=v;
            if (x==medianB) index=rtn.count();
            //if ((rtn.count()>colors/2)&&(index==0)) index=rtn.count();
        }
    }
    //Si tenemos los pixels poco distribuidos (4 colores tiene mas de la mitad), modififcamos la posicion de la mediana
    qSort(vector);

    if (m_bCoorDinamica) {
        for (int x=0;x<8;x++)
            foo+=vector[255-x];
        qDebug()<<QString("Colores granulados (%1\\%2)").arg(foo).arg(colors);
        if (foo>colors/2){
            qDebug()<<"Correccion medianCut";
            if (index<rtn.count()/2){
                index=index/2;
            }
            else{
                int remain=rtn.count()-index;
                index=index+remain/2;
            }
        }
    }

    if ((index==0)||(index==rtn.count())) index=rtn.count()/2;

    struct strMCPal strct;
    strct.pal=rtn;
    strct.index=index;

    return strct;
}


/** Ordena una subpaleta*/
/*QVector<QRgb> In2MedianCut::sortPaletteIn2(QVector<QRgb> palette){
    //Hay que ver que canal es el predominante en rango
    int colors=palette.count();
    unsigned int redMap[256];
    unsigned int greenMap[256];
    unsigned int blueMap[256];

    memset(&redMap[0],0x00,sizeof(redMap));
    memset(&greenMap[0],0x00,sizeof(greenMap));
    memset(&blueMap[0],0x00,sizeof(blueMap));

    quint64 medianR=0;
    quint64 medianG=0;
    quint64 medianB=0;


    QVector< QVector<QRgb> > redIndex(256);
    QVector< QVector<QRgb> > greenIndex(256);
    QVector< QVector<QRgb> > blueIndex(256);

    for (int x=0;x<colors;x++){
        QRgb c=palette.at(x);
        int r = (c >> 16) & 0xFF;
        int g = (c >> 8) & 0xFF;
        int b = c & 0xFF;
        redMap[r]++;
        greenMap[g]++;
        blueMap[b]++;
        redIndex[r].append(c);
        greenIndex[g].append(c);
        blueIndex[b].append(c);
    }

    int rMin=0xFF;
    int rMax=0;
    int gMin=0xFF;
    int gMax=0;
    int bMin=0xFF;
    int bMax=0;

    for (int x=0;x<256;x++){
        if (redMap[x]){
            if (x>rMax) rMax=x;
            if (x<rMin) rMin=x;
            medianR+=redMap[x]*x;

        }
        if (greenMap[x]){
            if (x>gMax) gMax=x;
            if (x<gMin) gMin=x;
            medianG+=greenMap[x]*x;
        }
        if (blueMap[x]){
            if (x>bMax) bMax=x;
            if (x<bMin) bMin=x;
            medianB+=blueMap[x]*x;
        }
    }


    //Hacemos calculos sobre los histogramas
    quint64 rangeR=(rMax-rMin);
    quint64 rangeG=(gMax-gMin);
    quint64 rangeB=(bMax-bMin);

    QVector<QRgb> c1;
    QVector<QRgb> c2;
    QVector<QRgb> c3;
    QVector<QRgb> c4;
    quint64 count=0;

    if ((rangeR>=rangeG)&&(rangeR>=rangeB)) {
        for (int x=0;x<256;x++){
            int limit=redMap[x];
            QVector<QRgb> v=redIndex.at(x);
            for (int y=0;y<limit;y++){
                count++;
                if ((count>=0)&&(count<colors/4)&&(x<32)){
                    c1.append(v.at(y));
                }
                else if ((count>=colors/4)&&(count<colors/2)||((x>32)&&(x<125))){
                    c2.append(v.at(y));
                }
                else if ((count>=colors/2)&&(count<colors*3/4)||((x>125)&&(x<225))){
                    c3.append(v.at(y));
                }
                else{
                    c4.append(v.at(y));
                }
            }
        }
    }
    else if ((rangeG>=rangeR)&&(rangeG>=rangeB)) {
        for (int x=255;x>=0;x--){
            count+=greenMap[x]*x;
            QVector<QRgb> v=greenIndex.at(x);
            if ((count>=0)&&(count<medianG/4)){
                c1+=v;
            }
            else if ((count>=medianG/4)&&(count<medianG/2)){
                c2+=v;
            }
            else if ((count>=medianG/2)&&(count<medianG*3/4)){
                c3+=v;
            }
            else{
                c4+=v;
            }
        }
    }
    else if ((rangeB>=rangeR)&&(rangeB>=rangeG)) {
        for (int x=255;x>=0;x--){
            count+=blueMap[x]*x;
            QVector<QRgb> v=blueIndex.at(x);
            if ((count>=0)&&(count<medianB/4)){
                c1+=v;
            }
            else if ((count>=medianB/4)&&(count<medianB/2)){
                c2+=v;
            }
            else if ((count>=medianB/2)&&(count<medianB*3/4)){
                c3+=v;
            }
            else{
                c4+=v;
            }
        }
    }

    QVector<QRgb> rtnPal;
    quint64 r=0;
    quint64 g=0;
    quint64 b=0;
    for (int x=0;x<c1.count();x++){
        QColor color(c1.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();
        //foo1+=c1.at(x);
    }
    r=r/c1.count();
    g=g/c1.count();
    b=b/c1.count();
    QColor color=QColor(r,g,b);
    rtnPal.append(color.rgb());
    r=0;
    g=0;
    b=0;
    for (int x=0;x<c2.count();x++){
        QColor color(c2.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();
        //foo1+=c2.at(x);
    }
    r=r/c2.count();
    g=g/c2.count();
    b=b/c2.count();
    color=QColor(r,g,b);
    rtnPal.append(color.rgb());

    r=0;
    g=0;
    b=0;
    for (int x=0;x<c3.count();x++){
        QColor color(c3.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();

        //foo1+=c3.at(x);
    }
    r=r/c3.count();
    g=g/c3.count();
    b=b/c3.count();
    color=QColor(r,g,b);
    rtnPal.append(color.rgb());

    r=0;
    g=0;
    b=0;
    for (int x=0;x<c4.count();x++){
        QColor color(c4.at(x));
        r+=color.red();
        g+=color.green();
        b+=color.blue();
        //foo1+=c4.at(x);
    }
    r=r/c4.count();
    g=g/c4.count();
    b=b/c4.count();
    color=QColor(r,g,b);
    rtnPal.append(color.rgb());




    //std::sort(rtn.begin(), rtn.end(), std::greater<int>());
    qSort(rtnPal);
    unsigned long foo=0;
    if (m_bCoorDinamica) {
        for (int x=0;x<8;x++)
            foo+=vector[255-x];
        qDebug()<<QString("Colores granulados (%1\\%2)").arg(foo).arg(colors);
        if (foo>colors/2){
            qDebug()<<"Correccion medianCut";
            if (index<rtn.count()/2){
                index=index/2;
            }
            else{
                int remain=rtn.count()-index;
                index=index+remain/2;
            }
        }
    }

    if ((index==0)||(index==rtn.count())) index=rtn.count()/2;


    return rtnPal;
}*/




