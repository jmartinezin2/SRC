/** @addtogroup Maquina
 *  @{*/
/** @file Maquina.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Maquina.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "clienteopc.h"
#include "maquina.h"
#include "maquinaAdv.h"
#include "globals.h"
#include "MemoryMap.h"
#include "principal.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
#include "principal.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QSessionManager>
#include <QApplication>
#include <Windows.h>
#include <QFontComboBox>
#include <assert.h>
#include <QHash>
#include <QPixmap>
#include <QFontComboBox>
#include "webcam.h"
#include <QDebug>
#include <QRect>
#include <QAxObject>
#include <QMetaMethod>
#include <QMetaProperty>

//Fichero de cabecera para Xaar
#include "ScorpionDLL-ext.h"
#include "inkjetmultiview.h"
#include "inkjetxusbctrl.h"
#include "imgvargenerator.h"

#include "colaimpresion.h"

/*ITEMS

CINFA 1
n(172_16_170_6)_1.s1.m10.p20 (Integer16) trip0
n(172_16_170_6)_1.s1.m10.p21 (Integer16) trip1
n(172_16_170_6)_1.s1.m10.p22 (Integer16) trip2
n(172_16_170_6)_1.s1.m10.p23 (Integer16) trip3
n(172_16_170_6)_1.s1.m10.p24 (Integer16) trip4
n(172_16_170_6)_1.s1.m10.p25 (Integer16) trip5
n(172_16_170_6)_1.s1.m10.p26 (Integer16) trip6
n(172_16_170_6)_1.s1.m10.p27 (Integer16) trip7
n(172_16_170_6)_1.s1.m10.p28 (Integer16) trip8
n(172_16_170_6)_1.s1.m10.p29 (Integer16) trip9
n(172_16_170_6)_1.s1.m18.p02 (Integer16) velocidadLineaActual
n(172_16_170_6)_1.s1.m18.p15 (Integer16) velocidadLineaPreset
n(172_16_170_6)_1.s1.m18.p18 (Integer16) diametroActual
n(172_16_170_6)_1.s1.m18.p19 (Integer16) diametroPreset
n(172_16_170_6)_1.s1.m18.p20 (Integer16) longitudPreset
n(172_16_170_6)_1.s1.m18.p21 (Integer16) longitudActual
n(172_16_170_6)_1.s1.m18.p32 (Bool) Fijar diámetro
n(172_16_170_6)_1.s1.m18.p34 (Bool)	condicionesImpresion
n(172_16_170_6)_1.s1.m18.p35 (Bool)	Trabajo nuevo
n(172_16_170_6)_1.s1.m18.p36 (Bool)	condicionesMovimiento

Para el diametro actual primero escribes el 18.19 y luego pones a true el 18.32. El programa del driver se encarga de poner el 18.32 a false.
Para el de paro no hace falta hacer nada más.
*/

//#include "dbgmem.h"


extern QPalette paleta_textedit_enabled;
extern QPalette paleta_textedit_disabled;

static int wait=500;

static QString ImagenXaar;
static QString nombrefichero;
static char VariableTokenIni=0;


Maquina::Maquina(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::MaquinaUI)
{

 //   if (!launched){
        ui->setupUi(this);
        GlobalSetScreenSize(this);
        //memset(&pulsacion,0x00,sizeof(struct Pulsacion));
            //Info del mapa de memoria
        //this->ShowInfo();
        //Para la imagen variable
        m_TotalSwatheSent=0;
        //Swathe actual
        m_ActualSwathe=0;
        //Xusb
        m_iTotalXUSB=0;

        //Inicializa zona variable
        InitVariableFrame();

        setDataCommon();
        CargaDatos();
        TestRoot();
        ui->m_ImagenCentral->setMaximumWidth(ui->m_ImagenCentral->width());
        ui->m_ImagenCentral->setMaximumHeight(ui->m_ImagenCentral->height());
        ui->tabWidget->setMaximumHeight(ui->tabWidget->height());
        ui->tabWidget->setMaximumWidth(ui->tabWidget->width());
        ui->m_VelocidadCinta->setPalette(paleta_textedit_enabled);
        ui->m_MetrosTrabajo->setPalette(paleta_textedit_enabled);
        ui->m_DiametroBobinaEntrada->setPalette(paleta_textedit_enabled);
        ui->m_DiametroBobinaSalida->setPalette(paleta_textedit_enabled);
        ui->m_DiametroBobinaEntradaParo->setPalette(paleta_textedit_enabled);

        ui->m_gota1->setCurrentIndex(memoryMap->m_mapa.m_variables.VolumenGota[0]/DEFAULT_DROP_VOL);
        ui->m_gota2->setCurrentIndex(memoryMap->m_mapa.m_variables.VolumenGota[1]/DEFAULT_DROP_VOL);
        ui->m_gota->setText(QString("%1-%2").arg(memoryMap->m_mapa.m_variables.VolumenGota[0]).arg(memoryMap->m_mapa.m_variables.VolumenGota[1]));

    //primeravez=true;

        connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
        connect(ui->m_CancelButton,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
        //connect(ui->m_ShowCam,SIGNAL(clicked()),this,SLOT(ShowCam()));
        connect(ui->m_ColaButton,SIGNAL(clicked()),this,SLOT(LaunchColaImpresion()));
        connect(ui->SelImgFija,SIGNAL(clicked()),this,SLOT(SelImagenFija()));
        connect(ui->SendImgFija,SIGNAL(clicked()),this,SLOT(SendToXaar()));
        connect(ui->SendIni,SIGNAL(clicked()),this,SLOT(SendIniToXaar()));
        connect(ui->m_cbAnchoBobina,SIGNAL(currentIndexChanged(int)),this,SLOT(changeFreno(int)));

    #ifdef CON_TECLADOVIRTUAL
        connect(ui->m_VelocidadCinta,SIGNAL(selectionChanged()),this,SLOT(AceptarValorVelocidad()));
        connect(ui->m_MetrosTrabajo,SIGNAL(selectionChanged()),this,SLOT(AceptarValorMetros()));
        connect(ui->m_DiametroBobinaEntrada,SIGNAL(selectionChanged()),this,SLOT(AceptarValorDiametro()));
        connect(ui->m_DiametroBobinaSalida,SIGNAL(selectionChanged()),this,SLOT(AceptarValorDiametroSalida()));
        connect(ui->m_DiametroBobinaEntradaParo,SIGNAL(selectionChanged()),this,SLOT(AceptarValorDiametroParo()));
    #endif

        /*connect(ui->m_CambiarVelocidadCinta,SIGNAL(clicked()),this,SLOT(CambiarVelocidad()));
        connect(ui->m_CambiarMetrosTrabajo,SIGNAL(clicked()),this,SLOT(CambiarMetros()));
        connect(ui->m_CambiarDiametroBobina,SIGNAL(clicked()),this,SLOT(CambiarDiametro()));
        connect(ui->m_CambiarDiametroBobinaParo,SIGNAL(clicked()),this,SLOT(CambiarDiametroParo()));*/

        connect(ui->m_CambiarDiametroSalida,SIGNAL(clicked()),this,SLOT(CambiarDiametroSalida()));
        connect(ui->m_CambiarDiametroBobina,SIGNAL(clicked()),this,SLOT(CambiarDiametro()));
        connect(ui->m_TrabajoNuevo,SIGNAL(clicked()),this,SLOT(CambiarTrabajo()));
        //connect(ui->m_SoftStop,SIGNAL(clicked()),this,SLOT(SoftStop()));
        connect(ui->m_JogMas,SIGNAL(pressed()),this,SLOT(EjecutarJogMasOn()));
        connect(ui->m_JogMenos,SIGNAL(pressed()),this,SLOT(EjecutarJogMenosOn()));
        connect(ui->m_JogMas,SIGNAL(released()),this,SLOT(EjecutarJogMasOff()));
        connect(ui->m_JogMenos,SIGNAL(released()),this,SLOT(EjecutarJogMenosOff()));
        connect(ui->m_BuscarMacula,SIGNAL(clicked()),this,SLOT(EjecutarBuscarMacula()));
        connect(ui->m_Empalme,SIGNAL(clicked()),this,SLOT(EjecutarEmpalme()));
        connect(ui->m_BobinaNueva,SIGNAL(clicked()),this,SLOT(EjecutarBobinaNueva()));
        connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
        connect(GlobalGetMainWindow(), SIGNAL(FullStopPerformed()),this, SLOT(ExternalSoftStop()));
        connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    //connect(ui->m_ButtonShowKb,SIGNAL(clicked()),this,SLOT(sltLaunchTeclado()));
        connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
        connect(ui->m_ToMaquina2,SIGNAL(clicked()),this,SLOT(LaunchMaquina2()));
        connect(GlobalGetMainWindow(),SIGNAL(Refresh()),this,SLOT(ForceRefreshValues()));
        connect(ui->m_gota1,SIGNAL(currentIndexChanged(int)),this,SLOT(CambiarVolumenGota1(int)));
        connect(ui->m_gota2,SIGNAL(currentIndexChanged(int)),this,SLOT(CambiarVolumenGota2(int)));

        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            connect(GlobalGetMainWindow(), SIGNAL(ActualizaDataOPC()),this, SLOT(ActualizaDataSlotOPC()));
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            memoryMap->m_mapa.m_maquina.ref_velocidad=((Principal*)dummy)->miCliente->LeerDato(ITEM_VELOCIDAD_REF).toInt();
        }

        QMapIterator<int, int> i(memoryMap->m_mapa.m_maquina.qmDiamPresion);
        i.toBack();
        int dummySelection=0;
        ui->m_cbAnchoBobina->blockSignals(true);

        while (i.hasPrevious()) {
            i.previous();
            if (ui->m_leFreno->text().isEmpty()) ui->m_leFreno->setText(QString("%1").arg(i.value()));
            if (memoryMap->m_mapa.m_maquina.AnchoBobina==i.key()){
                dummySelection=ui->m_cbAnchoBobina->count();
                ui->m_leFreno->setText(QString("%1").arg(i.value()));
            }
            ui->m_cbAnchoBobina->addItem(QString("%1").arg(i.key()));
        }
       ui->m_cbAnchoBobina->blockSignals(false);
       ui->m_cbAnchoBobina->setCurrentIndex(dummySelection);


       //changeFreno(0);
       ui->tabWidget->setCurrentIndex(0);

        //Inicializa zona fja
        InitFixFrame();


}
/**Cambia el volumen de gota xub1*/
void Maquina::CambiarVolumenGota1(int index){
    if (memoryMap->m_mapa.m_variables.VolumenGota[0]!=index*DEFAULT_DROP_VOL){
        memoryMap->m_mapa.m_variables.VolumenGota[0]=index*DEFAULT_DROP_VOL;
        memoryMap->m_mapa.m_variables.SalvarFicheroConfiguracion=1;
        ui->m_gota->setText(QString("%1 %2").arg(memoryMap->m_mapa.m_variables.VolumenGota[0]).arg(memoryMap->m_mapa.m_variables.VolumenGota[1]));
    }
}
/**Cambia el volumen de gota xusb2*/
void Maquina::CambiarVolumenGota2(int index){
    if (memoryMap->m_mapa.m_variables.VolumenGota[1]!=index*DEFAULT_DROP_VOL){
        memoryMap->m_mapa.m_variables.VolumenGota[1]=index*DEFAULT_DROP_VOL;
        memoryMap->m_mapa.m_variables.SalvarFicheroConfiguracion=1;
        ui->m_gota->setText(QString("%1 %2").arg(memoryMap->m_mapa.m_variables.VolumenGota[0]).arg(memoryMap->m_mapa.m_variables.VolumenGota[1]));
    }
}

/** Cambia la fuerza de freno de manera automatica*/
void Maquina::SetFuerzaFrenoAuto(){
   // if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            //Buscamos en nuestras tablas el ancho segun la composicion
            int ancho=(int)memoryMap->m_mapa.m_variables.WebWidth;
            QMap<int, int>::iterator ip;
            QMap<int, int>::iterator i;
            for (i = memoryMap->m_mapa.m_maquina.qmDiamPresion.begin(); i != memoryMap->m_mapa.m_maquina.qmDiamPresion.end(); ++i){
                ip=i;
                if (ip!=memoryMap->m_mapa.m_maquina.qmDiamPresion.end()){
                    ip++;
                    if (ip.key()>=ancho) break;
                }
            }
            if ((abs(ancho-ip.key()))<=(abs(ancho-i.key()))) i=ip;
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            QVariant valor=QVariant(i.value());
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_FUERZA_FRENO,valor);
            memoryMap->m_mapa.m_maquina.AnchoBobina=i.key();
            emit SendDebugInfo(LOGGER_WARNING,QString("WebWidth %1 AutoAncho %2 AutoFreno %3").arg(memoryMap->m_mapa.m_variables.WebWidth).arg(i.key()).arg(i.value()));
            int index=ui->m_cbAnchoBobina->findText(QString("%1").arg(i.key()));
            ui->m_cbAnchoBobina->setCurrentIndex(index);
            ui->m_leFreno->setText(QString("%1").arg(i.value()));
  //  }


}

/** Actualiza el freno seguno el ancho de bobina*/
void Maquina::changeFreno(int i){
    QString dummy=ui->m_cbAnchoBobina->itemText(i);
    int newValue=memoryMap->m_mapa.m_maquina.qmDiamPresion.value(dummy.toInt());
    ui->m_leFreno->setText(QString("%1").arg(newValue));
    //Enviamos nuevo item
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            QVariant valor=QVariant(newValue);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_FUERZA_FRENO,valor);
    }
    memoryMap->m_mapa.m_maquina.AnchoBobina=dummy.toInt();
    emit SendDebugInfo(LOGGER_WARNING,QString("Ancho %1 Freno %2").arg(dummy).arg(newValue));
}

/** Inicializa la zona de fija*/
void Maquina::InitFixFrame(){
    if (!memoryMap->m_mapa.m_variables.DisablePrinting){
        newCard=new InkjetXusbCtrl(0,0);
        //newCard2=new InkjetXusbCtrl(0,1);
    }
}

/** Inicializa la zona de variable*/
void Maquina::InitVariableFrame(){
    if (!memoryMap->m_mapa.m_variables.DisablePrinting){
        newVarCard=new InkjetXusbVarCtrl(0,0,NULL);
        for (int x=0;x<SIZE_BUFFER_IMAGES;x++){
            this->BufferVarImages[x].m_state=XAAR_IMAGEN_VACIA;
        }
    }
    //Creamos los hilos para variable
    threadSender=new QThread;
    threadCreator=new QThread;
    ImageSender=NULL;
    ImageCreator=NULL;
    //Sender de imagenes
    /*ImageSender=new ImageVarXaarSender(0,0,BufferVarImages);
    //Creator de imagenes
    ImageCreator=new ImageVarXaarCreator(0,BufferVarImages);
    connect(this,SIGNAL(StartGenerating()),ImageCreator,SLOT(StartGeneration()));
    connect(this,SIGNAL(PauseGenerating()),ImageCreator,SLOT(PauseGeneration()));
    connect(this,SIGNAL(StopThread()),ImageCreator,SLOT(Stop()));
    connect(this,SIGNAL(StopThread()),threadCreator,SLOT(deleteLater()));
    connect(this,SIGNAL(StopThread()),threadSender,SLOT(deleteLater()));
    connect(this,SIGNAL(CurrentPrintSwathe(int)),ImageSender,SLOT(XaarLastSwathePrinted(int)));
    connect(ImageSender,SIGNAL(CompleteBufferReplaced()),this,SLOT(UpdateTotalVarXarSentSwathe()));
    connect(ImageSender,SIGNAL(LastSwatheReplaced(int)),this,SLOT(UpdateVarXarSentSwathe(int)));
    connect(ImageSender, SIGNAL(error(unsigned char,QString)), GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));*/
}

/** Se ha ejecutado un soft stop externo*/
void Maquina::ExternalSoftStop(){
    //emit PauseGenerating();
}

/** Ejecuta soft stop*/
void Maquina::SoftStop(){
    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea parar la impresión?","Si","No",QString::null, 0, 1 );
    //int respuesta=0;
    if (respuesta==0){
        memoryMap->m_mapa.m_maquina.softstopear=true;
        Insertar_Evento(ERROR_GUI,QString("Parada desde interfaz"));
    }
}


void Maquina::TestRoot(){
   if (isRoot<1){
        /*ui->m_CambiarMetrosTrabajo->setEnabled(false);
        ui->m_CambiarVelocidadCinta->setEnabled(false);
        ui->m_CambiarDiametroSalida->setEnabled(false);
        ui->m_CambiarDiametroBobinaParo->setEnabled(false);
        ui->m_CambiarDiametroBobina->setEnabled(false);*/
       //ui->m_CambiarDiametroBobinaParo->setEnabled(false);
       //ui->m_CambiarDiametroBobina->setEnabled(false);
       ui->m_BuscarMacula->setEnabled(false);
        ui->m_JogMas->setEnabled(false);
        ui->m_JogMenos->setEnabled(false);
        ui->m_Empalme->setEnabled(false);

        //ui->m_SoftStop->setEnabled(false);
        ui->m_TrabajoNuevo->setEnabled(false);
	}

   if (isRoot<3){
       ui->m_ToMaquina2->setEnabled(false);
       ui->tabWidget->setTabEnabled(1,false);
   }
}

Maquina::~Maquina(){
    delete newCard;
    /*delete ImageSender;
    delete ImageCreator;*/
}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void Maquina::AceptarSeleccion(){
    StopThreads();
    LaunchPrincipal();
    this->deleteLater();
}


void Maquina::CancelarSeleccion(){
    StopThreads();
    LaunchPrincipal();
    this->deleteLater();
    /*LaunchPrincipal();
    this->showMinimized();*/
}


// Actualiza los datos de pantalla
void Maquina::ActualizaDataSlot(){
    static char counter=0;
    //emit SendDebugInfo(LOGGER_ERROR,"Escribo!!!!!!!");
    ui->m_gota->setText(QString("%1-%2").arg(memoryMap->m_mapa.m_variables.VolumenGota[0]).arg(memoryMap->m_mapa.m_variables.VolumenGota[1]));
   /* ui->m_gota1->setCurrentIndex(memoryMap->m_mapa.m_variables.VolumenGota[0]/DEFAULT_DROP_VOL);
    ui->m_gota2->setCurrentIndex(memoryMap->m_mapa.m_variables.VolumenGota[1]/DEFAULT_DROP_VOL);*/

    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
    if (memoryMap->ImagenVariable){
        //Pedimos datos de impresion al XAAR cada segundo!
        //bXaarScorpionGenerateProductDetect(0);
        int dummyCounter=m_ActualSwathe;
        m_ActualSwathe=AskXaar();
        if (dummyCounter!=m_ActualSwathe){
            //Cambiamos la imagen a mostrar cada 10 sg, en caso de que haya algo
            counter++;
            counter=counter%10;
            if (!counter){
                if (BufferVarImages[m_ActualSwathe].m_state!=XAAR_IMAGEN_VACIA) MuestraImagen(QPixmap::fromImage(BufferVarImages[m_ActualSwathe].m_Imagen));
            }
        }
    }
    //Aqui pregunto por el numero de xusb todavia funcionando
    //else AskXaarPrintState();

}
/** Preguntamos por el estado de los xaar*/
void Maquina::AskXaarPrintState(){
    if (!m_xUSBWorking.isEmpty()){
        for (int x=0;x<m_iTotalXUSB;x++){
            if (bXaarScorpionPrintComplete(x)){
                emit SendDebugInfo(LOGGER_WARNING,QString("Print complete XUSB %1").arg(x));
                m_xUSBWorking.remove(0);
            }
        }
    }
}

void Maquina::CambiarVelocidad(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la velocidad?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            memoryMap->m_mapa.m_maquina.ref_velocidad=ui->m_VelocidadCinta->toPlainText().toInt();
            //QVariant valor=QVariant(memoryMap->m_mapa.m_maquina.ref_velocidad);
            //((Principal*)dummy)->miCliente->EscribirDato(ITEM_VELOCIDAD_REF,valor);

            ((Principal*)dummy)->SetNuevaVelocidadReferencia(memoryMap->m_mapa.m_maquina.ref_velocidad);
            ((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_AUTOMATICO);
            //((Principal*)dummy)->AjustarAceleracion(MOVIMIENTO_AUTOMATICO);
            //((Principal*)dummy)->AjustarDeceleracion(MOVIMIENTO_AUTOMATICO);
            //((Principal*)dummy)->AjustarTiempoBusquedaMacula(MOVIMIENTO_AUTOMATICO);
            ((Principal*)dummy)->Asignacion_Referencia_Velocidad();
        }
        else{
            int valor_actual=ui->m_VelocidadCinta->toPlainText().toInt();
            ui->m_VelocidadCinta->setText(QString("<font color=\"red\">%1</font>").arg(valor_actual));
        }
    }
}

void Maquina::CambiarMetros(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar los metros de trabajo?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            memoryMap->m_mapa.m_maquina.ref_metros_trabajo=ui->m_MetrosTrabajo->toPlainText().toInt();
            QVariant valor=QVariant(memoryMap->m_mapa.m_maquina.ref_metros_trabajo);
            //((Principal*)dummy)->miCliente->MiServidorOPC->escribeDato(OPC_Bobinadora.LongitudRef.client_handle,valor);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_METROS_REF,valor);
        }
        else{
            int valor_actual=ui->m_MetrosTrabajo->toPlainText().toInt();
            ui->m_MetrosTrabajo->setText(QString("<font color=\"red\">%1</font>").arg(valor_actual));
        }
    }
}

void Maquina::CambiarDiametro(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar el diametro de bobina de entrada?\nLos servos deben estar inhabilitados.\nPulse la seta de emergencia antes de continuar.","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            QWidget *dummy;
            QVariant valor;
            dummy=GlobalGetMainWindow();
            memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=ui->m_DiametroBobinaEntrada->toPlainText().toInt();
            memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada;
            ui->m_DiametroBobinaEntradaAct->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.diametro_bobina_entrada));
            valor=QVariant(memoryMap->m_mapa.m_maquina.diametro_bobina_entrada);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_DIAMETRO_ENTRADA_REF,valor);
            _Sleep(wait);
            valor=QVariant(true);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,valor);
            _Sleep(wait);
            valor=QVariant(false);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_ENTRADA,valor);
        }
        else{
            int valor_actual=ui->m_DiametroBobinaEntrada->toPlainText().toInt();
            ui->m_DiametroBobinaEntrada->setText(QString("<font color=\"red\">%1</font>").arg(valor_actual));
        }
    }
}

void Maquina::CambiarDiametroSalida(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar el diametro de bobina de salida?\nLos servos deben estar inhabilitados.\nPulse la seta de emergencia antes de continuar.","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            QWidget *dummy;
            QVariant valor;
            dummy=GlobalGetMainWindow();
            memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=ui->m_DiametroBobinaSalida->toPlainText().toInt();
            //memoryMap->m_mapa.m_maquina.diametro_bobina_salida=memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida;
            //ui->m_DiametroBobinaSalidaAct->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.diametro_bobina_salida));
            valor=QVariant(memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_DIAMETRO_SALIDA_REF,valor);
            _Sleep(wait);
            valor=QVariant(true);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,valor);
            _Sleep(wait);
            valor=QVariant(false);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_FIJAR_DIAMETRO_SALIDA,valor);
        }
        else{
            int valor_actual=ui->m_DiametroBobinaSalida->toPlainText().toInt();
            ui->m_DiametroBobinaSalida->setText(QString("<font color=\"red\">%1</font>").arg(valor_actual));
        }
    }
}

void Maquina::CambiarDiametroParo(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar el diametro de paro de bobina?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            memoryMap->m_mapa.m_maquina.diametro_parar=ui->m_DiametroBobinaEntradaParo->toPlainText().toInt();
            QVariant valor=QVariant(memoryMap->m_mapa.m_maquina.diametro_parar);
            //((Principal*)dummy)->miCliente->MiServidorOPC->escribeDato(OPC_Bobinadora.DiametroParar.client_handle,valor);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_DIAMETRO_PARAR,valor);
        }
        else{
            int valor_actual=ui->m_DiametroBobinaEntradaParo->toPlainText().toInt();
            ui->m_DiametroBobinaEntradaParo->setText(QString("<font color=\"red\">%1</font>").arg(valor_actual));
        }
    }
}


void Maquina::CambiarTrabajo(){
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar de trabajo?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
        QWidget *dummy;
            dummy=GlobalGetMainWindow();
            //((Principal*)dummy)->miCliente->MiServidorOPC->escribeDato(OPC_Bobinadora.TrabajoNuevo.client_handle,valor);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_TRABAJO_NUEVO,QVariant(1));
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_TRABAJO_NUEVO,QVariant(1));
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_TRABAJO_NUEVO,QVariant(0));
        }
    }
}

// Actualiza la info
void Maquina::ActualizaDataSlotOPC(){

    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){

        //QVariant valor;
        int valor_int;
        static unsigned char iRefresh=4;
        static bool primeravez=true;


        QWidget *dummy;
        dummy=GlobalGetMainWindow();

        iRefresh++;

        if (memoryMap->m_mapa.m_maquina.impresion==false){
            ui->m_BuscarMacula->setEnabled(true);
            ui->m_JogMas->setEnabled(true);
            ui->m_JogMenos->setEnabled(true);
            ui->m_Empalme->setEnabled(true);
            ui->m_BobinaNueva->setEnabled(true);
            ui->m_TrabajoNuevo->setEnabled(true);
           ///ui->m_CambiarDiametroBobinaParo->setEnabled(true);
            //ui->m_SoftStop->setEnabled(false);
            ui->m_AceptarButton->setEnabled(true);
            ui->m_CancelButton->setEnabled(true);
            ui->m_ColaButton->setEnabled(true);
            ui->SelImgFija->setEnabled(true);
            ui->SendIni->setEnabled(true);

        }
        else{
            ui->m_BuscarMacula->setEnabled(false);
            ui->m_JogMas->setEnabled(false);
            ui->m_JogMenos->setEnabled(false);
            ui->m_Empalme->setEnabled(false);
            ui->m_BobinaNueva->setEnabled(false);
            ui->m_TrabajoNuevo->setEnabled(false);
            ///ui->m_CambiarDiametroBobinaParo->setEnabled(false);
            //ui->m_SoftStop->setEnabled(true);

            ui->m_AceptarButton->setEnabled(false);
            ui->m_CancelButton->setEnabled(false);
            ui->m_ColaButton->setEnabled(false);
            ui->SelImgFija->setEnabled(false);
            ui->SendIni->setEnabled(false);
        }


        if (iRefresh==5){

            //Leer metros trabajo actuales
            //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.LongitudActual.client_handle).toInt();
            valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_METROS_ACT).toInt();
            ui->m_MetrosTrabajoAct->setText(QString("%1").arg(valor_int));

            //Leer velocidad actual
            //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.VelocidadActual.client_handle).toInt();
            valor_int=memoryMap->m_mapa.m_maquina.velocidad;
            if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
                //dummy_f=(float) (valor_int/1000);
                int IntRef=(valor_int/1000);
                int DecRef=(valor_int%1000);
                //ui->m_VelocidadCintaAct->setText(QString("%1").arg(dummy_f).left(4));
                ui->m_VelocidadCintaAct->setText(QString("%1.%2").arg(IntRef).arg(DecRef).left(5));
            }
            else{
                //dummy_f=(float) (valor_int/100);
                //ui->m_VelocidadCintaAct->setText(QString("%1").arg(dummy_f).left(4));
                int IntRef=(valor_int/100);
                int DecRef=(valor_int%100);
                ui->m_VelocidadCintaAct->setText(QString("%1.%2").arg(IntRef).arg(DecRef).left(5));
            }

            //Leer diametro actual
            //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.DiametroActual.client_handle).toInt();
            valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_DIAMETRO_ENTRADA_ACT).toInt();
            ui->m_DiametroBobinaEntradaAct->setText(QString("%1").arg(valor_int));

            valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_DIAMETRO_SALIDA_ACT).toInt();
            ui->m_DiametroBobinaSalidaAct->setText(QString("%1").arg(valor_int));

            //Leer temperatura de lámpara
            valor_int=memoryMap->m_mapa.m_maquina.temperatura_lampara;
            ui->m_TemperaturaLampara->setText(QString("%1").arg(valor_int));
            //Leer Intensidad de lampara
            valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_INTENSIDAD_LAMPARA).toInt();
            ui->m_Intensidad->setText(QString("%1").arg(valor_int));
            //Leer referencias
            if (primeravez==true){ //lo quitamos si la pantalla de máquina no se destruye
                ForceRefreshValues();
                primeravez=false;
            }

            iRefresh=0;
        }
    }

}
/** Fuerzo un refresco de valores de referencia*/
void Maquina::ForceRefreshValues(){
    int valor_int;
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
       //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.LongitudRef.client_handle).toInt();
        valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_METROS_REF).toInt();
        ui->m_MetrosTrabajo->setText(QString("%1").arg(valor_int));

     //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.VelocidadRef.client_handle).toInt();
        valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_VELOCIDAD_REF).toInt();
        ui->m_VelocidadCinta->setText(QString("%1").arg(valor_int));

     //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.DiametroRef.client_handle).toInt();
        valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_DIAMETRO_ENTRADA_REF).toInt();
        ui->m_DiametroBobinaEntrada->setText(QString("%1").arg(valor_int));

        valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_DIAMETRO_SALIDA_REF).toInt();
        ui->m_DiametroBobinaSalida->setText(QString("%1").arg(valor_int));

     //valor_int=((Principal*)dummy)->miCliente->MiServidorOPC->leeDato(OPC_Bobinadora.DiametroParar.client_handle).toInt();
        valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_DIAMETRO_PARAR).toInt();
        ui->m_DiametroBobinaEntradaParo->setText(QString("%1").arg(valor_int));
    }
    ui->m_gota1->setCurrentIndex(memoryMap->m_mapa.m_variables.VolumenGota[0]/DEFAULT_DROP_VOL);
    ui->m_gota2->setCurrentIndex(memoryMap->m_mapa.m_variables.VolumenGota[1]/DEFAULT_DROP_VOL);


}


void Maquina::AceptarValorVelocidad(){
    //if (memoryMap->m_mapa.m_maquina.impresion==false){
    if (!InputData::exist()){
        InputData *input = InputData::request(this,"Velocidad Cinta [m/min]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarVelocidad(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarVelocidad(QString value){
    int dummy_int;
    //ui->m_VelocidadCinta->setText(value);
    ui->m_VelocidadCinta->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    //dummy_int=ui->m_VelocidadCinta->toPlainText().toInt();
    dummy_int=value.toInt();
    if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
        if (dummy_int<REF_VELOCIDAD_MIN) dummy_int=REF_VELOCIDAD_MIN;
        if (dummy_int>REF_VELOCIDAD_MAX) dummy_int=REF_VELOCIDAD_MAX;
    }
    else if (memoryMap->m_mapa.m_maquina.VersionMaquina==2){
        if (memoryMap->m_mapa.m_variables.NumColores==1){
            if (dummy_int<REF_VELOCIDAD_MIN) dummy_int=REF_VELOCIDAD_MIN;
            if (dummy_int>REF_VELOCIDAD_MAX) dummy_int=REF_VELOCIDAD_MAX;
        }
        else{
            if (dummy_int<REF_VELOCIDAD_MIN_2C) dummy_int=REF_VELOCIDAD_MIN_2C;
            if (dummy_int>REF_VELOCIDAD_MAX_2C) dummy_int=REF_VELOCIDAD_MAX_2C;
        }
    }
    ui->m_VelocidadCinta->setText(QString("<font color=\"black\">%1</font>").arg(dummy_int));
    //memoryMap->m_mapa.m_maquina.ref_velocidad=dummy_int;
    this->CambiarVelocidad();
}

void Maquina::AceptarValorMetros(){
   // if (memoryMap->m_mapa.m_maquina.impresion==false){
   if (!InputData::exist()){
        InputData *input = InputData::request(this,"Metros trabajo [m]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarMetros(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarMetros(QString value){
    int dummy_int;
    ui->m_MetrosTrabajo->setText(value);
    ui->m_MetrosTrabajo->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_MetrosTrabajo->toPlainText().toInt();
    if (dummy_int<METROS_TRABAJO_MIN) dummy_int=METROS_TRABAJO_MIN;
    if (dummy_int>METROS_TRABAJO_MAX) dummy_int=METROS_TRABAJO_MAX;
    //ui->m_MetrosTrabajo->setText(QString("%1").arg(dummy_int));
    ui->m_MetrosTrabajo->setText(QString("<font color=\"black\">%1</font>").arg(dummy_int));
    //memoryMap->m_mapa.m_maquina.metros_trabajo=dummy_int;
    this->CambiarMetros();
}

void Maquina::AceptarValorDiametro(){
   // if (memoryMap->m_mapa.m_maquina.impresion==false){
    if (!InputData::exist()){
        InputData *input = InputData::request(this,"Diámetro bobina entrada [mm]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDiametro(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarDiametro(QString value){
    int dummy_int;
    ui->m_DiametroBobinaEntrada->setText(value);
    ui->m_DiametroBobinaEntrada->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_DiametroBobinaEntrada->toPlainText().toInt();
    if (dummy_int<DIAMETRO_BOBINA_ENTRADA_MIN) dummy_int=DIAMETRO_BOBINA_ENTRADA_MIN;
    if (dummy_int>DIAMETRO_BOBINA_ENTRADA_MAX) dummy_int=DIAMETRO_BOBINA_ENTRADA_MAX;
    ui->m_DiametroBobinaEntrada->setText(QString("<font color=\"black\">%1</font>").arg(dummy_int));
    //ui->m_DiametroBobinaEntrada->setText(QString("%1").arg(dummy_int));
    //memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=dummy_int;
    this->CambiarDiametro();
}

void Maquina::AceptarValorDiametroParo(){
   // if (memoryMap->m_mapa.m_maquina.impresion==false){
   if (!InputData::exist()){
        InputData *input = InputData::request(this,"Diámetro paro bobina entrada [mm]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDiametroParo(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarDiametroParo(QString value){
    int dummy_int;
    ui->m_DiametroBobinaEntradaParo->setText(value);
    ui->m_DiametroBobinaEntradaParo->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_DiametroBobinaEntradaParo->toPlainText().toInt();
    if (dummy_int<DIAMETRO_PARO_BOBINA_MIN) dummy_int=DIAMETRO_PARO_BOBINA_MIN;
    if (dummy_int>DIAMETRO_PARO_BOBINA_MAX) dummy_int=DIAMETRO_PARO_BOBINA_MAX;
    ui->m_DiametroBobinaEntradaParo->setText(QString("<font color=\"black\">%1</font>").arg(dummy_int));
    //ui->m_DiametroBobinaEntradaParo->setText(QString("%1").arg(dummy_int));
    //memoryMap->m_mapa.m_maquina.diametro_parar=dummy_int;
    this->CambiarDiametroParo();
}

void Maquina::AceptarValorDiametroSalida(){
    //if (memoryMap->m_mapa.m_maquina.impresion==false){
    if (!InputData::exist()){
       InputData *input = InputData::request(this,"Diámetro bobina salida [mm]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDiametroSalida(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarDiametroSalida(QString value){
    int dummy_int;
    ui->m_DiametroBobinaSalida->setText(value);
    ui->m_DiametroBobinaSalida->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_DiametroBobinaSalida->toPlainText().toInt();
    if (dummy_int<DIAMETRO_BOBINA_SALIDA_MIN) dummy_int=DIAMETRO_BOBINA_SALIDA_MIN;
    if (dummy_int>DIAMETRO_BOBINA_SALIDA_MAX) dummy_int=DIAMETRO_BOBINA_SALIDA_MAX;
    ui->m_DiametroBobinaSalida->setText(QString("<font color=\"black\">%1</font>").arg(dummy_int));
    //ui->m_DiametroBobinaSalida->setText(QString("%1").arg(dummy_int));
    //memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=dummy_int;
    this->CambiarDiametroSalida();
}

void Maquina::LaunchPrincipal(){
    StopThreads();
    GlobalShowMainWindow();
    this->deleteLater();

    //this->showMinimized();

}

void Maquina::LaunchMaquina2(){
    //StopThreads();
    MaquinaAdv *ScrMaquina2;
    ScrMaquina2=new MaquinaAdv(0);
    ScrMaquina2->show();
    //Así cuando muera, actualizo la info de pantalla
    connect(ScrMaquina2,SIGNAL(destroyed()),this,SLOT(ForceRefreshValues()));
    //this->deleteLater();
    /*MaquinaAdv *ScrMaquina2=new MaquinaAdv(0);
    ScrMaquina2->show();
    this->showMinimized();*/
}

void Maquina::ShowCam(){
    Webcam *MiWebcam;
    MiWebcam= new Webcam(0);
    //Así cuando muera, actualizo la info de pantalla
    connect(MiWebcam,SIGNAL(destroyed()),this,SLOT(ForceRefreshValues()));

}

void Maquina::LaunchColaImpresion(){
    ColaTrabajo *ScrColaTrabajo;
    ScrColaTrabajo=new ColaTrabajo(this);
    ScrColaTrabajo->show();
    //this->deleteLater();
    /*ColaTrabajo *ScrColaTrabajo=new ColaTrabajo(0);
    ScrColaTrabajo->show();
    this->showMinimized();*/
    //Así cuando muera, actualizo la info de pantalla
    connect(ScrColaTrabajo,SIGNAL(destroyed()),this,SLOT(ForceRefreshValues()));
}

void Maquina::EjecutarJogMasOn(){
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            //Si estamos en Modo Manual
            ((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_JOGMAS);
            //((Principal*)dummy)->AjustarAceleracion(MOVIMIENTO_JOGMAS);
            //((Principal*)dummy)->AjustarDeceleracion(MOVIMIENTO_JOGMAS);
            if (memoryMap->m_mapa.m_maquina.jogmas_conlampara) ((Principal*)dummy)->miCliente->EscribirDato(ITEM_LAMPARA_ON,0x01);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_JOG_MAS,0x01);
        }
    }
}

void Maquina::EjecutarJogMenosOn(){
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            //Si estamos en Modo Manual
            ((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_JOGMENOS);
            //((Principal*)dummy)->AjustarAceleracion(MOVIMIENTO_JOGMAS);
            //((Principal*)dummy)->AjustarDeceleracion(MOVIMIENTO_JOGMAS);
            if (memoryMap->m_mapa.m_maquina.jogmenos_conlampara) ((Principal*)dummy)->miCliente->EscribirDato(ITEM_LAMPARA_ON,0x01);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_JOG_MENOS,0x01);
        }
    }
}


void Maquina::EjecutarJogMasOff(){
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            //Si estamos en Modo Manual
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_LAMPARA_ON,0x00);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_JOG_MAS,0x00);
        }
    }
}

void Maquina::EjecutarJogMenosOff(){
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            //Si estamos en Modo Manual
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_LAMPARA_ON,0x00);
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_JOG_MENOS,0x00);
         }
    }
}


void Maquina::EjecutarBuscarMacula(){

    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea buscar mácula?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(1));
            ((Principal*)dummy)->miCliente->EscribirDato(ITEM_BUSCAR_MACULA,QVariant(0));
        }
    }

}

void Maquina::EjecutarEmpalme(){

    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea iniciar un empalme de bobina?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            memoryMap->m_mapa.m_maquina.InicioSecuenciaEmpalme=0x01;
            memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=0x01;
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            QTimer::singleShot(0,((Principal*)dummy),SLOT(SecuenciaEmpalme_1()));
        }
    }
}

void Maquina::EjecutarBobinaNueva(){

    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea poner una bobina nueva?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            memoryMap->m_mapa.m_maquina.InicioSecuenciaBobinaNueva=0x01;
            memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=0x01;
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            QTimer::singleShot(0,((Principal*)dummy),SLOT(SecuenciaBobinaNueva_1()));

        }
    }
}


void Maquina::SelImagenFija(){

    SelImagen();

}


bool Maquina::SelImagen(){

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
            nombrefichero=QString("%1").arg(Ficheros.at(0)); //Nombre y ruta del bmp
            m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();
            memoryMap->setImagen(nombrefichero);
            MuestraImagen(memoryMap->m_Imagen);
            memoryMap->ImagenVariable=0;

            QString nombre=DevuelveNombre(nombrefichero);
            QString ruta=DevuelveRuta(nombrefichero);
            strncpy(memoryMap->m_mapa.m_variables.NombreBmpEnviado,nombre.toAscii().data(),RUTA_LEN);
            strncpy(memoryMap->m_mapa.m_variables.RutaBmpEnviado,ruta.toAscii().data(),RUTA_LEN);

            memoryMap->m_UltimaAccion=UA_IMAGEN_SELECCIONADA;
        }
     }

    ImagenXaar=nombrefichero;
    ui->SendImgFija->setEnabled(true);
    return true;
}

/** Termina con los hilos de impresion si los hubiera*/
void Maquina::StopThreads(){
    emit StopThread();
}

/** Envia Xaar*/
void Maquina::SendIniToXaar(){

    //this->blockSignals(true);
    //Reinicio el driver
  /*  XaarScorpionBootScorpionFPGA(0);
    XaarScorpionBootScorpionFPGA(1);*/


    //Manda el ini actual al Xaar
    //Para la imagen variable
/*    BOOL dummyError=bXaarScorpionDriverLoaded();
    if (dummyError==0){
        DWORD LastError=XaarScorpionGetErrorValue();
        QString dummy=QString("Error de conexión con XUSB. Last Error:%1").arg(LastError);
        emit SendDebugInfo(LOGGER_WARNING,dummy);
        return;
    }*/

    if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
        QWidget *dummy=GlobalGetMainWindow();
        ((Principal*)dummy)->miCliente->EscribirDato(ITEM_CONDICIONES_MOVIMIENTO,QVariant(1));
    }

    memoryMap->ImagenVariable=0x00;
    for(unsigned char i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01) memoryMap->ImagenVariable=0x01;
    }

    if (memoryMap->ImagenVariable==0x00){

        SendToXaar();
    }
    else{
        //Generar la primera imagen variable
        /*ImgVarGenerator *GeneradorImagenVariable=new ImgVarGenerator(0);
        indice=0;
        QImage dummy=GeneradorImagenVariable->CreaImagenVariable(indice);
        dummy.save("C:\\convariables.bmp");*/
        SendToVarXaar();
    }
    //this->blockSignals(false);

}
/** Enviamos imagen variable-1 xUSB*/
void Maquina::SendToVarXaar(){

    QLabel *InfoLabel=ShowMensaje("Enviando imagen...\nPor favor espere");

    VariableTokenIni=0;
    memoryMap->m_ImagenEnviada=true;
    memoryMap->m_UltimaAccion=UA_IMAGEN_ENVIADA;
    //Contador total de swathes enviadas
    m_TotalSwatheSent=0;
    //Swathe actual
    m_ActualSwathe=0;
    //De momento un solo xusb
    //m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();
    m_iTotalXUSB=1;
    //Se resetea el Buffer de Imagenes cada vez que le demos a un envio nuevo
    for (int x=0;x<SIZE_BUFFER_IMAGES;x++){
        this->BufferVarImages[x].m_state=XAAR_IMAGEN_VACIA;
    }

    if (!threadSender->isRunning()){
        SendDebugInfo(LOGGER_WARNING,QString("Launching threadSender"));
        threadSender->start();
    }
    if (!threadCreator->isRunning()){
        SendDebugInfo(LOGGER_WARNING,QString("Launching threadCreator"));
        threadCreator->start();
    }


    //ImageCreator->Reset();
    if (ImageCreator==NULL){
        ImageCreator=new ImageVarXaarCreator(0,BufferVarImages);
        connect(this,SIGNAL(StartGenerating()),ImageCreator,SLOT(StartGeneration()));
        connect(this,SIGNAL(PauseGenerating()),ImageCreator,SLOT(PauseGeneration()));
        connect(this,SIGNAL(StopThread()),ImageCreator,SLOT(Stop()));
        connect(this,SIGNAL(StopThread()),threadCreator,SLOT(deleteLater()));
        connect(this,SIGNAL(StopThread()),threadSender,SLOT(deleteLater()));
        ImageCreator->moveToThread(threadCreator);
    }
    else{
        ImageCreator->Reset();
    }
    /*ImageSender->Reset();
    ImageSender->PreloadImages();*/
    if (ImageSender==NULL){
        ImageSender=new ImageVarXaarSender(0,0,BufferVarImages);
        ImageSender->PreloadImages();
        connect(this,SIGNAL(CurrentPrintSwathe(int)),ImageSender,SLOT(XaarLastSwathePrinted(int)));
        connect(ImageSender,SIGNAL(CompleteBufferReplaced()),this,SLOT(UpdateTotalVarXarSentSwathe()));
        connect(ImageSender,SIGNAL(LastSwatheReplaced(int)),this,SLOT(UpdateVarXarSentSwathe(int)));
        ImageSender->moveToThread(threadSender);
    }
    else{
        ImageSender->Reset();
        ImageSender->PreloadImages();
    }
    MuestraImagen(QPixmap::fromImage(BufferVarImages[0].m_Imagen));
    //ui->m_ImagenCentral->setText("Imprimiendo");
    delete InfoLabel;
    QMessageBox::warning(0,"INFORMACION","Imagen enviada:\nPulsa el botón de inicio",QMessageBox::Ok);
    emit StartGenerating();
}

/*Actualiza el contador de swathes*/
void Maquina::UpdateTotalVarXarSentSwathe(){
    m_TotalSwatheSent+=SIZE_BUFFER_IMAGES;
}
void Maquina::UpdateVarXarSentSwathe(int last){

    int total=m_TotalSwatheSent+last;
    static int lastTotal=0;
    if (total>=VARIABLE_MAX_SWATHE_PRINT){
        if (!VariableTokenIni){
            /*emit PauseGenerating();
            StopThreads();*/
            SoftStop();
            VariableTokenIni=1;
            SendDebugInfo(LOGGER_DEBUG,QString("PARADA POR LLEGAR A %1").arg(VARIABLE_MAX_SWATHE_PRINT));
        }
    }
    //Par que no esté continuamente enviando
    if (lastTotal!=total){
        QString dummy=QString("NowPrinting(%1)-LastReplaced(%2)-TotalSent(%3)").arg(m_ActualSwathe).arg(last).arg(total);
        emit SendDebugInfo(LOGGER_DEBUG,dummy);
        lastTotal=total;
    }
}




/** Enviamos imagen fija-multiples Xusb*/
void Maquina::SendToXaar(){

    QString dummy;
    QString nombre;
    QString ruta;
    QString nombreruta;

    QLabel *InfoLabel=ShowMensaje("Enviando imagen...\nPor favor espere");

    nombreruta=nombrefichero; //nombrefichero:nombre y ruta

    nombreruta.replace("/","\\");
    nombre=DevuelveNombre(nombreruta);
    ruta=DevuelveRuta(nombreruta);

    memoryMap->m_ImagenEnviada=true;
    memoryMap->m_UltimaAccion=UA_IMAGEN_ENVIADA;

    strncpy(memoryMap->m_mapa.m_variables.NombreRutaBmpEnviado,nombreruta.toAscii(),RUTA_LEN);
    strncpy(memoryMap->m_mapa.m_variables.NombreBmpEnviado,nombre.toAscii(),RUTA_LEN);
    strncpy(memoryMap->m_mapa.m_variables.RutaBmpEnviado,ruta.toAscii(),RUTA_LEN);

     //Codigo XAar modificado para Qt...este debiera funcionar

   /* InkjetMultiview InkjetXaar;
    InkjetXaar.SendImage(ImagenXaar);
    GeneratePD();
    */

    //Nuestro codigo adaptado a multithread
    //m_xUSBWorking.clear();
    m_threads=0;
    m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();
    //QMessageBox::warning(0,"Info",QString("Num USB detectados %1").arg(m_iTotalXUSB),QMessageBox::Ok);
    m_threads=m_iTotalXUSB;

    /*newCard1->setCard(0);
    newCard1->Reset();
    bool bRet;
    QString dummyString;
    QString dummyString2;

    bRet=ImagenXaar.endsWith("_C1_comp.bmp",Qt::CaseSensitive);
    if (bRet) dummyString=ImagenXaar.remove("_C1_comp.bmp");
    else dummyString=ImagenXaar.remove("_C2_comp.bmp");
    dummyString.append(QString("_C1_comp.bmp"));

    QFile dummyFile(dummyString);
    if (dummyFile.exists()){
        QImage processedImage(dummyString,"BMP");
        //connect(newCard1, SIGNAL(error(unsigned char,QString)), this, SLOT(ReceiveDebugInfo(unsigned char,QString)));
        connect(newCard1, SIGNAL(error(unsigned char,QString)), GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
        newCard1->SetImage(processedImage);
        newCard1->process();
        emit SendDebugInfo(LOGGER_WARNING,QString("Fichero %1 to xUSB 0 OK").arg(dummyString));
    }

    newCard1->setCard(1);
    newCard1->Reset();

    bRet=ImagenXaar.endsWith("_C1_comp.bmp",Qt::CaseSensitive);
    if (bRet) dummyString2=ImagenXaar.remove("_C1_comp.bmp");
    else dummyString2=ImagenXaar.remove("_C2_comp.bmp");
    dummyString2.append(QString("_C2_comp.bmp"));

    QFile dummyFile2(dummyString2);
    if (dummyFile2.exists()){
        QImage processedImage2(dummyString2);
        connect(newCard1, SIGNAL(error(unsigned char,QString)), GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
        newCard1->SetImage(processedImage2);
        newCard1->process();
        emit SendDebugInfo(LOGGER_WARNING,QString("Fichero %1 to xUSB 1 OK").arg(dummyString2));
    }*/

  //  char volGota=(char)ui->cbVolGota->currentIndex();
    bool dummyExist=false;
    //Carga la fuerza de freno de manera automatica
    SetFuerzaFrenoAuto();
    for (int x=0;x<m_iTotalXUSB;x++){
        newCard->setCard(x);
        newCard->Reset();
        char volGota=(memoryMap->m_mapa.m_variables.VolumenGota[x])/DEFAULT_DROP_VOL;
        emit SendDebugInfo(LOGGER_WARNING,QString("xUSB %1 Vol.Gota %2(%3 pl)").arg(x).arg((int)volGota).arg((int)memoryMap->m_mapa.m_variables.VolumenGota));
        newCard->setNewPalette(volGota);
        QString dummyString=ImagenXaar.remove(QString("_C%1_comp.bmp").arg(x+1));
        dummyString.append(QString("_C%1_comp.bmp").arg(x+1));
        QFile dummyFile(dummyString);
        if (dummyFile.exists()){
            QImage processedImage(dummyString,"BMP");
            //connect(newCard1, SIGNAL(error(unsigned char,QString)), this, SLOT(ReceiveDebugInfo(unsigned char,QString)));
            if (x==0) connect(newCard, SIGNAL(error(unsigned char,QString)), GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
            newCard->SetImage(processedImage);
            newCard->process();
            emit SendDebugInfo(LOGGER_WARNING,QString("Fichero %1 to xUSB %2 OK").arg(dummyString).arg(x));
            dummyExist=true;
        }
    }

    if (memoryMap->m_mapa.m_maquina.VersionMaquina==1) GeneratePD();
    //JORGE LO QUITO Y LO PONGO EN LA SECUENCIA DE AUTOMATICO GeneratePD();
    delete InfoLabel;

    if (dummyExist){
        QMessageBox::warning(0,"INFORMACION","Imagen enviada:\nPulsa el botón de inicio",QMessageBox::Ok);
        if (memoryMap->m_UltimaAccion==UA_IMAGEN_ENVIADA) Insertar_Evento(ENVIA_COMPOSICION,QString("%1").arg(memoryMap->m_mapa.m_variables.NombreRutaBmpEnviado));
        else if (memoryMap->m_UltimaAccion==UA_CONFIGURACION_CARGADA) Insertar_Evento(ENVIA_COMPOSICION,QString("%1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));
    }else{
        QMessageBox::warning(0,"ATENCION","Error al enviar imagen",QMessageBox::Ok);
    }
    if (memoryMap->m_UltimaAccion==UA_IMAGEN_ENVIADA) Insertar_Evento(ENVIA_COMPOSICION,QString("%1").arg(memoryMap->m_mapa.m_variables.NombreRutaBmpEnviado));
    else if (memoryMap->m_UltimaAccion==UA_CONFIGURACION_CARGADA) Insertar_Evento(ENVIA_COMPOSICION,QString("%1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));


/*
    for (int Card=0;Card<m_iTotalXUSB;Card++){
        //if (Card==0){
        InkjetXusbCtrl *newCard=new InkjetXusbCtrl(0,Card);
        QThread *newThread= new QThread;
        //Para cada tarjeta habrá que saber los cabezales (width)
        QString dummyString=ImagenXaar.remove("_C1_comp.bmp");
        dummyString.append(QString("_C%1_comp.bmp").arg(Card+1));
        QFile dummyFile(dummyString);
        if (dummyFile.exists()){
            QImage processedImage(dummyString);
            newCard->SetImage(processedImage);            
            connect(newThread, SIGNAL(started()), newCard, SLOT(process()));
            connect(newCard, SIGNAL(finished()), newThread, SLOT(quit()));
            connect(newCard, SIGNAL(finished()), this, SLOT(sltLaunchPD()));
            connect(newCard, SIGNAL(finished()), newCard, SLOT(deleteLater()));
            connect(newThread, SIGNAL(finished()), newThread, SLOT(deleteLater()));
            connect(newCard, SIGNAL(error(QString,int)), this, SLOT(sltErrorString(QString,int)));
            newCard->moveToThread(newThread);
            newThread->start();
            _Sleep(1000);
            emit SendDebugInfo(LOGGER_WARNING,QString("Fichero %1 to xUSB %2 OK").arg(dummyString).arg(Card));
        }
        else{
            m_threads--;
            emit SendDebugInfo(LOGGER_WARNING,QString("No existe fichero %1").arg(dummyString));
        }
        //m_xUSBWorking.append(1);
        //}
    }
*/
}

/** Preguntamos al xaar por donde va*/
int Maquina::AskXaar(){
    int dummy=0;
    dummy=(int)newVarCard->GetCurrentPrintSwathe();
    //SendDebugInfo(LOGGER_WARNING,QString("Printing swathe %1").arg(dummy));
    //qDebug()<<QString("Printing swathe %1").arg(dummy);
    //Emitimos la swathe actual
    emit CurrentPrintSwathe(dummy);
    return dummy;
}

QImage Maquina::createSubImage(QImage* image, const QRect & rect) {
    size_t offset = rect.x() * image->depth() / 8
                    + rect.y() * image->bytesPerLine();
    return QImage(image->bits() + offset, rect.width(), rect.height(),
                  image->bytesPerLine(), image->format());
}

/** Maneja los errores de los hilos*/
void Maquina::sltErrorString(QString err,int xusb){
    QString texto=QString("%1 [xUsb %2]").arg(err).arg(xusb);
    QMessageBox::warning(0,"INFORMACION",texto,QMessageBox::Ok);
}

/** Lanza el PD en caso de que todos los hilos hayan terminado*/
void Maquina::sltLaunchPD(){
    m_threads--;
    if (m_threads==0){
        GeneratePD();
        QMessageBox::warning(0,"INFORMACION","IMAGEN ENVIADA-PULSA BOTON INICIO",QMessageBox::Ok);
    }
}


//Carga los datos de memoria
void Maquina::CargaDatos(){

    ui->SendImgFija->setVisible(false);

    if (memoryMap->m_FilePath!=QString("none")){
        ImagenXaar=memoryMap->m_FilePath;
        MuestraImagen(memoryMap->m_Imagen);
        ui->SendImgFija->setEnabled(true);
        //emit SendDebugInfo(LOGGER_DEBUG,"Hay imagen cargada previamente");
        qDebug() << "Hay imagen cargada previamente" << memoryMap->m_FilePath;
    }
    else{
        ui->SendImgFija->setEnabled(false);
        qDebug() << "No hay imagen en memoria";
    }

}

void Maquina::GeneratePD(){
    for (int Card=0;Card<m_iTotalXUSB;Card++){
        bXaarScorpionGenerateProductDetect(Card);
        emit SendDebugInfo(LOGGER_DEBUG,QString("PD %1").arg(Card));
    }
    qDebug() << "PD generado";
    //QMessageBox::warning(0,"Info","PD enviado",QMessageBox::Ok);
}

void Maquina::StopPrinting(){
    for (int Card=0;Card<m_iTotalXUSB;Card++)
        bXaarScorpionEnablePrintMode(Card,0xFFFF,false);
}

//Muestra la imagen central
void Maquina::MuestraImagen(QPixmap Imagen){
    unsigned char escalado;
    QPixmap localImage=Imagen;
    int offset_x;
    int offset_y;

    int lw=ui->m_ImagenCentral->width();
    int lh=ui->m_ImagenCentral->height();
    int iw=Imagen.width();
    int ih=Imagen.height();


    if ((iw==0)||(ih==0)) return;
    escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

    //Testeamos si hay que hacer una composicion de lo que hay
    QString OriginalName=memoryMap->m_FilePath;
    if (OriginalName.contains("_C1_comp.bmp")){
        QString ImageHead1=OriginalName;
        QString ImageHead2=OriginalName.remove("_C1_comp.bmp");
        ImageHead2.append("_C2_comp.bmp");
        QFile testFile(ImageHead2);
        if (testFile.exists()){
            QPixmap pixmapHead1(ImageHead1);
            QPixmap pixmapHead2(ImageHead2);
            QPixmap MaskPixmap(pixmapHead2.size());
            MaskPixmap.fill(Qt::red);
            QPainter p(&pixmapHead2);
            p.setCompositionMode(QPainter::RasterOp_SourceOrDestination);
            p.drawPixmap(0,0,MaskPixmap);
            p.end();
            QPainter p2(&pixmapHead1);
            p2.setCompositionMode(QPainter::CompositionMode_Multiply);
            p2.drawPixmap(0,0,pixmapHead2);
            localImage=pixmapHead1;
        }
    }


    ui->m_ImagenCentral->setOriginalPixmap(localImage);

    QPixmap pic(ui->m_ImagenCentral->size());
    QPainter p(&pic);   // create a QPainter for it
    //pic.fill(Qt::gray);
    QLinearGradient lg(0, 0, 0, this->ui->m_ImagenCentral->height());
    lg.setColorAt(0, Qt::gray);
    //lg.setColorAt(0, QColor(18,18,18));
    lg.setColorAt(1, Qt::lightGray);
    p.setPen(Qt::NoPen);
    p.setBrush(lg);
    p.drawRect(ui->m_ImagenCentral->rect());

    QPixmap dummy;

    switch (escalado){
    case IMAGEN_SINESCALAR:
        dummy=localImage;
        offset_x=(ui->m_ImagenCentral->width()-dummy.width())/2;
        offset_y=(ui->m_ImagenCentral->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        //ui->m_ImagenCentral->setOriginalPixmap(Imagen);
        ui->m_ImagenCentral->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2W:
        dummy=localImage.scaledToWidth(ui->m_ImagenCentral->width(),Qt::SmoothTransformation);
        offset_x=(ui->m_ImagenCentral->width()-dummy.width())/2;
        offset_y=(ui->m_ImagenCentral->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        //ui->m_ImagenCentral->setOriginalPixmap(Imagen);
        ui->m_ImagenCentral->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2H:
        dummy=localImage.scaledToHeight(ui->m_ImagenCentral->height(),Qt::SmoothTransformation);
        offset_x=(ui->m_ImagenCentral->width()-dummy.width())/2;
        offset_y=(ui->m_ImagenCentral->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        //ui->m_ImagenCentral->setOriginalPixmap(Imagen);
        ui->m_ImagenCentral->setPixmap((QPixmap)(pic));
        break;
    }

    p.end();
    ui->m_ImagenCentral->show();
}


 /** Cargamos datos comunes*/
 void Maquina::setDataCommon(){
     struct stCommon dummyDataCommon;
     dummyDataCommon.fecha=this->ui->fecha;
     //dummyDataCommon.hora=this->ui->hora;
     dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
     dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
     dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
     dummyDataCommon.m_Imagen=this->ui->m_ImagenCentral;
     dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
     dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
     dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
     dummyDataCommon.m_User=this->ui->m_User;
     dummyDataCommon.usuario_actual=this->ui->usuario_actual;
     dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
     dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
     dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
     DataCommon=new InkjetCommon(this,dummyDataCommon);
     //emit SendDebugInfo(LOGGER_ERROR,"Inicializacion OK");
 //Actualizamos
     DataCommon->ShowInfo();
     DataCommon->ShowEstadoImpresora();
     DataCommon->ShowComms();
 }


