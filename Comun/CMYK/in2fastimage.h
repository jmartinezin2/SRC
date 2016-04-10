#ifndef IN2FASTIMAGE_H
#define IN2FASTIMAGE_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QImage>

class In2FastImage : public QObject
{
    Q_OBJECT
public:
    explicit In2FastImage(QObject *parent = 0);
    ~In2FastImage();
    void changeColorTable(QVector<QRgb> table);

signals:
    void sgnCyanImage(const QImage &img);
    void sgnMagentaImage(const QImage &img);
    void sgnYellowImage(const QImage &img);
    void sgnBlackImage(const QImage &img);


private:
    int m_brightness;
    float m_contrast;
    bool m_MonoMode;
    int m_difAlgoritmo;
    bool m_bCorrBlancos;
    QVector<QRgb> m_table256;

public slots:
    QImage greyFromCyan(const QImage & img);
    QImage _greyFromCyan(const QImage & img,int bright,float contrast,bool monoMode);
    QImage greyFromMagenta(const QImage & img);
    QImage _greyFromMagenta(const QImage & img,int bright,float contrast,bool monoMode);
    QImage greyFromYellow(const QImage & img);
    QImage _greyFromYellow(const QImage & img,int bright,float contrast,bool monoMode);
    QImage greyFromBlack(const QImage & img);
    QImage _greyFromBlack(const QImage & img,int bright,float contrast,bool monoMode);
    void setBrightness(int val);
    void setContrast(float val);
    void setMonoMode(bool val);
    void setCorrBlancos(bool val);
    void setDifAlgoritmo(int val);



};

#endif // IN2FASTIMAGE_H
