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
#include "gestorimpresion.h"
#include <QTimer>
#include <QFile>
#include <QMessageBox>
//#include <QSound>
#include "inkjetdebug.h"
#ifndef NO_OPC
    #include "clienteopc.h"
#endif
#include "in2message.h"

//Cada 500 msg se pregunta
#define TIMEOUT_PRINT_MONITOR 500
//Gota por defecto en la tabla por defecto
#define DEFAULT_DROP 3

GestorImpresion *GestorImpresion::mInstance  = NULL;



//Solo para iniciar el driver
/*class initThread:public QThread{
    private:
        void run(){
            bDriverLoaded=bXaarScorpionDriverLoaded();
            if (bDriverLoaded){
                int cards=XaarScorpionGetXUSBCount();
                for (int x=0;x<cards;x++){
                    bXaarScorpionHeadPowerControl(x,0x1f,true);
                }
            }

        }
};*/

/** devuelve si se ha cargado el driver*/
bool GestorImpresion::isDriverLoaded(){
    return bDriverLoaded;
}

/** Singleton*/
GestorImpresion *GestorImpresion::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new GestorImpresion(parent);
    }
    return mInstance;

}

void GestorImpresion::deleteModule(){
    delete (mInstance);
    mInstance=NULL;
}
/** Devuelve si tenemos imagen enviada*/
bool GestorImpresion::isImagenEnviada(){
    return m_ImagenEnviada;
}

/** Constructor*/
GestorImpresion::GestorImpresion(QObject *parent) :
    QObject(parent)
{
    bDriverLoaded=false;
    m_inumCards=0;
    bInit=false;
    m_Repeticiones=1;
    m_VariableDataOrigin=ORG_COMPOFILE;
    m_ImagenEnviada=false;
    memoryMap=MemoryMap::request(this);
}
/** Inicializa el gestor*/
bool GestorImpresion::InitGestor(){
    /*initThread *t=new initThread();
    connect(t,SIGNAL(finished()),this,SLOT(_init()));
    connect(t,SIGNAL(finished()),t,SLOT(deleteLater()));
    t->start();*/
    _init();
    return bDriverLoaded;
}



/** Init interno. Esperamos a que termine de cargar el driver*/
void GestorImpresion::_init(){
    m_bHighSpeedActive=false;
    m_AuxPalette[0]=DEFAULT_DROP;
    m_AuxPalette[1]=DEFAULT_DROP;
    m_AuxPalette[2]=DEFAULT_DROP;
    m_AuxPalette[3]=DEFAULT_DROP;
    m_AuxPalette[4]=DEFAULT_DROP;
    m_AuxPalette[5]=DEFAULT_DROP;
    m_AuxPalette[6]=0;
    m_AuxPalette[7]=0;

    m_ConfigFile=CONFIG_FILE_NORMALSPEED;

    bDriverLoaded=bXaarScorpionDriverLoaded();

    QFile dummyFile(XUSBDLL);
    if (dummyFile.exists()){
        dummyFile.remove();
        emit SendDebugInfo(LOGGER_WARNING,tr("Eliminado XUSBDLL.xml previo"));
    }

    if (!bDriverLoaded){
        int LastError=XaarScorpionGetErrorValue();
        QString dummy=tr("Error driver XUSB. Last Error:%1").arg(LastError);
        emit SendDebugInfo(LOGGER_ERROR,dummy);
    }
    else{

        QString dummy=QString("Driver-XUSB cargado correctamente");
        emit SendDebugInfo(LOGGER_WARNING,dummy);
    }

    m_VariableBufferData.clear();


    sltRefreshInfo(); //Refresca informacion de numero de cabezales y tarjetas

    for (int x=0;x<m_inumCards;x++){
        InkjetXusbCtrl *p=new InkjetXusbCtrl(this,x);
        m_cards.append(p);
        connect(p, SIGNAL(error(unsigned char,QString)),this, SLOT(sltSendDebugInfo(unsigned char,QString)));
    }


    if (m_inumCards>1) {
        /*int time=memoryMap->m_mapa.m_variables.DefaultNumHeads*m_inumCards*TIME_HEAD_TEST+5000;
        QTimer::singleShot(time,this,SLOT(sltTestCards()));*/
        sltTestCards();
    }

    sltPowerDownHeads();
    QTimer::singleShot(5000,this,SLOT(sltPowerUpHeads()));
    m_MonitorPrintCount=false;
    m_VariableGenLimit=0;
    threadCreator=NULL;
    threadSender=NULL;
}

/** Apaga los cabezales*/
void GestorImpresion::sltPowerDownHeads(){
    for (int x=0;x<m_inumCards;x++){
        InkjetXusbCtrl *p=m_cards.at(x);
        p->powerControlHeads(0xFF,false);
    }
}
/** Enciende los cabezales*/
void GestorImpresion::sltPowerUpHeads(){
    for (int x=0;x<m_inumCards;x++){
        InkjetXusbCtrl *p=m_cards.at(x);
        p->powerControlHeads(0xFF,true);
    }
}

/** Numero maximo de imagenes a generar*/
void GestorImpresion::setVariableGenerationLimit(int x){
    m_VariableGenLimit=x;
}

/** Devuelve una imagen del buffer variable*/
struct In2VarImage GestorImpresion::getVariableImage(int card,int index){
    struct In2VarImage p;
    if (index<m_VariableBufferData.count()){
        p=m_VariableBufferData.at(index);
     }
    return p;
}
/** Origen de los datos variables*/
void GestorImpresion::setVariableDataOrigin(int origin){
    m_VariableDataOrigin=origin;
}
/** Crea la interfaz variable*/
void GestorImpresion::sltCreateVarImages(){
    /*if (threadCreator==NULL){
        threadCreator=new QThread(this);
        threadCreator->start();
    }*/
    //qDeleteAll(m_ImageCreator.begin(), m_ImageCreator.end());
    m_ImageCreator.clear();
    //m_inumCards=1;
    for (int x=0;x<m_inumCards;x++){
        QThread *t=new QThread();
        t->start();
        ImageVarXaarCreator* ImageCreator=new ImageVarXaarCreator(0,&m_VariableBufferData);
        ImageVarXaarCreator* foo=ImageCreator;
        m_ImageCreator.append(foo);
        //Numero maximo de imagenes a generar
        ImageCreator->setDataOrigin(m_VariableDataOrigin);
        ImageCreator->setGenerationLimit(m_VariableGenLimit);
        ImageCreator->setRepeticiones(m_Repeticiones);
        ImageCreator->setColorConversion(QImage::Format_Indexed8);
        if (m_bHighSpeedActive){
            ImageCreator->setHighSpeed(true);
        }
        else
            ImageCreator->setHighSpeed(false);
        //Precarga de imagenes
        connect(this,SIGNAL(StartPrecreate(int)),ImageCreator,SLOT(PrecreateImages(int)));
        connect(this,SIGNAL(StartGenerating()),ImageCreator,SLOT(StartGeneration()));
        connect(this,SIGNAL(PauseGenerating()),ImageCreator,SLOT(PauseGeneration()));
        connect(this,SIGNAL(StopGenerating()),ImageCreator,SLOT(Stop()));
        connect(ImageCreator,SIGNAL(destroyed()),t,SLOT(deleteLater()));
        connect(this,SIGNAL(newExternalImage(QString)),ImageCreator,SLOT(sltLoadExternalImage(QString)));
        connect(ImageCreator,SIGNAL(FinishedPrecreating()),this,SLOT(sltLoadVarImages()));
        connect(ImageCreator, SIGNAL(SendDebugInfo(unsigned char,QString)),this, SLOT(sltSendDebugInfo(unsigned char,QString)));
        connect(ImageCreator,SIGNAL(ImagePreCreated(int)),this,SLOT(sltImagePreCreated(int)));
        //ImageCreator->moveToThread(threadCreator);
        ImageCreator->moveToThread(t);
        emit StartPrecreate(m_VariableDataOrigin);
    }
}
/** Carga imagenes en la electronica*/
void GestorImpresion::sltLoadVarImages(){
    /*if (threadSender==NULL){
        threadSender=new QThread();
        threadSender->start();
    }
    qDeleteAll(m_ImageSender.begin(), m_ImageSender.end());*/
    m_ImageSender.clear();
    for (int x=0;x<m_inumCards;x++){
        QThread *t=new QThread();
        t->start();
        ImageVarXaarSender *ImageSender=new ImageVarXaarSender(0,x,&m_VariableBufferData);
        ImageVarXaarSender *foo=ImageSender;
        m_ImageSender.append(foo);
        ImageSender->changePalette(&m_AuxPalette[0]);
        ImageSender->setDataOrigin(m_VariableDataOrigin);

        if (m_bHighSpeedActive){
            ImageSender->setNewConfigFile(CONFIG_FILE_HIGHSPEED);
        }
        else{
            ImageSender->setNewConfigFile(CONFIG_FILE_NORMALSPEED);
        }
        connect(this,SIGNAL(StartPreload()),ImageSender,SLOT(PreloadImages()));
        connect(this,SIGNAL(PauseGenerating()),ImageSender,SLOT(sltPauseSending()));
        connect(this,SIGNAL(StartLoading()),ImageSender,SLOT(sltStartSending()));
        connect(this,SIGNAL(StopGenerating()),ImageSender,SLOT(sltStopSending()));
        connect(ImageSender,SIGNAL(destroyed()),t,SLOT(deleteLater()));
        connect(ImageSender,SIGNAL(LastSwatheReplaced(int)),this,SLOT(sltLastVarSwatheReplaced(int)));
        connect(ImageSender,SIGNAL(FinishedPreLoading()),this,SLOT(sltInitImpresionVariable()));
        connect(ImageSender, SIGNAL(SendDebugInfo(unsigned char,QString)),this, SLOT(sltSendDebugInfo(unsigned char,QString)));
        connect(ImageSender,SIGNAL(ImagePreLoaded(int)),this,SLOT(sltImagePreLoaded(int)));
        ImageSender->moveToThread(t);
        emit StartPreload();
    }
}
/** Pausa la impresion variable*/
void GestorImpresion::sltPauseVariablePrinting(){
    emit PauseGenerating();
}

/** Numero de imagen precreada*/
void GestorImpresion::sltImagePreCreated(int x){
    emit newImagePreCreated(x);
}
/** Numero de imagen precargada*/
void GestorImpresion::sltImagePreLoaded(int x){
    emit newImagePreLoaded(x);
}

/** Manda cuantas swathes han sido reemplazadas*/
void GestorImpresion::sltLastVarSwatheReplaced(int x){
     emit LastVarSwatheReplaced(x);
}

/** Lanza la impresion variable*/
void GestorImpresion::sltInitImpresionVariable(){
    emit StartLoading();
    emit StartGenerating();

}

/** Lanza y conecta los elementos de la impresion variable*/
void GestorImpresion::sltLaunchImpresionVariable(){
    QTimer::singleShot(0,this,SLOT(sltCreateVarImages()));
}
/** Lanza y conecta los elementos de la impresion variable*/
void GestorImpresion::sltLaunchImpresionVariableNoPrecreate(){
    QTimer::singleShot(0,this,SLOT(sltLaunchCreatorAndSenderThreads()));
}
/** Lanza y conecta los hilosde generacion y emision*/
void GestorImpresion::sltLaunchCreatorAndSenderThreads(){
    m_ImageSender.clear();
    m_ImageCreator.clear();
    for (int x=0;x<m_inumCards;x++){
        QThread *ts=new QThread();
        ts->start();
        ImageVarXaarSender *ImageSender=new ImageVarXaarSender(0,x,&m_VariableBufferData);
        ImageVarXaarSender *fooS=ImageSender;
        m_ImageSender.append(fooS);
        ImageSender->changePalette(&m_AuxPalette[0]);
        ImageSender->setDataOrigin(m_VariableDataOrigin);

        if (m_bHighSpeedActive){
            ImageSender->setNewConfigFile(CONFIG_FILE_HIGHSPEED);
        }
        else{
            ImageSender->setNewConfigFile(CONFIG_FILE_NORMALSPEED);
        }

        connect(this,SIGNAL(PauseGenerating()),ImageSender,SLOT(sltPauseSending()));
        connect(this,SIGNAL(StartLoading()),ImageSender,SLOT(sltNoPreloadStartSending()));
        connect(this,SIGNAL(StopGenerating()),ImageSender,SLOT(sltStopSending()));
        connect(ImageSender,SIGNAL(destroyed()),ts,SLOT(deleteLater()));
        connect(ImageSender,SIGNAL(LastSwatheReplaced(int)),this,SLOT(sltLastVarSwatheReplaced(int)));
        connect(ImageSender, SIGNAL(SendDebugInfo(unsigned char,QString)),this, SLOT(sltSendDebugInfo(unsigned char,QString)));
        ImageSender->moveToThread(ts);


        QThread *tc=new QThread();
        tc->start();
        ImageVarXaarCreator* ImageCreator=new ImageVarXaarCreator(0,&m_VariableBufferData);
        ImageVarXaarCreator* fooC=ImageCreator;
        m_ImageCreator.append(fooC);
        //Numero maximo de imagenes a generar
        ImageCreator->setDataOrigin(m_VariableDataOrigin);
        ImageCreator->setGenerationLimit(m_VariableGenLimit);
        ImageCreator->setRepeticiones(m_Repeticiones);
        ImageCreator->setColorConversion(QImage::Format_Indexed8);
        if (m_bHighSpeedActive){
            ImageCreator->setHighSpeed(true);
        }
        else
            ImageCreator->setHighSpeed(false);

        connect(this,SIGNAL(StartGenerating()),ImageCreator,SLOT(StartGeneration()));
        connect(this,SIGNAL(StopGenerating()),ImageCreator,SLOT(Stop()));
        connect(ImageCreator,SIGNAL(destroyed()),tc,SLOT(deleteLater()));
        connect(this,SIGNAL(newExternalImage(QString)),ImageCreator,SLOT(sltLoadExternalImage(QString)));
        connect(ImageCreator, SIGNAL(SendDebugInfo(unsigned char,QString)),this, SLOT(sltSendDebugInfo(unsigned char,QString)));
        ImageCreator->moveToThread(tc);
        emit StartGenerating();
        emit StartLoading();
    }
}



/** Reinicia el buffer de imagenes variables*/
void GestorImpresion::setVariableBufferSize(int n){
    m_VariableBufferData.clear();
    for (int x=0;x</*SIZE_BUFFER_IMAGES*/n;x++){
        //this->BufferVarImages[x].m_state=XAAR_IMAGEN_VACIA;
        struct In2VarImage p;
        p.m_state=XAAR_IMAGEN_VACIA;
        p.m_copyCount=1;
        p.cabezal=0xFFFFFFFF;
        m_VariableBufferData.append(p);
    }
}
/** Numero de repeticiones para cada etiqueta*/
void GestorImpresion::setVariableReps(int reps){
    m_Repeticiones=reps;
}

/** Devuelve el serial*/
void GestorImpresion::getSerial(int x,char *buffer){
    if (x<m_cards.count())
        m_cards.at(x)->getSerial(buffer);
    else memset(buffer,0x00,sizeof(buffer));

}
/** Precarga imagenes variables en la electronica*/
/*void GestorImpresion::preLoadVarImages(){
    for (int x=0;x<m_inumCards;x++){
        ImageVarXaarSender *p=m_ImageSender.at(x);
        p->PreloadImages();
    }
}*/
/** Limpia el Buffer variable*/
void GestorImpresion::CleanVariableBuffer(){
    m_VariableBufferData.clear();
}
/** Añade imagen al Buffer variable de impresion*/
void GestorImpresion::appendVariableImage(In2VarImage p){
    m_VariableBufferData.append(p);
}

/** Manda informacion de debug*/
void GestorImpresion::sltSendDebugInfo(unsigned char level,QString data){
    emit SendDebugInfo(level,data);
}
/** Testea si las tarjetas están bien conectadas*/
void GestorImpresion::sltTestCards(){
    char buffer[128];
    QVector<InkjetXusbCtrl*> m_cardsSorted;
    QVector<int> iOrden;
    bool bsorted=true;
    static int count=0;
    emit SendDebugInfo(LOGGER_ERROR,tr("Testing serial number XUSB"));
    for (int x=0;x<m_inumCards;x++){
        iOrden.append(x);
        memset(&buffer,0x00,sizeof(buffer));
        InkjetXusbCtrl *p=m_cards.at(x);
        p->getSerial(buffer);
        QString cmpString(buffer);
        emit SendDebugInfo(LOGGER_ERROR,tr("Xusb %1 --> %2 (expected %3)").arg(x).arg(buffer).arg(memoryMap->m_mapa.m_maquina.m_xusbs[x].SN));
        //No son iguales...busco su nueva posicion
        if (cmpString.compare(memoryMap->m_mapa.m_maquina.m_xusbs[x].SN,Qt::CaseInsensitive)){
            bsorted=false;
            for (int y=0;y<m_inumCards;y++){
                if (!cmpString.compare(memoryMap->m_mapa.m_maquina.m_xusbs[y].SN,Qt::CaseInsensitive)){
                    iOrden.replace(x,y);
                }
            }
        }
    }
    // se ha intentado ordenar...se hace el cambio y comprueba
    if (!bsorted){
        if (count==0){
            QString txt=tr("Reasignando electronica, por favor espere");
            In2Message *m=new In2Message(0,txt);
            m->sltUpdateProgressBar(0);
            QObject::connect(this,SIGNAL(sgnReinicioSistema(int)),m,SLOT(sltUpdateProgressBar(int)));
            QObject::connect(this,SIGNAL(sgnKillIn2Msg()),m,SLOT(deleteLater()));
            emit SendDebugInfo(LOGGER_ERROR,txt);
            //QMessageBox::warning(0,"ERROR",txt,QMessageBox::Ok);
            QTimer::singleShot(0,this,SLOT(sltReiniciarSistema()));
            count++;
        }
        else{
            emit sgnKillIn2Msg();
            QString txt=QString("NO SE PUDO ESTABLECER ORDEN CORRECTO ELECTRONICA! PROCEDA MANUALMENTE EN MAQUINA");
            QMessageBox::warning(0,"ERROR",txt,QMessageBox::Ok);

        }
    }
    else{
        emit sgnKillIn2Msg();
        emit SendDebugInfo(LOGGER_ERROR,tr("ORDEN CORRECTO XUSB"));
        bInit=true;
        if (memoryMap->m_mapa.m_maquina.m_opc.estado){
            In2Message *m=new In2Message(0,tr("Testeando cabezales...por favor espere"));
            Qt::WindowFlags flags=m->windowFlags();
            flags|=Qt::WindowStaysOnTopHint;
            m->setWindowFlags(flags);
            int time=memoryMap->m_mapa.m_variables.DefaultNumHeads*TIME_HEAD_TEST*m_inumCards;
            m->sltEnableAutomaticPB(time);
            m->sltDeleteLater(time);
            m->show();
            //connect(this,SIGNAL(sgnKillIn2Msg()),m,SLOT(deleteLater()));
        }
    }
}

/** Actualiza la info*/
void GestorImpresion::sltRefreshInfo(){
    m_inumCards=XaarScorpionGetXUSBCount();
    /*DWORD foo1,foo2;
    bool foo=bXaarScorpionXPMsConnected(&foo1,&foo2);
    m_inumCards+=foo1;*/

    /*QMessageBox::warning(0,"INFO","SIMULACION DE 1 XUSB GestorImpresion::sltRefreshInfo",QMessageBox::Ok);
    m_inumCards=1;*/
}
/** Devuelve el numero de tarjetas de control*/
int GestorImpresion::getNumCards(){
    return m_inumCards;
}
/** Habilita la tarjeta para imprimir*/
int GestorImpresion::EnablePrintMode(int card){
    int rtn=0;
    if (card<m_inumCards) {
        InkjetXusbCtrl *p=m_cards.at(card);
        p->EnablePrintMode(true);
    }
    else rtn=-1;
    return rtn;
}
/** Habilita la tarjeta para imprimir*/
int GestorImpresion::EnableMaskPrintMode(DWORD mask,int card){
    int rtn=0;
    if (card<m_inumCards) {
        InkjetXusbCtrl *p=m_cards.at(card);
        p->EnableMaskPrintMode(mask,true);
    }
    else rtn=-1;
    return rtn;
}

/** Deshabilita la tarjeta para imprimir*/
int GestorImpresion::DisablePrintMode(int card){
    int rtn=0;
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->EnablePrintMode(false);
        m_ImagenEnviada=false;
    }
    else rtn=-1;
    return rtn;
}
/** Deshabilita todas las tarjetas*/
void GestorImpresion::FullDisablePrintMode(){
    int n=getNumCards();
    for (int x=0;x<n;x++){
        DisablePrintMode(x);
        m_ImagenEnviada=false;
    }
}

/** Manda la imagen a imprimir*/
void GestorImpresion::sendImageToPrint(int card,const QImage & image){
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->SetImage(image);
        p->process();
        m_ImagenEnviada=true;
    }
}
/** Resetea una tarjeta*/
void GestorImpresion::resetCard(int card){
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->Reset();
    }
}
/** Cambia la paleta*/
void GestorImpresion::changePalette(int card,char *data){
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->setNewFullPalette(data);
    }
}
/** Guarda la paleta*/
void GestorImpresion::storePalette(char *data){
    memcpy(&m_AuxPalette[0],data,8);
}
/** Ejecuta FullSoftStop*/
void GestorImpresion::ExecFullSoftStop(){
    DWORD SwatheCountArray[2*MAXHEADS];
    DWORD maxSwathe=0;
    if (m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(0);
        //if (!p) p=m_cards.at(0);
        p->readSwatheCount(&SwatheCountArray[0]);
        //Aumentar estos valores en 1
        for(DWORD i=0;i<2*MAXHEADS;i++){
            if (SwatheCountArray[i]>maxSwathe) maxSwathe=SwatheCountArray[i];
        }
        maxSwathe+=2;
        for(DWORD i=0;i<2*MAXHEADS;i++){
            SwatheCountArray[i]=maxSwathe;
        }
        for (int Card=0;Card<this->m_inumCards;Card++){
            m_cards.at(Card)->writeSwatheCount(&SwatheCountArray[0]);
        }
        for (int Card=0;Card<this->m_inumCards;Card++){
            m_cards.at(Card)->softStop();
        }
        m_ImagenEnviada=false;
    }
    //emit FullStopPerformed();
}

/** Pregunta por la impresion por la que va*/
void GestorImpresion::sltAskPrintCount(){
    DWORD SwatheCount[MAXHEADS * MAXROWS];
    static DWORD lastCount=-1;
    int card=0; //Solo pregunto al cero
    InkjetXusbCtrl *p=m_cards.at(card);
    p->readSwatheCount(&SwatheCount[0]);
    if ((SwatheCount[0]!=lastCount)&&(SwatheCount[0]>0)){ //Para enviar solo una vez
        emit currentPrintNumber(card,SwatheCount[0]);
        lastCount=SwatheCount[0];
        DWORD limit=p->getSwathePrintCount();
        if (lastCount>limit){
            emit lastSwathePrinted(0);
            //QTimer::singleShot(TIMEOUT_PRINT_MONITOR,this,SLOT(sltAskPrintCount()));
            lastCount=-1;
        }
        else{
            if (m_MonitorPrintCount) QTimer::singleShot(TIMEOUT_PRINT_MONITOR,this,SLOT(sltAskPrintCount()));
        }
    }
    else
        if (m_MonitorPrintCount) QTimer::singleShot(TIMEOUT_PRINT_MONITOR,this,SLOT(sltAskPrintCount()));
}

/** Pregunta cuantas impresiones lleva esa tarjeta*/
bool GestorImpresion::readSwathePrintCount(int card,DWORD *count){
    bool rtn=false;
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        rtn=p->readSwatheCount(count);
    }
    return rtn;
}


/** Activa el last Swatheprinted...espera hasta guard msg*/
/*void GestorImpresion::sltwaitLastSwathe(){
    emit SendDebugInfo(LOGGER_ERROR,QString("sltLastWathe"));
    InkjetXusbCtrl *p=m_cards.at(0);
    int guard=0;
    while (!p->printComplete()){
         _Sleep(TIMEOUT_PRINT_MONITOR);
         guard+=TIMEOUT_PRINT_MONITOR;
         if (guard==TIMEOUT_PRINT_MONITOR*10){
             emit SendDebugInfo(LOGGER_ERROR,QString("Timeout!"));
             break; //Dejamos un maximo
         }
    }
    emit lastSwathePrinted(0) ;
}*/

/** Activa o desactiva el monitoreo de impresion*/
void GestorImpresion::sltMonitorPrintCount(bool activate){
    m_MonitorPrintCount=activate;
    if (m_MonitorPrintCount) QTimer::singleShot(TIMEOUT_PRINT_MONITOR,this,SLOT(sltAskPrintCount()));
}
/** Establece numero de copias */
void GestorImpresion::WriteSwathePrintCount(int card,DWORD *swathePrintCount){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->writeSwatheCount(swathePrintCount);
    }
}
/** Ignora un numero de PDs*/
void GestorImpresion::IgnorePd(int card,DWORD count){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->IgnorePd(count);
    }
}
/** Habilita/Deshabilita el PD interno*/
void GestorImpresion::EnableInternalPD(int card, int value){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->EnableInternalPD(value);
    }
}
/** Cambia el fichero de configuracion*/
void GestorImpresion::setNewConfigFile(QString filename){
    for (int x=0;x<m_cards.count();x++){
        InkjetXusbCtrl *p=m_cards.at(x);
        p->setNewConfigFile(filename);
    }
    m_ConfigFile=filename;
}

/** Configuración de alta velocidad*/
void GestorImpresion::setHighSpeedActive(QString file,bool active){
    setNewConfigFile(file);
    m_bHighSpeedActive=active;
}



/** Genera un PD*/
void GestorImpresion::GeneratePD(int card){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->GenerateProductDetect();
    }
}

/** Devuelve el estado de un cabezal*/
DWORD GestorImpresion::getHeadStatus(int card,int head){
    DWORD status=0;
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        status=p->getHeadStatus(head);
    }
    return status;
}

/** Devuelve el estado de un cabezal*/
QString GestorImpresion::getHeadStatusDescription(int card,int head){
    QString rtn;
    DWORD status=0;
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        status=p->getHeadStatus(head);
        if (status==0) rtn=tr("No hay cabezal");
        else if (status==1) rtn=tr("Iniciando cabezal");
        else if (status==2) rtn=tr("Cabezal OK");
        else if (status==3) rtn=tr("Error iniciando");
        else if (status==4) rtn=tr("Error TC");
        else if (status==5) rtn=tr("Error parametros");
    }
    return rtn;
}


/** Reinicia un cabezal*/
bool GestorImpresion::bootHead(int card,int head){
    DWORD rtn=false;
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        rtn=p->bootHead(head);
    }
    return rtn;
}

/** Devuelve el numero de cabezales conectados*/
DWORD GestorImpresion::getNumConnectedHeads(int card){
    DWORD numCabezales=0;
    for (DWORD x=0;x<MAXHEADS;x++){
        int rtn=this->getHeadStatus(card,x);
        if (rtn==2) numCabezales++;
    }
    return numCabezales;
}

/** Habilita los cabezales*/
bool GestorImpresion::PowerControlHead(int card,char mask, bool enable){
    bool  rtn=false;
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        rtn=p->powerControlHeads(mask,enable);
    }
    return rtn;
}

/** Establece el cabezal de control*/
void GestorImpresion::setControlHead(int card,int head){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->setControlHead(head);
    }
}

/** establece la imagen a imprimir en una cabezal de una tarjeta*/
void GestorImpresion::sendImageToHead(int card, int head, const QImage & image){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->Reset();
        p->SetImage(image);
        p->SelectHeadToPrint(head);
        p->process();
        m_ImagenEnviada=true;
    }
}

/** Devueleve el srial de un cabezal*/
void GestorImpresion::getSerialHead(int card, int head, char *buffer){
    if (card<m_cards.count()){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->getSerialHead(head, buffer);

    }
}

/** Intenta reparar automaticamente un cabezal..devuelve false si el cabezal no está bien detectado*/
unsigned int GestorImpresion::checkSerialHead(int card,int head){
    char buffer[25];
    memset(&buffer[0],0x00,sizeof(buffer));
    getSerialHead(card,head,buffer);
    unsigned int s1=(unsigned char)buffer[0];
    unsigned int s2=(unsigned char)buffer[1];
    unsigned int s3=(unsigned char)buffer[2];
    unsigned int s4=(unsigned char)buffer[3];
    unsigned int serial=s1+s2*0x0100+s3*0x010000+s4*0x01000000;
    return serial;
}


void GestorImpresion::sltReiniciarSistema(){
#ifdef OPC
#ifdef ITEM_APAGAR_XUSB1
    QFile dummyFile(XUSBDLL);
    if (dummyFile.exists()){
        dummyFile.remove();
        emit SendDebugInfo(LOGGER_WARNING,tr("Eliminado XUSBDLL.xml previo"));
    }
    emit SendDebugInfo(LOGGER_ERROR,tr("GestorImpresion:sltReiniciarSistema()"));
    ClienteOPC *opc=ClienteOPC::request(0);
    opc->EscribirDato(ITEM_APAGAR_XUSB1,QVariant(1));    
    opc->EscribirDato(ITEM_APAGAR_XUSB2,QVariant(1));    
    QTimer::singleShot(5000,this,SLOT(sltReiniciarSistema1()));
#endif

#endif
}
void GestorImpresion::sltReiniciarSistema1(){
#ifdef OPC
#ifdef ITEM_APAGAR_XUSB1
    emit SendDebugInfo(LOGGER_ERROR,tr("GestorImpresion:sltReiniciarSistema1()"));
    emit sgnReinicioSistema(0);
    ClienteOPC *opc=ClienteOPC::request(0);
    opc->EscribirDato(ITEM_APAGAR_XUSB1,QVariant(0));
    QTimer::singleShot(10000,this,SLOT(sltReiniciarSistema2()));
#endif
#endif
}

void GestorImpresion::sltReiniciarSistema2(){
#ifdef OPC
    emit sgnReinicioSistema(33);
    emit SendDebugInfo(LOGGER_ERROR,tr("GestorImpresion:sltReiniciarSistema2()"));
    XaarScorpionReInitialiseSystem();
    QTimer::singleShot(5000,this,SLOT(sltReiniciarSistema3()));
#endif
}
void GestorImpresion::sltReiniciarSistema3(){
#ifdef OPC
#ifdef ITEM_APAGAR_XUSB2
    emit sgnReinicioSistema(66);
    emit SendDebugInfo(LOGGER_ERROR,tr("GestorImpresion:sltReiniciarSistema3()"));
    ClienteOPC *opc=ClienteOPC::request(0);
    opc->EscribirDato(ITEM_APAGAR_XUSB2,QVariant(0));
    QTimer::singleShot(10000,this,SLOT(sltReiniciarSistema4()));
#endif
#endif

}
void GestorImpresion::sltReiniciarSistema4(){
#ifdef OPC
    emit SendDebugInfo(LOGGER_ERROR,tr("GestorImpresion:sltReiniciarSistema4()"));
    XaarScorpionReInitialiseSystem();
    emit sgnReinicioSistema(100);
    if (!bInit) QTimer::singleShot(0,this,SLOT(sltTestCards()));
#endif
}



/** Devuelve el fichero de configuracion*/
QString GestorImpresion::getConfigFile(){
    return m_ConfigFile;
}

/** Monitoriza la impresion*/
void GestorImpresion::sltMonitorHeads(bool monitor){
    if (monitor==m_monitorHeads) return;
    else{
       m_monitorHeads=monitor;
       if (m_monitorHeads){
           _sltMonitorHeads();
      }
    }
}
/** Monitor de cabezales*/
void GestorImpresion::_sltMonitorHeads(){
    if (m_monitorHeads){
        DWORD heads[8];
        for (int card=0;card<m_inumCards;card++){
            memset(&heads,0x00,sizeof(heads));
            //InkjetXusbCtrl *p=m_cards.at(card);
            for (int head=0;head<memoryMap->m_mapa.m_variables.DefaultNumHeads;head++){
                int rtn=this->checkSerialHead(card,head);
                if (rtn==0){
                    //QSound::play(QString("%1\\Config\\ReadyToPrint.wav").arg(QApplication::applicationDirPath()));
                    emit SendDebugInfo(LOGGER_ERROR,tr("Error Card %1 Head%2").arg(card).arg(head));
                    emit errorImpresion(FIN_CABEZALES);
                    InkjetDebug *w=InkjetDebug::request(0);
                    w->show();
                }
            }
        }
        QTimer::singleShot(5500,this,SLOT(_sltMonitorHeads()));
    }
}

/** Carga Imagen externa*/
void GestorImpresion::sltLoadExternalVarImage(QString  txt){
    emit newExternalImage(txt);
}

/** Emite un stop*/
void GestorImpresion::sltStopVariable(){
    emit StopGenerating();
    m_ImageSender.clear();
    m_ImageCreator.clear();
    m_ImagenEnviada=false;
}
/** Devuelve los threads-creators activos*/
int GestorImpresion::getActiveCreatorThreads(){
    return m_ImageCreator.count();
}
/** Devuelve los threads-senders activos*/
int GestorImpresion::getActiveSenderThreads(){
    return m_ImageSender.count();
}
/** Devuelve una lista de impresion*/
QStringList GestorImpresion::getCurrentVariableList(int card){
    QStringList foo;
    if (card<m_ImageSender.count()){
        ImageVarXaarCreator *p=m_ImageCreator.at(card);
        foo=p->getVariableList();
    }
    return foo;
}
/** devuelve el volatge de una row*/
DWORD GestorImpresion::getHeadVOffset(int card,int head,int row){
    DWORD rtn=0xFFFF;
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        rtn=p->getVOffset(head,row);
    }
    return rtn;
}
/** Carga el volatge de una row*/
void GestorImpresion::setHeadVOffset(int card,int head,int row,DWORD value){
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->setVOffset(head,row,value);
    }
}
/** Habilita/Deshabilita el encoder interno*/
void GestorImpresion::EnableInternalEncoder(int card,BYTE val){
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->setInternalEncoder(val);
    }
}
/** Habilita/Deshabilita el encoder interno*/
void GestorImpresion::setInternalEncoderValue(int card,DWORD val){
    if (card<m_inumCards){
        InkjetXusbCtrl *p=m_cards.at(card);
        p->setInternalEncoderValue(val);
    }
}
/** Fuerza un reinicio*/
void GestorImpresion::sltForceReinit(){
    XaarScorpionReInitialiseSystem();
}






