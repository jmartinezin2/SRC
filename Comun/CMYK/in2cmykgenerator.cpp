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
#include "in2cmykgenerator.h"
#include <QColor>
#include <QPainter>
#include <QThread>
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include "cmykconfig.h"
//#include "globals.h"
#include "in2defines.h"
#include <QElapsedTimer>


/** Template de ejecucion*/
template<class T> inline const T& limits( const T& x, const T& low, const T& high )
{
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}



In2CMYKGenerator::In2CMYKGenerator()
{

}

In2CMYKGenerator::In2CMYKGenerator(const QImage & image)
{
    render(image);
}
/** Guarda una tabla*/
void In2CMYKGenerator::setTable(QVector<QRgb> table){
    m_table256=table;
}

/** Renderiza la imagen segun la algoritmia que se la dicho*/
void In2CMYKGenerator::render(const QImage & image){
    if (image.format()!=QImage::Format_Indexed8){
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        int format=config.getCalcFormat();        
        m_image=image.convertToFormat((QImage::Format)format,flags);
        //Mejora de blancos
        if (config.getAlgMejoraBlancos()){
            QVector<QRgb> fooTable=m_image.colorTable();
            QImage m_Blancos=image.createMaskFromColor(QColor(Qt::white).rgb());
            int indexWhite=fooTable.indexOf(0xFFFFFFFF);
            if (indexWhite!=-1){
                for (int x=0;x<m_image.width();x++){
                        for (int y=0;y<m_image.height();y++){
                            QRgb white=m_Blancos.pixel(x,y);
                            if (white==0xFFFFFFFF) m_image.setPixel(x,y,indexWhite);
                        }
                }
            }
        }
    }
    else{
        m_image=image;
    }
}

/** Cambio de brillo*/
QImage In2CMYKGenerator::changeBrightnessContrast(const QImage & image, int value,float cvalue){
    QImage rtn;
    if (image.format()==QImage::Format_Indexed8){
        rtn=image;
        QVector<QRgb> fooTable=rtn.colorTable();

        float contrast=cvalue;
        if (cvalue==0) contrast=1;
        else if (cvalue<0) contrast=1/qAbs(cvalue);
        //rtn=image;
        for (int x=0;x<fooTable.count();x++){
            QColor color=QColor(fooTable.at(x));

            int foo=contrast*color.blue()+value;
            int blue=limits(foo,0,255);
            foo=contrast*color.red()+value;
            int red=limits(foo,0,255);
            foo=contrast*color.green()+value;
            int green=limits(foo,0,255);
            color.setRgb(red,green,blue);
            //QColor newVal=QColor(red,green,blue);
            //newTable.replace(x,newVal);
            //rtn.setColor(x,newVal);
            color.setRgb(red,green,blue);
            fooTable[x]=color.rgb();
        }

        rtn.setColorTable(fooTable);
        //rtn=image.convertToFormat(QImage::Format_Indexed8,newTable);
    }

    return rtn;
}


/** Cambio de brillo*/
QImage In2CMYKGenerator::changeBrightness(const QImage & image, int value){
    QImage rtn;
    if (image.format()==QImage::Format_Indexed8){
        QVector<QRgb> fooTable=image.colorTable();
        QVector<QRgb> newTable=fooTable;
        rtn=image;
        for (int x=0;x<fooTable.count();x++){
            QRgb val=fooTable.at(x);
            int foo=qBlue(val)+value;
            int blue=limits(foo,0,255);
            foo=qRed(val)+value;
            int red=limits(foo,0,255);
            foo=qGreen(val)+value;
            int green=limits(foo,0,255);
            QRgb newVal=qRgb(red,green,blue);
            newTable.replace(x,newVal);
        }
        //rtn=image;
        rtn.setColorTable(newTable);
    }
    return rtn;
}

/** Cambio de brillo*/
QImage In2CMYKGenerator::changeContrast(const QImage & image,  float value){
    QImage rtn;
    if (image.format()==QImage::Format_Indexed8){
        QVector<QRgb> fooTable=image.colorTable();
        QVector<QRgb> newTable=fooTable;
        float contrast=value;
        if (value==0) contrast=1;
        else if (value<0) contrast=1/qAbs(value);

        for (int x=0;x<fooTable.count();x++){
            QRgb val=fooTable.at(x);
            int foo=qBlue(val)*contrast;
            int blue=limits(foo,0,255);
            foo=qRed(val)*contrast;
            int red=limits(foo,0,255);
            foo=qGreen(val)*contrast;
            int green=limits(foo,0,255);
            QRgb newVal=qRgb(red,green,blue);
            newTable.replace(x,newVal);
        }
        rtn=image;
        rtn.setColorTable(newTable);
    }
    return rtn;
}


/** Guarda la imagen*/
void In2CMYKGenerator::setImage(const QImage & image){
    m_image=image;
}

/** Guarda la imagen*/
QImage In2CMYKGenerator::getImage(){
    return m_image;
}
/** Devuelve la componente cyan*/
QImage In2CMYKGenerator::getCyanImage(){
   CMYKConfig config;
   int format=config.getCalcFormat();

  if (format==QImage::Format_RGB32){
    QImage rtn(m_image.width(),m_image.height(),m_image.format());
    QRgb color;
    QPainter p(&rtn);
    rtn.fill(QColor(Qt::white).rgb());
    QColor dest;
    for (int x=0;x<m_image.width();x++){
            for (int y=0;y<m_image.height();y++){
                color=m_image.pixel(x,y);
                int cyan=QColor(color).cyan();
                if (cyan){
                    dest.setCmyk(cyan,0,0,0);
                    rtn.setPixel(x,y,dest.rgb());
                }
            }
        }
        p.end();
        return rtn;
  }
  else{
      QVector<QRgb> colors = m_image.colorTable();
      int icolor=0;
      int iBlack=0;
      QColor color=QColor(Qt::white);
      for( int i = 0;i < m_image.colorCount();++i ){
            icolor=QColor(colors.at(i)).cyan();
            iBlack=QColor(colors.at(i)).black();
            if (iBlack==255) icolor=0;
            color.setCmyk(icolor,0,0,0);
            colors[i]=color.rgb();
      }


     QImage rtn=m_image;
     rtn.setColorTable(colors);
     return rtn;

  }


}
/** Devuelve la componente magenta*/
QImage In2CMYKGenerator::getMagentaImage(){
    CMYKConfig config;
    int format=config.getCalcFormat();

    if (format==QImage::Format_RGB32){
        QImage rtn(m_image.width(),m_image.height(),m_image.format());
        QRgb color;
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        QColor dest;
        for (int x=0;x<m_image.width();x++){
              for (int y=0;y<m_image.height();y++){
                  color=m_image.pixel(x,y);
                  int magenta=QColor(color).magenta();
                  if (magenta){
                    dest.setCmyk(0,magenta,0,0);
                    rtn.setPixel(x,y,dest.rgb());
                 }
             }
        }
         p.end();
        return rtn;
    }
    else{
        QVector<QRgb> colors = m_image.colorTable();
        for( int i = 0;i < m_image.colorCount();++i ){
            int icolor=QColor(colors.at(i)).magenta();
            int iBlack=QColor(colors.at(i)).black();
            if (iBlack==255) icolor=0;

            QColor color;
            color.setCmyk(0,icolor,0,0);
            colors[i]=color.rgb();
        }
       QImage rtn=m_image;
       rtn.setColorTable(colors);
       return rtn;
    }
}

/** Devuelve la componente yellow*/
QImage In2CMYKGenerator::getYellowImage(){
    CMYKConfig config;
    int format=config.getCalcFormat();

    if (format==QImage::Format_RGB32){
        QImage rtn(m_image.width(),m_image.height(),m_image.format());
        QRgb color;
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        QColor dest;
        for (int x=0;x<m_image.width();x++){
              for (int y=0;y<m_image.height();y++){
                  color=m_image.pixel(x,y);
                  int yellow=QColor(color).yellow();
                  if (yellow){
                    dest.setCmyk(0,0,yellow,0);
                    rtn.setPixel(x,y,dest.rgb());
                  }
               }
         }
        p.end();
        return rtn;
    }
    else{
        QVector<QRgb> colors = m_image.colorTable();
        for( int i = 0;i < m_image.colorCount();++i ){
            int icolor=QColor(colors.at(i)).yellow();
            int iBlack=QColor(colors.at(i)).black();
            if (iBlack==255) icolor=0;

            QColor color;
            color.setCmyk(0,0,icolor,0);
            colors[i]=color.rgb();
        }
       QImage rtn=m_image;
       rtn.setColorTable(colors);
       return rtn;
    }
}

/** Devuelve la componente black*/
QImage In2CMYKGenerator::getKeyImage(){
    CMYKConfig config;
    int format=config.getCalcFormat();

    if (format==QImage::Format_RGB32){
        QImage rtn(m_image.width(),m_image.height(),m_image.format());
        QRgb color;
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        QColor dest;
        for (int x=0;x<m_image.width();x++){
              for (int y=0;y<m_image.height();y++){
                  color=m_image.pixel(x,y);
                  int black=QColor(color).black();
                  if (black){
                    dest.setCmyk(0,0,0,black);
                    rtn.setPixel(x,y,dest.rgb());
                   }
               }
        }
        p.end();
        return rtn;
    }
    else{
        QVector<QRgb> colors = m_image.colorTable();
        for( int i = 0;i < m_image.colorCount();++i ){
            int icolor=QColor(colors.at(i)).black();
            QColor color;
            color.setCmyk(0,0,0,icolor);
            colors[i]=color.rgb();
        }
       QImage rtn=m_image;
       rtn.setColorTable(colors);
       return rtn;
    }
}
/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromCyan(const QImage & image){

    QVector<QRgb> table=image.colorTable();
    QVector<QRgb> newTable=table;
    if (image.format()==QImage::Format_Indexed8){
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
         p.drawImage(0,0,image.copy(0,0,width,image.height()));


        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int cyan=QColor(color).cyan();
            int grey=255-cyan;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        return rtn;
    }
    else{
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QRgb color;
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int cyan=QColor(color).cyan();
                int grey=255-cyan;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        p.end();
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }

}

/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromMagenta(const QImage & image){    
    QVector<QRgb> table=image.colorTable();
    QVector<QRgb> newTable=table;
    if (image.format()==QImage::Format_Indexed8){        
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        p.drawImage(0,0,image.copy(0,0,width,image.height()));

        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int magenta=QColor(color).magenta();
            int grey=255-magenta;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
    rtn.setColorTable(newTable);
    return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int magenta=QColor(color).magenta();
                int grey=255-magenta;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        p.end();
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }
}


/** Devuelve una imagen grey */
QImage In2CMYKGenerator::getGreyImageFromYellow(const QImage & image){

    QVector<QRgb> table=image.colorTable();
    QVector<QRgb> newTable=table;
    if (image.format()==QImage::Format_Indexed8){
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        p.drawImage(0,0,image.copy(0,0,width,image.height()));

        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int yellow=QColor(color).yellow();
            int grey=255-yellow;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int yellow=QColor(color).yellow();
                int grey=255-yellow;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();

        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }
}

/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromBlack(const QImage & image){
    QVector<QRgb> table=image.colorTable();
    QVector<QRgb> newTable=table;
    if (image.format()==QImage::Format_Indexed8){
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        p.drawImage(0,0,image.copy(0,0,width,image.height()));

        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int black=QColor(color).black();
            int grey=255-black;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int black=QColor(color).black();
                int grey=255-black;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();

        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }

}
/** Por compatibilidad, hace uso del m_image*/
QImage In2CMYKGenerator::_getGreyImageFromCyan8bit(){
    return getGreyImageFromCyan8bit(m_image);
}
/** Por compatibilidad, hace uso del m_image*/
QImage In2CMYKGenerator::_getGreyImageFromMagenta8bit(){
    return getGreyImageFromMagenta8bit(m_image);
}
/** Por compatibilidad, hace uso del m_image*/
QImage In2CMYKGenerator::_getGreyImageFromYellow8bit(){
    return getGreyImageFromYellow8bit(m_image);
}
/** Por compatibilidad, hace uso del m_image*/
QImage In2CMYKGenerator::_getGreyImageFromBlack8bit(){
    return getGreyImageFromBlack8bit(m_image);
}



/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromCyan8bit(const QImage &image){
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;
        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int cyan=QColor(color).cyan();
            //intTable[x]=cyan;
            int black=QColor(color).black();
            int grey=255;
            if (black!=255)
                grey=255-cyan;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256);
        return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,rtn.height(),rtn.format());
        //QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<rtn.height();y++){
                color=image.pixel(x,y);
                int black=QColor(color).black();
                int cyan=QColor(color).cyan();
                int grey=255;
                if (black!=255)
                    grey=255-cyan;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }
}

/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromMagenta8bit(const QImage & image){
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;

        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int magenta=QColor(color).magenta();
            //intTable[x]=cyan;
            int black=QColor(color).black();
            int grey=255;
            if (black!=255)
                grey=255-magenta;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256);
        return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        //QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int black=QColor(color).black();
                int magenta=QColor(color).magenta();
                int grey=255;
                if (black!=255)
                    grey=255-magenta;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }
}




/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromYellow8bit(const QImage & image){
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;

        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int yellow=QColor(color).yellow();
            //intTable[x]=cyan;
            int black=QColor(color).black();
            int grey=255;
            if (black!=255)
                grey=255-yellow;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256);
        return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        //QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int black=QColor(color).black();
                int yellow=QColor(color).yellow();
                int grey=255;
                if (black!=255)
                    grey=255-yellow;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }
}

/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromBlack8bit(const QImage & image){
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;

        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            //intTable[x]=cyan;
            int black=QColor(color).black();
            int grey=255-black;
            newTable[x]=QColor(grey,grey,grey).rgb();
        }
        rtn.setColorTable(newTable);
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256);
        return rtn;
    }
    else{
        QRgb color;
        QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
        //QPainter p(&rtn);
        rtn.fill(QColor(Qt::white).rgb());
        //p.drawImage(0,0,image);
        int width;
        (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
        for (int x=0;x<width;x++){
            for (int y=0;y<image.height();y++){
                color=image.pixel(x,y);
                int black=QColor(color).black();
                int grey=255-black;
                rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());

            }
        }
        CMYKConfig config;
        Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();
        return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
    }
}


/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getMaskImageFromImage(){
    //return image.convertToFormat(QImage::Format_Indexed8,m_table256,Qt::ColorOnly);
    QVector<QRgb> colors = m_image.colorTable();
    for( int i = 0;i < m_image.colorCount();++i ){
        QColor color(Qt::black);
        if (colors.at(i)==QColor(Qt::white).rgb()) color=QColor(Qt::white);
        colors[i]=color.rgb();
    }
   QImage rtn=m_image;
   rtn.setColorTable(colors);
   return rtn;

}

/** Devuelve una imagen grey*/
QImage In2CMYKGenerator::getGreyImageFromMask(const QImage & image){
    QImage rtn(DEFAULT_HEAD_WIDTH,image.height(),image.format());
    QPainter p(&rtn);
    rtn.fill(QColor(Qt::white).rgb());
    p.drawImage(0,0,image);
    int width;
    (image.width()>DEFAULT_HEAD_WIDTH?width=DEFAULT_HEAD_WIDTH:width=image.width());
    for (int x=0;x<width;x++){
        for (int y=0;y<image.height();y++){
            QRgb color=image.pixel(x,y);
            int black=QColor(color).black();
            if (black) rtn.setPixel(x,y,QColor(Qt::black).rgb());
            else rtn.setPixel(x,y,QColor(Qt::white).rgb());
            /*int grey=255-black;
            rtn.setPixel(x,y,QColor(grey,grey,grey).rgb());*/

        }
    }

    CMYKConfig config;
    Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();

    return rtn.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
}

/** Devuelve una imagen grey transformada*/
QImage In2CMYKGenerator::getGreyImage(const QImage &image){
    CMYKConfig config;
    Qt::ImageConversionFlag flags=(Qt::ImageConversionFlag)config.getQtConversionFlag();

    return image.convertToFormat(QImage::Format_Indexed8,m_table256,flags);
}

void In2CMYKGenerator::sltConvertCyan(){
    sltGetColorFromImage(CYAN_INDEX);
}
void In2CMYKGenerator::sltConvertMagenta(){
    sltGetColorFromImage(MAGENTA_INDEX);
}
void In2CMYKGenerator::sltConvertYellow(){
    sltGetColorFromImage(YELLOW_INDEX);
}
void In2CMYKGenerator::sltConvertBlack(){
    sltGetColorFromImage(BLACK_INDEX);
}
void In2CMYKGenerator::sltConvertWhite(){
    sltGetColorFromImage(WHITE_INDEX);
}

/** Carga una de las transformaciones*/
void In2CMYKGenerator::sltGetColorFromImage(int i){
    QString strPath;
    QImage rtn;
    CMYKConfig config;
    int index=0;
    QFile file;

    switch (i){
        case CYAN_INDEX:

            rtn=getGreyImageFromCyan(m_image);
            index=config.getCyanIndex();
            strPath=QString("%1\\Output1%2.bmp").arg(QApplication::applicationDirPath()).arg(index);
            file.setFileName(strPath);
            file.remove();
            rtn.save(strPath);
            emit sgnCyanConverted(strPath);                        
            emit sgnConvertFinished(CYAN_INDEX);
        break;
        case MAGENTA_INDEX:
            rtn=getGreyImageFromMagenta(m_image);
            index=config.getMagentaIndex();
            strPath=QString("%1\\Output1%2.bmp").arg(QApplication::applicationDirPath()).arg(index);
            file.setFileName(strPath);
            file.remove();
            rtn.save(strPath);            
            emit sgnMagentaConverted(strPath);                        
            emit sgnConvertFinished(MAGENTA_INDEX);
        break;
        case YELLOW_INDEX:
            rtn=rtn=getGreyImageFromYellow(m_image);
            index=config.getYellowIndex();
            strPath=QString("%1\\Output1%2.bmp").arg(QApplication::applicationDirPath()).arg(index);
            file.setFileName(strPath);
            file.remove();
            rtn.save(strPath);
            emit sgnYellowConverted(strPath);
            emit sgnConvertFinished(YELLOW_INDEX);
        break;
        case BLACK_INDEX:
            rtn=rtn=getGreyImageFromBlack(m_image);
            index=config.getBlackIndex();
            strPath=QString("%1\\Output1%2.bmp").arg(QApplication::applicationDirPath()).arg(index);
            file.setFileName(strPath);
            file.remove();
            rtn.save(strPath);
            emit sgnBlackConverted(strPath);
            emit sgnConvertFinished(BLACK_INDEX);
        break;
        case WHITE_INDEX:
            rtn=getGreyImageFromMask(m_image);
            index=config.getWhiteIndex();
            strPath=QString("%1\\Output1%2.bmp").arg(QApplication::applicationDirPath()).arg(index);
            file.setFileName(strPath);
            file.remove();
            rtn.save(strPath);
            emit sgnWhiteConverted(strPath);
            emit sgnConvertFinished(WHITE_INDEX);
        break;
    }    
}

/** Automatico, genera los grises a partir del cyan*/
void In2CMYKGenerator::_sltGetGreyFromCyan(const QImage &img){
    QImage rtn=getGreyImageFromCyan8bit(img);
    emit sgnCyanImage(rtn);

}
/** Automatico, genera los grises a partir del magenta*/
void In2CMYKGenerator::_sltGetGreyFromMagenta(const QImage &img){
    QImage rtn=getGreyImageFromMagenta8bit(img);
    emit sgnMagentaImage(rtn);

}
/** Automatico, genera los grises a partir del yellow*/
void In2CMYKGenerator::_sltGetGreyFromYellow(const QImage &img){    
    QImage rtn=getGreyImageFromYellow8bit(img);
    emit sgnYellowImage(rtn);

}
/** Automatico, genera los grises a partir del cyan*/
void In2CMYKGenerator::_sltGetGreyFromBlack(const QImage &img){    
    QImage rtn=getGreyImageFromBlack8bit(img);
    emit sgnBlackImage(rtn);

}




/** Busca los archivos de imagen CMYK y trasnforma a RGB*/
void In2CMYKGenerator::converToRGBFromCMYK(){
}
