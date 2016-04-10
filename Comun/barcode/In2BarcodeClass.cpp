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
#include <QDebug>
#include <QMap>
#include "In2BarcodeClass.h"
#include "zint.h"
#include "MemoryMap.h"
#include "globals.h"

In2BarcodeClass::In2BarcodeClass()
    : QGraphicsItem()
{
    w=600;
    h=300;
    setDefault();

}
/**Inicializamos*/
In2BarcodeClass::In2BarcodeClass(int width,int height)
    : QGraphicsItem(){
    w=width;
    h=height;
    setDefault();
}
/** Por defectO*/
void In2BarcodeClass::setDefault(){
    //Algunas propiedades por defecto
    ar=(Zint::QZint::AspectRatioMode)0;
    bc.setSecurityLevel(0);
    //bc.setWidth(0);
    bc.setInputMode(UNICODE_MODE);
    bc.setHideText(TRUE);
    bc.setSymbol(BARCODE_CODE128);
    bc.setBorderWidth(1);
    bc.setHeight(25);//(35);
    bc.setWhitespace(0);
    bc.setScale(1);
    bc.setFgColor(qRgb(0,0,0));
    bc.setBgColor(qRgb(0xff,0xff,0xff));
    bc.fontPixelSizeLarge=10;
    bc.fontPixelSizeLarge=9;

}

In2BarcodeClass::In2BarcodeClass(struct m_BarcodeParameters barcodeParameters)
    : QGraphicsItem(){
    w=barcodeParameters.width;
    h=barcodeParameters.height;
    //ar=(Zint::QZint::AspectRatioMode)1;
    //ar=barcodeParameters.ar;
    if (barcodeParameters.ar==0) ar=(Zint::QZint::AspectRatioMode)0;
    else if (barcodeParameters.ar==1) ar=(Zint::QZint::AspectRatioMode)1;
    else if (barcodeParameters.ar==2) ar=(Zint::QZint::AspectRatioMode)2;
    else ar=(Zint::QZint::AspectRatioMode)0;

    bc.setSecurityLevel(barcodeParameters.nivelSeguridad);

    bc.setWidth(barcodeParameters.width);
    bc.setHeight(barcodeParameters.height);
    bc.setScale(barcodeParameters.escala);
    bc.setWhitespace(barcodeParameters.whitespace);
    bc.setSymbol(barcodeParameters.codificacion);
    bc.setInputMode(barcodeParameters.inputMode);
    bc.setHideText(barcodeParameters.hideText);
    bc.setBorderWidth(barcodeParameters.borderWidth);
    bc.setOption2(barcodeParameters.option2);
    bc.setTextAlignment(barcodeParameters.textAlignement);
    Zint::QZint::BorderType tipoBorde;
    if (barcodeParameters.borderType==0) tipoBorde=Zint::QZint::NO_BORDER;
    else if (barcodeParameters.borderType==2) tipoBorde=Zint::QZint::BIND;
    else if (barcodeParameters.borderType==4) tipoBorde=Zint::QZint::BOX;
    else tipoBorde=Zint::QZint::NO_BORDER;

    bc.setBorderType(tipoBorde);

    bc.setfontPixelSizeLarge(barcodeParameters.fontSizeLarge);
    bc.setfontPixelSizeSmall(barcodeParameters.fontSizeSmall);
    bc.setfontType(barcodeParameters.fontType);

    //bc.setBgColor(barcodeParameters.BgColor);
    //bc.setFgColor(barcodeParameters.FgColor);
    bc.setFgColor(qRgb(0,0,0));
    bc.setBgColor(qRgb(0xff,0xff,0xff));
}


In2BarcodeClass::In2BarcodeClass(bool fijo, unsigned char NumCampo)
    : QGraphicsItem(){

    MemoryMap *memMap=MemoryMap::request(0);
    if (fijo){
        struct m_BarcodeParameters  barcodeParameters=memMap->m_mapa.m_campos[NumCampo].m_barcodeParameters;
        this->setParams(barcodeParameters);
    }
    else{
        struct m_BarcodeParameters  barcodeParameters=memMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters;
        this->setParams(barcodeParameters);


    }
    //bc.setFgColor(qRgb(0,0,0));

    //bc.setBgColor(qRgb(0xff,0xff,0xff));

}
/** Carga una estructura de parametros*/
void In2BarcodeClass::setParams(const struct m_BarcodeParameters & barcodeParameters){
    w=barcodeParameters.width;
    h=barcodeParameters.height;
    //ar=(Zint::QZint::AspectRatioMode)1;
    //ar=barcodeParameters.ar;
    if (barcodeParameters.ar==0) ar=(Zint::QZint::AspectRatioMode)0;
    else if (barcodeParameters.ar==1) ar=(Zint::QZint::AspectRatioMode)1;
    else if (barcodeParameters.ar==2) ar=(Zint::QZint::AspectRatioMode)2;
    else ar=(Zint::QZint::AspectRatioMode)0;
    bc.setSecurityLevel(barcodeParameters.nivelSeguridad);
    bc.setWidth(barcodeParameters.width);
    bc.setHeight(barcodeParameters.height);
    bc.setScale(barcodeParameters.escala);
    bc.setWhitespace(barcodeParameters.whitespace);
    bc.setSymbol(barcodeParameters.codificacion);
    bc.setInputMode(barcodeParameters.inputMode);
    bc.setHideText(barcodeParameters.hideText);
    bc.setBorderWidth(barcodeParameters.borderWidth);
    bc.setOption2(barcodeParameters.option2);
    bc.setTextAlignment(barcodeParameters.textAlignement);
    Zint::QZint::BorderType tipoBorde;
    if (barcodeParameters.borderType==0) tipoBorde=Zint::QZint::NO_BORDER;
    else if (barcodeParameters.borderType==2) tipoBorde=Zint::QZint::BIND;
    else if (barcodeParameters.borderType==4) tipoBorde=Zint::QZint::BOX;
    else tipoBorde=Zint::QZint::NO_BORDER;
    bc.setBorderType(tipoBorde);
    bc.setfontPixelSizeLarge(barcodeParameters.fontSizeLarge);
    bc.setfontPixelSizeSmall(barcodeParameters.fontSizeSmall);
    bc.setfontType(barcodeParameters.fontType);
    bc.setFgColor(qRgb(0,0,0));
    bc.setBgColor(qRgb(0xff,0xff,0xff));
}



void In2BarcodeClass::SetText(const QString & text){
     bc.setText(text);
}

void In2BarcodeClass::SetWidth(int width){
     bc.setWidth(width);
}

void In2BarcodeClass::SetHeight(int height){
     bc.setHeight(height);
}

void In2BarcodeClass::SetInputMode(int input){
     bc.setInputMode(input);
}

void In2BarcodeClass::SetHideText(bool hide){
     bc.setHideText(hide);
}

/** Carga la codificacion que se le diga*/
void In2BarcodeClass::SetSymbol(int symbol){
    bc.setSymbol(symbol);
}

void In2BarcodeClass::SetScale(float scale){
	bc.setScale(scale);
}

void In2BarcodeClass::SetFgColor(const QColor & fgColor){
	bc.setFgColor(fgColor);
}

void In2BarcodeClass::SetBgColor(const QColor & bgColor){
	bc.setBgColor(bgColor);
}

void In2BarcodeClass::SetBorderWidth(int boderWidth){
	bc.setBorderWidth(boderWidth);
}
void In2BarcodeClass::SetBorderType(Zint::QZint::BorderType border){
    bc.setBorderType(border);
}

void In2BarcodeClass::SetWhitespace(int whitespace){
	bc.setWhitespace(whitespace);
}

QString In2BarcodeClass::GetLastError(){
    QString error;
    error=bc.lastError();
    return error;
}

void In2BarcodeClass::SetOption2(int option2){
    bc.setOption2(option2);
}

int In2BarcodeClass::pixmapWidth(){
    int w=bc.getPixmapWidth();
    return w;
}

int In2BarcodeClass::pixmapHeight(){
    int h=bc.getPixmapHeight();
    return h;
}

void In2BarcodeClass::SetFontSmall(int small_value){
     bc.setfontPixelSizeSmall(small_value);
}

void In2BarcodeClass::SetFontLarge(int large_value){
     bc.setfontPixelSizeLarge(large_value);
}

void In2BarcodeClass::SetFontType(QString fontType){
     bc.setfontType(fontType);
}

void In2BarcodeClass::SetForeColor(QColor color_fg){
     bc.setforegroundColor(color_fg);
}

void In2BarcodeClass::SetBackColor(QColor color_bg){
     bc.setforegroundColor(color_bg);
}

void In2BarcodeClass::SetSecurityLevel(int level){
     bc.setSecurityLevel(level);
}


In2BarcodeClass::~In2BarcodeClass()
{
}

QRectF In2BarcodeClass::boundingRect() const
{
	return QRectF(0, 0, w, h);
}
//Deshabilito esto....por si acaso le da por rendereizar
void In2BarcodeClass::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{

    //bc.render(*painter,boundingRect(),ar);
}

/** En vez de renderizar como en paint, renderizo sobre un QPai9nter en un QPixmap*/
void In2BarcodeClass::SetPixmap()
{
    QImage dummyImagen(boundingRect().width(),boundingRect().height(), QImage::Format_Mono);
    QPainter imagePainter(&dummyImagen);
    bc.render(imagePainter, dummyImagen.rect(), ar);
    qpxImagen = QPixmap::fromImage(dummyImagen);
}

/** La misma pero con el texto*/
void In2BarcodeClass::SetPixmap(QString text)
{
    /*bc.setText(text);
    QImage dummyImagen(boundingRect().width(),boundingRect().height(), QImage::Format_Mono);
    QPainter imagePainter(&dummyImagen);
    bc.render(imagePainter, dummyImagen.rect(), ar);
    qpxImagen = QPixmap::fromImage(dummyImagen);*/
    bc.setText(text);
        qpxImagen=QPixmap(boundingRect().width(),boundingRect().height());
        QPainter imagePainter(&qpxImagen);
        bc.render(imagePainter, qpxImagen.rect(), ar);
        imagePainter.end();


    //qpxImagen.save("C:\\Barcode.bmp","BMP");
}

/*
QPixmap In2BarcodeClass::In2BarcodeClass(bool fijo, int NumCampo,QString texto)
    : QGraphicsItem(){

    if (fijo){
        w=memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.width;
        h=memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.height;
        ar=(Zint::QZint::AspectRatioMode)1;
        bc.setSecurityLevel(0);

        bc.setWidth(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.width);
        bc.setHeight(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.height);
        bc.setScale(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.escala);
        bc.setWhitespace(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.whitespace);
        bc.setSymbol(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.codificacion);
        bc.setInputMode(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.inputMode);
        bc.setHideText(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.hideText);
        bc.setBorderWidth(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.borderWidth);
        bc.setOption2(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.option2);
        Zint::QZint::BorderType tipoBorde;
        if (memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.borderType==0) tipoBorde=Zint::QZint::NO_BORDER;
        else if (memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.borderType==2) tipoBorde=Zint::QZint::BIND;
        else if (memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.borderType==4) tipoBorde=Zint::QZint::BOX;
        else tipoBorde=Zint::QZint::NO_BORDER;

        bc.setBorderType(tipoBorde);

        bc.setfontPixelSizeLarge(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.fontSizeLarge);
        bc.setfontPixelSizeSmall(memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.fontSizeSmall);

        if (memoryMap->m_mapa.m_variables.NumColores==1){
            bc.setFgColor(qRgb(0,0,0));
        }
        else{
            int indice_color= memoryMap->m_mapa.m_campos[NumCampo].m_barcodeParameters.indiceFgColor-1;
            bc.setFgColor(memoryMap->m_mapa.m_colores[indice_color].valor);
        }


    }
    else{
        w=memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.width;
        h=memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.height;
        ar=(Zint::QZint::AspectRatioMode)1;
        bc.setSecurityLevel(0);

        bc.setWidth(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.width);
        bc.setHeight(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.height);
        bc.setScale(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.escala);
        bc.setWhitespace(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.whitespace);
        bc.setSymbol(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.codificacion);
        bc.setInputMode(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.inputMode);
        bc.setHideText(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.hideText);
        bc.setBorderWidth(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.borderWidth);
        bc.setOption2(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.option2);
        Zint::QZint::BorderType tipoBorde;
        if (memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.borderType==0) tipoBorde=Zint::QZint::NO_BORDER;
        else if (memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.borderType==2) tipoBorde=Zint::QZint::BIND;
        else if (memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.borderType==4) tipoBorde=Zint::QZint::BOX;
        else tipoBorde=Zint::QZint::NO_BORDER;

        bc.setBorderType(tipoBorde);

        bc.setfontPixelSizeLarge(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.fontSizeLarge);
        bc.setfontPixelSizeSmall(memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.fontSizeSmall);

        if (memoryMap->m_mapa.m_variables.NumColores==1){
            bc.setFgColor(qRgb(0,0,0));
        }
        else{
            int indice_color= memoryMap->m_mapa.m_camposvariables[NumCampo].m_barcodeParameters.indiceFgColor-1;
            bc.setFgColor(memoryMap->m_mapa.m_colores[indice_color].valor);
        }

    }

    bc.setFgColor(qRgb(0,0,0));

    bc.setBgColor(qRgb(0xff,0xff,0xff));

    bc.setText(texto);
    QImage dummyImagen(boundingRect().width(),boundingRect().height(), QImage::Format_Mono);
    QPainter imagePainter(&dummyImagen);

    bc.render(imagePainter, dummyImagen.rect(), ar);

    qpxImagen = QPixmap::fromImage(dummyImagen);

    return qpxImagen;

}
*/
