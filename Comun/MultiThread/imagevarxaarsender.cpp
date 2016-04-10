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
#include "imagevarxaarsender.h"
#include "in2defines.h"
#include <QThread>
#include <QTimer>
#include <QDebug>

#define MAXROWS 1
#define MAXHEADS 5
//Para no saturar el SO
#define NUM_SWATHE_BASE_SEND 1
//En caso de no tener imagen para enviar no hace falta que preguntemos tan seguido
#define TIME_NO_SWATHE 500
#define TIME_SWATHE 250
//Timeout para hacer update de datos
#define UPDATE_SIGNALS_TIMEOUT 1000

ImageVarXaarSender::ImageVarXaarSender(QObject *parent, int Card,/*QList<struct In2VarImage>*/In2BufferImages *data) :
    QObject(parent)
{
    m_Card=Card;
    m_data=data;
    m_lastSentSwathe=0;
    m_TotalSwatheSent=0;
    m_BufferSize=m_data->count();
    newCard=new InkjetXusbVarCtrl(0,Card,"",m_BufferSize);
    m_state=SENDER_PAUSED;
    m_DataOrigin=ORG_COMPOFILE;
}

/** Cambia el fichero de configuracion*/
void ImageVarXaarSender::setNewConfigFile(QString filename){
    newCard->setNewConfigFile(filename);
}

/** Reset*/
void ImageVarXaarSender::Reset(){
     m_BufferSize=m_data->count();
     m_lastSentSwathe=0;
     m_TotalSwatheSent=0;
     newCard->Reset();
     newCard->setNumImages(m_BufferSize);
     m_state=SENDER_PAUSED;
}
/** Cambia la paleta*/
void ImageVarXaarSender::changePalette(char *palette){
    newCard->setNewFullPalette(palette);
}

/** Recibe la ultima swathe impresa*/
void ImageVarXaarSender::XaarLastSwathePrinted(int lastSwathe){
    int lastPrintedSwathe=lastSwathe;
    int swatheToSend=0;
    int swatheSent=0;
    MemoryMap *memMap=MemoryMap::request(this);
    //Se ha dado la vuelta...actualizo
    if (m_lastSentSwathe>lastPrintedSwathe){
        swatheToSend=(m_BufferSize-m_lastSentSwathe)+lastPrintedSwathe;
    }
    else swatheToSend=lastPrintedSwathe-m_lastSentSwathe;
    //Mandamos swathe
    int index=(m_lastSentSwathe+swatheSent)%m_BufferSize;

    if (swatheSent<swatheToSend){
        emit LastSwatheReplaced(swatheToSend);
        //emit SwatheToReplace(swatheToSend);
    }
    //emit SendDebugInfo(LOGGER_WARNING,QString("index %1 sent %2 toSend %3").arg(index).arg(swatheSent).arg(swatheToSend));
    while (swatheSent<swatheToSend){
        //Enviamos swathe
        struct In2VarImage data=m_data->at(index);
         if(data.m_state==XAAR_IMAGEN_CREADA){
            //AQUI ENVIAR
            newCard->SetImage(data.m_Imagen);
            for (int head=0;head<memMap->m_mapa.m_variables.DefaultNumHeads;head++){
                for (int row=0;row<MAXROWS;row++){
                    newCard->ReplaceSwathe(head,row,index);
                }
            }
            data.m_state=XAAR_IMAGEN_ENVIADA;
            m_data->replace(index,data);
            //qDebug()<<QString("Replaced swathe %1").arg(index);
            swatheSent++;
            index=(m_lastSentSwathe+swatheSent)%m_BufferSize;
        }
        else{
            //O es vacia o es enviada.
            break;
        }
    }
  m_lastSentSwathe+=swatheSent;
  if (m_lastSentSwathe>=m_BufferSize) {
        emit CompleteBufferReplaced();
  //Nos damos la vuelta
    m_lastSentSwathe=m_lastSentSwathe%m_BufferSize;
  }
}

/** Comineza a enviar*/
void ImageVarXaarSender::sltStartSending(){
    m_state=SENDER_LAUNCHED;
#if QT_VERSION >=0x050000
    //DWORD id=(DWORD)QThread::currentThreadId();
    //emit SendDebugInfo(LOGGER_ERROR,QString("STARTING SENDER ID:%1").arg(id));
#else
    DWORD id=(DWORD)QThread::currentThreadId();
    emit SendDebugInfo(LOGGER_ERROR,tr("STARTING SENDER ID:%1").arg(id));
#endif
    bool emptyBuffer=TestBufferVacio();
    if (emptyBuffer) {
        emit SendDebugInfo(LOGGER_WARNING,tr("SENDER Buffer vacio"));
        emit FinishedSending();
    }

    else{
        //bXaarScorpionGenerateProductDetect(0);
        QTimer::singleShot(0,this,SLOT(sltSendingLoop()));
    }
    //QTimer::singleShot(UPDATE_SIGNALS_TIMEOUT,this,SLOT(sltUpdateSignals()));
}
/** Comineza a enviar*/
void ImageVarXaarSender::sltNoPreloadStartSending(){
    m_state=SENDER_LAUNCHED;

#if QT_VERSION >=0x050000
    //DWORD id=(DWORD)QThread::currentThreadId();
    //emit SendDebugInfo(LOGGER_ERROR,QString("STARTING SENDER ID:%1").arg(id));
#else
    DWORD id=(DWORD)QThread::currentThreadId();
    emit SendDebugInfo(LOGGER_ERROR,tr("STARTING SENDER ID:%1").arg(id));
#endif
    bool emptyBuffer=TestBufferVacio();
    if (emptyBuffer) {
        emit SendDebugInfo(LOGGER_WARNING,tr("SENDER Buffer vacio"));
        emit FinishedSending();
    }

    else{
        //bXaarScorpionGenerateProductDetect(0);
        m_memoryEmpty=true;
        QTimer::singleShot(0,this,SLOT(sltNoPreloadSendingLoop()));
    }
    //QTimer::singleShot(UPDATE_SIGNALS_TIMEOUT,this,SLOT(sltUpdateSignals()));
}



/** Pausa el envio*/
void ImageVarXaarSender::sltPauseSending(){
    m_state=SENDER_PAUSED;
    emit SendDebugInfo(LOGGER_ERROR,tr("ImageVarXaarSender PAUSED"));
}
/** Pausa el envio*/
void ImageVarXaarSender::sltStopSending(){
    emit SendDebugInfo(LOGGER_ERROR,tr("ImageVarXaarSender STOPED"));
    m_state=SENDER_STOP;
    this->deleteLater();
}

/** Bucle para ir enviando imagenes*/
void ImageVarXaarSender::sltSendingLoop(){
    //static int prevSwatheToSend=0;
    if (m_state==SENDER_LAUNCHED){
        int lastSwathe=newCard->GetCurrentPrintSwathe();
        int lastPrintedSwathe=lastSwathe;
        int swatheToSend=0;
        int swatheSent=0;
        MemoryMap *memMap=MemoryMap::request(this);
        //Se ha dado la vuelta...actualizo
        if (m_lastSentSwathe>lastPrintedSwathe){
            swatheToSend=(m_BufferSize-m_lastSentSwathe)+lastPrintedSwathe;
        }
        else swatheToSend=lastPrintedSwathe-m_lastSentSwathe;
        //Mandamos swathe
        int index=(m_lastSentSwathe+swatheSent)%m_BufferSize;

        if (swatheSent<swatheToSend){
            /*if (swatheToSend!=prevSwatheToSend){
                emit LastSwatheReplaced(swatheToSend);
                prevSwatheToSend=swatheToSend;
            }*/
            emit LastSwatheReplaced(swatheToSend);
            m_TotalSwatheSent+=swatheToSend;
            //SendDebugInfo(LOGGER_WARNING,QString("toSend %3").arg(swatheToSend));
            //emit SwatheToReplace(swatheToSend);
        }
        //emit SendDebugInfo(LOGGER_WARNING,QString("index %1 sent %2 toSend %3").arg(index).arg(swatheSent).arg(swatheToSend));
        while (swatheSent<swatheToSend){
            //Enviamos swathe
            struct In2VarImage data=m_data->at(index);
            int cabezal=data.cabezal;
            if((data.m_state==XAAR_IMAGEN_CREADA)||(data.m_state==XAAR_IMAGEN_BLANCA)){ //Envio imagen si es blanca o creada
                    //AQUI ENVIAR
                newCard->SetImage(data.m_Imagen);
                if (cabezal==0xFFFFFFFF){ //Original, la imagen se reparte entre todos los cabezales
                    for (int head=0;head<memMap->m_mapa.m_variables.DefaultNumHeads;head++){
                        for (int row=0;row<MAXROWS;row++){
                            newCard->ReplaceSwathe(head,row,index);
                        }
                    }
                }
                else{//Falta cambiar la paletta
                    for (int row=0;row<MAXROWS;row++){
                        newCard->ReplaceSwathe(cabezal,row,index);
                    }
                }
                if (data.m_state==XAAR_IMAGEN_BLANCA){ //Si es imagen blanca no la marco como enviada
                    bool emptyBuffer=TestBufferVacio();
                    if (emptyBuffer){
                        emit SendDebugInfo(LOGGER_WARNING,tr("SENDER Buffer vacio"));
                        emit FinishedSending();
                    }
                }
                else{
                    data.m_state=XAAR_IMAGEN_ENVIADA;
                    m_data->replace(index,data);
                }
                    //qDebug()<<QString("Replaced swathe %1").arg(index);
                swatheSent++;
                index=(m_lastSentSwathe+swatheSent)%m_BufferSize;
                    //Testeo si el buffer es completamente blanco
             }
             else{
                //O es vacia o es enviada.
                break;
             }
        }
        m_lastSentSwathe+=swatheSent;
        if (m_lastSentSwathe>=m_BufferSize) {
            //emit CompleteBufferReplaced();
            //Nos damos la vuelta
            m_lastSentSwathe=m_lastSentSwathe%m_BufferSize;
        }
        if (swatheToSend) QTimer::singleShot(TIME_SWATHE,this,SLOT(sltSendingLoop()));
        else QTimer::singleShot(TIME_NO_SWATHE,this,SLOT(sltSendingLoop()));
    }
}


/** Bucle para ir enviando imagenes*/
void ImageVarXaarSender::sltNoPreloadSendingLoop(){
    static int x=0;
    int currentPrintSwathe=newCard->GetCurrentPrintSwathe();
    if (m_state==SENDER_LAUNCHED){
        if (m_memoryEmpty){ //Esta es la primera vez, vamos cargando con datos de impresion
            if ((x<currentPrintSwathe)||(x==0)){
                struct In2VarImage data=m_data->at(x);
                if((data.m_state==XAAR_IMAGEN_CREADA)||(data.m_state==XAAR_IMAGEN_BLANCA)){ //Envio imagen si es blanca o creada
                    newCard->setNumImages(m_BufferSize);
                    newCard->setCopyCount(data.m_copyCount);
                    newCard->SetImage(data.m_Imagen);
                    //int xy=data.m_Imagen.colorCount();
                    if (x==0) {
                        newCard->SendFirstImage();
                        //newCard->SendImage();
                    }
                    else{
                        if (x==m_BufferSize-1){
                            newCard->SetLastSwathe();
                            m_memoryEmpty=false; //Como si le hubiera hecho el preload
                        }
                        newCard->SendImage();
                        //qDebug()<<QString("Preloaded swathe %1").arg(x);
                    }
                    data.m_state=XAAR_IMAGEN_ENVIADA;
                    if (x==0) newCard->SetupPrintMode();
                    x++;
                }
            }
        }
        else{ //aqui reemplazamos de manera normal

        }
        QTimer::singleShot(TIME_NO_SWATHE,this,SLOT(sltNoPreloadSendingLoop()));
    }
}




/** Bucle para ir enviando imagenes, pero a cada cabezal la suya*/
void ImageVarXaarSender::sltSendingCMYKLoop(){
    //static int prevSwatheToSend=0;
    if (m_state==SENDER_LAUNCHED){
        int lastSwathe=newCard->GetCurrentPrintSwathe();
        int lastPrintedSwathe=lastSwathe;
        int swatheToSend=0;
        int swatheSent=0;
        //Se ha dado la vuelta...actualizo
        if (m_lastSentSwathe>lastPrintedSwathe){
            swatheToSend=(m_BufferSize-m_lastSentSwathe)+lastPrintedSwathe;
        }
        else swatheToSend=lastPrintedSwathe-m_lastSentSwathe;
        //Mandamos swathe
        int index=(m_lastSentSwathe+swatheSent)%m_BufferSize;

        if (swatheSent<swatheToSend){
            emit LastSwatheReplaced(swatheToSend);
            m_TotalSwatheSent+=swatheToSend;
        }
        while (swatheSent<swatheToSend){
            //Enviamos swathe
            struct In2VarImage data=m_data->at(index);
            if((data.m_state==XAAR_IMAGEN_CREADA)||(data.m_state==XAAR_IMAGEN_BLANCA)){ //Envio imagen si es blanca o creada
                //AQUI ENVIAR...Ojo porque habrá que cambiar la paleta
                newCard->SetImage(data.m_Imagen);
                for (int row=0;row<MAXROWS;row++){
                    newCard->ReplaceSwathe(data.cabezal,row,index);
                }
                if (data.m_state==XAAR_IMAGEN_BLANCA){ //Si es imagen blanca no la marco como enviada
                    bool emptyBuffer=TestBufferVacio();
                    if (emptyBuffer){
                        emit SendDebugInfo(LOGGER_WARNING,tr("SENDER Buffer vacio"));
                        emit FinishedSending();
                    }
                }
                else{
                    data.m_state=XAAR_IMAGEN_ENVIADA;
                    m_data->replace(index,data);
                }
                //qDebug()<<QString("Replaced swathe %1").arg(index);
                swatheSent++;
                index=(m_lastSentSwathe+swatheSent)%m_BufferSize;
                //Testeo si el buffer es completamente blanco
            }
            else{
                //O es vacia o es enviada.
                break;
            }
        }
        m_lastSentSwathe+=swatheSent;
        if (m_lastSentSwathe>=m_BufferSize) {
            //emit CompleteBufferReplaced();
            //Nos damos la vuelta
            m_lastSentSwathe=m_lastSentSwathe%m_BufferSize;
        }
        if (swatheToSend) QTimer::singleShot(TIME_SWATHE,this,SLOT(sltSendingCMYKLoop()));
        else QTimer::singleShot(TIME_NO_SWATHE,this,SLOT(sltSendingCMYKLoop()));
    }
}


/** Testea si el buffer está vacio*/
bool ImageVarXaarSender::TestBufferVacio(){
    bool rtn=true;
    for (int x=0;x<m_data->count();x++){
        if (m_data->at(x).m_state!=XAAR_IMAGEN_BLANCA){
            rtn=false;
            break;
        }
    }
    return rtn;
}

/** Hace la primera precarga*/
void ImageVarXaarSender::PreloadImages(){
        int x=0;
        m_BufferSize=m_data->count();
        while (x<m_BufferSize){
            if (m_data->at(x).m_state!=XAAR_IMAGEN_ENVIADA) break;
            x++;
        }

        if (x<m_BufferSize){
            PreloadSingleImage(x);
            //emit SendDebugInfo(LOGGER_WARNING,QString("Loading %1/%2").arg(x).arg(m_BufferSize));
            emit ImagePreLoaded(x+1);
            QTimer::singleShot(0,this,SLOT(PreloadImages()));
            //PreloadImages();
        }
        else{
            if (m_DataOrigin==ORG_EXTERNAL){
                for (x=0;x<m_BufferSize;x++){
                    struct In2VarImage data=m_data->at(x);
                    data.m_state=XAAR_IMAGEN_CREADA;
                    m_data->replace(x,data);
                }
            }
            newCard->SetupPrintMode();
            emit FinishedPreLoading();
        }
}
/** Establece si hay preload de imagenes*/
void ImageVarXaarSender::setDataOrigin(int origin){
    m_DataOrigin=origin;
}

/** Carga la imagen donde se le diga*/
void ImageVarXaarSender::PreloadSingleImage(int x){
    if (m_DataOrigin!=ORG_EXTERNAL){
        struct In2VarImage data=m_data->at(x);
        newCard->setNumImages(m_BufferSize);
        newCard->setCopyCount(data.m_copyCount);
        newCard->SetImage(data.m_Imagen);

        //int xy=data.m_Imagen.colorCount();
        if (x==0) newCard->SendFirstImage();
        else{
            if (x==m_BufferSize-1){
                newCard->SetLastSwathe();
            }
            newCard->SendImage();
            //qDebug()<<QString("Preloaded swathe %1").arg(x);

        }
        data.m_state=XAAR_IMAGEN_ENVIADA;
        m_data->replace(x,data);
    }
    else{
        MemoryMap *memMap=MemoryMap::request(this);
        struct In2VarImage data;
        data.m_Imagen=QImage(1000,2000,QImage::Format_Indexed8);
        data.m_Imagen.fill(Qt::white);
        QVector<QRgb> table;
        table.resize(256);
        table.fill(QColor(Qt::white).rgb());
        data.m_Imagen.setColorTable(table);
                data.cabezal=x%(memMap->m_mapa.m_variables.DefaultNumHeads);
        newCard->setNumImages(m_BufferSize);
        newCard->setCopyCount(data.m_copyCount);
        newCard->SetImage(data.m_Imagen);
        //int xy=data.m_Imagen.colorCount();
        if (x==0) newCard->SendFirstImage();
        else{
            if (x==m_BufferSize-1){
                newCard->SetLastSwathe();
            }
            newCard->SendImage();
            //qDebug()<<QString("Preloaded swathe %1").arg(x);

        }
        data.m_state=XAAR_IMAGEN_ENVIADA;
        m_data->replace(x,data);
    }
}

/** Emite señales con informacion si está corriendo*/
void ImageVarXaarSender::sltUpdateSignals(){
    /*if (m_state==SENDER_LAUNCHED){
        emit TotalSwatheReplaced(m_TotalSwatheSent);
        QTimer::singleShot(UPDATE_SIGNALS_TIMEOUT,this,SLOT(sltUpdateSignals()));
    }*/
}
