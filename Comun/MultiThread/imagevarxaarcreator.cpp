/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "imagevarxaarcreator.h"
#include <QPainter>
#include <QFont>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QApplication>



ImageVarXaarCreator::ImageVarXaarCreator(QObject *parent,/*QList<struct In2VarImage>*/In2BufferImages *data) :
    QObject(parent)
{
    m_bHighSpeed=false;
    //Creamos la tabla
    table.clear();
    for( int j = 0; j < 256; ++j ){
        table.append(qRgb(j,j,j));
    }
    ImgVarGen = NULL;
    In2ImgCreator= NULL;
    m_data=data;
    m_state=CREATOR_PAUSED;
    m_lastGenerate=0;
    m_totalGenerated=0;
    m_Repeticiones=1;
    m_BufferSize=m_data->count();
    m_numImagesInSwathe=1;
    m_limitToGenerate=0;
    m_ConfigBBDD.m_FieldBBDDIn2Index=0;
    m_ConfigBBDD.m_FieldBBDDPrintedOK=0;
    m_ConfigBBDD.m_numSubImagesBBDD=1;
    m_ConfigBBDD.m_lastIndex=0;
    m_VectorBBDD.clear();
    m_dataOrigin=ORG_COMPOFILE;
    m_Format=QImage::Format_Indexed8;
    ImgVarGen = new ImgVarGenerator(this);
    In2ImgCreator= new In2ImageCreator(this);
    //m_DataBaseCampos=NULL;
}

ImageVarXaarCreator::~ImageVarXaarCreator(){

}

/** Formato de conversión a usar*/
void ImageVarXaarCreator::setColorConversion(QImage::Format n){
    m_Format=n;
}

/* Maximo numero de imagenes a generar*/
void ImageVarXaarCreator::setGenerationLimit(int n){
    m_limitToGenerate=n;
}
/** Repeticiones de cada impresion*/
void ImageVarXaarCreator::setRepeticiones(int n){
    m_Repeticiones=n;
}


/* Numero de imagenes que avanzo en datos por cada swathe*/
void ImageVarXaarCreator::setNumImagenesInSwathe(int n){
    m_numImagesInSwathe=n;
}
void ImageVarXaarCreator::setDataOrigin(int dataOrigin){
    m_dataOrigin=dataOrigin;
}

void ImageVarXaarCreator::PrecreateImages(int dataOrigin){
    m_dataOrigin=dataOrigin;
    m_Elapsedtimer.start();
    sltPrecreateImages();
}

void ImageVarXaarCreator::sltPrecreateImages(){
    int x=0;
    m_BufferSize=m_data->count();
    if (m_dataOrigin==ORG_EXTERNAL) x=m_BufferSize; //Aqui no se pre-crean imagenes
    while(x<m_BufferSize) {
        if ((m_data->at(x).m_state!=XAAR_IMAGEN_CREADA)&&(m_data->at(x).m_state!=XAAR_IMAGEN_BLANCA)) break;
        x++;
    }
    if (x!=m_BufferSize){
        if (m_dataOrigin==ORG_COMPOFILE)//Si hay que generar imagen según fichero de compo
            GenerateNewImage();
        else if (m_dataOrigin==ORG_DATABASE) //Si hay que generar imagen según BBDD
            GenerateNewImageFromBBDD();
        else if (m_dataOrigin==ORG_EXTERNAL)
            GenerateNewImageFromExternal();
        if (x==0) emit SendDebugInfo(LOGGER_WARNING,tr("Generando %1 imagenes").arg(m_BufferSize));
            emit ImagePreCreated(x+1);
            QTimer::singleShot(0,this,SLOT(sltPrecreateImages()));
        }

    else {
        double time=(double)m_Elapsedtimer.elapsed()/1000;
        emit SendDebugInfo(LOGGER_ERROR,tr("Elapsed time %1 sg").arg(time));
        emit FinishedPrecreating();
    }

}

/**Resetea*/
void ImageVarXaarCreator::Reset(bool precreate){
    m_state=CREATOR_PAUSED;
    m_lastGenerate=0;
    m_totalGenerated=0;
    m_numImagesInSwathe=1;
    m_limitToGenerate=0;
    m_Repeticiones=1;
    //m_dataOrigin=ORG_COMPOFILE;
    m_ConfigBBDD.m_FieldBBDDIn2Index=0;
    m_ConfigBBDD.m_FieldBBDDPrintedOK=0;
    m_ConfigBBDD.m_numSubImagesBBDD=1;
    m_ConfigBBDD.m_lastIndex=0;
    m_VectorBBDD.clear();
    ImgVarGen->Reset();
    m_Format=QImage::Format_Indexed8;
    if (precreate) PrecreateImages(m_dataOrigin);

}

/** Se pone a crear imagenes*/
void ImageVarXaarCreator::GenerateProcess(){
    if (m_state==CREATOR_LAUNCHED){
        if (m_dataOrigin==ORG_COMPOFILE)
            while (GenerateNewImage());
        else if (m_dataOrigin==ORG_DATABASE)
            while (GenerateNewImageFromBBDD());
        else if (m_dataOrigin==ORG_EXTERNAL)
            while(GenerateNewImageFromExternal());

      QTimer::singleShot(100,this,SLOT(GenerateProcess()));
   }
}
/** Configura la BBDD para imprimr*/
void ImageVarXaarCreator::ConfigBBDDToPrint(const struct ConfigPrintBBDD & data){
    setDataOrigin(ORG_DATABASE);
    m_ConfigBBDD=data;
}

/** Precarga datos desde la BBDD*/
bool ImageVarXaarCreator::GenerateNewImageFromBBDD(){
    bool rtn=false;
    /*struct In2VarImage data=m_data->at(m_lastGenerate);
    MemoryMap *memMap=MemoryMap::request(this);
    //Busco que tenga el slot o vacio o enviado
    if ((data.m_state!=XAAR_IMAGEN_CREADA)&&(data.m_state!=XAAR_IMAGEN_BLANCA)){
        if (m_VectorBBDD.isEmpty()){ //Seguimos la config de la BBDD
            int img=searchNextItemFromBBDD();
            if ((unsigned int)img<(m_DataBaseCampos->m_Tabla.count())/m_DataBaseCampos->CamposDatabase){ //No pasarse de rosca
                data.m_Imagen=ImgVarGen->CreaImagenVariablePorColumnas(img/m_ConfigBBDD.m_numSubImagesBBDD,QImage::Format_Indexed8);
                //data.m_Imagen=ImgVarGen->CreaImagenVariablePorColumnas(img/m_ConfigBBDD.m_numSubImagesBBDD);
                img+=m_ConfigBBDD.m_numSubImagesBBDD;
                m_ConfigBBDD.m_lastIndex=img;
                data.m_state=XAAR_IMAGEN_CREADA;
            }
            else {
                if (data.m_state!=XAAR_IMAGEN_BLANCA){ //Sio es blanca no habrá cambiado su status
                    QPixmap dummyPixmap(memMap->m_Imagen.width(),memMap->m_Imagen.height());
                    dummyPixmap.fill(Qt::white);
                    QImage dummyImage=dummyPixmap.toImage().convertToFormat(m_Format,table,Qt::AutoColor);
                    data.m_Imagen=dummyImage;
                    data.m_state=XAAR_IMAGEN_BLANCA;
                }
                    //emit FinishedGenerating(); //Como mucho tantos avisos como tamaño de buffer
            }

            //int x=data.m_Imagen.colorCount();
            m_data->replace(m_lastGenerate,data);
            //m_totalGenerated++;
            m_totalGenerated+=m_numImagesInSwathe;
            m_lastGenerate++;
            //Nos damos la vuelta
            m_lastGenerate=m_lastGenerate%m_BufferSize;
            rtn=true;
        }
        else{//Seguimos un vector
            int item=m_VectorBBDD.first();
            m_VectorBBDD.pop_front();
            data.m_Imagen=ImgVarGen->CreaImagenVariableUnitaria(item);
            data.m_state=XAAR_IMAGEN_CREADA;
            m_data->replace(m_lastGenerate,data);
            //m_totalGenerated++;
            m_totalGenerated+=m_numImagesInSwathe;
            m_lastGenerate++;
            //Nos damos la vuelta
            m_lastGenerate=m_lastGenerate%m_BufferSize;
            rtn=true;
            //if (m_VectorBBDD.isEmpty()) PauseGeneration();
        }
    }*/
    return rtn;

}
/** Busca siguiente item de la BBDD*/
int ImageVarXaarCreator::searchNextItemFromBBDD(){
    //El ultimo campo de la BBDD es la confirmacion de la impresión
     int item=0;
     /*int step=m_DataBaseCampos->CamposDatabase;
     int end=(m_DataBaseCampos->m_Tabla.count())/m_DataBaseCampos->CamposDatabase;

     for (item=m_ConfigBBDD.m_lastIndex;item<end;item++){
         int dummy=QString(m_DataBaseCampos->m_Tabla[item*step+this->m_ConfigBBDD.m_FieldBBDDPrintedOK]).toInt();
         if (dummy!=1) break;
     }
    m_ConfigBBDD.m_lastIndex=item;*/
    return item;
}

/** Establece la BBDD*/
/*void ImageVarXaarCreator::setBBDDCampos(GestorDBODBC *db){
    m_DataBaseCampos=db;
}*/

/** Carga un vector con determinados items*/
void ImageVarXaarCreator::setVectorToPrintBBDD(const QVector<int> &lista){
    this->m_VectorBBDD=lista;
}
/** Config de alta velocidad*/
void ImageVarXaarCreator::setHighSpeed(bool active){
    m_bHighSpeed=active;
}

/** Funcion que genera imagenes*/
bool ImageVarXaarCreator::GenerateNewImage(){
    bool rtn=false;
    struct In2VarImage data=m_data->at(m_lastGenerate);
    MemoryMap *memMap=MemoryMap::request(this);
    //Busco que tenga el slot o vacio o enviado
    if (data.m_state!=XAAR_IMAGEN_CREADA){
        if (m_totalGenerated<m_limitToGenerate) {
            data.m_Imagen=In2ImgCreator->CreaImagenVariablePorColumnasUpdated(m_totalGenerated/m_Repeticiones,m_Format,m_bHighSpeed);
            //data.m_Imagen.save(QString("%1\\%2.bmp").arg(QApplication::applicationDirPath()).arg(m_totalGenerated));
        }
        else {
            QPixmap dummyPixmap(memMap->m_Imagen.width(),memMap->m_Imagen.height());
            dummyPixmap.fill(Qt::white);
            QImage dummyImage=dummyPixmap.toImage().convertToFormat(m_Format,table,Qt::AutoColor);
            data.m_Imagen=dummyImage;
        }
        data.m_state=XAAR_IMAGEN_CREADA;
        data.m_copyCount=1;
        //int x=data.m_Imagen.colorCount();
        m_data->replace(m_lastGenerate,data);
        m_totalGenerated+=m_numImagesInSwathe;
        m_lastGenerate++;
        //Nos damos la vuelta
        m_lastGenerate=m_lastGenerate%m_BufferSize;
        rtn=true;
    }
    return rtn;
}

/** Funcion que genera imagenes, pero desde el exterior*/
bool ImageVarXaarCreator::GenerateNewImageFromExternal(){
    bool rtn=false;
    struct In2VarImage data=m_data->at(m_lastGenerate);
    MemoryMap *memMap=MemoryMap::request(this);
    //Busco que tenga el slot o vacio o enviado...cojo de la lista, veo si es imagen y la encasqueto
    if (data.m_state!=XAAR_IMAGEN_CREADA){
        if (m_totalGenerated<m_limitToGenerate) {
            if (m_PendingItems.count()){
                data.file=m_PendingItems.first();
                //data.m_Imagen=QImage(data.file);
                data.m_Imagen=QImage(4000,1000,QImage::Format_Indexed8);
                QVector<QRgb> table;
                table.resize(256);
                data.m_Imagen.setColorTable(table);
                if (data.m_Imagen.isNull()){
                    m_PendingItems.removeFirst();
                    data.file.clear();
                    return rtn;
                }
                else{
                    m_PendingItems.removeFirst();
                }
            }
            else{
                return rtn;
            }
            //data.m_Imagen=In2ImgCreator->CreaImagenVariablePorColumnasUpdated(m_totalGenerated/m_Repeticiones,m_Format,m_bHighSpeed);
            //data.m_Imagen.save(QString("%1\\%2.bmp").arg(QApplication::applicationDirPath()).arg(m_totalGenerated));
        }
        else {
            QPixmap dummyPixmap(memMap->m_mapa.m_variables.WebWidth*14.18,memMap->m_mapa.m_variables.WebHeight*14.18);
            dummyPixmap.fill(Qt::white);
            QImage dummyImage=dummyPixmap.toImage().convertToFormat(m_Format,table,Qt::AutoColor);
            data.m_Imagen=dummyImage;
        }
        data.m_state=XAAR_IMAGEN_CREADA;
        data.m_copyCount=1;
        //int x=data.m_Imagen.colorCount();
        m_data->replace(m_lastGenerate,data);
        m_totalGenerated+=m_numImagesInSwathe;
        m_lastGenerate++;
        //Nos damos la vuelta
        m_lastGenerate=m_lastGenerate%m_BufferSize;
        rtn=true;
    }
    return rtn;
}


/** Se pausa la generacion*/
void ImageVarXaarCreator::PauseGeneration(){
    m_state=CREATOR_PAUSED;
    emit SendDebugInfo(LOGGER_ERROR,tr("ImageVarXaarCreator PAUSED"));
    //this->deleteLater();
}
/** Se pausa la generacion*/
void ImageVarXaarCreator::StartGeneration(){
    m_state=CREATOR_LAUNCHED;
#if QT_VERSION >=0x050000
    //DWORD id=(DWORD)QThread::currentThreadId();
    //emit SendDebugInfo(LOGGER_ERROR,QString("STARTING CREATOR ID:%1").arg(id));
#else
    DWORD id=(DWORD)QThread::currentThreadId();
    emit SendDebugInfo(LOGGER_ERROR,tr("STARTING CREATOR ID:%1").arg(id));
#endif
    QTimer::singleShot(0,this,SLOT(GenerateProcess()));

}
/** Terminamos el hilo*/
void ImageVarXaarCreator::Stop(){
    emit SendDebugInfo(LOGGER_ERROR,tr("ImageVarXaarCreator STOPED"));
    m_state=CREATOR_STOP;
    this->deleteLater();
}

/** Devuelve una imagen por indice*/
QImage ImageVarXaarCreator::singleImageFromBBDD(int index){
    return ImgVarGen->CreaImagenVariablePorColumnas(index/m_ConfigBBDD.m_numSubImagesBBDD,QImage::Format_Indexed8);
    //return ImgVarGen->CreaImagenVariablePorColumnas(index/m_ConfigBBDD.m_numSubImagesBBDD);

}
/** Devuelve una imagen unitaria*/
QImage ImageVarXaarCreator::singleImageUnitariaFromBBDD(int index){
    return ImgVarGen->CreaImagenVariableUnitaria(index);
}

/** Carga una imagen external en la lista de pnedings*/
void ImageVarXaarCreator::sltLoadExternalImage(QString txt){
    m_PendingItems.append(txt);
}

/** Devuelve la lista pending*/
QStringList ImageVarXaarCreator::getVariableList(){
    return m_PendingItems;
}

