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
#include "In2Barcode.h"



In2Barcode::In2Barcode()
    : QGraphicsItem()
{
    w=600;
    h=300;
    setDefault();

}
/**Inicializamos*/
In2Barcode::In2Barcode(int width,int height)
    : QGraphicsItem(){
    w=width;
    h=height;
    setDefault();
}
/** Por defectO*/
void In2Barcode::setDefault(){
    //Algunas propiedades por defecto
    ar=(Zint::QZint::AspectRatioMode)1;
    bc.setSecurityLevel(0);
    bc.setWidth(0);
    bc.setInputMode(UNICODE_MODE);
    bc.setHideText(true);
    bc.setSymbol(BARCODE_CODE128);
    bc.setBorderWidth(1);
    bc.setHeight(25);//(35);
    bc.setWhitespace(0);
    bc.setScale(1);
    bc.setFgColor(qRgb(0,0,0));
    bc.setBgColor(qRgb(0xff,0xff,0xff));
}

/** Carga la codificacion que se le diga*/
void In2Barcode::SetSymbol(int symbol){
    bc.setSymbol(symbol);
}

In2Barcode::~In2Barcode()
{
}

QRectF In2Barcode::boundingRect() const
{
	return QRectF(0, 0, w, h);
}
//Deshabilito esto....por si acaso le da por rendereizar
void In2Barcode::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{

    //bc.render(*painter,boundingRect(),ar);
}

/** En vez de renderizar como en paint, renderizo sobre un QPai9nter en un QPixmap*/
void In2Barcode::SetPixmap()
{
    QImage dummyImagen(boundingRect().width(),boundingRect().height(), QImage::Format_Mono);
    QPainter imagePainter(&dummyImagen);
    bc.render(imagePainter, dummyImagen.rect(), ar);
    qpxImagen = QPixmap::fromImage(dummyImagen);
}

/** La misma pero con el texto*/
void In2Barcode::SetPixmap(QString text)
{
    bc.setText(text);
    QImage dummyImagen(boundingRect().width(),boundingRect().height(), QImage::Format_Mono);
    QPainter imagePainter(&dummyImagen);

    bc.render(imagePainter, dummyImagen.rect(), ar);
    qpxImagen = QPixmap::fromImage(dummyImagen);
}


