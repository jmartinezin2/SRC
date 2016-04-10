#ifndef CMYKFRAMEWORK_H
#define CMYKFRAMEWORK_H

#include <QImage>
#include "math.h"

//Cambia el brillo
QImage changeBrightness( const QImage& image, int brightness );
//Cambia el contraste
QImage changeContrast( const QImage& image, int contrast );
//Cambia el gamma
QImage changeGamma( const QImage& image, int gamma );

// Devuelve el valor más bajo de pixel
int getLowestValue(const QImage & image);
// Devuelve el brillo de la imagen
int getBrightnessFromGray(const QImage & image,int value);
// Devuelve el contraste de la imagen
int getContrastFromGray(const QImage & image,int value);
// Devuelve el gamma de la imagen
int getGammaFromGray(const QImage & image,int value);

#endif // CMYKFRAMEWORK_H
