#ifndef IMGVARGENERATOR_H
#define IMGVARGENERATOR_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QVector>
#include <QRgb>
#include <QColor>
#include <QFont>
#include "globals.h"
#include "MemoryMap.h"
#include "In2Barcode.h"
#include "In2BarcodeClass.h"

class ImgVarGenerator : public QObject
{
    Q_OBJECT
public:
    ImgVarGenerator(QObject *parent = 0);
    void Reset();
private:
    QFont configureFont(int i);
    QPixmap returnPixmapFromText(QPainter &p,const QString & text);

private:
    QImage ImagenPlantilla;
    QImage ImagenResultado;
    In2BarcodeClass m_barcodeInst;
    In2BarcodeClass *m_barcode;
    unsigned int Xo,Yo;
    unsigned int sx,sy;
    QVector<QRgb> table;
    long limite_inferior[NUM_CAMPOS_VARIABLES];
    long limite_superior[NUM_CAMPOS_VARIABLES];
    long valor_inicial[NUM_CAMPOS_VARIABLES];
    long incremento[NUM_CAMPOS_VARIABLES];
    long modulo[NUM_CAMPOS_VARIABLES];
    long modulo_inicial[NUM_CAMPOS_VARIABLES];
    long limite_index[NUM_CAMPOS_VARIABLES];
    long limite_index_inicial[NUM_CAMPOS_VARIABLES];
    long offset_columna[NUM_CAMPOS_VARIABLES];

    int filasMatriz;
    int columnasMatriz;
    int filasComposicion;
    int columnasComposicion;
    int impresionesPorPagina;

    int indicePagina;
    int indicePaginaModificado;
    int valorInicialPagina;
    int valor_actual;
    int moduloIndicePagina;

    bool valorActualOutOfLimits;
    MemoryMap *memoryMap;

    //In2BarcodeClass barcodeInst;
signals:

public slots:
    QImage CreaImagenVariablePorColumnas(int index,QImage::Format format);
    QImage CreaImagenVariablePorColumnasUpdated(int index,QImage::Format format);
    QImage CreaImagenVariablePorColumnasUpdated(int index,QImage::Format format,bool bHighSpeed);
    QImage CreaImagenVariableUnitaria(int index);

    int calculoNuevoIndex(unsigned char numCampo);
    void controlContadorValorInicialPagina(unsigned char numCampo);
    void valorActualFueraDeLimites(unsigned char numCampo);



};

#endif
