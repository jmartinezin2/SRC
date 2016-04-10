#include "in2fastimage.h"
#include "in2whitecorrectionimage.h"
#include <QVector>

//Tabla de datos para el xub XUSB
static int xusbData[]={0,30,75,105,150,175,210,255};

/** Template de ejecucion*/
template<class T> inline const T& limits( const T& x, const T& low, const T& high )
{
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}


In2FastImage::In2FastImage(QObject *parent) : QObject(parent)
{
    m_brightness=0;
    m_MonoMode=false;
    m_contrast=1;
    m_difAlgoritmo=Qt::DiffuseDither;
    m_bCorrBlancos=false;

    //Ponemos la tabla como la quiere el XUSB
    m_table256.resize(256);    
    for (int x=0;x<256;x++){
        m_table256[x]=qRgb(x,x,x);
    }

}
/** Cambia la tabla de color*/
void In2FastImage::changeColorTable(QVector<QRgb> table){

    if (table.count()!=256) return;
    m_table256=table;
    for (int x=0;x<256;x++){
        m_table256[x]=qRgb(x,x,x);
    }
}

In2FastImage::~In2FastImage()
{

}
/** Carga el brillo*/
void In2FastImage::setBrightness(int val){
    m_brightness=val;
}

/** Carga el contraste*/
void In2FastImage::setContrast(float val){
    if (val<0) m_contrast=1/qAbs(val);
    else if (val==0) m_contrast=1;
    else m_contrast=val;
}

/** Carga el MonoMode*/
void In2FastImage::setMonoMode(bool val){
    m_MonoMode=val;
}

/** Hace todas las operaciones*/
QImage In2FastImage::greyFromCyan(const QImage &image){
    QImage rtn=_greyFromCyan(image,m_brightness,m_contrast,m_MonoMode);
    emit sgnCyanImage(rtn);
    return rtn;
}

/** Hace todas las operaciones*/
QImage In2FastImage::_greyFromCyan(const QImage & image,int bright,float contrast,bool monoMode){
    if (image.isNull()) return QImage();
    QImage rtn=image;

    if (rtn.format()==QImage::Format_Indexed8){

        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;
        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int cyan=QColor(color).cyan();
            int black=QColor(color).black();
            int grey=255;
            if (black!=255)
                grey=255-cyan;

            int foo=contrast*grey+bright;
            cyan=limits(foo,0,255);

            newTable[x]=QColor(cyan,cyan,cyan).rgb();
        }
        rtn.setColorTable(newTable);
    }
    else{
        //Hay que recorre toda la imagen
        int height=rtn.height();
        int width=rtn.width();
        for (int y=0;y<height;y++){
            QRgb *data=(QRgb*)image.scanLine(y);
            for (int x=0;x<width;x++){
                QRgb color=data[x];
                int cyan=QColor(color).cyan();
                int black=QColor(color).black();
                int grey=255;
                if (black!=255)
                    grey=255-cyan;

                int foo=contrast*grey+bright;
                cyan=limits(foo,0,255);
                rtn.setPixel(x,y,QColor(cyan,cyan,cyan).rgb());
            }
        }
    }
    if (monoMode){
        QImage fooImage=rtn.convertToFormat(QImage::Format_Mono,(Qt::ImageConversionFlag)m_difAlgoritmo);
        QImage fooImage2=fooImage.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=fooImage2.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);


    }
    else{
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);
    }

    if (m_bCorrBlancos){
        In2WhiteCorrectionImage WhiteCorr;
        WhiteCorr.correctWhite(image,&rtn);

    }


    return rtn;
}


/** Hace todas las operaciones*/
QImage In2FastImage::greyFromMagenta(const QImage &image){
    QImage rtn=_greyFromMagenta(image,m_brightness,m_contrast,m_MonoMode);
    emit sgnMagentaImage(rtn);
    return rtn;

}

/** Hace todas las operaciones*/
QImage In2FastImage::_greyFromMagenta(const QImage & image,int bright,float contrast,bool monoMode){
    if (image.isNull()) return QImage();
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;
        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int magenta=QColor(color).magenta();
            int black=QColor(color).black();
            int grey=255;
            if (black!=255)
                grey=255-magenta;

            int foo=contrast*grey+bright;
            magenta=limits(foo,0,255);

            newTable[x]=QColor(magenta,magenta,magenta).rgb();
        }
        rtn.setColorTable(newTable);
    }
    else{
        //Hay que recorre toda la imagen
        int height=rtn.height();
        int width=rtn.width();
        for (int y=0;y<height;y++){
            QRgb *data=(QRgb*)image.scanLine(y);
            for (int x=0;x<width;x++){
                QRgb color=data[x];
                int magenta=QColor(color).magenta();
                int black=QColor(color).black();
                int grey=255;
                if (black!=255)
                    grey=255-magenta;

                int foo=contrast*grey+bright;
                magenta=limits(foo,0,255);
                rtn.setPixel(x,y,QColor(magenta,magenta,magenta).rgb());
            }
        }
    }
    if (monoMode){
        QImage fooImage=rtn.convertToFormat(QImage::Format_Mono,(Qt::ImageConversionFlag)m_difAlgoritmo);
        QImage fooImage2=fooImage.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=fooImage2.convertToFormat(QImage::Format_Indexed8,(Qt::ImageConversionFlag)m_difAlgoritmo);

    }
    else{
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);
    }

    if (m_bCorrBlancos){
        In2WhiteCorrectionImage WhiteCorr;
        WhiteCorr.correctWhite(image,&rtn);

    }


    return rtn;
}

/** Hace todas las operaciones*/
QImage In2FastImage::greyFromYellow(const QImage &image){
    QImage rtn=_greyFromYellow(image,m_brightness,m_contrast,m_MonoMode);
    emit sgnYellowImage(rtn);
    return rtn;

}

/** Hace todas las operaciones*/
QImage In2FastImage::_greyFromYellow(const QImage & image,int bright,float contrast,bool monoMode){
    if (image.isNull()) return QImage();
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;
        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int yellow=QColor(color).yellow();
            int black=QColor(color).black();
            int grey=255;
            if (black!=255)
                grey=255-yellow;

            int foo=contrast*grey+bright;
            yellow=limits(foo,0,255);

            newTable[x]=QColor(yellow,yellow,yellow).rgb();
        }
        rtn.setColorTable(newTable);
    }
    else{
        //Hay que recorre toda la imagen
        int height=rtn.height();
        int width=rtn.width();
        for (int y=0;y<height;y++){
            QRgb *data=(QRgb*)image.scanLine(y);
            for (int x=0;x<width;x++){
                QRgb color=data[x];
                int yellow=QColor(color).yellow();
                int black=QColor(color).black();
                int grey=255;
                if (black!=255)
                    grey=255-yellow;

                int foo=contrast*grey+bright;
                yellow=limits(foo,0,255);
                rtn.setPixel(x,y,QColor(yellow,yellow,yellow).rgb());
            }
        }
    }
    if (monoMode){
        QImage fooImage=rtn.convertToFormat(QImage::Format_Mono,(Qt::ImageConversionFlag)m_difAlgoritmo);
        QImage fooImage2=fooImage.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=fooImage2.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);
    }
    else{
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);
    }

    if (m_bCorrBlancos){
        In2WhiteCorrectionImage WhiteCorr;
        WhiteCorr.correctWhite(image,&rtn);

    }


    return rtn;
}

/** Hace todas las operaciones*/
QImage In2FastImage::greyFromBlack(const QImage &image){
    QImage rtn=_greyFromBlack(image,m_brightness,m_contrast,m_MonoMode);
    emit sgnBlackImage(rtn);
    return rtn;

}

/** Hace todas las operaciones*/
QImage In2FastImage::_greyFromBlack(const QImage & image,int bright,float contrast,bool monoMode){
    if (image.isNull()) return QImage();
    QImage rtn=image;
    if (rtn.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=rtn.colorTable();
        QVector<QRgb> newTable=table;
        int iSize=table.count();
        QRgb color;
        for (int x=0;x<iSize;x++){
            color=table.at(x);
            int black=QColor(color).black();
            int grey=255-black;

            int foo=contrast*grey+bright;
            black=limits(foo,0,255);

            newTable[x]=QColor(black,black,black).rgb();
        }
        rtn.setColorTable(newTable);
    }
    else{
        //Hay que recorre toda la imagen
        int height=rtn.height();
        int width=rtn.width();
        for (int y=0;y<height;y++){
            QRgb *data=(QRgb*)image.scanLine(y);
            for (int x=0;x<width;x++){
                QRgb color=data[x];
                int black=QColor(color).black();
                int grey=255-black;
                int foo=contrast*grey+bright;
                black=limits(foo,0,255);
                rtn.setPixel(x,y,QColor(black,black,black).rgb());
            }
        }
    }
    if (monoMode){
        QImage fooImage=rtn.convertToFormat(QImage::Format_Mono,(Qt::ImageConversionFlag)m_difAlgoritmo);
        QImage fooImage2=fooImage.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=fooImage2.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);
    }
    else{
        QImage dummy=rtn.convertToFormat(QImage::Format_ARGB32,(Qt::ImageConversionFlag)m_difAlgoritmo);
        rtn=dummy.convertToFormat(QImage::Format_Indexed8,m_table256,(Qt::ImageConversionFlag)m_difAlgoritmo);
    }

    if (m_bCorrBlancos){
        In2WhiteCorrectionImage WhiteCorr;
        WhiteCorr.correctWhite(image,&rtn);

    }
    return rtn;
}

/** Carga el algoritmo de correccion de blancos*/
void In2FastImage::setCorrBlancos(bool val){
    m_bCorrBlancos=val;
}
/** Carga el algoritmo de difusion*/
void In2FastImage::setDifAlgoritmo(int val){
    m_difAlgoritmo=val;
}
