#ifndef IN2CMYKGENERATOR_H
#define IN2CMYKGENERATOR_H

#include <QImage>

enum{
    IMAGE_INDEX=0,
    CYAN_INDEX,
    MAGENTA_INDEX,
    YELLOW_INDEX,
    BLACK_INDEX,
    BLACK2_INDEX,
    WHITE_INDEX
};

#define CYAN_FILE "_Cyan.bmp"
#define MAGENTA_FILE "_Magenta.bmp"
#define YELLOW_FILE "_Yellow.bmp"
#define BLACK_FILE "_Black.bmp"
#define WHITE_FILE "_White.bmp"



class In2CMYKGenerator: public QObject
{
    Q_OBJECT
public:
    In2CMYKGenerator();
    In2CMYKGenerator(const QImage &image);    
    void render(const QImage & image);
    void setTable(QVector<QRgb> table);
    QImage getImage();
    QImage getCyanImage();
    QImage getYellowImage();
    QImage getMagentaImage();
    QImage getKeyImage();
    QImage getMaskImageFromImage();
    QImage getGreyImageFromCyan(const QImage & image);
    QImage getGreyImageFromMagenta(const QImage & image);
    QImage getGreyImageFromYellow(const QImage & image);
    QImage getGreyImageFromBlack(const QImage & image);

    QImage _getGreyImageFromCyan8bit();
    QImage _getGreyImageFromMagenta8bit();
    QImage _getGreyImageFromYellow8bit();
    QImage _getGreyImageFromBlack8bit();


    QImage getGreyImageFromCyan8bit(const QImage & image);
    QImage getGreyImageFromMagenta8bit(const QImage & image);
    QImage getGreyImageFromYellow8bit(const QImage & image);
    QImage getGreyImageFromBlack8bit(const QImage & image);

    QImage getGreyImageFromMask(const QImage & image);
    QImage getGreyImage(const QImage & image);

    QImage changeBrightness(const QImage & image, int value);
    QImage changeContrast(const QImage & image, float value);
    QImage changeBrightnessContrast(const QImage & image, int value,float cvalue);

public slots:
    void setImage(const QImage & image);
    void sltGetColorFromImage(int i);
    void converToRGBFromCMYK();
    void sltConvertCyan();
    void sltConvertMagenta();
    void sltConvertYellow();
    void sltConvertBlack();
    void sltConvertWhite();

    void _sltGetGreyFromCyan(const QImage &img);
    void _sltGetGreyFromMagenta(const QImage &img);
    void _sltGetGreyFromYellow(const QImage &img);
    void _sltGetGreyFromBlack(const QImage &img);

signals:
    void sgnConvertFinished(int index);
    void sgnCyanConverted(QString file);
    void sgnMagentaConverted(QString file);
    void sgnYellowConverted(QString file);
    void sgnBlackConverted(QString file);
    void sgnWhiteConverted(QString file);

    void sgnCyanImage(const QImage & img);
    void sgnMagentaImage(const QImage & img);
    void sgnYellowImage(const QImage & img);
    void sgnBlackImage(const QImage & img);

private:
    QImage m_image;
    QVector<QRgb> m_table256;
};

#endif // IN2CMYKGENERATOR_H
