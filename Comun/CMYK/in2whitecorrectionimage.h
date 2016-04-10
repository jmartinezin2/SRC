#ifndef IN2WHITECORRECTIONIMAGE_H
#define IN2WHITECORRECTIONIMAGE_H

#include <QImage>

class In2WhiteCorrectionImage
{
public:
    In2WhiteCorrectionImage();
    ~In2WhiteCorrectionImage();
    void correctWhite(const QImage & original, QImage * dest);
    void correctBlack(const QImage & original, QImage * dest);
    QRgb getDarkest(const QImage & original);
    void correctDark(const QImage & original, QImage * dest,QRgb val);
};

#endif // IN2WHITECORRECTIONIMAGE_H
