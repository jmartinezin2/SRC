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
#include "in2whitecorrectionimage.h"
#include <QImage>

In2WhiteCorrectionImage::In2WhiteCorrectionImage()
{

}

In2WhiteCorrectionImage::~In2WhiteCorrectionImage()
{

}
/** Elimina los blancos*/
void In2WhiteCorrectionImage::correctWhite(const QImage & original, QImage * dest){
}
/** Mantiene los negros*/
void In2WhiteCorrectionImage::correctBlack(const QImage & original, QImage * dest){
}
/** mantiene los oscuros*/
void In2WhiteCorrectionImage::correctDark(const QImage & original, QImage * dest,QRgb val){
}

/** Devuelve el color mas oscuro*/
QRgb In2WhiteCorrectionImage::getDarkest(const QImage & original){
    int width=original.width();
    int height=original.height();
    QRgb darkRGB=0;
    int dark=256;
    for (int x=0;x<width;x++){
        for (int y=0;y<height;y++){
            QRgb value=original.pixel(x,y);
            int red=(value>>16)&0xFF;
            int green=(value>>8)&0xFF;
            int blue=(value)&0xFF;
            int val=(red+green+blue)/3;
            if (val<dark){
                dark=val;
                darkRGB=value;
            }
        }
    }
    return darkRGB;
}

