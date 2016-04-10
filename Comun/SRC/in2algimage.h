#ifndef IN2ALGIMAGE_H
#define IN2ALGIMAGE_H

#include <QObject>
#include <QColor>
#include <QImage>


class In2AlgImage : public QObject
{
    Q_OBJECT
public:
    explicit In2AlgImage(QObject *parent = 0);
    QRgb getMaxColorTableValue(QImage & image);
    QRgb getMinColorTableValue(QImage & image);
    QImage oscureceImagen(const QImage & image,int x);
    QImage aclaraImagen(const QImage & image,int x);
    QImage getFloydSteimberg(QImage & image, QVector<QRgb> &xusbTable);
    QImage setNewXusbTable(QImage & image, QVector<QRgb> &xusbTable);
    int returnClosestValue(QVector<QRgb> &xusbTable,QRgb value);
    QImage copyAndGenImage256(QImage & image);
    
signals:
    
public slots:
    
};

#endif // IN2ALGIMAGE_H
