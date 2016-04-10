/** @addtogroup ParImpresion
 *  @{*/
/** @file ParImpresion.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ParImpresion.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "parimpresion.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFileDialog>
#include "principal.h"
#include "inkjetmultiview.h"
//#include "dbgmem.h"
#include <QDebug>
#include "clienteopc.h"
#include <QProgressBar>

//Fichero de cabecera para Xaar
#include "ScorpionDLL-ext.h"


//Token para saber que campo actualizar
static unsigned char lastSelected=0;
//Puntero a datos
unsigned char *Data;
//Flag de impresion
static unsigned char iPrinting=0;

unsigned char Imagen_FijaVariable;
//unsigned char flag_paso_a_principal=0;

unsigned int ancho_qlabel_parimpresion;
unsigned int alto_qlabel_parimpresion;

unsigned char flag_softstop_required=0;

static QString ImagenXaar;

DWORD LastError;
BOOL dummyError;
QString nombrefichero;

//int x1_window,y1_window,x2_window,y2_window;
int x1_qlabel,y1_qlabel,x2_qlabel,y2_qlabel;

//Para hacer zoom
static struct Pulsacion{
    int x1;
    int x2;
    int y1;
    int y2;
    bool zooming;
}pulsacion ;





ParImpresion::ParImpresion(QWidget *parent)
	: InkjetForm(parent)
{

    this->SetTitleInfo(QString("TRABAJOS DE IMPRESION"));
    this->SetTitleStatus(QString("STATUS"));
    this->SetTitleOptions(QString("OPCIONES"));
	
	lastSelected=0;
	iPrinting=0;
	
    SelImgFijaButton = new QPushButton(this->m_OptionZone);
    SelImgFijaButton->setText("Selección de\n imagen fija");
    SelImgFijaButton->setIcon(QPixmap(":/res/images/folder_images.png"));
    SelImgFijaButton->setIconSize(QSize(WICONSIZE,HICONSIZE));
    SelImgFijaButton->setFont( QFont( "Arial", QFONT8, QFont::Normal ) );

    m_AceptarButton=new QPushButton(this->m_OptionZone);
    m_AceptarButton->setText("Aceptar");
    m_AceptarButton->setIcon(QPixmap(":res/images/Ok.png"));
    m_AceptarButton->setIconSize(QSize(WICONSIZE,HICONSIZE));


    this->OrdenaBotonesOpciones();

    m_OpcionesImpresion=new QGroupBox(this->m_CentralFrame);
    m_OpcionesImpresion->setGeometry(DELTA,DELTA,OPCIONES_FORMAT_WIDTH,OPCIONES_FORMAT_HEIGHT);

    m_SendToXaarButton=new QPushButton(m_OpcionesImpresion);
    m_SendToXaarButton->setText("Enviar");
    m_SendToXaarButton->setGeometry(2*DELTA,2*DELTA,EDIT_W/2,EDIT_H);

    m_StopButton=new QPushButton(m_OpcionesImpresion);
    m_StopButton->setText("Stop");
    m_StopButton->setGeometry(2*DELTA,6*DELTA,EDIT_W/2,EDIT_H);
    m_StopButton->setVisible(false);

    m_SoftStopButton=new QPushButton(m_OpcionesImpresion);
    m_SoftStopButton->setText("Soft Stop");
    m_SoftStopButton->setGeometry(2*DELTA,10*DELTA,EDIT_W/2,EDIT_H);

    m_SendPDButton=new QPushButton(m_OpcionesImpresion);
    m_SendPDButton->setText("PD");
    m_SendPDButton->setGeometry(2*DELTA,14*DELTA,EDIT_W/2,EDIT_H);
    m_SendPDButton->setVisible(false);

    m_qlSwatheActual=new QTextEdit(m_OpcionesImpresion);
    m_qlSwatheActual->setGeometry(2*DELTA,18*DELTA,EDIT_W/2,EDIT_H);

    m_qlLastSwatheSent=new QTextEdit(m_OpcionesImpresion);
    m_qlLastSwatheSent->setGeometry(2*DELTA,22*DELTA,EDIT_W/2,EDIT_H);

    m_qlSwatheTotal=new QTextEdit(m_OpcionesImpresion);
    m_qlSwatheTotal->setGeometry(2*DELTA,26*DELTA,EDIT_W/2,EDIT_H);


    //Muestra imagen central
    m_ImagenParImpresion=new QLabel(this->m_CentralFrame);
    x1_qlabel=OPCIONES_FORMAT_WIDTH+2*DELTA;
    y1_qlabel=DELTA;
    ancho_qlabel_parimpresion=IMG_WIDTH-OPCIONES_FORMAT_WIDTH-4*DELTA;
    alto_qlabel_parimpresion=IMG_HEIGHT-2*DELTA;

    x2_qlabel=x1_qlabel+ancho_qlabel_parimpresion;
    y2_qlabel=y1_qlabel+alto_qlabel_parimpresion;

    m_ImagenParImpresion->setGeometry(x1_qlabel,y1_qlabel,ancho_qlabel_parimpresion,alto_qlabel_parimpresion);


    connect((Principal*)this->GetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
	connect(m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
	connect(SelImgFijaButton,SIGNAL(clicked()),this,SLOT(SelImagenFija()));
    connect(m_SendToXaarButton,SIGNAL(clicked()),this,SLOT(SendToVarXaar()));
    connect(m_SendPDButton,SIGNAL(clicked()),this,SLOT(GeneratePD()));
    connect(m_StopButton,SIGNAL(clicked()),this,SLOT(StopPrinting()));
    connect(m_SoftStopButton,SIGNAL(clicked()),this,SLOT(SoftStopPrinting()));

    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));

    memset(&pulsacion,0x00,sizeof(struct Pulsacion));
    rubberBand=NULL;
	//Info del mapa de memoria
	CargaDatos();
	this->ShowInfo();
    TestRoot();

    newVarCard=new InkjetXusbVarCtrl(0,0,NULL);
    for (int x=0;x<SIZE_BUFFER_IMAGES;x++){
        this->BufferVarImages[x].m_state=XAAR_IMAGEN_VACIA;
    }
    //zooming=false;

}



void ParImpresion::SelImagenFija(){
	
    SelImagen();
}

//Abre un explorer para seleccionar la imagen
bool ParImpresion::SelImagen(){

    QFileDialog fileName(this);

	QStringList Ficheros;

	fileName.setFileMode(QFileDialog::ExistingFile);
	fileName.setViewMode(QFileDialog::List);
	fileName.setFilter(tr("Image Files (*.png *.jpg *.bmp *.tif)"));
	QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
	foreach (QPushButton *w, widgets){
			w->setAutoDefault(false);
	 }
	if (fileName.exec()){
		Ficheros=fileName.selectedFiles();
		if (!Ficheros.isEmpty()){
            nombrefichero=QString("%1").arg(Ficheros.at(0));
            ImagenXaar=nombrefichero;
            memoryMap->setImagen(nombrefichero);
            MuestraImagen(memoryMap->m_Imagen);
        }
     }
    return true;
}
/*Actualiza el contador de swathes*/
void ParImpresion::UpdateTotalVarXarSentSwathe(){
    m_TotalSwatheSent+=SIZE_BUFFER_IMAGES;
}
void ParImpresion::UpdateVarXarSentSwathe(int last){
    //QString dummy=QString("NowPrinting(%1)-LastReplaced(%2)-TotalSent(%3)").arg(m_ActualSwathe).arg(last).arg(m_TotalSwatheSent+last);
    m_qlSwatheActual->setText(QString("%1").arg(m_ActualSwathe));
    m_qlLastSwatheSent->setText(QString("%1").arg(last));
    m_qlSwatheTotal->setText(QString("%1").arg(m_TotalSwatheSent+last));
}


/** Envia informacion variable*/
void ParImpresion::SendToVarXaar(){
    QThread *threadSender=new QThread;
    QThread *threadCreator=new QThread;
    //Contador total de swathes enviadas
    m_TotalSwatheSent=0;
    //Swathe actual
    m_ActualSwathe=0;
    //Se resetea el Buffer de Imagenes cada vez que le demos a un envio nuevo
    for (int x=0;x<SIZE_BUFFER_IMAGES;x++){
        this->BufferVarImages[x].m_state=XAAR_IMAGEN_VACIA;
    }
    connect(this,SIGNAL(StopThread()),threadCreator,SLOT(deleteLater()));
    connect(this,SIGNAL(StopThread()),threadSender,SLOT(deleteLater()));

    threadSender->start();
    threadCreator->start();

    emit SendDebugInfo(LOGGER_WARNING,"Creando Imagenes iniciales");
    ImageCreator=new ImageVarXaarCreator(0,BufferVarImages);
    connect(this,SIGNAL(StartGenerating()),ImageCreator,SLOT(StartGeneration()));
    connect(this,SIGNAL(PauseGenerating()),ImageCreator,SLOT(PauseGeneration()));
    connect(this,SIGNAL(StopThread()),ImageCreator,SLOT(Stop()));
    ImageCreator->moveToThread(threadCreator);

    emit SendDebugInfo(LOGGER_WARNING,"Enviando Imagenes iniciales");
    ImageSender=new ImageVarXaarSender(0,0,BufferVarImages);
    ImageSender->PreloadImages();
    connect(this,SIGNAL(CurrentPrintSwathe(int)),ImageSender,SLOT(XaarLastSwathePrinted(int)));
    connect(ImageSender,SIGNAL(CompleteBufferReplaced()),this,SLOT(UpdateTotalVarXarSentSwathe()));
    connect(ImageSender,SIGNAL(LastSwatheReplaced(int)),this,SLOT(UpdateVarXarSentSwathe(int)));
    ImageSender->moveToThread(threadSender);

    emit SendDebugInfo(LOGGER_WARNING,"Imprimiendo");
    emit StartGenerating();

    //Empezamos la impresion
    memoryMap->m_ImagenEnviada=true;


    /*
    connect(newVarCard, SIGNAL(error(QString,int)), this, SLOT(sltErrorString(QString,int)));

    QImage imagen; ///createNewAlbenizImage()
    imagen.save("D:\\Data\\INKJET\\Images\\kk.bmp");
    newVarCard->SetImage(imagen);
    MuestraImagen(QPixmap::fromImage(imagen));
    newVarCard->SendFirstImage();
    for (int x=0; x<3;x++){
        QImage imagen2; //createNewAlbenizImage();
        newVarCard->SetImage(imagen2);
        if (x==2) newVarCard->SetLastSwathe();
        newVarCard->SendImage();
    }
    qDebug()<<"Loaded 4 swathes";
    newVarCard->SetupPrintMode();
    bXaarScorpionGenerateProductDetect(0);*/
}

/** Maneja los errores de los hilos*/
void ParImpresion::sltErrorString(QString err,int xusb){
    QString texto=QString("%1 [xUsb %2]").arg(err).arg(xusb);
    emit SendDebugInfo(LOGGER_ERROR,texto);
    //QMessageBox::warning(0,"INFORMACION",texto,QMessageBox::Ok);
}


/** Envia multihilo...HAY QUE INTEGRAR ESTO CON EL ANTERIOR*/
void ParImpresion::SendToXaar(){


    QString nombre;
    QString ruta;
    QString nombreruta;

    nombreruta=nombrefichero;

    /*int posicion_barra;
    int posicion_contrabarra;
    int ultima_posicion;

    dummy=nombrefichero;

    posicion_barra=dummy.lastIndexOf("\\");
    posicion_contrabarra=dummy.lastIndexOf("/");
    if (posicion_contrabarra==-1) posicion_contrabarra=0;
    if (posicion_barra>posicion_contrabarra) ultima_posicion=posicion_barra;
    else ultima_posicion=posicion_contrabarra;

    ruta.clear();
    nombre.clear();
    ruta = dummy.left(ultima_posicion);
    nombre = dummy.mid(ultima_posicion+1);

    ruta.replace("/","\\");
    nombre.replace("/","\\");*/

    nombreruta.replace("/","\\");
    nombre=DevuelveNombre(nombreruta);
    ruta=DevuelveRuta(nombreruta);

    memoryMap->m_ImagenEnviada=true;
    memoryMap->m_UltimaAccion=UA_IMAGEN_ENVIADA;

    memoryMap->m_mapa.m_variables.NombreRutaBmpEnviado=nombreruta;
    memoryMap->m_mapa.m_variables.NombreBmpEnviado=nombre;
    memoryMap->m_mapa.m_variables.RutaBmpEnviado=ruta;

    /*qDebug() << nombreruta;
    qDebug() << nombre;
    qDebug() << ruta;*/

     //Codigo XAar modificado para Qt...este debiera funcionar

   /* InkjetMultiview InkjetXaar;
    InkjetXaar.SendImage(ImagenXaar);
    GeneratePD();
    */

    //Nuestro codigo adaptado a multithread
    m_threads=0;
    m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();    
    //QMessageBox::warning(0,"Info",QString("Num USB detectados %1").arg(m_iTotalXUSB),QMessageBox::Ok);
    m_threads=m_iTotalXUSB;
    QImage dummyImage(ImagenXaar);
    int newWidth=0;
    int totalWidth=0;
    int newHeight=dummyImage.height();
    /*QProgressBar *progBar=new QProgressBar(0);
    progBar->setMinimum(0);
    progBar->setMaximum(m_iTotalXUSB);
    progBar->setVisible(true);*/
    emit SendDebugInfo(LOGGER_WARNING,"Imprimiendo");
    for (int Card=0;Card<m_iTotalXUSB;Card++){
        //if (Card==1){
        InkjetXusbCtrl *newCard=new InkjetXusbCtrl(0,Card);
        //Para cada tarjeta habrá que saber los cabezales (width)
        newWidth=newCard->returnNumCabezales()*1000;
        //QMessageBox::warning(0,"INFO",QString("Width %1-%2").arg(Card).arg(newWidth),QMessageBox::Ok);
        QRect rect(totalWidth,0,newWidth,newHeight);
        totalWidth+=newWidth;
        //QRect rect(0,0,newWidth,newHeight);
        QImage processedImage=dummyImage.copy(rect);
        newCard->SetImage(processedImage);
        QThread *newThread= new QThread;
        newCard->moveToThread(newThread);
        connect(newThread, SIGNAL(started()), newCard, SLOT(process()));
        connect(newCard, SIGNAL(finished()), newThread, SLOT(quit()));
        connect(newCard, SIGNAL(finished()), this, SLOT(sltLaunchPD()));
        connect(newCard, SIGNAL(finished()), newCard, SLOT(deleteLater()));
        connect(newThread, SIGNAL(finished()), newThread, SLOT(deleteLater()));
        connect(this,SIGNAL(destroyed()),newCard,SLOT(deleteLater()));
        connect(this,SIGNAL(destroyed()),newThread,SLOT(deleteLater()));
        connect(newCard, SIGNAL(error(QString,int)), this, SLOT(sltErrorString(QString,int)));
        newThread->start();
        //}
    }
    //QApplication::setStyle(new InkjetStyle(THEME_PRINTING));
}

QImage ParImpresion::createSubImage(QImage* image, const QRect & rect) {
    size_t offset = rect.x() * image->depth() / 8
                    + rect.y() * image->bytesPerLine();
    return QImage(image->bits() + offset, rect.width(), rect.height(),
                  image->bytesPerLine(), image->format());
}


/** Lanza el PD en caso de que todos los hilos hayan terminado*/
void ParImpresion::sltLaunchPD(){
    m_threads--;
    if (m_threads==0) GeneratePD();
}

/** Si no es root, bloqueo funcionalidad.*/
void ParImpresion::TestRoot(){

}


ParImpresion::~ParImpresion()
{
	delete(m_AceptarButton);
	delete(SelImgFijaButton);
}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void ParImpresion::GuardaDatos(){


}

//Acepta la selección realizada y vuelve a la pantalla principal
void ParImpresion::AceptarSeleccion(){
    StopThreads();
	GuardaDatos();

    this->ShowMainWindow();
	this->deleteLater();
	this->ShowInfo();
    //flag_paso_a_principal=1;

}
//Termina con los hilos de impresion si los hubiera
void ParImpresion::StopThreads(){
    emit StopThread();
}



//Cancela la selección y vuelve a la pantalla principal
void ParImpresion::CancelarSeleccion(){
    StopThreads();
    this->ShowMainWindow();
	this->deleteLater();
	this->ShowInfo();
    //flag_paso_a_principal=1;
}

/** Actualiza los datos de pantalla*/
void ParImpresion::ActualizaDataSlot(){
    static char counter=0;
    this->ShowComms();
    this->ShowEstadoImpresora();
    //Pedimos datos de impresion al XAAR cada segundo!
    m_ActualSwathe=AskXaar();
    //Cambiamos la imagen a mostrar cada 10 sg, en caso de que haya algo
    counter++;
    counter=counter%10;
    if (!counter){
        if (BufferVarImages[m_ActualSwathe].m_state!=XAAR_IMAGEN_VACIA) MuestraImagen(QPixmap::fromImage(BufferVarImages[m_ActualSwathe].m_Imagen));
    }
    //bXaarScorpionGenerateProductDetect(0);
}
/** Preguntamos al xaar por donde va*/
int ParImpresion::AskXaar(){
    int dummy=0;
    dummy=(int)newVarCard->GetCurrentPrintSwathe();
    //qDebug()<<QString("Printing swathe %1").arg(dummy);
    //Emitimos la swathe actual
    emit CurrentPrintSwathe(dummy);
    return dummy;
    /*if (lastUpload!=dummy){
        QImage imagen=createNewAlbenizImage();
        newCard->SetImage(imagen);
        newCard->ReplaceSwathe(0,0,lastUpload);
        qDebug()<<QString("REPLACING swathe %1").arg(lastUpload);
        lastUpload=dummy;

    }*/
}

//Imprime
void ParImpresion::Print(){
	GuardaDatos();
}

//Carga los datos de memoria
void ParImpresion::CargaDatos(){
	
    if (memoryMap->m_FilePath!=QString("none")){
        ImagenXaar=memoryMap->m_FilePath;
        MuestraImagen(memoryMap->m_Imagen);
        emit SendDebugInfo(LOGGER_ERROR,"Hay imagen cargada previamente");
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,"No hay imagen en memoria");
    }

}


void ParImpresion::ActualizaCompletado(int idone){

}

//Actualiza informacion impresa
void ParImpresion::UpdateTemporizador(){
}

void ParImpresion::GeneratePD(){
    for (int Card=0;Card<m_iTotalXUSB;Card++){
        bXaarScorpionGenerateProductDetect(Card);
    }
    //qDebug() << "PD generado";
    emit SendDebugInfo(LOGGER_WARNING,"PD enviado");
    //QMessageBox::warning(0,"Info","PD enviado",QMessageBox::Ok);
}

void ParImpresion::StopPrinting(){

    for (int Card=0;Card<m_iTotalXUSB;Card++)
        bXaarScorpionEnablePrintMode(Card,0xFFFF,false);
}

void ParImpresion::SoftStopPrinting(){
    //Pausamos la generacion de momento
    emit PauseGenerating();
    emit SendDebugInfo(LOGGER_WARNING,"Ejecutado soft stop");
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        //QVariant valor2(0);

        //flag_esperar_PrintComplete=1;
    }
    Insertar_Evento(ERROR_GUI,QString("Parada desde interfaz"));
}

//Muestra la imagen central
void ParImpresion::MuestraImagen(QPixmap Imagen){
    unsigned char escalado;

    int offset_x;
    int offset_y;

    int lw=ancho_qlabel_parimpresion;
    int lh=alto_qlabel_parimpresion;
    int iw=Imagen.width();
    int ih=Imagen.height();

    if ((iw==0)||(ih==0)) return;
    escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

    QPixmap pic(m_ImagenParImpresion->size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);

    QPixmap dummy;

    switch (escalado){
    case IMAGEN_SINESCALAR:
        //Reescalo tambien esta parte para hacer zoom
        if ((1000*Imagen.height()/m_ImagenParImpresion->height())>(1000*Imagen.width()/m_ImagenParImpresion->width())){
            dummy=Imagen.scaledToHeight(m_ImagenParImpresion->height(),Qt::SmoothTransformation);
        }
        else{
            dummy=Imagen.scaledToWidth(m_ImagenParImpresion->width(),Qt::SmoothTransformation);
        }
        //dummy=Imagen;
        offset_x=(m_ImagenParImpresion->width()-dummy.width())/2;
        offset_y=(m_ImagenParImpresion->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        m_ImagenParImpresion->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2W:
        dummy=Imagen.scaledToWidth(m_ImagenParImpresion->width(),Qt::SmoothTransformation);
        offset_x=(m_ImagenParImpresion->width()-dummy.width())/2;
        offset_y=(m_ImagenParImpresion->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        m_ImagenParImpresion->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2H:
        dummy=Imagen.scaledToHeight(m_ImagenParImpresion->height(),Qt::SmoothTransformation);
        offset_x=(m_ImagenParImpresion->width()-dummy.width())/2;
        offset_y=(m_ImagenParImpresion->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        m_ImagenParImpresion->setPixmap((QPixmap)(pic));
        break;
    }

    p.end();
    m_ImagenParImpresion->show();
}


void ParImpresion::mousePressEvent(QMouseEvent *event)
 {
     if (event->button() == Qt::LeftButton) {
         pulsacion.x1 = event->x();
         pulsacion.y1 = event->y();
         if ((pulsacion.x1>=x1_qlabel) && (pulsacion.x1<=x2_qlabel) && (pulsacion.y1>=y1_qlabel) && (pulsacion.y1<=y2_qlabel) ){
            pulsacion.zooming = true;
         }
     }

     origin = event->pos();

     if (!rubberBand)
         rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
     rubberBand->setGeometry(QRect(origin, QSize()));
     rubberBand->show();
 }

void ParImpresion::mouseMoveEvent(QMouseEvent *event){
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}
 void ParImpresion::mouseReleaseEvent(QMouseEvent *event)
 {
      rubberBand->hide();
     if (event->button() == Qt::LeftButton /*&& zooming*/) {
         pulsacion.x2 = event->x();
         pulsacion.y2 = event->y();
         if ((pulsacion.x2>=x1_qlabel) && (pulsacion.x2<=x2_qlabel) && (pulsacion.y2>=y1_qlabel) && (pulsacion.y2<=y2_qlabel) ){

             unsigned char escalado=0;
             int offset_x=0;
             int offset_y=0;

             int lw=ancho_qlabel_parimpresion;
             int lh=alto_qlabel_parimpresion;
             int iw=memoryMap->m_Imagen.width();
             int ih=memoryMap->m_Imagen.height();


             escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);


             QPixmap dummy;

             switch (escalado){
             case IMAGEN_SINESCALAR:
                 dummy=memoryMap->m_Imagen;
                 offset_x=(m_ImagenParImpresion->width()-dummy.width())/2;
                 offset_y=(m_ImagenParImpresion->height()-dummy.height())/2;
                 break;
             case IMAGEN_S2W:
                 dummy=memoryMap->m_Imagen.scaledToWidth(m_ImagenParImpresion->width(),Qt::SmoothTransformation);
                 offset_x=(m_ImagenParImpresion->width()-dummy.width())/2;
                 offset_y=(m_ImagenParImpresion->height()-dummy.height())/2;
                 break;
             case IMAGEN_S2H:
                 dummy=memoryMap->m_Imagen.scaledToHeight(m_ImagenParImpresion->height(),Qt::SmoothTransformation);
                 offset_x=(m_ImagenParImpresion->width()-dummy.width())/2;
                 offset_y=(m_ImagenParImpresion->height()-dummy.height())/2;
                 break;
             }


             int x_ini=pulsacion.x1-x1_qlabel;
             int y_ini=pulsacion.y1-IMG_Y;
             int x_fin=pulsacion.x2-x1_qlabel;
             int y_fin=pulsacion.y2-IMG_Y;

            //En dummy tenemos las dimensiones
             if (x_ini<offset_x){
                x_ini=0;
             }
             else{
                 x_ini=(x_ini-offset_x)*100/dummy.width();
             }

             if (x_fin<offset_x){
                x_fin=0;
             }
             else{
                 x_fin=(x_fin-offset_x)*100/dummy.width();
             }

             if (y_ini<offset_y){
                y_ini=0;
             }
             else{
                 y_ini=(y_ini-offset_y)*100/dummy.height();
             }

             if (y_fin<offset_y){
                y_fin=0;
             }
             else{
                 y_fin=(y_fin-offset_y)*100/dummy.height();
             }

             //memoryMap->m_Imagen
            //Hacemos un zoom sobre la imagen
            x_ini=(memoryMap->m_Imagen.width()*x_ini)/100;
            x_fin=(memoryMap->m_Imagen.width()*x_fin)/100;
            y_ini=(memoryMap->m_Imagen.height()*y_ini)/100;
            y_fin=(memoryMap->m_Imagen.height()*y_fin)/100;
            int zoom_height=y_fin-y_ini;
            int zoom_width=x_fin-x_ini;
            QRect rect(x_ini,y_ini,zoom_width,zoom_height);
            dummy=memoryMap->m_Imagen.copy(rect);
            MuestraImagen(dummy);
         }
     }
 }

 /** Clase que se encarga de enviar imagenes al Xaar*/
 class XaarImageSender{

 };
