#include "in2colortransformer.h"
#include <QImage>
#include "math.h"

In2ColorTransformer::In2ColorTransformer()
{
}

/** Template de ejecucion*/
template<class T> inline const T& kClamp( const T& x, const T& low, const T& high )
{
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}

/** Cambia brillo*/
static int changeBrightness( int value, int brightness )
    {
    return kClamp( value + brightness * 255 / 100, 0, 255 );
    }
/** Cambia contratste*/
inline static int changeContrast( int value, int contrast )
    {
    return kClamp((( value - 127 ) * contrast / 100 ) + 127, 0, 255 );
    }

/** Cambiar gamma*/
inline static int changeGamma( int value, int gamma )
    {
    return kClamp( int( pow( value / 255.0, 100.0 / gamma ) * 255 ), 0, 255 );
    }

/** uso de tabla*/
inline static int changeUsingTable( int value, const int table[] )
    {
    return table[ value ];
    }
/** selector de operación*/
static QImage changeImage( const QImage& image, int value )
    {
    QImage im = image;
    im.detach();
    if( im.numColors() == 0 ) /* truecolor */
        {
        if( im.format() != QImage::Format_RGB32 ) /* just in case */
            im = im.convertToFormat( QImage::Format_RGB32 );
        int table[ 256 ];
        for( int i = 0;
             i < 256;
             ++i )
            table[ i ] = operation( i, value );
        if( im.hasAlphaChannel() )
            {
            for( int y = 0;
                 y < im.height();
                 ++y )
                {
                QRgb* line = reinterpret_cast< QRgb* >( im.scanLine( y ));
                for( int x = 0;
                     x < im.width();
                     ++x )
                    line[ x ] = qRgba( changeUsingTable( qRed( line[ x ] ), table ),
                        changeUsingTable( qGreen( line[ x ] ), table ),
                        changeUsingTable( qBlue( line[ x ] ), table ),
                        changeUsingTable( qAlpha( line[ x ] ), table ));
                }
            }
        else
            {
            for( int y = 0;
                 y < im.height();
                 ++y )
                {
                QRgb* line = reinterpret_cast< QRgb* >( im.scanLine( y ));
                for( int x = 0;
                     x < im.width();
                     ++x )
                    line[ x ] = qRgb( changeUsingTable( qRed( line[ x ] ), table ),
                        changeUsingTable( qGreen( line[ x ] ), table ),
                        changeUsingTable( qBlue( line[ x ] ), table ));
                }
            }
        }
    else
        {
        QVector<QRgb> colors = im.colorTable();
        for( int i = 0;
             i < im.numColors();
             ++i )
            colors[ i ] = qRgb( operation( qRed( colors[ i ] ), value ),
                operation( qGreen( colors[ i ] ), value ),
                operation( qBlue( colors[ i ] ), value ));
        }
    return im;
    }


// brightness is multiplied by 100 in order to avoid floating point numbers
QImage In2ColorTransformer::changeBrightness( const QImage& image, int brightness )
    {
    if( brightness == 0 ) // no change
        return image;
    return changeImage< changeBrightness >( image, brightness );
    }


// contrast is multiplied by 100 in order to avoid floating point numbers
QImage In2ColorTransformer::changeContrast( const QImage& image, int contrast )
    {
    if( contrast == 100 ) // no change
        return image;
    return changeImage< changeContrast >( image, contrast );
    }

// gamma is multiplied by 100 in order to avoid floating point numbers
QImage In2ColorTransformer::changeGamma( const QImage& image, int gamma )
    {
    if( gamma == 100 ) // no change
        return image;
    return changeImage< changeGamma >( image, gamma );
    }

/** Devuelve el color mas bajo */
int In2ColorTransformer::getLowestValue(const QImage & image){
    QImage im = image;
    im.detach();
    int value=qRed(im.pixel(0,0));
    for( int x=0;x <im.width();++x){
        for (int y=0;y<im.height();y++){
            if (qRed(im.pixel(x,y))<value) value=qRed(im.pixel(x,y));
        }
    }
    return value;
}
/** Devuelve el brillo  de una imagen*/
int In2ColorTransformer::getBrightnessFromGray(const QImage & image,int value){
    int lowestvalue=value;
    if (lowestvalue==0) lowestvalue=getLowestValue(image);
    return kClamp((lowestvalue*100)/255, 0, 255 );
}
/** Devuelve el contraste  de una imagen*/
int In2ColorTransformer::getContrastFromGray(const QImage & image,int value){
    int lowestvalue=value;
    if (lowestvalue==0) lowestvalue=getLowestValue(image);
    return kClamp(((lowestvalue-127)*100)/255, 0, 255 );
}
/** Devuelve el gamma  de una imagen*/
int In2ColorTransformer::getGammaFromGray(const QImage & image,int value){
    int lowestvalue=value;
    if (lowestvalue==0) lowestvalue=getLowestValue(image);
    return kClamp((lowestvalue*100)/255, 0, 255 );
}
