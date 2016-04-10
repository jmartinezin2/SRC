#ifndef IN2COLORTRANSFORMER_H
#define IN2COLORTRANSFORMER_H

#include <QImage>
class In2ColorTransformer
{
public:
    In2ColorTransformer();

    // brightness is multiplied by 100 in order to avoid floating point numbers
    QImage changeBrightness( const QImage& image, int brightness );
    QImage changeContrast( const QImage& image, int contrast );
    QImage changeGamma( const QImage& image, int gamma );
    int getLowestValue(const QImage & image);
    int getBrightnessFromGray(const QImage & image,int value);
    int getContrastFromGray(const QImage & image,int value);
    int getGammaFromGray(const QImage & image,int value);

};

#endif // IN2COLORTRANSFORMER_H
