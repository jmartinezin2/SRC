/** @addtogroup ImageVarXaarCreator
 *  @{*/
/** @file ImageVarXaarCreator.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. 
 *
 *    @brief Generador Imagen Variable
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ImageVarXaarCreator.h
 *    @li Versión: x.0
 *    @li Fecha: 01/01/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef IMAGEVARXAARCREATOR_H
#define IMAGEVARXAARCREATOR_H
#include <QObject>
#include "globals.h"
#include "imgvargenerator.h"
#include "in2bufferimages.h"
#include <QElapsedTimer>
#include "in2imagecreator.h"

enum{
    CREATOR_PAUSED=0,
    CREATOR_LAUNCHED,
    CREATOR_STOP
};

//Estructura con datos para la impresión de BBDD
struct ConfigPrintBBDD{
    int m_FieldBBDDPrintedOK;
    int m_FieldBBDDIn2Index;
    int m_numSubImagesBBDD;
    int m_lastIndex;
};

class ImageVarXaarCreator : public QObject
{
    Q_OBJECT
public:
    explicit ImageVarXaarCreator(QObject *parent = 0,/*QList<struct In2VarImage>*/In2BufferImages *data=NULL);
    ~ImageVarXaarCreator();
    void Reset(bool precreate);
    void setNumImagenesInSwathe(int n);
    void setGenerationLimit(int n);
    void setRepeticiones(int n);
    void setColorConversion(QImage::Format n);
    void ConfigBBDDToPrint(const struct ConfigPrintBBDD & data);
    void setDataOrigin(int dataOrigin);
    QImage singleImageFromBBDD(int index);
    QImage singleImageUnitariaFromBBDD(int index);
    void setHighSpeed(bool Active);
    //void setBBDDCampos(GestorDBODBC *db);
    QStringList getVariableList();


private:
    int searchNextItemFromBBDD();
private slots:
    bool GenerateNewImage();
    bool GenerateNewImageFromBBDD();
    bool GenerateNewImageFromExternal();
    void GenerateProcess();
    void sltPrecreateImages();

signals:
    void SendDebugInfo(unsigned char code,const QString &);
    void FinishedPrecreating();
    void ImagePreCreated(int x);
    void FinishedGenerating();

public slots:
    void sltLoadExternalImage(QString txt);
    void PauseGeneration();
    void StartGeneration();
    void Stop();
    void PrecreateImages(int origin);
    void setVectorToPrintBBDD(const QVector<int> & lista);



private:
    In2BufferImages *m_data;
    char m_state;
    int m_lastGenerate;
    int m_totalGenerated;
    ImgVarGenerator *ImgVarGen;
    In2ImageCreator *In2ImgCreator;
    QVector<QRgb> table;
    int m_BufferSize;
    int m_numImagesInSwathe;
    int m_limitToGenerate;
    int m_Repeticiones;
    QImage::Format m_Format;
    int m_dataOrigin;
    struct ConfigPrintBBDD m_ConfigBBDD;
    QVector<int> m_VectorBBDD;
    bool m_bHighSpeed;
    QElapsedTimer m_Elapsedtimer;
    //GestorDBODBC *m_DataBaseCampos;
    QStringList m_PendingItems;

};

#endif // IMAGEVARXAARCREATOR_H
