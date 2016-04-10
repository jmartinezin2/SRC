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
#include "in2floydsteinberg.h"
#include <QMap>
#include <qmath.h>
#include <QElapsedTimer>
#include <qDebug>
#include "in2mediancut.h"
#if QT_VERSION>=0x050000
    #include <QtConcurrent>
#endif
#include <QtCore>

static const short floyd_steinberg_error1[] = {
-112,-111,-111,-110,-110,-109,-109,-109,-108,-108,-107,-107,-106,-106,-105,-105,-105,-104,-104,-103,-103,-102,-102,-102,-101,-101,-100,-100,-99,-99,-98,-98,-98,-97,-97,-96,-96,-95,-95,-95,-94,-94,-93,-93,-92,-92,-91,-91,-91,-90,-90,-89,-89,-88,-88,-88,-87,-87,-86,-86,-85,-85,-84,-84,-84,-83,-83,-82,-82,-81,-81,-81,-80,-80,-79,-79,-78,-78,-77,-77,-77,-76,-76,-75,-75,-74,-74,-74,-73,-73,-72,-72,-71,-71,-70,-70,-70,-69,-69,-68,-68,-67,-67,-67,-66,-66,-65,-65,-64,-64,-63,-63,-63,-62,-62,-61,-61,-60,-60,-60,-59,-59,-58,-58,-57,-57,-56,-56,-56,-55,-55,-54,-54,-53,-53,-53,-52,-52,-51,-51,-50,-50,-49,-49,-49,-48,-48,-47,-47,-46,-46,-46,-45,-45,-44,-44,-43,-43,-42,-42,-42,-41,-41,-40,-40,-39,-39,-39,-38,-38,-37,-37,-36,-36,-35,-35,-35,-34,-34,-33,-33,-32,-32,-32,-31,-31,-30,-30,-29,-29,-28,-28,-28,-27,-27,-26,-26,-25,-25,-25,-24,-24,-23,-23,-22,-22,-21,-21,-21,-20,-20,-19,-19,-18,-18,-18,-17,-17,-16,-16,-15,-15,-14,-14,-14,-13,-13,-12,-12,-11,-11,-11,-10,-10,-9,-9,-8,-8,-7,-7,-7,-6,-6,-5,-5,-4,-4,-4,-3,-3,-2,-2,-1,-1,0,0,1,1,2,2,3,3,4,4,4,5,5,6,6,7,7,7,8,8,9,9,10,10,11,11,11,12,12,13,13,14,14,14,15,15,16,16,17,17,18,18,18,19,19,20,20,21,21,21,22,22,23,23,24,24,25,25,25,26,26,27,27,28,28,28,29,29,30,30,31,31,32,32,32,33,33,34,34,35,35,35,36,36,37,37,38,38,39,39,39,40,40,41,41,42,42,42,43,43,44,44,45,45,46,46,46,47,47,48,48,49,49,49,50,50,51,51,52,52,53,53,53,54,54,55,55,56,56,56,57,57,58,58,59,59,60,60,60,61,61,62,62,63,63,63,64,64,65,65,66,66,67,67,67,68,68,69,69,70,70,70,71,71,72,72,73,73,74,74,74,75,75,76,76,77,77,77,78,78,79,79,80,80,81,81,81,82,82,83,83,84,84,84,85,85,86,86,87,87,88,88,88,89,89,90,90,91,91,91,92,92,93,93,94,94,95,95,95,96,96,97,97,98,98,98,99,99,100,100,101,101,102,102,102,103,103,104,104,105,105,105,106,106,107,107,
};

static const short floyd_steinberg_error2[] = {
-48,-48,-47,-47,-47,-47,-47,-47,-46,-46,-46,-46,-46,-45,-45,-45,-45,-45,-44,-44,-44,-44,-44,-44,-43,-43,-43,-43,-43,-42,-42,-42,-42,-42,-41,-41,-41,-41,-41,-41,-40,-40,-40,-40,-40,-39,-39,-39,-39,-39,-38,-38,-38,-38,-38,-38,-37,-37,-37,-37,-37,-36,-36,-36,-36,-36,-35,-35,-35,-35,-35,-35,-34,-34,-34,-34,-34,-33,-33,-33,-33,-33,-32,-32,-32,-32,-32,-32,-31,-31,-31,-31,-31,-30,-30,-30,-30,-30,-29,-29,-29,-29,-29,-29,-28,-28,-28,-28,-28,-27,-27,-27,-27,-27,-26,-26,-26,-26,-26,-26,-25,-25,-25,-25,-25,-24,-24,-24,-24,-24,-23,-23,-23,-23,-23,-23,-22,-22,-22,-22,-22,-21,-21,-21,-21,-21,-20,-20,-20,-20,-20,-20,-19,-19,-19,-19,-19,-18,-18,-18,-18,-18,-17,-17,-17,-17,-17,-17,-16,-16,-16,-16,-16,-15,-15,-15,-15,-15,-14,-14,-14,-14,-14,-14,-13,-13,-13,-13,-13,-12,-12,-12,-12,-12,-11,-11,-11,-11,-11,-11,-10,-10,-10,-10,-10,-9,-9,-9,-9,-9,-8,-8,-8,-8,-8,-8,-7,-7,-7,-7,-7,-6,-6,-6,-6,-6,-5,-5,-5,-5,-5,-5,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,15,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,18,19,19,19,19,19,20,20,20,20,20,21,21,21,21,21,21,22,22,22,22,22,23,23,23,23,23,24,24,24,24,24,24,25,25,25,25,25,26,26,26,26,26,27,27,27,27,27,27,28,28,28,28,28,29,29,29,29,29,30,30,30,30,30,30,31,31,31,31,31,32,32,32,32,32,33,33,33,33,33,33,34,34,34,34,34,35,35,35,35,35,36,36,36,36,36,36,37,37,37,37,37,38,38,38,38,38,39,39,39,39,39,39,40,40,40,40,40,41,41,41,41,41,42,42,42,42,42,42,43,43,43,43,43,44,44,44,44,44,45,45,45,45,45,45,46,46,46,46
};

static const short floyd_steinberg_error3[] = {
-80,-79,-79,-79,-78,-78,-78,-78,-77,-77,-77,-76,-76,-76,-75,-75,-75,-74,-74,-74,-73,-73,-73,-73,-72,-72,-72,-71,-71,-71,-70,-70,-70,-69,-69,-69,-68,-68,-68,-68,-67,-67,-67,-66,-66,-66,-65,-65,-65,-64,-64,-64,-63,-63,-63,-63,-62,-62,-62,-61,-61,-61,-60,-60,-60,-59,-59,-59,-58,-58,-58,-58,-57,-57,-57,-56,-56,-56,-55,-55,-55,-54,-54,-54,-53,-53,-53,-53,-52,-52,-52,-51,-51,-51,-50,-50,-50,-49,-49,-49,-48,-48,-48,-48,-47,-47,-47,-46,-46,-46,-45,-45,-45,-44,-44,-44,-43,-43,-43,-43,-42,-42,-42,-41,-41,-41,-40,-40,-40,-39,-39,-39,-38,-38,-38,-38,-37,-37,-37,-36,-36,-36,-35,-35,-35,-34,-34,-34,-33,-33,-33,-33,-32,-32,-32,-31,-31,-31,-30,-30,-30,-29,-29,-29,-28,-28,-28,-28,-27,-27,-27,-26,-26,-26,-25,-25,-25,-24,-24,-24,-23,-23,-23,-23,-22,-22,-22,-21,-21,-21,-20,-20,-20,-19,-19,-19,-18,-18,-18,-18,-17,-17,-17,-16,-16,-16,-15,-15,-15,-14,-14,-14,-13,-13,-13,-13,-12,-12,-12,-11,-11,-11,-10,-10,-10,-9,-9,-9,-8,-8,-8,-8,-7,-7,-7,-6,-6,-6,-5,-5,-5,-4,-4,-4,-3,-3,-3,-3,-2,-2,-2,-1,-1,-1,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,15,16,16,16,17,17,17,18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,23,23,23,24,24,24,25,25,25,25,26,26,26,27,27,27,28,28,28,29,29,29,30,30,30,30,31,31,31,32,32,32,33,33,33,34,34,34,35,35,35,35,36,36,36,37,37,37,38,38,38,39,39,39,40,40,40,40,41,41,41,42,42,42,43,43,43,44,44,44,45,45,45,45,46,46,46,47,47,47,48,48,48,49,49,49,50,50,50,50,51,51,51,52,52,52,53,53,53,54,54,54,55,55,55,55,56,56,56,57,57,57,58,58,58,59,59,59,60,60,60,60,61,61,61,62,62,62,63,63,63,64,64,64,65,65,65,65,66,66,66,67,67,67,68,68,68,69,69,69,70,70,70,70,71,71,71,72,72,72,73,73,73,74,74,74,75,75,75,75,76,76,76,77
};

static const short floyd_steinberg_error4[] = {
-16,-16,-16,-16,-16,-16,-16,-16,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-12,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-8,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,16
};


/*#define ERROR_CORR1 9
#define ERROR_CORR2 4
#define ERROR_CORR3 6
#define ERROR_CORR4 1*/

#define ERROR_CORR1 0
#define ERROR_CORR2 0
#define ERROR_CORR3 0
#define ERROR_CORR4 0



/** Template de ejecucion*/
template<class T> inline const T& limits( const T & x, const T& low, const T& high )
{
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}


In2FloydSteinberg::In2FloydSteinberg()
{

}

In2FloydSteinberg::~In2FloydSteinberg()
{

}
/** Aplica Floyd Steimberg*/
QImage In2FloydSteinberg::convert(QImage &img, QVector<QRgb> & table){

}
/** Devuelve la paleta optima*/
QVector<QRgb> In2FloydSteinberg::getOptimumTable(QImage &img,int colors,bool corr){
    int width=img.width();
    int height=img.height();
    QMap<QRgb,int> histogram;
    QVector<QRgb> table;
    for (int x=0;x<width;x++){
        for (int y=0;y<height;y++){
            QRgb val=img.pixel(x,y);
            int key=histogram.value(val,0)+1;
            histogram.insert(val,key);
        }
    }
    //Dividimos tantas zonas como colores y cogemos al mas representativo
    QList<QRgb> keys=histogram.keys();
    QList<int> vals = histogram.values();        

    if (vals.count()<=colors){
        for (int x=0;x<vals.count();x++){
            table.append(keys.at(x));
        }
    }
    else{
        int number=keys.count()/colors;
        int residuo=keys.count()%colors;
        int counter=number+residuo;
        int index=0;
        for (int x=0;x<colors;x++){
            QList<QRgb> subkeys=keys.mid(index,number);
            QList<int> subValues=vals.mid(index,number);
            QMap<QRgb,int> subHistogram;
            for (int i=0;i<subkeys.count();i++){
                subHistogram.insert(subkeys.at(i),subValues.at(i));
            }
            qSort(subValues);            
            int subVal=subValues.last();
            //Cojo la media de cada
            int r=0;
            int g=0;
            int b=0;
            for (int x=0;x<subkeys.count();x++) {
                QColor color=QColor(subkeys.at(x));
                r+=color.red();
                g+=color.green();
                b+=color.blue();
            }
            r=r/subkeys.count();
            g=g/subkeys.count();
            b=b/subkeys.count();

            QRgb key=subHistogram.key(subVal);
            table.append(key);
            /*QColor out(r,g,b);
            table.append(out.rgb());*/
            index=counter;
            counter+=number;
        }
    }
    return table;
}

/** Devuelve la paleta optima*/
QVector<QRgb> In2FloydSteinberg::getOptimumTableExt(QImage &img,int colors){
    int width=img.width();
    int height=img.height();
    QMap<QRgb,int> histogram;
    QVector<QRgb> table;
    for (int x=0;x<width;x++){
        for (int y=0;y<height;y++){
            QRgb val=img.pixel(x,y);
            int key=histogram.value(val,0)+1;
            histogram.insert(val,key);
        }
    }
    //Dividimos tantas zonas como colores y cogemos al mas representativo
    QList<QRgb> keys=histogram.keys();
    QList<int> vals = histogram.values();

    if (vals.count()<=colors){
        for (int x=0;x<vals.count();x++){
            table.append(keys.at(x));
        }
    }
    else{
        /*int number=keys.count()/colors;
        int residuo=keys.count()%colors;
        int counter=number+residuo;*/
        int index=0;
        for (int x=0;x<colors;x++){
            int foo=((xusb_4map[x]-xusb_4map[x+1])*keys.count())/255;
            QList<QRgb> subkeys=keys.mid(index,foo);
            QList<int> subValues=vals.mid(index,foo);
            QMap<QRgb,int> subHistogram;
            for (int i=0;i<subkeys.count();i++){
                subHistogram.insert(subkeys.at(i),subValues.at(i));
            }
            qSort(subValues);
            int subVal=subValues.last();
            //Cojo la media de cada
            int r=0;
            int g=0;
            int b=0;
            for (int x=0;x<subkeys.count();x++) {
                QColor color=QColor(subkeys.at(x));
                r+=color.red();
                g+=color.green();
                b+=color.blue();
            }
            r=r/subkeys.count();
            g=g/subkeys.count();
            b=b/subkeys.count();

            QRgb key=subHistogram.key(subVal);
            table.append(key);
            /*QColor out(r,g,b);
            table.append(out.rgb());*/
            index+=subkeys.count();
            //counter+=number;
        }
    }
    return table;
}


/** Desglosa una liea de la imagen*/
void getPal(Elem &e) {
  for(int i=0;i<e.lineSize;++i){
    QRgb *rgb = (QRgb*)e.scanLine[i];
    e.v.append(*rgb);
  }
}


/** Devuelve la paleta optima segun la median cut*/
QVector<QRgb> In2FloydSteinberg::getOptimumTableMediaCut(QImage &img,int colors,bool corr){
    QVector<QRgb> table;
    if (img.format()==QImage::Format_Indexed8) table=img.colorTable();
    else{
        int width=img.width();
        int height=img.height();
        for (int y=0;y<height;y++){
            QRgb *line=(QRgb*)img.scanLine(y);
            for (int x=0;x<width;x++){
                QRgb val=line[x];
                //if (!table.contains(val)) table.append(val);
                table.append(val);
            }
        }
    }

    In2MedianCut medCut;
    QVector<QRgb> rtn=medCut.medianCut(table,colors,corr);
    return rtn;
}

/** Devuelve la paleta optima segun la median cut*/
QVector<QRgb> In2FloydSteinberg::getOptimumTableIn2(QImage &img,int colors,bool corr){
    QVector<QRgb> table;
    QElapsedTimer time;
    if (img.format()==QImage::Format_Indexed8) table=img.colorTable();
    else{
        int width=img.width();
        int height=img.height();
        QList<Elem> lista;
        //Tenemos que actualizar el tema de los threads
        int threads=QThread::idealThreadCount();
        if (threads<=0) threads=1;
        int chunk=height/threads;
        int remain=height%threads;
        qDebug()<<QString("Threads=%1->chunk=%2->remain=%3").arg(threads).arg(chunk).arg(remain);
        //time.start();
        for (int y=0;y<height;y++){
            Elem e;                        
            e.lineIndex=y;            
            e.lineSize=width;
            lista.append(e);
        }

        QtConcurrent::blockingMap(lista,getPal);
        /*for (int y=0;y<height;y++){
            table+=lista.at(y).v;
        }*/
        for (int y=0;y<lista.count();y++){
            table+=lista.at(y).v;
        }
    }
    qDebug()<<QString("GetPal %1").arg(time.elapsed());

    In2MedianCut medCut;
    QVector<QRgb> rtn=medCut.medianCut(table,colors,corr);
    return rtn;
}

