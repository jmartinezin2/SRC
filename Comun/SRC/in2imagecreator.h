#ifndef IN2IMAGECREATOR_H
#define IN2IMAGECREATOR_H

#include <QObject>
#include <QVector>
#include <QRgb>
#include "globals.h"
#include "MemoryMap.h"

//Pixeles de margen...dejamos un cabezal
#define DEFAULT_MARGIN 0

class In2ImageCreator : public QObject
{
    Q_OBJECT
public:
    explicit In2ImageCreator(QObject *parent = 0);
    
signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void sgnImgCrtInit();
    void sgnImgCrtEnd();
    void sgnImgCrtProgress(int x,int total);
    void sgnDitherCompleted();
public:
    void CargaPlantilla();
    QPixmap DrawFixedItem(int i, int matriz);
    QPixmap DrawVariableItem(int i, int matriz);
    QPixmap returnImageWithMargin(const QPixmap & pixmap);
    QImage ditherMono(QImage image);
    QImage ditherMono(QString image);
    void Convert();
    QImage getColorImage(QString dummy_bmp,int index);
    QVector<int> getTonos(const QImage & img);

private slots:
    void sltSabreAlgorithm();
    void sltDilateImage();
private:
    QImage ditherImage(QImage & image,struct m_Colores & color);
    void CopiarBBDD();
    QString _CopiarBBDD();
    void GeneraFicheroMascaraComposicion();
    void drawHoverRect(QPainter &p, QRect rect,int index);
    QPixmap convertToTransparent(const QPixmap & q);
    QPixmap returnPixmapFromText(QPainter & p,const QString & text);
    QSize drawBarcode(QPainter &p,struct m_BarcodeParameters bc, int Orientacion,QString text);
    QFont _setFont(int i, bool campoFijo);
    QString _getCounterValue(int campo,int fila, int columna,int impresion);
    QString _getBBDDValue(int campo);
    QString _getDateTimeValue(int campo);
    QString _getRandomValue(int campo);



public slots:
    void sltGenerateImage();
    void sltGenerateImageToPrint();
    void sltCreaImagenFondo(unsigned int indice);
    void sltCreaImagenUnitariaEnBlanco(unsigned int indice);
    void sltCreaImagenUnitariaColoresPlenos(unsigned int indice);
    void sltCreaImagenEnviarColoresPlenos();
    QImage CreaImagenVariablePorColumnasUpdated(int index,QImage::Format format,bool bHighSpeed);
    QPixmap returnCompoImage(QString name);
    QPixmap returnCompoImage(QString name1,QString name2);
    QPixmap returnCompoImageExt(QString name);

private:
    QImage m_genImageToSend;
    QPixmap m_PixmapUnitario;
    //QList<QPixmap> qgenpixMapArray;//Lista de pixmaps a generar para el caso de monocromo
    QList<QImage> pixMapArray[NUM_COLORES]; //Lista de pixmaps a generar para el caso de colores plenos
    //QPixmap PixmapUnitario[NUM_COLORES];
    //QImage genImageToSend[NUM_COLORES];
    QVector<QRgb> table;
    MemoryMap *m_memoryMap;
    int m_Impresiones;
};

#endif // IN2IMAGECREATOR_H
