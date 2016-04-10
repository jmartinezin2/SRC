#ifndef IMGFIXGENERTOR_H
#define IMGFIXGENERTOR_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include "globals.h"
#include "MemoryMap.h"

class ImgFixGenerator : public QObject
{
    Q_OBJECT
public:
    ImgFixGenerator(QObject *parent = 0, QString CurrentNombreRutaIni="",int iCabezales=0);

private:
    QString NombreRutaIniActual;
    QPixmap PixmapUnitario[NUM_COLORES];
    QImage genImageToSend[NUM_COLORES];
    int m_NumCabezales;
    MemoryMap *m_memoryMap;
signals:

public slots:
    void AbrirFicheroComposicionFija();
    void CreaImagenUnitaria_ColoresPlenos(unsigned int indice);
    void CreaImagenEnviar_ColoresPlenos();
    void CargaPlantilla();
private slots:


};

#endif
