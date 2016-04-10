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
#include "clienteopc.h"
#include "maquina.h"
#include "maquinaAdv.h"
#include "globals.h"
#include "memoryMap.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
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
//#include "webcam.h"
#include <QDebug>
#include <QRect>
#include <QAxObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QProgressBar>
#include <QSignalMapper>
#include <QToolBar>
//Fichero de cabecera para Xaar
#include "ScorpionDLL-ext.h"
#include "inkjetmultiview.h"
#include "inkjetxusbctrl.h"
#include "imgvargenerator.h"
#include <QtCore/qmath.h>
#include <QSettings>
#include "in2colaimpresion.h"
#include "in2colaimpresionmono.h"
#include "in2gramarcolaimpresion.h"
#include "in2xmlparser.h"
#include "Fluidica.h"
#include "in2message.h"
#include "in2xmlparser.h"
#include "in2gestorchiller.h"
#include "chiller.h"
#include "in2configprint.h"
#include "in2counters.h"
#include "in2managerreport.h"
#include "in2gestormaquina.h"
#include "in2managererp.h"
#include "in2esteveerp.h"
#include "in2erp.h"
#include "in2configuratorerp.h"
#include "in2configuratorvision.h"
#include "in2aisvisionselector.h"
#include "in2eventfilter.h"
#include "in2gestorvision.h"
#include "compileversion.h"
#include "in2esteveend.h"
#include "status.h"
#include "In2Framework.h"
#include "in2windowsdetector.h"
#include "in2compofile.h"
#include "in2configuratordesigner.h"
#include "in2configlampara.h"
#include "in2gestorcorte.h"
#include "in2gestorlampara.h"
#include "in2configcorte.h"
#include "in2configuratorpermisos.h"

#define PRINT_GUARD 10
//Para limpieza...en diecisesavos
#define CLEAN_TRIM_VALUE 240

#define SIMULATE_TIME_MACULAS 500

#define CLEAN_TIME 120
//Imagen por defecto limpia
#define DEFAULT_CLEAN_IMAGE ":res/images/CleanImage.bmp"
//Imagen por defecto para limpiar
#define DEFAULT_SPIT_IMAGE ":res/images/SpitImage.bmp"
//Tiempo de espera
#define SLEEP_TIME 500
//Tiempo de refresco de datos
#define TIMEOUT_REFRESH_DATA 1500
//Encoder de limpieza
//#define DEFAULT_CLEAN_ENCODER 20000
//Velocidad de inspeccion
#define VELOCIDAD_INSPECCION 6
//Tiempo de inspeccion en msg
#define TIEMPO_INSPECCION 15000
//Tiempo para actualizar pantalla
#define UPDATE_SCREEN_TIMEOUT 3000
//Tiempo de refresco de la foto
#define TIMEOUT_PHOTO 5000

//Tama?o m?ximo de meoria en KB a usar del XAAR
#define MIN_XAAR_MEM_TO_USE 10000//250000
#define MAX_XAAR_MEM_TO_USE 450000//250000

//Las columnas de la tabla principal
#define COL_REF 0
#define COL_VALUE 1
//Las filas de la tabla principal
#define ROW_METROS          0
#define ROW_VELOCIDAD       1
#define ROW_DENTRADA        2
#define ROW_PARO_DENTRADA   3
#define ROW_DSALIDA         4
#define ROW_COUNTER         5
#define ROW_TLAMPARA        6
#define ROW_ILAMPARA        7
#define ROW_GOTA1           8
#define ROW_GOTA2           9
#define ROW_MACULAMAX       10
#define ROW_MACULAMIN       11
#define ROW_ANCHO_BOBINA    12
#define ROW_FUERZA_FRENO    13
#define ROW_TCHILLER        14
#define ROW_PCHILLER        15

//Filas de la tabla de paleta
#define ROW_LEVEL0      0
#define ROW_LEVEL1      1
#define ROW_LEVEL2      2
#define ROW_LEVEL3      3
#define ROW_LEVEL4      4
#define ROW_LEVEL5      5
#define ROW_LEVEL6      6
#define ROW_LEVEL7      7
#define NUM_ROWS_PALETA 8
//Nombre generico de los botones de paleta
#define NAME_APLICA "m_pbAplicaPaleta"

static QString ImagenXaar;

//QToolBar *fileToolBar;

//Flag para actualizar info de pantalla cuando enviamos info en streaming


//Limite de intentos de encendido
#define BOOT_LIMIT 3

Maquina::Maquina(QWidget *parent)
    : ui(new Ui::MaquinaSPass)
{

 //   if (!launched){
       ui->setupUi(this);
       In2GestorLampara *lampara=In2GestorLampara::request(0);
       if (lampara->posDinamica()) lampara->init();

       QPalette palette = ui->m_teMessage->palette();
       palette.setColor(QPalette::Text,Qt::red);
       ui->m_teMessage->setPalette(palette);
       m_TrabajoNuevo=false;
       m_bLoadFromCola=false;
       ui->m_pbEndSGL->setEnabled(false);
       m_bCodigoCorrecto=false;
       m_parent=parent;
       MemoryMap *memoryMap=MemoryMap::request(0);

       In2ConfiguratorDesigner configDesigner;
       palette=ui->m_leFile1->palette();
       palette.setColor(QPalette::Text,QColor(Qt::lightGray));
       palette.setColor(QPalette::Base,QColor((QRgb)configDesigner.getColor1()));
       ui->m_leFile1->setPalette(palette);
       palette.setColor(QPalette::Base,QColor((QRgb)configDesigner.getColor2()));
       ui->m_leFile2->setPalette(palette);
       palette.setColor(QPalette::Base,QColor((QRgb)configDesigner.getColor3()));
       ui->m_leFile3->setPalette(palette);
       palette.setColor(QPalette::Base,QColor((QRgb)configDesigner.getColor4()));
       ui->m_leFile4->setPalette(palette);



       //m_memoryMap=MemoryMap::request(0);
       table.clear();
       for( int i = 0; i < 256; ++i ){
           table.append(qRgb(i,i,i));
       }

        /*fileToolBar = this->addToolBar(tr("File"));
        connect(this,SIGNAL(destroyed()),fileToolBar,SLOT(deleteLater()));
        fileToolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        fileToolBar->setFloatable(false);*/

        connect(ui->m_pbSelFile1,SIGNAL(clicked(bool)),this,SLOT(sltLoadFileColor1()));
        connect(ui->m_pbSelFile2,SIGNAL(clicked(bool)),this,SLOT(sltLoadFileColor2()));
        connect(ui->m_pbSelFile3,SIGNAL(clicked(bool)),this,SLOT(sltLoadFileColor3()));
        connect(ui->m_pbSelFile4,SIGNAL(clicked(bool)),this,SLOT(sltLoadFileColor4()));

        connect(ui->m_pbDelFile1,SIGNAL(clicked(bool)),this,SLOT(sltDeleteFileColor1()));
        connect(ui->m_pbDelFile2,SIGNAL(clicked(bool)),this,SLOT(sltDeleteFileColor2()));
        connect(ui->m_pbDelFile3,SIGNAL(clicked(bool)),this,SLOT(sltDeleteFileColor3()));
        connect(ui->m_pbDelFile4,SIGNAL(clicked(bool)),this,SLOT(sltDeleteFileColor4()));

#ifndef HIGH_SPEED
        //ui->m_cbHighSpeed->setVisible(false);
        //ui->m_leResolution->setVisible(false);
#endif
        //Para la imagen variable
        m_TotalSwatheSent=0;
        //Swathe actual
        m_ActualSwathe=0;
        //Xusb
        m_iTotalXUSB=0;
        //Modo de limpieza
        m_ModeClean=0;
        ImageSender=NULL;
        ImageCreator=NULL;

        //Inicializo las escenas y views a NULL y aprovecho a redimensionar la vista
        m_Scene.resize(ui->m_TabCentral->count());
        m_View.resize(ui->m_TabCentral->count());
        for (int x=0;x<ui->m_TabCentral->count();x++){
            m_Scene[x]=NULL;
            m_View[x]=NULL;
        }

        //ui->m_CleanTime->setText(QString("%1").arg(CLEAN_TIME));
        memoryMap->m_ImagenEnviada=false;
        memset(&m_trimValues,0x00,sizeof(m_trimValues));

        ui->m_twData->setColumnWidth(COL_REF,ui->m_twData->width()/4);
        ui->m_twData->setColumnWidth(COL_VALUE,ui->m_twData->width()/4);

        ui->m_twData->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        ui->m_twData->verticalHeader()->setResizeMode(QHeaderView::Stretch);

        ui->m_twDataVariable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        ui->m_twDataVariable->verticalHeader()->setResizeMode(QHeaderView::Stretch);

        //ui->m_twData->horizontalHeader()->setResizeMode( COL_REF, QHeaderView::Stretch );

        GestorImpresion *printEngine=GestorImpresion::request(0);

        m_iTotalXUSB=printEngine->getNumCards();
        //Damos acceso
        for (int x=0;x<m_iTotalXUSB;x++){
            QStringList drops=memoryMap->m_mapa.m_variables.VolumenGota[x].split('@');
            while (drops.count()<8) drops.append("0");
            //Ponemos en la tabla principal
            for (int y=0;y<NUM_ROWS_PALETA;y++){
                QTableWidgetItem *p=ui->m_TablaPaletas->item(y,x);
                if (p){
                    Qt::ItemFlags Flags=p->flags();
                    Flags|=(Qt::ItemIsEditable)|(Qt::ItemIsEnabled);
                    p->setFlags(Flags);
                    p->setText(QString("%1").arg(drops.at(y)));

                }
            }
            sltChangeParameter(ROW_GOTA1+x,COL_REF);
        }
        connect(ui->m_TablaPaletas,SIGNAL(cellChanged(int,int)),this,SLOT(sltPaletteItemChanged(int,int)));

        ui->m_TablaPaletas->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        ui->m_TablaPaletas->verticalHeader()->setResizeMode(QHeaderView::Stretch);

        TestRoot();
        ClienteOPC *opc=ClienteOPC::request(this);
        connect(opc,SIGNAL(sgnOPCFlanco(int)),this,SLOT(sltFlancoOPC(int)));
        //connect(opc,SIGNAL(sgnOPCBallufChange()),this,SLOT(sltUpdateBalluf()));

        memoryMap->m_mapa.m_impresion.bReverse=opc->LeerDato(ITEM_IMPRESION_REVERSE).toBool();

        GestorMaquina *gestorMaquina=GestorMaquina::request(this);
        connect(gestorMaquina,SIGNAL(sgnGesMaquinaMessage(QString)),this,SLOT(sltUpdateMessage(QString)));

        connect(this,SIGNAL(sglSetFuerzaFreno(QVariant)),gestorMaquina,SLOT(sltSetFuerzaFreno(QVariant)));
        connect(this,SIGNAL(sglSetMetros(QVariant)),gestorMaquina,SLOT(sltSetMetros(QVariant)));
        connect(this,SIGNAL(sglSetDiametroEntrada(QVariant)),gestorMaquina,SLOT(sltSetDiametroEntrada(QVariant)));
        connect(this,SIGNAL(sglSetDiametroSalida(QVariant)),gestorMaquina,SLOT(sltSetDiametroSalida(QVariant)));
        connect(this,SIGNAL(sglSetDiametroParo(QVariant)),gestorMaquina,SLOT(sltSetDiametroParo(QVariant)));
        connect(this,SIGNAL(sglTrabajoNuevo()),gestorMaquina,SLOT(sltTrabajoNuevo()));
        connect(this,SIGNAL(sglJogMasOn(bool)),gestorMaquina,SLOT(sltJogMasOn(bool)));
        connect(this,SIGNAL(sglJogMasOff()),gestorMaquina,SLOT(sltJogMasOff()));
        connect(this,SIGNAL(sglJogMenosOn(bool)),gestorMaquina,SLOT(sltJogMenosOn(bool)));
        connect(this,SIGNAL(sglJogMenosOff()),gestorMaquina,SLOT(sltJogMenosOff()));
        connect(this,SIGNAL(sglBuscarMacula()),gestorMaquina,SLOT(sltBuscarMacula()));
        //connect(this,SIGNAL(sglHabilitarCondicionesMovimiento(unsigned char)),m_GestorMaquina,SLOT(sltHabilitarCondicionesMovimiento(unsigned char)));

        connect(ui->actionPrint,SIGNAL(triggered()),this,SLOT(sltLaunchPrintConfig()));
        connect(ui->actionConfig_Vision,SIGNAL(triggered()),this,SLOT(sltConfigVision()));
        connect(ui->actionInfo,SIGNAL(triggered()),this,SLOT(sltShowCompileInfo()));
        connect(ui->actionCambiar_usuario,SIGNAL(triggered()),this,SLOT(sltChangeUser()));
        connect(ui->m_pbFluidica,SIGNAL(clicked()),this,SLOT(sltLaunchFluidica()));
        connect(ui->m_pbChiller,SIGNAL(clicked()),this,SLOT(sltLaunchChiller()));
        connect(ui->m_pbLog,SIGNAL(clicked()),this,SLOT(sltShowLogScreen()));
        connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
        connect(ui->m_ColaButton,SIGNAL(clicked()),this,SLOT(LaunchColaImpresion()));
        connect(ui->actionAbrir_ini,SIGNAL(triggered()),this,SLOT(sltLoadIni()));
        connect(ui->actionConfigurar,SIGNAL(triggered(bool)),this,SLOT(sltConfigLampara()));
        connect(ui->actionConfigurar_2,SIGNAL(triggered(bool)),this,SLOT(sltConfigCorte()));
        //connect(ui->SelImgFija,SIGNAL(clicked()),this,SLOT(SelImagenFija()));
        connect(ui->SendIni,SIGNAL(clicked()),this,SLOT(SendIniToXaar()));

        //connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(testTab(int)));
        connect(ui->m_pbInspeccion,SIGNAL(clicked()),this,SLOT(sltInspeccion()));
        connect(ui->m_twData,SIGNAL(cellClicked(int,int)),this,SLOT(sltChangeParameter(int,int)));
        connect(ui->m_TrabajoNuevo,SIGNAL(clicked()),this,SLOT(CambiarTrabajo()));
        connect(ui->m_JogMas,SIGNAL(pressed()),this,SLOT(EjecutarJogMasOn()));
        connect(ui->m_JogMenos,SIGNAL(pressed()),this,SLOT(EjecutarJogMenosOn()));
        connect(ui->m_JogMas,SIGNAL(released()),this,SLOT(EjecutarJogMasOff()));
        connect(ui->m_JogMenos,SIGNAL(released()),this,SLOT(EjecutarJogMenosOff()));

        connect(ui->m_TensionMenos,SIGNAL(pressed()),this,SLOT(EjecutarTensionMenosOn()));
        connect(ui->m_TensionMenos,SIGNAL(released()),this,SLOT(EjecutarTensionMenosOff()));

        connect(ui->m_TensionMas,SIGNAL(pressed()),this,SLOT(EjecutarTensionMasOn()));
        connect(ui->m_TensionMas,SIGNAL(released()),this,SLOT(EjecutarTensionMasOff()));

        connect(ui->m_TensionAuto,SIGNAL(clicked()),this,SLOT(sltAutoTensionado()));

        connect(ui->m_BuscarMacula,SIGNAL(clicked()),this,SLOT(EjecutarBuscarMacula()));
        connect(ui->m_Empalme,SIGNAL(clicked()),this,SLOT(EjecutarEmpalme()));
        connect(ui->m_ToMaquina2,SIGNAL(clicked()),this,SLOT(LaunchMaquina2()));

        connect(ui->m_leCampo1,SIGNAL(editingFinished()),this,SLOT(sltUpdateInfoImagen()));
        connect(ui->m_leCampo2,SIGNAL(editingFinished()),this,SLOT(sltUpdateInfoImagen()));
        connect(ui->m_leCampo3,SIGNAL(editingFinished()),this,SLOT(sltUpdateInfoImagen()));
        connect(ui->m_leCampo4,SIGNAL(editingFinished()),this,SLOT(sltUpdateInfoImagen()));
        connect(ui->m_TabCentral,SIGNAL(currentChanged(int)),this,SLOT(sltChangeCentralTab(int)));

        In2GestorCorte *corte=In2GestorCorte::request(0);
        if (corte->isEnabled()){
            connect(ui->m_pbCorte,SIGNAL(clicked(bool)),this,SLOT(sltToogleCorte()));
            QTimer::singleShot(0,this,SLOT(sltTestCorte()));
        }
        else ui->m_pbCorte->setEnabled(false);


        In2GestorChiller *gChiller=In2GestorChiller::request(0,1);
        if (gChiller->isEnabled()){
            connect(gChiller,SIGNAL(sglCambioTemperatura(int)),this,SLOT(updateTemperaturaChiller(int)));
            connect(gChiller,SIGNAL(sglCambioPresion(int)),this,SLOT(updatePresionChiller(int)));
            connect(gChiller,SIGNAL(sglCambioSetpoint(int)),this,SLOT(updateSetPointChiller(int)));
            connect(gChiller,SIGNAL(sgnChillerCom(int)),this,SLOT(updateCOMChiller(int)));
            connect(gChiller,SIGNAL(sglCambioAlarm1(int)),this,SLOT(updateAlarmaChiller(int)));
            connect(gChiller,SIGNAL(sglCambioAlarm2(int)),this,SLOT(updateAlarmaChiller(int)));
            connect(gChiller,SIGNAL(sglCambioAlarm3(int)),this,SLOT(updateAlarmaChiller(int)));
            updateTemperaturaChiller(0);
            updatePresionChiller(0);
            updateSetPointChiller(0);
            updateCOMChiller(0);
            updateAlarmaChiller(0);
        }

        /*if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            connect(GlobalGetMainWindow(), SIGNAL(ActualizaDataOPC()),this, SLOT(ActualizaDataSlotOPC()));
        }*/

        QMapIterator<int, int> i(memoryMap->m_mapa.m_maquina.qmDiamPresion);
        i.toBack();

        QComboBox* comboAncho = new QComboBox();
        connect(this,SIGNAL(destroyed()),comboAncho,SLOT(deleteLater()));
        ui->m_twData->setCellWidget(ROW_ANCHO_BOBINA,COL_REF,comboAncho);

        connect(comboAncho,SIGNAL(currentIndexChanged(QString)),this,SLOT(CambiarFreno(QString)));

        while (i.hasPrevious()) {
            i.previous();
            //if (ui->m_leFreno->text().isEmpty()) ui->m_leFreno->setText(QString("%1").arg(i.value()));
            if (memoryMap->m_mapa.m_maquina.AnchoBobina==i.key()){
                int dummySelection=comboAncho->count();
                comboAncho->setCurrentIndex(dummySelection);
                //ui->m_leFreno->setText(QString("%1").arg(i.value()));
                QTableWidgetItem *p=ui->m_twData->item(ROW_FUERZA_FRENO,COL_VALUE);
                if (p){
                    p->setText(QString("%1").arg(i.value()));
                }
            }
            comboAncho->addItem(QString("%1").arg(i.key()));
            //ui->m_cbAnchoBobina->addItem(QString("%1").arg(i.key()));
            emit SendDebugInfo(LOGGER_WARNING,QString("Ancho %1 Freno %2").arg(i.key()).arg(i.value()));
        }

        In2ManagerERP *erp=In2ManagerERP::request(0);
        connect(erp,SIGNAL(sgnERPLaunchEmpalme()),this,SLOT(sltEmpalmeERP()));
        if (!erp->getClient().compare(ERP_ESTEVE)){
            ui->m_pbEndSGL->setVisible(true);
            if (memoryMap->getUserRights()<=ACCESS_PRINTER){
                ui->SendIni->setEnabled(false);
            }
            connect(ui->m_pbEndSGL,SIGNAL(clicked()),this,SLOT(sltEndERPWork()));
        }
        else{
            ui->m_pbEndSGL->setVisible(false);
        }


       //changeFreno(0);
       ui->tabWidget->setCurrentIndex(0);
       ui->m_twData->setSpan(ROW_ANCHO_BOBINA,0,1,2);
        //Inicializa zona fja
        //InitFixFrame();
        m_ActualHeight=0;
        ForceRefreshValues();
        m_prevVelocidad=0;
        m_MonitoringEnabled=false;

        //GlobalSetScreenSize(this);
        Qt::WindowFlags flags=Qt::Window;
        this->setWindowFlags(flags|Qt::CustomizeWindowHint);
        //this->setWindowFlags(Qt::WindowCloseButtonHint);


        for (int x=0;x<ui->m_TabCentral->count();x++){
            ui->m_TabCentral->setCurrentIndex(x);
        }
        ui->m_TabCentral->setCurrentIndex(0);

        updateBalluf();
        CargaDatos();
        sltUpdateTipoImpresion();

        enableMonitoring(true);

        sltUpdateReferencias();
        //As? actualizo los LEDS
        sltFlancoOPC(ITEM_CODIGO_PARADA);
        In2GestorMaquina *in2GM=In2GestorMaquina::request(0);
        connect(in2GM,SIGNAL(sgnChangeState(int)),this,SLOT(sltChangeState(int)));
        connect(in2GM,SIGNAL(sgnIn2GestorMaquinaMessage(QString)),this,SLOT(sltUpdateMessage(QString)));
        connect(in2GM,SIGNAL(sgnUpdateReferences()),this,SLOT(sltUpdateReferencias()));
        connect(in2GM,SIGNAL(sgnFinTrabajo()),this,SLOT(sltFinTrabajo()));
        connect(in2GM,SIGNAL(sgnFinImpresion(int)),this,SLOT(sltFinImpresion(int)));
        connect(in2GM,SIGNAL(sgnFinMacula(int)),this,SLOT(sltErrorMacula(int)));
        connect(in2GM,SIGNAL(sgnGestorMaquinaFinImpresion()),this,SLOT(sltCheckFinImpresion()));
        connect(this,SIGNAL(sgnMaxBobina()),in2GM,SLOT(sltCheckMaxBobina()));
        //connect(in2GM,SIGNAL(sgnChangeState(int)),this,SLOT(sltChangeState(int)));
        connect(ui->m_pbCleanMessage,SIGNAL(clicked()),this,SLOT(sltCleanMessage()));


//Deshabilita la gestion variable
#if !defined(MODULO_VARINFO) && !defined(DEPURACION_IN2)
    ui->m_twDataVariable->setEnabled(false);
#endif

    //Zona de vision
    In2ConfiguratorVision config;
    config.readConfig();
    m_VisionEnabled=false;
    if (config.isEnabled()){
        In2GestorVision *vision=In2GestorVision::request(0);
        connect(vision,SIGNAL(sgnVisionAlarm(QString)),this,SLOT(sltChangeToVision(QString)));
        connect(vision,SIGNAL(sgnVisionCommandAnswer(int,int)),this,SLOT(sltMessageFromVision(int, int)));
        connect(vision,SIGNAL(sgnVisionNoComm()),this,SLOT(sltNoVisionComm()));
        connect(vision,SIGNAL(sgnSocketDisconnected()),this,SLOT(sltVisionSocketFail()));
        connect(vision,SIGNAL(sgnSocketConnected()),this,SLOT(sltVisionSocketConnected()));

        m_GoodVisionPath=config.getGoodPath();
        m_VisionEnabled=true;
        m_lastVisionFile.clear();
        //En eset caso a?ado 1item
        /*m_ViewVision=new In2GraphicsView(ui->m_gvVision);
        In2GraphicsPixmapItem *item=new In2GraphicsPixmapItem(0);
        m_sceneVision=new In2GraphicsScene(0,0,0,0);
        m_sceneVision->addItem(item);
        ui->m_gvVision->setScene(m_sceneVision);
        //SOLO PARA TESTEO
        QTimer::singleShot(1000,this,SLOT(sltUpdateImagenVision()));
        //ui->m_TabCentral->setCurrentIndex(0);*/
    }
}

/** Configura el corte*/
void Maquina::sltConfigCorte(){
    In2ConfigCorte *p=new In2ConfigCorte(this);
    p->show();
}

/** Testea si el corte esta activado*/
void Maquina::sltTestCorte(){
    In2GestorCorte *p=In2GestorCorte::request(0);
    if (p->isActive()){
        QPalette palette=ui->m_pbCorte->palette();
        palette.setColor(QPalette::Button,Qt::blue);
        ui->m_pbCorte->setPalette(palette);
    }
    else{
        QPalette palette=ui->m_pbCorte->palette();
        palette.setColor(QPalette::Button,Qt::lightGray);
        ui->m_pbCorte->setPalette(palette);
    }
}

/** Invierte el valor del corte*/
void Maquina::sltToogleCorte(){
    In2GestorCorte *p=In2GestorCorte::request(0);
    bool val=p->sltToogleCorte();
    if (val){
        QPalette palette=ui->m_pbCorte->palette();
        palette.setColor(QPalette::Button,Qt::blue);
        ui->m_pbCorte->setPalette(palette);
    }
    else{
        QPalette palette=ui->m_pbCorte->palette();
        palette.setColor(QPalette::Button,Qt::lightGray);
        ui->m_pbCorte->setPalette(palette);
    }

}

/** Configura la lampara*/
void Maquina::sltConfigLampara(){
    In2ConfigLampara *foo=new In2ConfigLampara(this);
    foo->show();
}

/** Carga un fichero ini*/
void Maquina::sltLoadIni(){
    QFileDialog fileName(this);
    QStringList Ficheros;
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("ini Files (*.ini)"));
    QString defaultPath=QString("%1\\Images").arg(QApplication::applicationDirPath());
    fileName.setDirectory(defaultPath);
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (Ficheros.count()){
            QString fichero=Ficheros.at(0);
            QFileInfo info(fichero);
            MemoryMap *memoryMap=MemoryMap::request(0);

            memoryMap->m_mapa.m_variables.NombreIni=info.fileName();
            memoryMap->m_mapa.m_variables.RutaIni=info.path();
            memoryMap->m_mapa.m_variables.NombreRutaIni=fichero;
            memoryMap->m_FilePath=memoryMap->m_mapa.m_variables.NombreRutaIni;
            memoryMap->m_mapa.m_variables.NombreRutaBmp=memoryMap->m_mapa.m_variables.NombreRutaIni;
            memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");

            //AbrirFicheroIni();
            In2CompoFile ficheroIni;
            ficheroIni.AbrirFicheroIni();

            memoryMap->JumpToTempWorld();
            In2ImageCreator p;
            p.Convert();
            p.sltGenerateImage();

            QTimer::singleShot(0,this,SLOT(CargaDatos()));
        }
    }

}



/** Gestiona el fin de impresion*/
void Maquina::sltCheckFinImpresion(){
    ui->m_fSecondaryButtons->setEnabled(true);
    TestRoot();
    In2ManagerERP *erp=In2ManagerERP::request(0);
    if (!erp->getClient().compare(ERP_ESTEVE)){
        /*if (m_bLoadFromCola)
            QTimer::singleShot(0,this,SLOT(sltEndERPWork()));*/
        QTimer::singleShot(0,this,SLOT(sltEndERPWork()));
    }
}

/** Error de macula*/
void Maquina::sltErrorMacula(int type){
    sltUpdateMessage(tr("ERROR distancia entre maculas. Revise log para mas informacion"));
    ui->m_ledMacula->setEnabled(true);
    if (type==MACULA_MAXIMA){
        QTableWidgetItem *p=ui->m_twData->item(ROW_MACULAMAX,COL_VALUE);
        QTableWidgetItem *p1=ui->m_twData->item(ROW_MACULAMAX,COL_REF);
        if (p){
            p->setBackgroundColor(QColor(Qt::red));
            p->setText(p1->text());
        }
    }
    else if (type==MACULA_MINIMA){
        QTableWidgetItem *p=ui->m_twData->item(ROW_MACULAMIN,COL_VALUE);
        QTableWidgetItem *p1=ui->m_twData->item(ROW_MACULAMIN,COL_REF);
        if (p){
            p->setBackgroundColor(QColor(Qt::red));
            p->setText(p1->text());
        }
    }
}

/** Fin de impresion*/
void Maquina::sltFinImpresion(int mode){
    switch (mode){
        case FIN_ELECTRONICA:
        ui->m_ledMonitorHeads->setEnabled(true);
        break;
    }
}

/** Falla la conexion de socket*/
void Maquina::sltVisionSocketFail(){
    ui->m_ledVision->setEnabled(true);
    sltUpdateMessage(tr("ERROR:Socket de vision cerrado"));
}
/** Conectado a vision*/
void Maquina::sltVisionSocketConnected(){
    ui->m_ledVision->setEnabled(false);
    sltUpdateMessage("");
}
/** No hay vision*/
void Maquina::sltNoVisionComm(){
    ui->m_ledVision->setEnabled(true);
    QMessageBox::warning(this,"INFO",tr("No hay respuesta de equipo de vision. Ver Log para informacion"),QMessageBox::Ok);
    //sltUpdateMessage();
}

/** Hace fin de trabajo*/
void Maquina::sltFinTrabajo(){
    m_TrabajoNuevo=false;
    In2ManagerERP *erp=In2ManagerERP::request(0);
    if (!erp->getClient().compare(ERP_GRAMAR)){
        MemoryMap *memoryMap=MemoryMap::request(0);
        erp->deleteOrden("",memoryMap->m_mapa.m_variables.Proceso);
    }
}

/** Hablita o dehabilita botones segun el estado*/
void Maquina::sltChangeState(int state){
    if (state==THEME_PRINTING){
        //ui->m_gbPrincipalButtons->setEnabled(false);
        ui->m_fSecondaryButtons->setEnabled(false);
        ui->m_pbChiller->setEnabled(false);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_ToMaquina2->setEnabled(false);
        ui->m_ColaButton->setEnabled(false);
        ui->SendIni->setEnabled(false);
        ui->m_pbFluidica->setEnabled(false);
        ui->m_pbEndSGL->setEnabled(false);
        ui->m_AceptarButton->setEnabled(false);
        //m_fSecondaryButtons
    }
    else if (state==THEME_EMERGENCY){
        /*ui->m_gbPrincipalButtons->setEnabled(true);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_pbChiller->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_ToMaquina2->setEnabled(true);
        ui->m_ColaButton->setEnabled(true);
        ui->SendIni->setEnabled(true);
        ui->m_pbFluidica->setEnabled(true);
        ui->m_pbEndSGL->setEnabled(true);
        ui->m_AceptarButton->setEnabled(true);*/
        ui->m_fSecondaryButtons->setEnabled(true);
        TestRoot();
    }
    else if (state==THEME_NORMAL){
        /*ui->m_fSecondaryButtons->setEnabled(true);
        TestRoot();*/
    }
}

/** Lanza la pantalla de fin*/
void Maquina::sltEndERPWorkMode(int mode){
    QTimer::singleShot(0,this,SLOT(sltEndERPWork()));
}

/** Finaliza el work y se lo dice al ERP*/
void Maquina::sltEndERPWork(){
    In2EsteveEnd *p=new In2EsteveEnd(this);
    connect(p,SIGNAL(sgnERPWorkClosed()),this,SLOT(sltResetWork()));
    p->setSGLUpdate(m_bLoadFromCola);
    p->show();
}
/** Resetea valores*/
void Maquina::sltResetWork(){
    ui->m_pbEndSGL->setEnabled(false);    
    ui->m_leFile1->clear();
    ui->m_leFile2->clear();
    ui->m_leFile3->clear();
    ui->m_leFile4->clear();

    ui->m_leFileVision->clear();
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->m_FilePath.clear();
    ui->m_gbCentral->setTitle("");
    MuestraImagen(QPixmap());
    if (memoryMap->getUserRights()<=ACCESS_PRINTER){
        ui->SendIni->setEnabled(false);
    }

}


/** Muestra informacion de la compilacion*/
void Maquina::sltShowCompileInfo(){
    CompileVersion *p=new CompileVersion(this);
    p->show();

}

/** Tab central*/
void Maquina::sltChangeCentralTab(int index){
    //if (index==3) sltUpdateImagenVision();
}

/** Llega un mensaje de vision*/
void Maquina::sltMessageFromVision(int val, int command){
    if (this->isVisible()){
    QString txt;
    In2GestorVision *vision=In2GestorVision::request(0);
    if (val==VISION_ACK){
        if (command==NO_COMMAND);
        else if (command==LOAD_STRING){
            if (this->isVisible()){
                txt=tr("Vision[ACK]:LOAD_STRING [%1]").arg(vision->lastCommandSent());
                sltUpdateMessage(txt);
                //sltSendIni0();
            }
        }
        else if (command==LOAD_FILE){
            if (this->isVisible()){
                txt=tr("Vision[ACK]:LOAD_FILE [%1]").arg(vision->lastCommandSent());
                sltUpdateMessage(txt);

                //sltSendIni0();
            }
        }
        else if (command==SAVE_FILE);
        else if (command==STOP_VISION);

    }
    else if (val==VISION_NACK){
        if (command==NO_COMMAND){
            QMessageBox::information(0,"INFO","No existe plantilla de vision. Se ha cargado la plantilla maestra",QMessageBox::Ok);
            txt=tr("Vision[NACK]:Comando desconocido [%1]").arg(vision->lastCommandSent());
        }
        else if (command==LOAD_STRING) txt=tr("Vision[NACK]:LOAD_STRING [%1]").arg(vision->lastCommandSent());
        else if (command==LOAD_FILE){
            QMessageBox::information(0,"INFO","No existe plantilla de vision. Se ha cargado la plantilla maestra",QMessageBox::Ok);
            txt=tr("Vision[NACK]:LOAD_FILE [%1]").arg(vision->lastCommandSent());
        }
        else if (command==SAVE_FILE) txt=tr("Vision[NACK]:SAVE_FILE [%1]").arg(vision->lastCommandSent());
        else if (command==STOP_VISION) txt=tr("Vision[NACK]:STOP [%1]").arg(vision->lastCommandSent());

        sltUpdateMessage(txt);
    }
   }
}

/** Llega un error, cambio a vision*/
void Maquina::sltChangeToVision(QString txt){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString foo=QString("%1 [%2 m]").arg(txt).arg(memoryMap->m_mapa.m_maquina.metros_trabajo);
    In2GestorMaquina *gestor=In2GestorMaquina::request(0);
    if (gestor->startDelayed()){
        sltUpdateMessage(foo);
        ui->m_ledVision->setEnabled(true);
    }
    else{
        SendDebugInfo(LOGGER_STORE,foo);
    }
}

/** Lanza la configuracion de vision*/
void Maquina::sltConfigVision(){
    In2AISVisionSelector *p=new In2AISVisionSelector(this);
    connect(this,SIGNAL(destroyed()),p,SLOT(deleteLater()));
    QString foo=ui->m_leFile1->text();
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (!foo.isEmpty()){
        p->loadBMP(foo);
        p->loadProgram(memoryMap->m_mapa.m_variables.NombreRutaIni);
    }
    p->show();

}

/** Emplame automatico*/
void Maquina::sltEmpalmeERP(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_maquina.velocidad) QTimer::singleShot(500,this,SLOT(sltEmpalmeERP()));
    else QTimer::singleShot(3000,this,SLOT(sltEmpalmeERP1()));
}

void Maquina::sltEmpalmeERP1(){
    int rtn=QMessageBox::question(this,"INFO","Quiere empalmar bobina?",QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok) QTimer::singleShot(0,this,SLOT(EjecutarEmpalme()));

}


/** Limpia la zona de mensaje central*/
void Maquina::sltCleanMessage(){
    ui->m_teMessage->clear();
    ui->m_ledVision->setEnabled(false);
}

/** Actualiza el mensaje de pantalla*/
void Maquina::sltUpdateMessage(const QString & txt){
    //QString message= QString("<font color=\"red\">%1</font>\r\n").arg(txt);
    ui->m_teMessage->setText(txt);
    emit SendDebugInfo(LOGGER_WARNING,txt);
}

/** Cambia el usuario*/
void Maquina::sltChangeUser(){
    Status *status=new Status(this);
    status->show();
    connect(status,SIGNAL(StatusOculto()),this,SLOT(TestRoot()));
}

/** Lanza la configuraci?n de impresi?n*/
void Maquina::sltLaunchPrintConfig(){
    bool launch=true;
    /*MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->getClient().compare(CLIENT_ESTEVE)){
        if (memoryMap->getUserRights()<ACCESS_ROOT) {
            Status *status= new Status(this);
            status->show();
            launch=false;
        }
    }*/

    if (launch){
        In2ConfigPrint *p=new In2ConfigPrint(this);
        p->show();
        connect(p,SIGNAL(destroyed()),this,SLOT(sltUpdateTipoImpresion()));
        connect(p,SIGNAL(destroyed()),this,SLOT(sltSetRefMaculasLimits()));
    }
}

/** Cambia el Offset de  puntos a mm*/
void Maquina::sltChangeOffsetPoints(){
    /*bool ok;
    float foo=m_GOffsetPoints->text().toFloat(&ok);
    if (ok){
        foo=foo/14.18;
        m_GOffsetMm->setText(QString("%1").arg(foo));
        GestorImpresion *printEngine=GestorImpresion::request(this);
        QString configFile=printEngine->getConfigFile();
        In2XmlParser parser(0,configFile);
        QStringList params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
        QString txt=QString("New global Offset %1 (pts)").arg(m_GOffsetPoints->text());
        if (!parser.setValue(params,"value",m_GOffsetPoints->text())){
            QString err=QString("ERROR [sltChangeOffsetPoints] %1").arg(txt);
            QMessageBox::warning(this,"Error",err,QMessageBox::Ok);
            emit SendDebugInfo(LOGGER_ERROR,err);
        }
        else {
            emit SendDebugInfo(LOGGER_WARNING,txt);
            parser.saveContentsToFile(configFile);
        }

    }*/
}
/** Cambia el Offset de  mm a puntos*/
void Maquina::sltChangeOffsetMm(){
    /*bool ok;
    float foo=m_GOffsetMm->text().toFloat(&ok);
    if (ok){
        foo=foo*14.18;
        m_GOffsetPoints->setText(QString("%1").arg(foo));
        GestorImpresion *printEngine=GestorImpresion::request(this);
        QString configFile=printEngine->getConfigFile();
        In2XmlParser parser(0,configFile);
        QStringList params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
        QString txt=QString("New global Offset %1").arg(m_GOffsetPoints->text());
        if (!parser.setValue(params,"value",m_GOffsetPoints->text())){
            QString err=QString("ERROR [sltChangeOffsetMm] %1").arg(txt);
            QMessageBox::warning(this,"Error",err,QMessageBox::Ok);
            emit SendDebugInfo(LOGGER_ERROR,err);
        }
        else {
            emit SendDebugInfo(LOGGER_WARNING,txt);
            parser.saveContentsToFile(configFile);
        }

    }*/
}

/** Carga el fichero del color 1*/
void Maquina::sltLoadFileColor1(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QFileDialog fileName(this);
    QStringList Ficheros;
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    QString defaultPath=QString("%1\\Images").arg(QApplication::applicationDirPath());
    fileName.setDirectory(defaultPath);
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
        //MuestraImagenFromFile();
            m_bLoadFromCola=false;
            ui->m_leFile1->setText(Ficheros.at(0));
            MuestraImagenCentral();
            MuestraImagenColor1();
            sltLoadVisionProgram(Ficheros.at(0));
            QImage tmp(Ficheros.at(0));
            if (!tmp.isNull()){
                memoryMap->m_mapa.m_variables.WebHeight=(double)tmp.height()/14.18;
                memoryMap->m_mapa.m_variables.WebWidth=(double)tmp.width()/14.18;
                sltSetRefMaculas();
            }
        }
     }
}



/** Carga programa de vision*/
void Maquina::sltLoadVisionProgram(QString txt){
    if (m_VisionEnabled){
        QFileInfo fileInfo(txt);
        ui->m_leFileVision->setText(fileInfo.baseName());
    }
}

/** Carga el fichero del color 2*/
void Maquina::sltLoadFileColor2(){
    QFileDialog fileName(this);
    QStringList Ficheros;
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    QString defaultPath=QString("%1\\Images").arg(QApplication::applicationDirPath());
    fileName.setDirectory(defaultPath);
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
        //MuestraImagenFromFile();
            ui->m_leFile2->setText(Ficheros.at(0));
            MuestraImagenCentral();
            MuestraImagenColor2();
        }
     }
}

/** Carga el fichero del color 3*/
void Maquina::sltLoadFileColor3(){
    QFileDialog fileName(this);
    QStringList Ficheros;
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    QString defaultPath=QString("%1\\Images").arg(QApplication::applicationDirPath());
    fileName.setDirectory(defaultPath);
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
        //MuestraImagenFromFile();
            m_bLoadFromCola=false;
            ui->m_leFile3->setText(Ficheros.at(0));
            MuestraImagenCentral();
        }
     }
}
/** Carga el fichero del color 4*/
void Maquina::sltLoadFileColor4(){
    QFileDialog fileName(this);
    QStringList Ficheros;
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    QString defaultPath=QString("%1\\Images").arg(QApplication::applicationDirPath());
    fileName.setDirectory(defaultPath);
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
        //MuestraImagenFromFile();
            m_bLoadFromCola=false;
            ui->m_leFile4->setText(Ficheros.at(0));
            MuestraImagenCentral();
        }
     }
}

/** Elimina el fichero del color 1*/
void Maquina::sltDeleteFileColor1(){
    ui->m_leFile1->setText("");
    MuestraImagenCentral();
    MuestraImagenColor1();
}
/** Elimina el fichero del color 1*/
void Maquina::sltDeleteFileColor2(){
    ui->m_leFile2->setText("");
    MuestraImagenCentral();
    MuestraImagenColor2();
}
/** Elimina el fichero del color 3*/
void Maquina::sltDeleteFileColor3(){
    ui->m_leFile3->setText("");
    MuestraImagenCentral();
}
/** Elimina el fichero del color 4*/
void Maquina::sltDeleteFileColor4(){
    ui->m_leFile4->setText("");
    MuestraImagenCentral();
}


/** Cambia el modo de impresion*/
void Maquina::sltChangeModoImpresion(int modo){
   /* GestorImpresion *gestor=GestorImpresion::request(0);
    QString configFile=gestor->getConfigFile();

    In2XmlParser parser(0,configFile);
    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"PrintMode";
    QString val=QString("%1").arg(ui->m_cbModoImpresion->currentIndex());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error","No se pudo guardar Tipo de Impresion",QMessageBox::Ok);
    }


    parser.saveContentsToFile(configFile);*/
}

/** Cambia el modo de impresion*/
void Maquina::sltChangeTipoPD(int modo){
    /*GestorImpresion *gestor=GestorImpresion::request(0);
    QString configFile=gestor->getConfigFile();

    In2XmlParser parser(0,configFile);

    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"SequenceStart";
    QString val=QString("%1").arg(ui->m_cbTipoPD->currentIndex());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error","No se pudo guardar TipoPD",QMessageBox::Ok);
    }

    parser.saveContentsToFile(configFile);*/
}



/** Actualiza la informacion de imagen*/
void Maquina::sltUpdateInfoImagen(){
    bool rtn=false;
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_campos[0].OnOffCampo){
        if (ui->m_leCampo1->text().compare(memoryMap->m_mapa.m_campos[0].ValorActualCampo)){
            memoryMap->m_mapa.m_campos[0].ValorActualCampo=ui->m_leCampo1->text();
            rtn=true;
        }
    }
    if (memoryMap->m_mapa.m_campos[1].OnOffCampo){
        if (ui->m_leCampo2->text().compare(memoryMap->m_mapa.m_campos[1].ValorActualCampo)){
            memoryMap->m_mapa.m_campos[1].ValorActualCampo=ui->m_leCampo2->text();
            rtn=true;
        }
    }
    if (memoryMap->m_mapa.m_campos[2].OnOffCampo){
        if (ui->m_leCampo3->text().compare(memoryMap->m_mapa.m_campos[2].ValorActualCampo)){
            memoryMap->m_mapa.m_campos[2].ValorActualCampo=ui->m_leCampo3->text();
            rtn=true;
        }
    }
    if (memoryMap->m_mapa.m_campos[3].OnOffCampo){
        if (ui->m_leCampo4->text().compare(memoryMap->m_mapa.m_campos[3].ValorActualCampo)){
            memoryMap->m_mapa.m_campos[3].ValorActualCampo=ui->m_leCampo4->text();
            rtn=true;
        }
    }
    //Solo experimental
    /*if (ui->m_cbHighSpeed->isChecked()){
       rtn=true;
    }*/


    if (rtn){
        //AbrirFicheroIni();
        //m_memoryMap->JumpToTempWorld();

        QString aux=memoryMap->m_mapa.m_variables.NombreIni;
        memoryMap->m_mapa.m_variables.NombreBmp=aux.replace(".ini",".bmp");/*nombre_bmp*/
        memoryMap->m_mapa.m_variables.RutaBmp=memoryMap->m_mapa.m_variables.RutaIni;/*ruta*/;
        aux=memoryMap->m_mapa.m_variables.NombreRutaIni;
        memoryMap->m_mapa.m_variables.NombreRutaBmp=aux.replace(".ini",".bmp");/*nombreruta_bmp;*/
        memoryMap->m_mapa.m_variables.NombreRutaPdf=QString("%1\\%2").arg(memoryMap->m_mapa.m_variables.RutaPdf).arg(memoryMap->m_mapa.m_variables.NombrePdf);

        //Convert();

        In2ImageCreator p;
        p.Convert();
        //p.sltGenerateImageToPrint();
        p.sltGenerateImage();
        MuestraImagen(memoryMap->m_Imagen);
        //m_memoryMap->JumpToOriginalWorld();
        //m_memoryMap->m_Imagen.save("dummy.bmp");
    }
}

/** Flanco del OPC*/
void Maquina::sltFlancoOPC(int ID){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (ID==ITEM_CODIGO_PARADA){
        unsigned char cCodigoParada=(unsigned char)((memoryMap->m_mapa.m_maquina.codigo_parada&0xFF00)>>8);
        unsigned char cCodigoParada2=(unsigned char)(((char)memoryMap->m_mapa.m_maquina.codigo_parada&0x00FF));
        unsigned int code=cCodigoParada+(cCodigoParada2*256);
        if (code&EMERGENCIA_SETA) ui->m_ledSeta->setEnabled(true);//ui->m_ledSeta->setPixmap(QPixmap(LED_ROJO));
        else ui->m_ledSeta->setEnabled(false);//ui->m_ledSeta->setPixmap(QPixmap(LED_VERDE));
        if (code&EMERGENCIA_VARSALIDA) ui->m_ledVSalida->setEnabled(true);
        else ui->m_ledVSalida->setEnabled(false);
        if (code&EMERGENCIA_VARENTRADA) ui->m_ledVEntrada->setEnabled(true);
        else ui->m_ledVEntrada->setEnabled(false);
        if (code&EMERGENCIA_ARRASTRE) ui->m_ledArrastre->setEnabled(true);
        else ui->m_ledArrastre->setEnabled(false);
        if (code&EMERGENCIA_BANDEJA) ui->m_ledBandeja->setEnabled(true);
        else ui->m_ledBandeja->setEnabled(false);
        if (code&EMERGENCIA_MESA) ui->m_ledMesa->setEnabled(true);
        else ui->m_ledMesa->setEnabled(false);
        if (code&EMERGENCIA_CAPOTA) ui->m_ledCapota->setEnabled(true);
        else ui->m_ledCapota->setEnabled(false);
        if (code&EMERGENCIA_PRESION) ui->m_ledPaire->setEnabled(true);
        else ui->m_ledPaire->setEnabled(false);
        if (code&EMERGENCIA_LAMPARA_ELECTRICO) ui->m_ledLampElec->setEnabled(true);
        else ui->m_ledLampElec->setEnabled(false);
        if (code&EMERGENCIA_LAMPARA_APAGADA) ui->m_ledLampOff->setEnabled(true);
        else ui->m_ledLampOff->setEnabled(false);
        if (code&EMERGENCIA_DESPLAZADOR_LAMPARA) ui->m_ledDesplazador->setEnabled(true);
        else ui->m_ledDesplazador->setEnabled(false);

    }
}

/** Hace un autotensionado*/
void Maquina::sltAutoTensionado(){
    GestorMaquina *m_gestor=GestorMaquina::request(this);
    m_gestor->sltTensadoAutomatico();
    //m_gestor->sltCentrarBalluf();
}

/** Establece impresion reverse*/
void Maquina::sltSetImpresionReverse(bool reverse){
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Impresion REVERSE:Pare primero la maquina",QMessageBox::Ok);
        return;
    }*/

    GestorMaquina *gestorMaquina=GestorMaquina::request(this);
    gestorMaquina->sltSetImpresionReverse(reverse);
}
/** Establece impresion reverse*/
void Maquina::sltSetImpresionNormal(bool norma){
/*    if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la mquina",QMessageBox::Ok);
        return;
    }*/

    GestorMaquina *gestorMaquina=GestorMaquina::request(this);
    gestorMaquina->sltSetImpresionReverse(false);
}


/** Carga las referencias*/
void Maquina::sltUpdateReferencias(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_METROS,COL_REF);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.ref_metros_trabajo));
    }
    p=ui->m_twData->item(ROW_VELOCIDAD,COL_REF);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.ref_velocidad));
    }
    p=ui->m_twData->item(ROW_DENTRADA,COL_REF);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada));
    }
    p=ui->m_twData->item(ROW_DSALIDA,COL_REF);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida));
    }
    p=ui->m_twData->item(ROW_PARO_DENTRADA,COL_REF);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.diametro_parar));
    }


}

/** Muestra la pantalla de log*/
void Maquina::sltShowLogScreen(){
    emit sgnShowLog();
}
/** Lanza el chiller*/
void Maquina::sltLaunchChiller(){
    Chiller *ScrChiller;
    ScrChiller=new Chiller(this);
    ScrChiller->show();
    this->hide();
}


/** Accede a fluidica*/
void Maquina::sltLaunchFluidica(){
    Fluidica *ScrFluidica;
    ScrFluidica=new Fluidica(this);
    ScrFluidica->show();
    this->hide();
}


/** Ha cambiado la paleta*/
void Maquina::sltPaletteItemChanged(int row,int column){
    if (column==0){
        sltChangeParameter(ROW_GOTA1,COL_REF);
    }
    else if (column==1)
        sltChangeParameter(ROW_GOTA2,COL_REF);
   /* else if (column==2)
        sltChangeParameter(ROW_GOTA3,COL_REF);
    else if (column==3)
        sltChangeParameter(ROW_GOTA4,COL_REF);*/

}


/** Recoge la paleta*/
QByteArray Maquina::getPaleta(int index){
    QByteArray palette;
    palette.resize(8);
    for (int x=0;x<8;x++){
        QTableWidgetItem *p=ui->m_TablaPaletas->item(ROW_LEVEL0+x,index);
        if (p)
            palette[x]=p->text().toInt()/DEFAULT_DROP_VOL;
        else
            palette[x]=0;
    }
    return palette;
}

/** Recoge la paleta*/
QByteArray Maquina::getGrayScalePaleta(int index){
    QByteArray palette;
    palette.resize(8);
    for (int x=0;x<8;x++){
        QTableWidgetItem *p=ui->m_TablaPaletas->item(ROW_LEVEL0+x+8,index);
        if (p)
            palette[x]=p->text().toInt()/DEFAULT_DROP_VOL;
        else
            palette[x]=0;
    }
    return palette;
}




/** Cambia un parametro*/
void Maquina::sltChangeParameter(int row, int column){
    QStringList drops;
    QTableWidgetItem *p;
    QTableWidgetItem *p2;
    if (column==COL_REF){
        switch (row){
         case ROW_METROS:
            AceptarValorMetros();
            break;

        case ROW_VELOCIDAD:
            AceptarValorVelocidad();
            break;

        case ROW_DENTRADA:
            AceptarValorDiametro();
            break;

        case ROW_PARO_DENTRADA:
            AceptarValorDiametroParo();
            break;

        case ROW_DSALIDA:
            AceptarValorDiametroSalida();
            break;

        case ROW_TLAMPARA:
            break;

        case ROW_ILAMPARA:
            break;

        case ROW_GOTA1:
            p=ui->m_twData->item(ROW_GOTA1,COL_REF);
            p2=ui->m_TablaPaletas->item(0,0);
            p->setText(p2->text());
            p=ui->m_twData->item(ROW_GOTA1,COL_VALUE);
            p->setText(p2->text());

            /*drops=m_memoryMap->m_mapa.m_variables.VolumenGota[0].split('@');
            p->setText(drops.at(0));*/
            break;

        case ROW_GOTA2:
            p=ui->m_twData->item(ROW_GOTA2,COL_REF);
            p2=ui->m_TablaPaletas->item(0,1);
            p->setText(p2->text());
            p=ui->m_twData->item(ROW_GOTA2,COL_VALUE);
            p->setText(p2->text());
            break;

        /*case ROW_GOTA3:
            p=ui->m_twData->item(ROW_GOTA3,COL_REF);
            p2=ui->m_TablaPaletas->item(0,2);
            p->setText(p2->text());
            p=ui->m_twData->item(ROW_GOTA3,COL_VALUE);
            p->setText(p2->text());

            break;

        case ROW_GOTA4:
            p=ui->m_twData->item(ROW_GOTA4,COL_REF);
            p2=ui->m_TablaPaletas->item(0,3);
            p->setText(p2->text());
            p=ui->m_twData->item(ROW_GOTA4,COL_VALUE);
            p->setText(p2->text());
            break;*/
        }
    }
}

/** Reduce la velocidad unoss segundos para favorecer la inspeccion*/
void Maquina::sltInspeccion(){
//Hacer con GestorMaquina
    /*    if (m_memoryMap->m_mapa.m_maquina.impresion==true){
        QWidget *dummy;
        dummy=GlobalGetMainWindow();
        m_prevVelocidad=m_memoryMap->m_mapa.m_maquina.ref_velocidad;
        ui->m_VelocidadCinta->setText(QString("%1").arg(VELOCIDAD_INSPECCION));
        m_memoryMap->m_mapa.m_maquina.ref_velocidad=VELOCIDAD_INSPECCION;
        QString txt=QString("sltInspeccion v=%1 vprev=%2 time=%3").arg(VELOCIDAD_INSPECCION).arg(m_prevVelocidad).arg(TIEMPO_INSPECCION);
        emit SendDebugInfo(LOGGER_WARNING,txt);
        ((Principal*)dummy)->SetNuevaVelocidadReferencia(m_memoryMap->m_mapa.m_maquina.ref_velocidad);
        ((Principal*)dummy)->Asignacion_Referencia_Velocidad();
        QTimer::singleShot(TIEMPO_INSPECCION,this,SLOT(sltRestorePrevSpeed()));
    }*/
        MemoryMap *memoryMap=MemoryMap::request(0);
        GestorMaquina *gestorMaquina=GestorMaquina::request(this);
        gestorMaquina->sltHabilitarCalculoDiametro(0);
        memoryMap->m_mapa.m_maquina.prev_ref_velocidad=memoryMap->m_mapa.m_maquina.ref_velocidad;
        memoryMap->m_mapa.m_maquina.ref_velocidad=VELOCIDAD_INSPECCION;

        gestorMaquina->sltSetReferenciaVelocidadControlled();
        QTimer::singleShot(TIEMPO_INSPECCION,this,SLOT(sltRestorePrevSpeed()));
        emit SendDebugInfo(LOGGER_STORE,tr("Inspeccion realizada"));

}

/** Restaura la velocidad previa*/
void Maquina::sltRestorePrevSpeed(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    GestorMaquina *gestorMaquina=GestorMaquina::request(this);
    gestorMaquina->sltHabilitarCalculoDiametro(0);
    memoryMap->m_mapa.m_maquina.ref_velocidad=memoryMap->m_mapa.m_maquina.prev_ref_velocidad;
    //m_memoryMap->m_mapa.m_maquina.ref_velocidad=VELOCIDAD_INSPECCION;
    gestorMaquina->sltSetReferenciaVelocidadControlled();

    //Hacer con GestorMaquina
    /* QString txt=QString("sltRestorePrevSpeed v=%1").arg(m_prevVelocidad);
    emit SendDebugInfo(LOGGER_WARNING,txt);
    if (!m_memoryMap->m_mapa.m_maquina.softstopear){
        if (m_prevVelocidad!=0){
            QWidget *dummy;
            dummy=GlobalGetMainWindow();
            m_memoryMap->m_mapa.m_maquina.ref_velocidad=m_prevVelocidad;
            ui->m_VelocidadCinta->setText(QString("%1").arg(m_prevVelocidad));
            m_memoryMap->m_mapa.m_maquina.ref_velocidad=m_prevVelocidad;
            ((Principal*)dummy)->SetNuevaVelocidadReferencia(m_memoryMap->m_mapa.m_maquina.ref_velocidad);
            ((Principal*)dummy)->Asignacion_Referencia_Velocidad();
        }
    }*/
}

/** Cambia el freno*/
void Maquina::CambiarFreno(QString txt){
    MemoryMap *memoryMap=MemoryMap::request(0);
    int value=memoryMap->m_mapa.m_maquina.qmDiamPresion.value(txt.toInt());
    QTableWidgetItem *p1=ui->m_twData->item(ROW_FUERZA_FRENO,COL_VALUE);
    QTableWidgetItem *p2=ui->m_twData->item(ROW_ANCHO_BOBINA,COL_VALUE);
    if ((p1)&&(p2)){
        p1->setText(QString("%1").arg(value));
        p2->setText(QString("%1").arg(value));
        emit sglSetFuerzaFreno(value);
    }
}

/** Cambia la fuerza de freno de manera automatica*/
void Maquina::SetFuerzaFrenoAuto(){
    MemoryMap *memoryMap=MemoryMap::request(0);
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
    QVariant valor=QVariant(i.value());
    emit sglSetFuerzaFreno(valor);
    memoryMap->m_mapa.m_maquina.AnchoBobina=i.key();
    emit SendDebugInfo(LOGGER_WARNING,QString("WebWidth %1 AutoAncho %2 AutoFreno %3").arg(memoryMap->m_mapa.m_variables.WebWidth).arg(i.key()).arg(i.value()));
    QComboBox *combo=static_cast<QComboBox*>(ui->m_twData->cellWidget(ROW_ANCHO_BOBINA,COL_REF));
    if (combo){
        int index=combo->findText(QString("%1").arg(i.key()));
        combo->setCurrentIndex(index);
    }

    QTableWidgetItem *p1=ui->m_twData->item(ROW_FUERZA_FRENO,COL_VALUE);
    QTableWidgetItem *p2=ui->m_twData->item(ROW_ANCHO_BOBINA,COL_VALUE);
    if ((p1)&&(p2)){
        p1->setText(QString("%1").arg(i.value()));
        p2->setText(QString("%1").arg(i.value()));
    }
    emit SendDebugInfo(LOGGER_WARNING,QString("Ancho %1 Freno %2").arg(i.key()).arg(i.value()));


}

/** Inicializa la zona de fija*/
void Maquina::InitFixFrame(){
    /*if (!m_memoryMap->m_mapa.m_variables.DisablePrinting){
        newCard=new InkjetXusbCtrl(0,0);
        //newCard2=new InkjetXusbCtrl(0,1);
    }*/
}

/** Inicializa la zona de variable*/
void Maquina::InitVariableFrame(){
   /* if (!m_memoryMap->m_mapa.m_variables.DisablePrinting){
        newVarCard=new InkjetXusbVarCtrl(0,0,NULL,0); //solo para 1 xusb
    }
    //Creamos los hilos para variable
    threadSender=new QThread;
    threadCreator=new QThread;
    ImageSender=NULL;
    ImageCreator=NULL;*/
}

/** Se ha ejecutado un soft stop externo*/
void Maquina::ExternalSoftStop(){
    //emit PauseGenerating();
    m_prevVelocidad=0;
}

/** Ejecuta soft stop*/
void Maquina::SoftStop(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    int respuesta=QMessageBox::question(this,"ATENCION","Quiere parar la impresión?","Si","No",QString::null, 0, 1 );
    //int respuesta=0;
    if (respuesta==0){
        memoryMap->m_mapa.m_maquina.softstopear=true;
        //Insertar_Evento(ERROR_GUI,QString("Parada desde interfaz"));
    }
}
/** Muestra permisos segun Esteve*/
void Maquina::_MuestraPermisosEsteve(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()==ACCESS_PRINTER){
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->actionPrint->setEnabled(false);
        ui->actionConfig_Vision->setEnabled(false);
        //fileToolBar->setEnabled(false);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,false);
        }
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setCurrentIndex(0);
        ui->m_ToMaquina2->setVisible(false);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(false);
        ui->actionConfig_Vision->setEnabled(false);
        ui->actionAbrir_ini->setEnabled(false);
    }
    else if (memoryMap->getUserRights()==ACCESS_ROOT){
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->actionPrint->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        //fileToolBar->setEnabled(true);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,true);
        }
        ui->tabWidget->setCurrentIndex(0);
        ui->m_ToMaquina2->setVisible(false);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(false);
        ui->actionConfig_Vision->setEnabled(true);
        ui->actionAbrir_ini->setEnabled(true);

    }
    else if (memoryMap->getUserRights()==ACCESS_GOD){
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->actionPrint->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        //fileToolBar->setEnabled(true);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,true);
        }
        ui->tabWidget->setCurrentIndex(0);
        ui->m_ToMaquina2->setVisible(true);
        ui->m_ToMaquina2->setEnabled(true);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        ui->actionAbrir_ini->setEnabled(true);

    }
    else{
        ui->actionPrint->setEnabled(false);
        //fileToolBar->setEnabled(false);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,false);
        }
        ui->tabWidget->setCurrentIndex(0);
        ui->tabWidget->setTabEnabled(0,false);
        ui->m_ToMaquina2->setVisible(false);
        ui->m_gbPrincipalButtons->setEnabled(false);
        ui->m_fSecondaryButtons->setEnabled(false);
        ui->m_AceptarButton->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        ui->actionAbrir_ini->setEnabled(false);

    }
}
/** Muestra permisos segun Esteve*/
void Maquina::_MuestraPermisosGramar(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()==ACCESS_PRINTER){
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->actionPrint->setEnabled(false);
        ui->actionConfig_Vision->setEnabled(false);
        //fileToolBar->setEnabled(false);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,false);
        }
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setCurrentIndex(0);
        ui->m_ToMaquina2->setVisible(false);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(false);
        ui->actionConfig_Vision->setEnabled(false);
        ui->actionAbrir_ini->setEnabled(false);
    }
    else if (memoryMap->getUserRights()==ACCESS_ROOT){
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->actionPrint->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        //fileToolBar->setEnabled(true);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,true);
        }
        ui->tabWidget->setCurrentIndex(0);
        ui->m_ToMaquina2->setVisible(false);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(false);
        ui->actionConfig_Vision->setEnabled(true);
        ui->actionAbrir_ini->setEnabled(true);

    }
    else if (memoryMap->getUserRights()==ACCESS_GOD){
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->actionPrint->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        //fileToolBar->setEnabled(true);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,true);
        }
        ui->tabWidget->setCurrentIndex(0);
        ui->m_ToMaquina2->setVisible(true);
        ui->m_ToMaquina2->setEnabled(true);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        ui->actionAbrir_ini->setEnabled(true);

    }
    else{
        ui->actionPrint->setEnabled(false);
        //fileToolBar->setEnabled(false);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,false);
        }
        ui->tabWidget->setCurrentIndex(0);
        ui->tabWidget->setTabEnabled(0,false);
        ui->m_ToMaquina2->setVisible(false);
        ui->m_gbPrincipalButtons->setEnabled(false);
        ui->m_fSecondaryButtons->setEnabled(false);
        ui->m_AceptarButton->setEnabled(true);
        ui->actionConfig_Vision->setEnabled(true);
        ui->actionAbrir_ini->setEnabled(false);

    }
    ui->m_pbChiller->setEnabled(false);
}


/** Permisos por defecto In2*/
void Maquina::_MuestraPermisosIn2(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()==ACCESS_STRANGE) return;
    else if (memoryMap->getUserRights()<=ACCESS_PRINTER){
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_BuscarMacula->setEnabled(false);
        ui->m_JogMas->setEnabled(false);
        ui->m_JogMenos->setEnabled(false);
        ui->m_Empalme->setEnabled(false);
        ui->m_TrabajoNuevo->setEnabled(false);        
        ui->m_gbPrincipalButtons->setEnabled(true);

        ui->m_pbChiller->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_ToMaquina2->setEnabled(false);
        ui->m_ColaButton->setEnabled(true);
        ui->SendIni->setEnabled(true);
        ui->m_pbFluidica->setEnabled(true);
        ui->m_pbEndSGL->setEnabled(true);
        ui->m_AceptarButton->setEnabled(true);

    }
    else if (memoryMap->getUserRights()==ACCESS_ROOT){
        ui->m_ToMaquina2->setEnabled(true);
        //ui->tabWidget->setTabEnabled(2,false);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_BuscarMacula->setEnabled(true);
        ui->m_JogMas->setEnabled(true);
        ui->m_JogMenos->setEnabled(true);
        ui->m_Empalme->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(true);
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->m_pbChiller->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_ToMaquina2->setEnabled(true);
        ui->m_ColaButton->setEnabled(true);
        ui->SendIni->setEnabled(true);
        ui->m_pbFluidica->setEnabled(true);
        ui->m_pbEndSGL->setEnabled(true);
        ui->m_AceptarButton->setEnabled(true);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,true);
        }



    }
    else{
        ui->m_ToMaquina2->setEnabled(true);
        //ui->tabWidget->setTabEnabled(2,false);
        ui->m_fSecondaryButtons->setEnabled(true);
        ui->m_BuscarMacula->setEnabled(true);
        ui->m_JogMas->setEnabled(true);
        ui->m_JogMenos->setEnabled(true);
        ui->m_Empalme->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(true);
        ui->m_gbPrincipalButtons->setEnabled(true);
        ui->m_pbChiller->setEnabled(true);
        ui->m_pbInspeccion->setEnabled(true);
        ui->m_ToMaquina2->setEnabled(true);
        ui->m_ColaButton->setEnabled(true);
        ui->SendIni->setEnabled(true);
        ui->m_pbFluidica->setEnabled(true);
        ui->m_pbEndSGL->setEnabled(true);
        ui->m_AceptarButton->setEnabled(true);
        for (int x=0;x<ui->tabWidget->count();x++){
            ui->tabWidget->setTabEnabled(x,true);
        }

    }
}
/** Carga los permisos de maquina*/
void Maquina::TestRoot(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString user=QString("%1:%2").arg(memoryMap->getUser()).arg(memoryMap->getUserRightsText());
    ui->m_gbPrincipalButtons->setTitle(user);
    /*if (!memoryMap->getClient().compare(CLIENT_ESTEVE)){
        _MuestraPermisosEsteve();
    }
    else if (!memoryMap->getClient().compare(CLIENT_GRAMAR)){
        _MuestraPermisosGramar();
    }
    else{
        _MuestraPermisosIn2();
    }*/

    In2ConfiguratorPermisos permisos;
    if (!permisos.loadPermisos("MAQUINA")){
        QMessageBox::warning(this,"ERROR",tr("No se han definido permisos para esta pantalla/usuario"),QMessageBox::Ok);
        ui->m_AceptarButton->setEnabled(true);
        return;
    }
    bool val=permisos.permiso(PRMS_JOGMAS);
    ui->m_JogMas->setEnabled(val);
    val=permisos.permiso(PRMS_JOGMENOS);
    ui->m_JogMenos->setEnabled(val);
    val=permisos.permiso(PRMS_CORTE);
    ui->m_pbCorte->setEnabled(val);
    val=permisos.permiso(PRMS_TENSIONMAS);
    ui->m_TensionMas->setEnabled(val);
    val=permisos.permiso(PRMS_TENSIONMENOS);
    ui->m_TensionMenos->setEnabled(val);
    val=permisos.permiso(PRMS_AUTOTENSION);
    ui->m_TensionAuto->setEnabled(val);
    val=permisos.permiso(PRMS_BUSQMACULA);
    ui->m_BuscarMacula->setEnabled(val);
    val=permisos.permiso(PRMS_CAMBIOBOBINA);
    ui->m_Empalme->setEnabled(val);
    val=permisos.permiso(PRMS_TRABAJONUEVO);
    ui->m_TrabajoNuevo->setEnabled(val);
    val=permisos.permiso(PRMS_AVANZADA);
    ui->m_ToMaquina2->setEnabled(val);
    val=permisos.permiso(PRMS_COLAIMPRESION);
    ui->m_ColaButton->setEnabled(val);
    val=permisos.permiso(PRMS_ENVIARCOMPO);
    ui->SendIni->setEnabled(val);
    val=permisos.permiso(PRMS_INSPECCION);
    ui->m_pbInspeccion->setEnabled(val);
    val=permisos.permiso(PRMS_FLUIDICA);
    ui->m_pbFluidica->setEnabled(val);
    val=permisos.permiso(PRMS_CHILLER);
    ui->m_pbChiller->setEnabled(val);
    val=permisos.permiso(PRMS_LOG);
    ui->m_pbLog->setEnabled(val);
    val=permisos.permiso(CERRARTRABAJO);
    ui->m_pbEndSGL->setEnabled(val);
    val=permisos.permiso(VISIBLECERRARTRABAJO);
    ui->m_pbEndSGL->setVisible(val);
    val=permisos.permiso(PRMS_EXIT);
    ui->m_AceptarButton->setEnabled(val);
    val=permisos.permiso(PRMS_LOADFILE);    
    ui->m_pbSelFile1->setEnabled(val);
    ui->m_pbDelFile1->setEnabled(val);
    ui->m_pbSelFile2->setEnabled(val);
    ui->m_pbDelFile2->setEnabled(val);
    ui->m_pbSelFile3->setEnabled(val);
    ui->m_pbDelFile3->setEnabled(val);
    ui->m_pbSelFile4->setEnabled(val);
    ui->m_pbDelFile4->setEnabled(val);
    val=permisos.permiso(PRMS_CAMBIOCAMPOS);
    ui->m_leCampo1->setEnabled(val);
    ui->m_leCampo2->setEnabled(val);
    ui->m_leCampo3->setEnabled(val);
    ui->m_leCampo4->setEnabled(val);
    val=permisos.permiso(PRMS_CAMBIOTINTAS);
    ui->m_TablaPaletas->setEnabled(val);
    val=permisos.permiso(PRMS_ABRIRINI);
    ui->actionAbrir_ini->setEnabled(val);
    val=permisos.permiso(PRMS_CONFIGPRINT);
    ui->actionPrint->setEnabled(val);
    val=permisos.permiso(PRMS_CONFIGVISION);
    ui->actionConfig_Vision->setEnabled(val);
    val=permisos.permiso(PRMS_CONFIGLAMPARA);
    ui->actionConfigurar->setEnabled(val);
    val=permisos.permiso(PRMS_CONFIGCORTE);
    ui->actionConfigurar_2->setEnabled(val);
    val=permisos.permiso(PRMS_CHANGEUSER);
    ui->actionCambiar_usuario->setEnabled(val);
    val=permisos.permiso(PRMS_INFO);
    ui->actionInfo->setEnabled(val);
}

Maquina::~Maquina(){
    //delete newCard;

    /*if (ImageSender) ImageSender->deleteLater();
    if (ImageCreator) ImageCreator->deleteLater();*/

    for (int x=0;x<ui->m_TabCentral->count();x++){
        In2GraphicsScene *p=m_Scene.at(x);
        if (p){
            p->clear();
            delete (p);
        }
        In2GraphicsView *v=m_View.at(x);
        if (v) delete (v);
    }
}


//Acepta la selecci?n realizada y vuelve a la pantalla de Configuracion
void Maquina::AceptarSeleccion(){
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    StopThreads();
    LaunchPrincipal();
}


void Maquina::CancelarSeleccion(){
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    StopThreads();
    LaunchPrincipal();
}
/** Pregunto por donde va*/
void Maquina::AskPrintCount(){
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

void Maquina::CambiarTrabajo(){
    //Hacer con gestormaquina
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","Quiere cambiar de trabajo?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            emit sglTrabajoNuevo();
            //Actualizo
            memoryMap->m_mapa.m_maquina.metros_trabajo=0;
            QTableWidgetItem *p=ui->m_twData->item(ROW_METROS,COL_VALUE);
            if (p){
                p->setText("0");
            }
        }
    }
}
/** Habilita y deshabilita los botones*/
void Maquina::sltPrintStatusChange(bool monitor){
    if (!monitor){ //Sio no se monitoriza nada, habilito todos los botones
        ui->m_BuscarMacula->setEnabled(true);
        ui->m_JogMas->setEnabled(true);
        ui->m_JogMenos->setEnabled(true);
        ui->m_Empalme->setEnabled(true);
        ui->m_TrabajoNuevo->setEnabled(true);
        ui->m_AceptarButton->setEnabled(true);
        ui->m_ColaButton->setEnabled(true);
        //ui->SelImgFija->setEnabled(true);
        ui->SendIni->setEnabled(true);
        //ui->groupBox_2->setEnabled(true);

    }
    else{
        ui->m_BuscarMacula->setEnabled(false);
        ui->m_JogMas->setEnabled(false);
        ui->m_JogMenos->setEnabled(false);
        ui->m_Empalme->setEnabled(false);
        ui->m_TrabajoNuevo->setEnabled(false);
        ui->m_AceptarButton->setEnabled(false);
        ui->m_ColaButton->setEnabled(false);
        //ui->SelImgFija->setEnabled(false);
        ui->SendIni->setEnabled(false);
        //ui->groupBox_2->setEnabled(false);
    }
}

/** Actualiza la velcidad actual*/
void Maquina::updateVelocidadActual(){
    int value=0;
    int IntRef=0;
    int DecRef=0;
    MemoryMap *memoryMap=MemoryMap::request(0);
    value=memoryMap->m_mapa.m_maquina.velocidad;
    if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
        IntRef=(value/1000);
        DecRef=(value%1000);
    }
    else{
        IntRef=(value/100);
        DecRef=(value%100);
    }
    QTableWidgetItem *p=ui->m_twData->item(ROW_VELOCIDAD,COL_VALUE);
    if (p){
        p->setText(QString("%1.%2").arg(IntRef).arg(DecRef).left(5));
    }
}
/** Actualiza Diametro bonina entradsa*/
void Maquina::updateMetrosImpresos(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_METROS,COL_VALUE);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.metros_trabajo));
    }
}

/** Actualiza Diametro bonina entradsa*/
void Maquina::updateMaculaMax(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_MACULAMAX,COL_VALUE);
    if (p){
        if ((memoryMap->m_mapa.m_maquina.iMaxMacula==MIN_MACULA)|| (memoryMap->m_mapa.m_maquina.iMaxMacula==MAX_MACULA)); //p->setText("0");
        else
            p->setText(QString("%1").arg(memoryMap->m_mapa.m_impresion.m_fltrMacula.fMaxFiltered));
    }
}
/** Actualiza Diametro bonina entradsa*/
void Maquina::updateMaculaMin(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_MACULAMIN,COL_VALUE);
    if (p){
        if ((memoryMap->m_mapa.m_maquina.iMinMacula==MIN_MACULA)|| (memoryMap->m_mapa.m_maquina.iMinMacula==MAX_MACULA)); //p->setText("0");
        else
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_impresion.m_fltrMacula.fMinFiltered));
    }
}


/** Actualiza Diametro bonina entradsa*/
void Maquina::updateDBobinaEntrada(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_DENTRADA,COL_VALUE);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.diametro_bobina_entrada));
    }
}
/** Actualiza Diametro bonina salida*/
void Maquina::updateDBobinaSalida(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_DSALIDA,COL_VALUE);
    if (p){
        In2ManagerDiametros *man=In2ManagerDiametros::request(0);
        MonitorDiametro *t=man->getMonitor(IN2MANAGER_DOUT);
        if (t->isEnabled()){
            p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.diametro_bobina_salida_inst));
        }
        else{
            if (t->isPLCEnabled())
                p->setText(QString("%1").arg(t->getCurrentDiameter(0)));
            else
                p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.diametro_bobina_salida));
        }
        if ((memoryMap->m_mapa.m_maquina.diametro_bobina_entrada+memoryMap->m_mapa.m_maquina.diametro_bobina_salida)>DIAMETRO_PARO_BOBINA_MAX){
            emit sgnMaxBobina();
        }
    }
}

/** Actualiza Temperatura lamapara*/
void Maquina::updateTemperaturaLampara(){
#ifdef HONLE
#else
        MemoryMap *memoryMap=MemoryMap::request(0);
        QTableWidgetItem *p=ui->m_twData->item(ROW_TLAMPARA,COL_VALUE);
        if (p){
            p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.temperatura_lampara));
        }

#endif
}

/** Actualiza Temperatura lamapara*/
void Maquina::updateIntensidadLampara(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_ILAMPARA,COL_VALUE);
    if (p){
        p->setText(QString("%1").arg(memoryMap->m_mapa.m_maquina.intensidad_lampara));
    }
}

/** Actualiza Temperatura Chiller*/
void Maquina::updateTemperaturaChiller(int x){
    In2GestorChiller *gChiller=In2GestorChiller::request(0,1);
    quint16 val=gChiller->getDischargeTemp(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_TCHILLER,COL_VALUE);
    if (p){
        p->setText(QString("%1.%2").arg(val/10).arg(val%10));
    }
}
/** Actualiza setpoint Chiller*/
void Maquina::updateSetPointChiller(int x){
    In2GestorChiller *gChiller=In2GestorChiller::request(0,1);
    quint16 val=gChiller->getDischargeSetTemp(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_TCHILLER,COL_REF);
    if (p){
        p->setText(QString("%1.%2").arg(val/10).arg(val%10));
    }
}
/** Actualiza led de alarma chiller*/
void Maquina::updateAlarmaChiller(int x){
    In2GestorChiller *gChiller=In2GestorChiller::request(0,1);
    /*quint16 a1=gChiller->getAlarm1(x);
    quint16 a2=gChiller->getAlarm2(x);
    quint16 a3=gChiller->getAlarm3(x);

    if ((a1)||(a2)||(a3)) ui->m_ledChiller->setPixmap(QPixmap(LED_ROJO));
    else ui->m_ledChiller->setPixmap(QPixmap(LED_VERDE));*/
    if (gChiller->isReadyToPrint()) ui->m_ledChiller->setPixmap(QPixmap(LED_VERDE));
    else ui->m_ledChiller->setPixmap(QPixmap(LED_ROJO));


}

/** Actualiza setpoint Chiller*/
void Maquina::updateCOMChiller(int x){
    In2GestorChiller *gChiller=In2GestorChiller::request(0,1);
    char val=gChiller->GetFlag(FLAG_COMCHILLER_OK);
    QTableWidgetItem *p1=ui->m_twData->item(ROW_TCHILLER,COL_VALUE);
    QTableWidgetItem *p2=ui->m_twData->item(ROW_PCHILLER,COL_VALUE);
    if ((p1)&&(p2)){
        if (val==CHILLER_COMNOK){
            p1->setText(QString("NOCOM"));
            p1->setBackgroundColor(QColor(Qt::red));
            p2->setText(QString("NOCOM"));
            p2->setBackgroundColor(QColor(Qt::red));
        }
        else{
            p1->setBackgroundColor(QColor(Qt::white));
            p2->setBackgroundColor(QColor(Qt::white));
            updateTemperaturaChiller(0);
            updatePresionChiller(0);
        }
    }
}


/** Actualiza presion Chiller*/
void Maquina::updatePresionChiller(int x){
    In2GestorChiller *gChiller=In2GestorChiller::request(0,1);
    quint16 val=gChiller->getDischargePressure(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_PCHILLER,COL_VALUE);
    if (p){
        p->setText(QString("%1.%2").arg(val/10).arg(val%10));
    }
}



/** Empieza o termina de monitorizar*/
void Maquina::enableMonitoring(bool value){
    m_MonitoringEnabled=value;
    //sltPrintStatusChange(value);
    if (value){
        sltMonitoringData();
    }
    else{

    }


}
/** Actualiza el balluf*/
void Maquina::updateBalluf(){
    /*if (m_memoryMap->m_mapa.m_maquina.balluf_centrado) ui->->setPixmap(QPixmap(LED_VERDE));
    else ui->m_ledBalluf->setPixmap(QPixmap(LED_ROJO));*/
}

// Actualiza la info
void Maquina::sltMonitoringData(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){ //Valores del OPC
        updateVelocidadActual();
        updateMetrosImpresos();
        updateDBobinaEntrada();
        updateDBobinaSalida();
        updateTemperaturaLampara();
        updateIntensidadLampara();
        updateImpresiones();
        if (memoryMap->m_mapa.m_machinedefaults.filtroMaculas){
            updateMaculaMax();
            updateMaculaMin();
        }
        //updateBalluf();
    }

    if (m_MonitoringEnabled){
        QTimer::singleShot(TIMEOUT_REFRESH_DATA,this,SLOT(sltMonitoringData()));
    }
}
/** Actualizo impresiones*/
void Maquina::updateImpresiones(){
    GestorImpresion *gs=GestorImpresion::request(this);
    if (gs){
        DWORD SwatheCount[MAXHEADS * MAXROWS];
        memset(&SwatheCount,0x00,sizeof(SwatheCount));
        gs->readSwathePrintCount(0,&SwatheCount[0]);
        MemoryMap *memoryMap=MemoryMap::request(0);
        memoryMap->m_mapa.m_maquina.iCounter=SwatheCount[0];
    }
}

/** Fuerzo un refresco de valores de referencia*/
void Maquina::ForceRefreshValues(){

    /*if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        ui->m_MetrosTrabajo->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo));
        ui->m_MetrosTrabajoAct->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.metros_trabajo));
        ui->m_VelocidadCinta->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.ref_velocidad));
        ui->m_DiametroBobinaEntrada->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada));
        ui->m_DiametroBobinaSalida->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida));
        ui->m_DiametroBobinaEntradaParo->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.diametro_parar));
    }
    ui->m_gota1->setCurrentIndex(m_memoryMap->m_mapa.m_variables.VolumenGota[0]/DEFAULT_DROP_VOL);
    ui->m_gota2->setCurrentIndex(m_memoryMap->m_mapa.m_variables.VolumenGota[1]/DEFAULT_DROP_VOL);*/
}


void Maquina::AceptarValorVelocidad(){
    //if (m_memoryMap->m_mapa.m_maquina.impresion==false){
    if (!InputData::exist()){
        InputData *input = InputData::request(this,"Velocidad Cinta [m/min]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarVelocidad(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarVelocidad(QString value){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_VELOCIDAD,COL_REF);
    if (p){
        int dummy=value.toInt();
        if (memoryMap->m_mapa.m_maquina.VersionMaquina==1){
            if (dummy<REF_VELOCIDAD_MIN) dummy=REF_VELOCIDAD_MIN;
            if (dummy>REF_VELOCIDAD_MAX) dummy=REF_VELOCIDAD_MAX;
        }
        else if (memoryMap->m_mapa.m_maquina.VersionMaquina==2){
#ifndef HIGH_SPEED
            if ((memoryMap->m_mapa.m_variables.NumColores==1)||(memoryMap->m_mapa.m_variables.NumColores==0)){
                if (dummy<REF_VELOCIDAD_MIN) dummy=REF_VELOCIDAD_MIN;
                if (dummy>REF_VELOCIDAD_MAX) dummy=REF_VELOCIDAD_MAX;
            }
            else{
                if (dummy<REF_VELOCIDAD_MIN_2C) dummy=REF_VELOCIDAD_MIN_2C;
                if (dummy>REF_VELOCIDAD_MAX_2C) dummy=REF_VELOCIDAD_MAX_2C;
            }
#else //En caso de HighSpeed module dejo ir al doble
            if ((memoryMap->m_mapa.m_variables.NumColores==1)||(memoryMap->m_mapa.m_variables.NumColores==0)){
                if (dummy<REF_VELOCIDAD_MIN) dummy=REF_VELOCIDAD_MIN;
                if (dummy>REF_VELOCIDAD_MAX*2) dummy=REF_VELOCIDAD_MAX*2;
            }
            else{
                if (dummy<REF_VELOCIDAD_MIN_2C) dummy=REF_VELOCIDAD_MIN_2C;
                if (dummy>REF_VELOCIDAD_MAX_2C*2) dummy=REF_VELOCIDAD_MAX_2C*2;
            }
#endif

        }

        QString newValue=QString("%1").arg(dummy);
        p->setText(newValue);
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            signed short prev=memoryMap->m_mapa.m_maquina.ref_velocidad;
            memoryMap->m_mapa.m_maquina.ref_velocidad=dummy;
            GestorMaquina *gestorMaquina=GestorMaquina::request(this);
            gestorMaquina->sltHabilitarCalculoDiametro(0);
            gestorMaquina->sltSetReferenciaVelocidadControlled();
            emit SendDebugInfo(LOGGER_STORE,tr("Nueva referencia velocidad: [%1->%2]").arg(prev).arg(dummy));
        }
    }
}

void Maquina::AceptarValorMetros(){
   // if (m_memoryMap->m_mapa.m_maquina.impresion==false){
   if (!InputData::exist()){
        InputData *input = InputData::request(this,"Metros trabajo [m]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarMetros(QString)));
            input->setFocus();
        }
    }
}
/** Pide valor de bobina*/
void Maquina::AceptarValorBobina(){
   // if (m_memoryMap->m_mapa.m_maquina.impresion==false){
   if (!InputData::exist()){
        InputData *input = InputData::request(this,tr("Introduzca ID de bobina"));
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarBobina(QString)));
            input->setFocus();
        }
    }
}
/** Comprueba si la bobina es correcta*/
void Maquina::AceptarBobina(QString value){
    In2ManagerERP *erp=In2ManagerERP::request(0);
    QString bob=erp->getValueFromERP(in_COD_BULTO_PADRE);
    if (!bob.compare(value)){
        QString txt=tr("OK Cod_bobina=%1, expected=%2").arg(value).arg(bob);
        emit SendDebugInfo(LOGGER_STORE,txt);
        In2GestorMaquina *m=In2GestorMaquina::request(this);
        m->InitEmpalme();
    }
    else {
        QString txt=tr("ERROR Cod_bobina=%1, expected=%2").arg(value).arg(bob);
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_STORE,txt);
    }
}

/** Pide valor de bobina*/
void Maquina::TestValorCBBobina(){
   // if (m_memoryMap->m_mapa.m_maquina.impresion==false){
   if (!InputData::exist()){
        InputData *input = InputData::request(this,tr("Introduzca ID de bobina"));
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(TestCBBobina(QString)));
            input->setFocus();
        }
    }
}
/** Comprueba si la bobina es correcta*/
void Maquina::TestCBBobina(QString value){
    In2ManagerERP *erp=In2ManagerERP::request(0);
    QString bob=erp->getValueFromERP(in_COD_BULTO_PADRE);
    if (!bob.compare(value)){
        QString txt=tr("OK Cod_bobina=%1, expected=%2").arg(value).arg(bob);
        emit SendDebugInfo(LOGGER_STORE,txt);
        m_bCodigoCorrecto=true;
        QTimer::singleShot(0,this,SLOT(SendIniToXaar()));
    }
    else {
        QString txt=tr("ERROR Cod_bobina=%1, expected=%2").arg(value).arg(bob);
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_STORE,txt);
        m_bCodigoCorrecto=false;
    }
}



/** Acepta cambiar los metros de referencia*/
void Maquina::AceptarMetros(QString value){
    QTableWidgetItem *p=ui->m_twData->item(ROW_METROS,COL_REF);
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (p){
        int dummy=value.toInt();
        if (dummy<METROS_TRABAJO_MIN) dummy=METROS_TRABAJO_MIN;
        else if (dummy>METROS_TRABAJO_MAX) dummy=METROS_TRABAJO_MAX;
        QString newValue=QString("%1").arg(dummy);
        p->setText(newValue);
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            signed short prev=memoryMap->m_mapa.m_maquina.ref_metros_trabajo;
            memoryMap->m_mapa.m_maquina.ref_metros_trabajo=dummy;
            QVariant valor=QVariant(dummy);
            emit sglSetMetros(valor);
            emit SendDebugInfo(LOGGER_STORE,tr("Nueva referencia metros: [%1->%2]").arg(prev).arg(dummy));
        }
    }
}

void Maquina::AceptarValorDiametro(){
    if (!InputData::exist()){
        InputData *input = InputData::request(this,"Diametro bobina entrada [mm]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDiametro(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarDiametro(QString value){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_DENTRADA,COL_REF);
    int diametroMin=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaEntrada;
    int diametroMax=memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaEntrada;
    if (p){
        int dummy=value.toInt();
        if (dummy<diametroMin) dummy=diametroMin;
        else if (dummy>diametroMax) dummy=diametroMax;
        QString newValue=QString("%1").arg(dummy);
        p->setText(newValue);
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            memoryMap->m_mapa.m_maquina.ref_diametro_bobina_entrada=dummy;
            memoryMap->m_mapa.m_maquina.diametro_bobina_entrada=dummy;
            QVariant valor=QVariant(dummy);
            emit sglSetDiametroEntrada(valor);
            emit SendDebugInfo(LOGGER_STORE,tr("Nueva referencia diametro entrada: %1").arg(dummy));
        }
    }
}

void Maquina::AceptarValorDiametroParo(){
   // if (m_memoryMap->m_mapa.m_maquina.impresion==false){
   if (!InputData::exist()){
        InputData *input = InputData::request(this,"Diametro paro bobina entrada [mm]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDiametroParo(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarDiametroParo(QString value){
    QTableWidgetItem *p=ui->m_twData->item(ROW_PARO_DENTRADA,COL_REF);
    MemoryMap *memoryMap=MemoryMap::request(0);
    int diametroMin=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaEntrada;
    int diametroMax=memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaEntrada;
    if (p){
        int dummy=value.toInt();
        if (dummy<diametroMin) dummy=diametroMin;
        else if (dummy>diametroMax) dummy=diametroMax;
        QString newValue=QString("%1").arg(dummy);
        p->setText(newValue);
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            signed short prev=memoryMap->m_mapa.m_maquina.diametro_parar;
            memoryMap->m_mapa.m_maquina.diametro_parar=dummy;
            QVariant valor=QVariant(dummy);
            emit sglSetDiametroParo(valor);
            emit SendDebugInfo(LOGGER_STORE,tr("Nueva referencia diametro parada: [%1->%2]").arg(prev).arg(dummy));
        }
    }

}

void Maquina::AceptarValorDiametroSalida(){
    //if (m_memoryMap->m_mapa.m_maquina.impresion==false){
    if (!InputData::exist()){
       InputData *input = InputData::request(this,"Diametro bobina salida [mm]");
        if (input){
            connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDiametroSalida(QString)));
            input->setFocus();
        }
    }
}

void Maquina::AceptarDiametroSalida(QString value){
    QTableWidgetItem *p=ui->m_twData->item(ROW_DSALIDA,COL_REF);
    MemoryMap *memoryMap=MemoryMap::request(0);
    int diametroMin=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaEntrada;
    int diametroMax=memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaEntrada;
    if (p){
        int dummy=value.toInt();
        if (dummy<diametroMin) dummy=diametroMin;
        else if (dummy>diametroMax) dummy=diametroMax;
        QString newValue=QString("%1").arg(dummy);
        p->setText(newValue);
        MemoryMap *memoryMap=MemoryMap::request(0);
        if (memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            memoryMap->m_mapa.m_maquina.ref_diametro_bobina_salida=dummy;
            QVariant valor=QVariant(dummy);
            emit sglSetDiametroSalida(valor);
            emit SendDebugInfo(LOGGER_STORE,tr("Nueva referencia diametro salida: %1").arg(dummy));
        }
    }
}

void Maquina::LaunchPrincipal(){
    //GlobalShowMainWindow();
    m_parent->show();
    this->deleteLater();
}

void Maquina::LaunchMaquina2(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()==ACCESS_GOD){
        MaquinaAdv *ScrMaquina2;
        ScrMaquina2=new MaquinaAdv(0);
        ScrMaquina2->show();
        connect(ScrMaquina2,SIGNAL(destroyed()),this,SLOT(ForceRefreshValues()));
    }
}

void Maquina::ShowCam(){
    /*Webcam *MiWebcam;
    MiWebcam= new Webcam(0);
    //As? cuando muera, actualizo la info de pantalla
    connect(MiWebcam,SIGNAL(destroyed()),this,SLOT(ForceRefreshValues()));*/

}

void Maquina::LaunchColaImpresion(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString client=memoryMap->getClient();

    if (!client.compare(CLIENT_ESTEVE)){
        In2ColaImpresionMono *p;
        p=new In2ColaImpresionMono(this);
        InkjetDebug *dbg=InkjetDebug::request(0);
        connect(p,SIGNAL(sgnWorkLoaded(bool)),this,SLOT(sltLoadFromCola(bool)));
        connect(p,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
        //Por si acaso
        connect(this,SIGNAL(destroyed()),p,SLOT(deleteLater()));
        //connect(p,SIGNAL(sgnColaNewMetrosRef(int)),machine,SLOT(sltChangeMetrosReferencia(int)));
        p->show();
    }
    else if (!client.compare(CLIENT_GRAMAR)){
        In2GramarColaImpresion *p=new In2GramarColaImpresion(this);
        InkjetDebug *dbg=InkjetDebug::request(0);
        In2GestorMaquina *gestor=In2GestorMaquina::request(0);
        connect(p,SIGNAL(sgnWorkLoaded(bool)),this,SLOT(sltLoadFromCola(bool)));
        connect(p,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
        connect(this,SIGNAL(destroyed()),p,SLOT(deleteLater()));
        connect(p,SIGNAL(sgnERPTrabajoNuevo()),gestor,SLOT(sltTrabajoNuevo()));
        p->show();

    }
    else{
        In2ColaImpresion *p;
        p=new In2ColaImpresion(this);
        connect(p,SIGNAL(sgnWorkLoaded()),this,SLOT(CargaDatos()));
        p->show();
    }

}

/** Carga un trabajo*/
void Maquina::sltLoadFromCola(bool val){
    if (val) CargaDatos();
}

void Maquina::EjecutarJogMasOn(){
    //Hacer con GestorMaquina
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        GestorImpresion *gI=GestorImpresion::request(this);
        if ((gM)&&(gI)){
            gI->FullDisablePrintMode();
            gM->sltJogMasOn(memoryMap->m_mapa.m_maquina.jogmas_conlampara);
        }
    }
}

void Maquina::EjecutarJogMenosOn(){
    //hacer con Gestormaquina
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        GestorImpresion *gI=GestorImpresion::request(this);
        if ((gM)&&(gI)){
            gI->FullDisablePrintMode();
            gM->sltJogMenosOn(memoryMap->m_mapa.m_maquina.jogmenos_conlampara);
        }
    }
}


void Maquina::EjecutarJogMasOff(){
    //hacer con gestorMaquina
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        if (gM){
            gM->sltJogMasOff();
        }
    }
}

void Maquina::EjecutarJogMenosOff(){
   //hacer con Gestormaquina
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        if (gM){
            gM->sltJogMenosOff();
        }
    }
}


void Maquina::EjecutarTensionMasOn(){
    //Hacer con GestorMaquina
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        GestorImpresion *gI=GestorImpresion::request(this);
        if ((gM)&&(gI)){
            gI->FullDisablePrintMode();
            gM->sltTensarMasOn();
            //gM->sltJogMasOn(m_memoryMap->m_mapa.m_maquina.jogmas_conlampara);
        }
    }
}

void Maquina::EjecutarTensionMenosOn(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        GestorImpresion *gI=GestorImpresion::request(this);
        if ((gM)&&(gI)){
            gI->FullDisablePrintMode();
            gM->sltDestensarMasOn();
        }
    }
}


void Maquina::EjecutarTensionMasOff(){
    //hacer con gestorMaquina
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        if (gM){
            gM->sltTensarMasOff();
            //gM->sltJogMasOff();
        }
    }
}

void Maquina::EjecutarTensionMenosOff(){
   //hacer con Gestormaquina
    MemoryMap *memoryMap=MemoryMap::request(0);
    if ((memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        GestorMaquina *gM= GestorMaquina::request(this);
        if (gM){
            gM->sltDestensarMasOff();
            //gM->sltJogMenosOff();
        }
    }
}





void Maquina::EjecutarBuscarMacula(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    int respuesta=QMessageBox::question(this,"ATENCION","Quiere buscar mácula?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (memoryMap->m_mapa.m_maquina.impresion==false){
            for (int i=0;i<m_iTotalXUSB;i++)
                bXaarScorpionEnablePrintMode(i,0xffff,false);
            emit sglBuscarMacula();
            memoryMap->m_ImagenEnviada=false;
        }
    }

}


void Maquina::EjecutarEmpalme(){
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    In2ManagerERP *erp=In2ManagerERP::request(0);
    if (erp->empalmePermitido()){
        if (!erp->getClient().compare(ERP_ESTEVE)){
            AceptarValorBobina();
        }
        else{
            In2GestorMaquina *m=In2GestorMaquina::request(this);
            m->InitEmpalme();
        }
    }
    else{
        QString txt=tr("No se permite empalme");
        emit SendDebugInfo(LOGGER_WARNING,txt);
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
    }
}


void Maquina::SelImagenFija(){
    /*if (m_memoryMap->m_mapa.m_maquina.velocidad>1){
        QMessageBox::warning(this,"ERROR","Pare primero la m?quina",QMessageBox::Ok);
        return;
    }*/
    SelImagen();

}


bool Maquina::SelImagen(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QFileDialog fileName(this);
    QStringList Ficheros;
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    QString defaultPath=QString("%1\\Images").arg(QApplication::applicationDirPath());
    fileName.setDirectory(defaultPath);
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
            m_nombrefichero=QString("%1").arg(Ficheros.at(0)); //Nombre y ruta del bmp
            QRegExp p=QRegExp("[0-9]{1,1}_comp.bmp");
            if (!m_nombrefichero.contains(p)){
                QMessageBox::warning(0,"Error","El nombre de fichero debe contener _C[num]_comp.bmp",QMessageBox::Ok);
                m_nombrefichero.clear();
                return false;
            }
            MuestraImagenFromFile();
            memoryMap->ImagenVariable=0;
            QFileInfo info(m_nombrefichero);
            memoryMap->m_mapa.m_variables.NombreBmpEnviado=info.fileName();/*DevuelveNombre(nombrefichero);*/
            memoryMap->m_mapa.m_variables.RutaBmpEnviado=info.path();/*DevuelveRuta(nombrefichero);*/
            memoryMap->m_UltimaAccion=UA_IMAGEN_SELECCIONADA;
            //De momento le ponemos como 1 color
            memoryMap->m_mapa.m_variables.NumColores=1;
        }
     }
    ImagenXaar=m_nombrefichero;
    return true;
}

/** Termina con los hilos de impresion si los hubiera*/
void Maquina::StopThreads(){
    //emit StopThread();
}
/** Condicion de arranque de que el trabajo no este ya impreso*/
bool Maquina::_checkTrabajoImpreso(){
    bool rtn=false;
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->getClient().compare(CLIENT_ESTEVE)){
        if (!m_TrabajoNuevo){
            rtn=true;
        }
     }
    return rtn;
}
/** Condicion de arranque para testear CB*/
bool Maquina::_checkCB(){
    bool rtn=true;
    if (m_bLoadFromCola){
        In2ManagerERP *erp=In2ManagerERP::request(0);
        QString cb=erp->getValueFromERP(in_COD_BULTO_PADRE);
        if (!cb.isEmpty()){
            if (!m_bCodigoCorrecto){
                TestValorCBBobina();
                rtn=false;
            }
            //rtn=false;
        }
    }
    return rtn;
}
/** Condicion de arranque para testear vision*/
bool Maquina::_sltCheckVision(){
    bool rtn=false;

    sltUpdateMessage("");
    ui->m_ledVision->setEnabled(false);
    ui->m_ledMacula->setEnabled(false);
    QString visionProgram=ui->m_leFileVision->text();
    MemoryMap *memoryMap=MemoryMap::request(0);
    In2GestorVision *vision=In2GestorVision::request(0);
    if ((vision->isEnabled())&&(memoryMap->m_mapa.m_impresion.bMonitorVision)){
       if (!vision->isConnected()){
           sltVisionSocketFail();
       }
       else if (visionProgram.isEmpty()){
           QString txt=tr("No hay programa de vision...elija programa de vision manual");
           emit SendDebugInfo(LOGGER_STORE,txt);
           sltUpdateMessage(txt);
       }
       else{
           if (vision->sltLoadWork(visionProgram))rtn=true; //sale desde aqui
           else {
               QString txt=tr("No se pudo escribir en socket de vision. Revise conexiones");
               sltUpdateMessage(txt);
           }
       }
    }
    else{
       QString txt=QString("Vision deshabilitada. Enabled[%1] Monitor[%2]").arg(vision->isEnabled()).arg(memoryMap->m_mapa.m_impresion.bMonitorVision);
       emit SendDebugInfo(LOGGER_STORE,txt);
       //sltUpdateMessage(txt);
       rtn=true;
    }

    if (!rtn){
        QMessageBox::warning(this,"INFO","No se pudo acceder a equipo de vision, revise LOG",QMessageBox::Ok);
    }

    return rtn;
}
/** Condicion de arranque condiciones fisicas*/
bool Maquina::_checkCondicionesFisicas(){
    bool rtn=true;
    In2ManagerERP *erp=In2ManagerERP::request(0);
    //Condiciones fisicas del ERP
    if (!erp->checkCondicionesFisicasEntrada()){       
        QString txt=erp->getLastError();
        sltUpdateMessage(txt);
        emit SendDebugInfo(LOGGER_STORE,txt);
        //rtn=false;
       //if (!m_bLoadFromCola){
         QString question=QString("%1\nDesea continuar?").arg(txt);
         int answer=QMessageBox::question(this,"ERROR",question,QMessageBox::Ok,QMessageBox::Cancel);
         if (answer==QMessageBox::Ok){
            emit SendDebugInfo(LOGGER_STORE,tr("Validacion manual de condiciones fisicas de maquina"));
             rtn=true;
         }
         else rtn=false;
       //}
    }
    In2ManagerDiametros *managerDiam=In2ManagerDiametros::request(0);
    double diamSalida=0;
    double diamEntrada=0;
    MonitorDiametro *p=managerDiam->getMonitor(IN2MANAGER_DOUT);
    if (p) diamSalida=p->getCurrentDiameter(0);
    p=managerDiam->getMonitor(IN2MANAGER_DIN);
    if (p) diamEntrada=p->getCurrentDiameter(0);

    MemoryMap *memoryMap=MemoryMap::request(0);
    int diamEntradaMinimo=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaEntrada;
    int diamSalidaMinimo=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBobinaSalida;
    int diamEntradaMaximo=memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaEntrada;
    int diamSalidaMaximo=memoryMap->m_mapa.m_machinedefaults.DiametroMaximoBobinaSalida;


    if (diamEntrada<diamEntradaMinimo){
        emit SendDebugInfo(LOGGER_ERROR,QString("Diametro Bulto padre %1. Minimo %2").arg(diamEntrada).arg(diamEntradaMinimo));
        rtn=false;
    }
    if (diamSalida<diamSalidaMinimo){
        emit SendDebugInfo(LOGGER_ERROR,QString("Diametro Bulto hijo %1. Minimo %2").arg(diamSalida).arg(diamSalidaMinimo));
        rtn=false;
    }
    if (diamEntrada>diamEntradaMaximo){
        emit SendDebugInfo(LOGGER_ERROR,QString("Diametro Bulto padre %1. Maximo %2").arg(diamEntrada).arg(diamEntradaMaximo));
        rtn=false;
    }
    if (diamSalida>diamSalidaMaximo){
        emit SendDebugInfo(LOGGER_ERROR,QString("Diametro Bulto hijo %1. Maximo %2").arg(diamSalida).arg(diamSalidaMaximo));
        rtn=false;
    }


    return rtn;
}

/** Envia Xaar*/
void Maquina::SendIniToXaar(){

    //emit sgnInitImpresion(true);
    /*In2GestorLampara *lampara=In2GestorLampara::request(0);
    if (lampara->posDinamica()){
        lampara->setMoving(false); //Se deshabilita la marca de fin de movimiento
        lampara->moveToInit();
    }*/

    QTableWidgetItem *p=ui->m_twData->item(ROW_MACULAMAX,COL_VALUE);
    if (p)
        p->setBackgroundColor(QColor(Qt::white));
    p=ui->m_twData->item(ROW_MACULAMIN,COL_VALUE);
    if (p)
        p->setBackgroundColor(QColor(Qt::white));
    ui->m_ledMacula->setEnabled(false);


    //Testeo condiciones
    if (_checkTrabajoImpreso()) {
        QString txt=QString("Trabajo ya impreso, consulte SGL");
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_STORE,txt);
        return;
    }
    if (!_checkCB()){
        /*QString txt=QString("Error en Codigo de barras");
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_STORE,txt);*/
        return;
    }
    if (!_checkCondicionesFisicas()){
        QString txt=QString("Error en condiciones fisicas. Revise log");
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        m_bCodigoCorrecto=false;
        emit SendDebugInfo(LOGGER_STORE,txt);
        return;
    }

    /*if (!_sltCheckVision()){
        return;
    }*/
    sltSendIni0();

}

/** Secuencia 0 de inicio*/
void Maquina::sltSendIni0()
{
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->ImagenVariable=0x00;
    for(unsigned char i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01) memoryMap->ImagenVariable=0x01;
    }
    GestorMaquina *gestorMaquina=GestorMaquina::request(this);
    gestorMaquina->sltHabilitarCalculoDiametro(0);
    sltUpdateMessage(tr("TENSANDO BALLUF"));
    gestorMaquina->sltTensadoAutomatico();
    QTimer::singleShot(3000,this,SLOT(sltSendIni1()));
}

/** Secuencia 1 de inicio*/
void Maquina::sltSendIni1(){
    ClienteOPC *opc=ClienteOPC::request(this);
    if (opc->LeerDato(ITEM_TENSAR_LAMINA).toInt()){
        QTimer::singleShot(150,this,SLOT(sltSendIni1()));
    }
    else {
        //emit KillIn2Msg();
        sltSendIni2();
    }
}
/** Secuencia 2 de inicio*/
void Maquina::sltSendIni2(){
    GestorImpresion *gestorPrint=GestorImpresion::request(this);
    MemoryMap *memoryMap=MemoryMap::request(0);
    int cards=gestorPrint->getNumCards();
    int heads=memoryMap->m_mapa.m_variables.DefaultNumHeads;
    unsigned int rtn=1;
    bool check=true;
    for (int x=0;x<cards;x++){
        if (!check) break;
        for (int y=0;y<heads;y++){
            rtn=gestorPrint->checkSerialHead(x,y);
            if (rtn==0){
                check=false;
                QString txt=QString("Fallo %1 en Card[%2] Head[%3]. Restaurando en 40 sg").arg(memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico).arg(x).arg(y);
                //ui->m_leMessage->setText(txt);
                sltUpdateMessage(txt);
                memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico++;
                gestorPrint->sltReiniciarSistema();
                break;
            }
        }
    }
    if (!check) {
        if (memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico<BOOT_LIMIT)
            QTimer::singleShot(60000,this,SLOT(sltSendIni2()));
        else{
            memoryMap->m_mapa.m_maquina.EstadoSecuenciaAutomatico=0;
            //emit KillIn2Msg();
            ui->m_teMessage->setText("");

            QMessageBox::warning(this,"ERROR","Fallo continuo en XUSB.Reinicie aplicación",QMessageBox::Ok);
        }
        return;
    }
    //emit KillIn2Msg();
    ui->m_teMessage->setText("");


    memoryMap->m_mapa.m_maquina.iCounter=0;
    memoryMap->m_mapa.m_maquina.iTCounter=0;
    int iCountToDo=(memoryMap->m_mapa.m_maquina.ref_metros_trabajo-memoryMap->m_mapa.m_maquina.metros_trabajo);
    if (iCountToDo>0){
        if (memoryMap->m_mapa.m_variables.WebHeight){
            iCountToDo=(iCountToDo*1000)/memoryMap->m_mapa.m_variables.WebHeight;
            memoryMap->m_mapa.m_maquina.iCounter=0;
            memoryMap->m_mapa.m_maquina.iTCounter=iCountToDo;
        }
    }
    /* AQUI ACTUALIZAMOS CONTADORES EN PANTALLA---DESHABILITADO POR DEFECTO
    QTableWidgetItem *p=ui->m_twData->item(ROW_COUNTER,COL_VALUE);
    if (p){
        p->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.iCounter));
    }
    p=ui->m_twData->item(ROW_COUNTER,COL_REF);
    if (p){
        p->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.iTCounter));
    }*/

    GestorMaquina *gestorMaquina=GestorMaquina::request(this);
    gestorMaquina->sltEnableCondicionesMovimiento();
    //Testeo si es impresi?n normal o impresión reverse
    QString xusbFile=XUSBXML;
#ifdef HIGH_SPEED
    if (memoryMap->m_mapa.m_impresion.bHighSpeed){
        emit SendDebugInfo(LOGGER_ERROR,QString("CONFIGURACION HIGHSPEED activa"));
        xusbFile=XUSBHIGHSPEEDXML;
    }
#endif
    gestorPrint->setNewConfigFile(xusbFile);
    if (memoryMap->ImagenVariable==0x00){
        QTimer::singleShot(0,this,SLOT(SendToXaar()));
    }
    else{
        #if defined(MODULO_VARINFO) || defined(DEPURACION_IN2)
            QMessageBox::warning(this,"INFO","La composicón contiene datos variables",QMessageBox::Ok);
            SendToVarXaar();
        #else
            QMessageBox::warning(this,"ERROR","Modulo de informacion variable no incluido en esta versión. Contacte con In2 Printing Solutions para ampliar funcionalidad",QMessageBox::Ok);
        #endif
    }

}
/** Muestra los leds y el tipo de impresion*/
void Maquina::sltUpdateTipoImpresion(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_impresion.bEscalaGrises){
        ui->m_ledGSPrint->setEnabled(true);
        ui->m_ledMonoPrint->setEnabled(false);
    }
    else{
        ui->m_ledGSPrint->setEnabled(false);
        ui->m_ledMonoPrint->setEnabled(true);
    }
    if (memoryMap->m_mapa.m_impresion.bReverse){
        ui->m_ledReversePrint->setEnabled(true);
        ui->m_ledNormalPrint->setEnabled(false);
    }
    else{
        ui->m_ledReversePrint->setEnabled(false);
        ui->m_ledNormalPrint->setEnabled(true);
    }

    sltSetImpresionReverse(memoryMap->m_mapa.m_impresion.bReverse);

    if (memoryMap->m_mapa.m_impresion.bMonitorHead){
        ui->m_ledMonitorHeads->setEnabled(false);
    }
    else ui->m_ledMonitorHeads->setEnabled(true);
#ifdef HIGH_SPEED
    if (memoryMap->m_mapa.m_impresion.bHighSpeed){
        ui->m_ledHSPrint->setEnabled(true);
        ui->m_ledStPrint->setEnabled(false);
    }
    else{
        ui->m_ledHSPrint->setEnabled(false);
        ui->m_ledStPrint->setEnabled(true);
    }
#endif
    GestorImpresion *printEngine=GestorImpresion::request(this);
    QString configFile=printEngine->getConfigFile();
    In2XmlParser parser(0,configFile);
    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"PrintMode";
    QString val=parser.getValue(params,"value");
    if (val.toInt()==0){
        ui->m_ledModeTrig->setEnabled(true);
        ui->m_ledModeFix->setEnabled(false);
        ui->m_ledModeB2B->setEnabled(false);
    }
    else if (val.toInt()==1){
        ui->m_ledModeTrig->setEnabled(false);
        ui->m_ledModeFix->setEnabled(true);
        ui->m_ledModeB2B->setEnabled(false);
    }
    else{
        ui->m_ledModeTrig->setEnabled(false);
        ui->m_ledModeFix->setEnabled(false);
        ui->m_ledModeB2B->setEnabled(true);
    }
    params=QStringList()<<"XUSB"<<"Configuration"<<"SequenceStart";
    val=parser.getValue(params,"value");

    if (val.toInt()==0){
        ui->m_ledPDExt->setEnabled(true);
        ui->m_ledPDInt->setEnabled(false);
        ui->m_ledPDAbs->setEnabled(false);
    }
    else if (val.toInt()==1){
        ui->m_ledPDExt->setEnabled(false);
        ui->m_ledPDInt->setEnabled(true);
        ui->m_ledPDAbs->setEnabled(false);
    }
    else{
        ui->m_ledPDExt->setEnabled(false);
        ui->m_ledPDInt->setEnabled(false);
        ui->m_ledPDAbs->setEnabled(true);
    }

    /*params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
    val=parser.getValue(params,"value");
    float foo=val.toFloat()/14.18;
    m_GOffsetPoints->setText(val);
    m_GOffsetMm->setText(QString("%1").arg(foo));*/
}

/** Calcula el tama?o de Buffer y las imagenes que le puedo enviar*/
int Maquina::CalculateXaarBufferSize(){
    int n=15;
    /*int bufferSize=ui->m_leBufferMB->text().toInt()*1000;

    if (bufferSize>MAX_XAAR_MEM_TO_USE){
        bufferSize=MAX_XAAR_MEM_TO_USE;
        ui->m_leBufferMB->setText(QString("%1").arg(bufferSize/1000));
        QMessageBox::warning(this,"ERROR",QString("El rango del buffer es [%1-%2] MB").arg(MIN_XAAR_MEM_TO_USE/1000).arg(MAX_XAAR_MEM_TO_USE/1000));
    }
    else if (bufferSize<MIN_XAAR_MEM_TO_USE){
        bufferSize=MIN_XAAR_MEM_TO_USE;
        ui->m_leBufferMB->setText(QString("%1").arg(bufferSize/1000));
        QMessageBox::warning(this,"ERROR",QString("El rango del buffer es [%1-%2] MB").arg(MIN_XAAR_MEM_TO_USE/1000).arg(MAX_XAAR_MEM_TO_USE/1000));
    }

    float singleImageKb=(m_memoryMap->m_Imagen.width()*m_memoryMap->m_Imagen.height())/1000;

    if ((singleImageKb>0)&&(singleImageKb<bufferSize))
        n=bufferSize/singleImageKb;
    else
        n=0;*/

    return n;
}


/** Comienza el  proceso de pre-generacion de imagenes*/
void Maquina::PreCreateImagesToXaar(){
    //ImageCreator->Reset();
    if (ImageCreator!=NULL){
        connect(ImageCreator,SIGNAL(destroyed()),this,SLOT(sltPreCreateImagesToXaarSync()));
        //delete ImageCreator;
        emit StopThreadCreator();
    }
    else sltPreCreateImagesToXaarSync();
}
/** Porceso retardado de carga de imagenes*/
void Maquina::sltPreCreateImagesToXaarSync(){
    ImageCreator=NULL;
    if (ImageCreator==NULL){

        if (!threadCreator->isRunning()){
            SendDebugInfo(LOGGER_WARNING,QString("Launching threadCreator"));
            threadCreator->start();
        }
        QProgressBar *pCrear=new QProgressBar(0);
        pCrear->setWindowTitle(QString("Creando %1 imágenes...Por favor espere").arg(BufferVarImages.count()));
        pCrear->setMinimum(0);
        pCrear->setMaximum(BufferVarImages.count());
        pCrear->setFixedWidth(this->width()/2);
        pCrear->setWindowFlags(/*Qt::Window|Qt::CustomizeWindowHint*/Qt::Dialog);
        pCrear->move(this->x()+this->width()/4,this->height()/2);
        pCrear->setVisible(true);
        ImageCreator=new ImageVarXaarCreator(0,&BufferVarImages);
        //ImageCreator->setGenerationLimit(ui->m_leTotalVariableToPrint->text().toInt());
        ImageCreator->setColorConversion(QImage::Format_Indexed8);

        connect(this,SIGNAL(StartPrecreate(int)),ImageCreator,SLOT(PrecreateImages(int)));
        connect(this,SIGNAL(StartGenerating()),ImageCreator,SLOT(StartGeneration()));
        connect(this,SIGNAL(PauseGenerating()),ImageCreator,SLOT(PauseGeneration()));
        //connect(this,SIGNAL(StopThread()),threadCreator,SLOT(deleteLater()));
        connect(this,SIGNAL(StopThreadCreator()),ImageCreator,SLOT(Stop()));
        //connect(ImageCreator,SIGNAL(destroyed()),threadCreator,SLOT(terminate()));
        connect(ImageCreator,SIGNAL(FinishedPrecreating()),this,SLOT(PreLoadImagesToXaar()));
        connect(ImageCreator,SIGNAL(FinishedPrecreating()),pCrear,SLOT(deleteLater()));
        connect(ImageCreator, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
        connect(ImageCreator,SIGNAL(ImagePreCreated(int)),pCrear,SLOT(setValue(int)));
        ImageCreator->moveToThread(threadCreator);
        emit StartPrecreate(ORG_COMPOFILE);
        //ImageCreator->PrecreateImages();
    }
}

/** Comienza el  proceso de pre-Carga de imagenes*/
void Maquina::PreLoadImagesToXaar(){
    if (ImageSender!=NULL){
        connect(ImageCreator,SIGNAL(destroyed()),this,SLOT(sltPreLoadImagesToXaarSync()));
        emit StopThreadSender();
    }
    else sltPreLoadImagesToXaarSync();
}

void Maquina::sltPreLoadImagesToXaarSync(){
    ImageSender=NULL;
    if (ImageSender==NULL){
        if (!threadSender->isRunning()){
            SendDebugInfo(LOGGER_WARNING,QString("Launching threadSender"));
            threadSender->start();
        }
    }
}
/** Comienza el  proceso de impresion variable*/
void Maquina::LaunchImpresionVariable(){
    m_TotalSwatheSent=BufferVarImages.count();
    emit SendDebugInfo(LOGGER_WARNING,QString("STARTING MAIN ID:%1").arg((DWORD)(QThread::currentThreadId())));
    //threadCreator->setPriority(QThread::HighestPriority);
    MuestraImagen(QPixmap::fromImage(BufferVarImages.at(0).m_Imagen));
    QMessageBox::warning(0,"INFORMACION","Imagen enviada:\nPulsa el botón de inicio",QMessageBox::Ok);
    emit StartGenerating();
    emit StartLoading();
    ui->m_AceptarButton->setEnabled(false);

}

/** Enviamos imagen variable-1 xUSB*/
void Maquina::SendToVarXaar(){

    //Primero limpiamos lo que haya en los cabezales
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->m_ImagenEnviada=true;
    memoryMap->m_UltimaAccion=UA_IMAGEN_ENVIADA;
    //Contador total de swathes enviadas
    m_TotalSwatheSent=0;
    //Swathe actual
    m_ActualSwathe=0;
    //De momento un solo xusb
    m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();
    //m_iTotalXUSB=1;
    //Se resetea el Buffer de Imagenes cada vez que le demos a un envio nuevo
    for (int x=0;x<m_iTotalXUSB;x++)
        bXaarScorpionEnablePrintMode(x,0xFFFF,false);
    for (int x=0;x<m_iTotalXUSB;x++)
        bXaarScorpionEnablePrintMode(x,0xFFFF,true);

    this->BufferVarImages.clear();
    //Le damos un tama?o m?ximo a la lista
    int n=CalculateXaarBufferSize();
    if (n==0){
        QMessageBox::warning(this,"ERROR","La imagen no cabe en memoria, reduzca tamaño o aumente buffer",QMessageBox::Ok);
        return;
    }
    for (int x=0;x</*SIZE_BUFFER_IMAGES*/n;x++){
        //this->BufferVarImages[x].m_state=XAAR_IMAGEN_VACIA;
        struct In2VarImage p;
        p.m_state=XAAR_IMAGEN_VACIA;
        this->BufferVarImages.append(p);
    }
    //newVarCard->setNumImages(n);

    //Este llama luego al PreloadImages y al LaunchImages
    //PreCreateImagesToXaar();
    QString nameFile=ui->m_leFile1->text();//m_leFileColor1->text();
    memoryMap->m_Imagen=QPixmap(nameFile,"BMP");
    GestorImpresion *printEngine=GestorImpresion::request(this);
    printEngine->setVariableBufferSize(n);
    //Meter en variable!
    printEngine->setVariableGenerationLimit(1000);
    printEngine->sltLaunchImpresionVariable();

}

/*Actualiza el contador de swathes*/
void Maquina::UpdateTotalVarXarSentSwathe(){
    //m_TotalSwatheSent+=/*SIZE_BUFFER_IMAGES;*/this->BufferVarImages.count();
}

/* Update variable data */
void Maquina::UpdateVarXarSentSwathe(int last){
   /* m_TotalSwatheSent+=last;
    m_ActualSwathe=m_TotalSwatheSent%BufferVarImages.count();
    if (!updateScreenFlag){
        updateScreenFlag=1;
        QTimer::singleShot(UPDATE_SCREEN_TIMEOUT,this,SLOT(UpdateScreenStreamingMode()));
    }
    static int lastTotal=0;
    static int max=0;
    if (last>=max){
        emit SendDebugInfo(LOGGER_WARNING,QString("Replace Max:%1").arg(last));
        max=last;
    }
    if (m_TotalSwatheSent>=ui->m_leTotalVariableToPrint->text().toInt()){
        if (!VariableTokenIni){
            VariableTokenIni=1;
            QString txt=QString("PARADA POR LLEGAR A %1").arg(ui->m_leTotalVariableToPrint->text().toInt());
            SendDebugInfo(LOGGER_DEBUG,txt);
        }
    }
    //Par que no est? continuamente enviando
    if (lastTotal!=m_TotalSwatheSent){
        ui->m_lastSwatheMemory->setText(QString("%1").arg(last));
        ui->m_leSwatheActual->setText(QString("%1").arg(m_ActualSwathe));
        ui->m_leTotalSwate->setText(QString("%1").arg(m_TotalSwatheSent));
        ui->m_leSwathesBuffered->setText(QString("%1").arg(BufferVarImages.count()-last));
        lastTotal=m_TotalSwatheSent;
    }*/
}

/** Actualiza la pantalla*/
void Maquina::UpdateScreenStreamingMode(){
    struct In2VarImage data=BufferVarImages.at(m_ActualSwathe);
    if (data.m_state!=XAAR_IMAGEN_VACIA) MuestraImagen(QPixmap::fromImage(data.m_Imagen));

}

/** Enviamos las imagenes*/
void Maquina::SendToXaar(){
    //emit KillIn2Msg();
    MemoryMap *memoryMap=MemoryMap::request(0);
    ui->m_teMessage->setText("");
    memoryMap->m_mapa.m_variables.strPlantImages.clear();
    In2GestorFluidica * m_fluid=In2GestorFluidica::request(this);
    if (memoryMap->m_mapa.m_variables.NumColores>m_fluid->numEquipos()){
        QString txt=QString("Numero de colores [%1] de imagen es mayor que numero de fluidicas encendidas [%2]").arg(memoryMap->m_mapa.m_variables.NumColores).arg(m_fluid->numEquipos());
        QMessageBox::warning(this,"Error",txt,QMessageBox::Ok);
        //return;
    }
    //In2Message *msg=new In2Message(this,"Cargando...Por favor espere");
    //connect(this,SIGNAL(KillIn2Msg()),msg,SLOT(sltDelete()));
    ui->m_teMessage->setText(tr("Cargando, por favor espere"));
    GestorImpresion *printEngine=GestorImpresion::request(this);
    m_iTotalXUSB=printEngine->getNumCards();
    //Limpiamos todo lo que hubiera de antes
    printEngine->FullDisablePrintMode();
    //Carga la fuerza de freno de manera automatica
    SetFuerzaFrenoAuto();

    //Enviamos al color 1...si existe
    QString nameFile=ui->m_leFile1->text();//m_leFileColor1->text();
    QByteArray pal;
    QFileInfo info;
    bool bSend=false;


    if (memoryMap->m_mapa.m_variables.iBitsDithering>1){
        int rtn=QMessageBox::question(this,"INFORMACION","La imagen tiene información de dithering...desea habilitar la escala de grises?",QMessageBox::Yes,QMessageBox::Cancel);
        if (rtn==QMessageBox::Yes) memoryMap->m_mapa.m_impresion.bEscalaGrises=true;
        else memoryMap->m_mapa.m_impresion.bEscalaGrises=false;
        sltUpdateTipoImpresion();
    }
    bool bGrayScale=memoryMap->m_mapa.m_impresion.bEscalaGrises;

    emit SendDebugInfo(LOGGER_WARNING,QString("bGrayScale=%1").arg(bGrayScale));

    if ((m_iTotalXUSB>0)&&(!nameFile.isEmpty())){
        printEngine->resetCard(0);
        if (!bGrayScale) pal=getPaleta(0);
        else pal=getGrayScalePaleta(0);
        printEngine->changePalette(0,pal.data());

        QString foo;
        for (int x=0;x<pal.size();x++){
            int z=pal.at(x);
            foo.append(QString("%1-").arg(z));
        }
        foo.chop(1);

        emit SendDebugInfo(LOGGER_STORE,tr("palette 0->%1").arg(foo));
        info=QFileInfo(nameFile);
        if (QFile::exists(nameFile)){
            QImage processedImage(nameFile,"BMP");
            memoryMap->m_mapa.m_variables.strPlantImages.append(nameFile);
            if (memoryMap->m_mapa.m_impresion.bHighSpeed){
                QImage tmpPixmap1=processedImage.scaled(processedImage.width(),processedImage.height()/2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                QImage tmpPixmap=tmpPixmap1.convertToFormat(QImage::Format_Indexed8,table,Qt::AvoidDither);
                QString txt=QString("Fichero (HIGHSPEED) %1 to xUSB 0 OK").arg(info.fileName());
                //emit SendDebugInfo(LOGGER_WARNING,txt);
                sltUpdateMessage(txt);
                //ui->m_teMessage->setText(txt);
                printEngine->sendImageToPrint(0,tmpPixmap);
                bSend=true;
                //tmpPixmap.save("D:\\dummy.bmp");
            }
            else{
            //Mandamos la cuenta a imprimir
                QString txt=QString("Fichero %1 to xUSB 0 OK").arg(info.fileName());
                //emit SendDebugInfo(LOGGER_WARNING,txt);
                sltUpdateMessage(txt);
                printEngine->sendImageToPrint(0,processedImage);
                bSend=true;
            }
        }
    }

    nameFile=ui->m_leFile2->text();
    if ((m_iTotalXUSB>1)&&(!nameFile.isEmpty())){
        printEngine->resetCard(1);
        if (!bGrayScale) pal=getPaleta(1);
        else pal=getGrayScalePaleta(1);
        printEngine->changePalette(1,pal.data());
        QString foo;
        for (int x=0;x<pal.size();x++){
            int z=pal.at(x);
            foo.append(QString("%1-").arg(z));
        }
        foo.chop(1);

        emit SendDebugInfo(LOGGER_STORE,tr("palette 1->%1").arg(foo));
        info=QFileInfo(nameFile);
        if (QFile::exists(nameFile)){
            QImage processedImage(nameFile,"BMP");
            //Mandamos la cuenta a imprimir
            QString txt=QString("Fichero %1 to xUSB 1 OK").arg(info.fileName());
            sltUpdateMessage(txt);
            printEngine->sendImageToPrint(1,processedImage);
            bSend=true;
            memoryMap->m_mapa.m_variables.strPlantImages.append(nameFile);
        }
    }

    nameFile=ui->m_leFile3->text();
    if ((m_iTotalXUSB>2)&&(!nameFile.isEmpty())){
        printEngine->resetCard(2);
        if (!bGrayScale) pal=getPaleta(2);
        else pal=getGrayScalePaleta(2);
        printEngine->changePalette(2,pal.data());
        QString foo;
        for (int x=0;x<pal.size();x++){
            int z=pal.at(x);
            foo.append(QString("%1-").arg(z));
        }
        foo.chop(1);

        emit SendDebugInfo(LOGGER_STORE,tr("palette 2->%1").arg(foo));
        info=QFileInfo(nameFile);
        if (QFile::exists(nameFile)){
            QImage processedImage(nameFile,"BMP");
            //Mandamos la cuenta a imprimir
            QString txt=QString("Fichero %1 to xUSB 2 OK").arg(info.fileName());
            sltUpdateMessage(txt);
            printEngine->sendImageToPrint(2,processedImage);
            bSend=true;
            memoryMap->m_mapa.m_variables.strPlantImages.append(nameFile);
        }
    }

    nameFile=ui->m_leFile4->text();
    if ((m_iTotalXUSB>3)&&(!nameFile.isEmpty())){
        printEngine->resetCard(3);
        if (!bGrayScale) pal=getPaleta(3);
        else pal=getGrayScalePaleta(3);
        printEngine->changePalette(3,pal.data());
        QString foo;
        for (int x=0;x<pal.size();x++){
            int z=pal.at(x);
            foo.append(QString("%1-").arg(z));
        }
        foo.chop(1);

        emit SendDebugInfo(LOGGER_STORE,tr("palette 3->%1").arg(foo));
        info=QFileInfo(nameFile);
        if (QFile::exists(nameFile)){
            QImage processedImage(nameFile,"BMP");
            //Mandamos la cuenta a imprimir
            QString txt=QString("Fichero %1 to xUSB 3 OK").arg(info.fileName());
            sltUpdateMessage(txt);
            printEngine->sendImageToPrint(3,processedImage);
            bSend=true;
            memoryMap->m_mapa.m_variables.strPlantImages.append(nameFile);
        }
    }

    GeneratePD();
    //if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1) GeneratePD();
    //delete (msg);
    ui->m_teMessage->setText("");
    if (bSend){
        memoryMap->m_ImagenEnviada=true;
        memoryMap->m_UltimaAccion=UA_IMAGEN_ENVIADA;
        QMessageBox::warning(0,"INFORMACION","Imagen enviada:\nPulsa el botón de inicio",QMessageBox::Ok);
        //sltUpdateImagenVision();
        /*if (m_memoryMap->m_UltimaAccion==UA_IMAGEN_ENVIADA) Insertar_Evento(ENVIA_COMPOSICION,QString("%1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaBmpEnviado));
        else if (m_memoryMap->m_UltimaAccion==UA_CONFIGURACION_CARGADA) Insertar_Evento(ENVIA_COMPOSICION,QString("%1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));*/
    }else{
        QMessageBox::warning(0,"ATENCION","Error al enviar imagen",QMessageBox::Ok);
    }

}


/** Ponemos el numero de impresiones...para todos los xusb igual*/
void Maquina::SetPrintCount(){
/*    DWORD SwatheCount[MAXHEADS * MAXROWS];
    if (m_ActualHeight){
        SendDebugInfo(LOGGER_WARNING,QString("%1 %2 %3 %4 %5").arg(m_ActualHeight).arg(m_memoryMap->m_mapa.m_maquina.numImpresiones).arg(m_memoryMap->m_mapa.m_maquina.impresionesPrevias).arg(m_memoryMap->m_mapa.m_maquina.impresionesAcumuladas).arg(m_memoryMap->m_mapa.m_maquina.metros_trabajo));
        //AskPrintCount(); //As? tengo datos actualizados
        DWORD impresiones=((m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo)*1000*FACTOR_PIXELS_MM)/m_ActualHeight;
        DWORD impresiones_done=((m_memoryMap->m_mapa.m_maquina.metros_trabajo)*1000*FACTOR_PIXELS_MM)/m_ActualHeight;
        //int remainMeters=m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo-m_memoryMap->m_mapa.m_maquina.metros_trabajo;
        //SendDebugInfo(LOGGER_WARNING,QString("done %1").arg(impresiones_done));
        m_memoryMap->m_mapa.m_maquina.ref_numImpresiones=((m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo)*1000*FACTOR_PIXELS_MM)/m_ActualHeight;

        if (m_memoryMap->m_mapa.m_maquina.metros_trabajo>=m_memoryMap->m_mapa.m_maquina.ref_metros_trabajo){
          //  SendDebugInfo(LOGGER_WARNING,QString("aqui1"));
            m_memoryMap->m_mapa.m_maquina.numImpresiones=0;
            m_memoryMap->m_mapa.m_maquina.impresionesPrevias=impresiones;
            m_memoryMap->m_mapa.m_maquina.impresionesAcumuladas=impresiones;
        }
        else{
            if (m_memoryMap->m_mapa.m_maquina.numImpresiones!=0){ //Si vengo de una impresion no se tocan los acumulados, sino los reseteo
            //    SendDebugInfo(LOGGER_WARNING,QString("aqui2"));
                m_memoryMap->m_mapa.m_maquina.impresionesPrevias+=m_memoryMap->m_mapa.m_maquina.numImpresiones;
            }
            else{
              //  SendDebugInfo(LOGGER_WARNING,QString("aqui3"));
                m_memoryMap->m_mapa.m_maquina.impresionesPrevias=impresiones_done;
                m_memoryMap->m_mapa.m_maquina.impresionesAcumuladas=impresiones_done;
            }
            m_memoryMap->m_mapa.m_maquina.numImpresiones=0;
        }
        DWORD impresiones_to_do=impresiones-m_memoryMap->m_mapa.m_maquina.impresionesPrevias;
        if (impresiones_to_do!=0) impresiones_to_do+=PRINT_GUARD; //Se pone una guarda de 10 impresiones por si acaso
        for (unsigned int i=0;i<MAXHEADS * MAXROWS;i++){
            SwatheCount[i]=impresiones_to_do;
        }
        for (int i=0;i<m_iTotalXUSB;i++)
            bXaarScorpionWriteSwathePrintCount(i,SwatheCount);
        //SendDebugInfo(LOGGER_WARNING,QString("SetPrintCount: %1 (done %2)").arg(impresiones).arg(impresiones_done));
        ui->m_RefImpresiones->setText(QString("%1\n(%2)").arg(impresiones).arg(impresiones-m_memoryMap->m_mapa.m_maquina.impresionesPrevias));

        //int valor_int=((Principal*)dummy)->miCliente->LeerDato(ITEM_METROS_ACT).toInt();
        ui->m_MetrosTrabajoAct->setText(QString("%1\n(%2)").arg(m_memoryMap->m_mapa.m_maquina.metros_trabajo).arg(((m_memoryMap->m_mapa.m_maquina.impresionesAcumuladas)*m_ActualHeight/(1000*FACTOR_PIXELS_MM))));
        ui->m_ActImpresiones->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.impresionesAcumuladas));
        //SendDebugInfo(LOGGER_WARNING,QString("%1 %2 %3 %4 %5").arg(m_ActualHeight).arg(m_memoryMap->m_mapa.m_maquina.numImpresiones).arg(m_memoryMap->m_mapa.m_maquina.impresionesPrevias).arg(m_memoryMap->m_mapa.m_maquina.impresionesAcumuladas).arg(m_memoryMap->m_mapa.m_maquina.metros_trabajo));
    }*/
}

/** Preguntamos al xaar por donde va*/
int Maquina::AskXaar(){
    int dummy=0;
   // dummy=(int)newVarCard->GetCurrentPrintSwathe();
   // emit CurrentPrintSwathe(dummy);
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
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->m_FilePath.isEmpty()){
        //QString prevImage=ImagenXaar;


        m_bCodigoCorrecto=false;
        m_bLoadFromCola=true;
        memoryMap->m_FilePath.replace("/","\\");
        ImagenXaar=memoryMap->m_FilePath;
        m_nombrefichero=memoryMap->m_FilePath;
        ui->m_gbCentral->setTitle(m_nombrefichero);

        QImage dummyImage(ImagenXaar);
        if (dummyImage.isNull()){
            QMessageBox::warning(this,"INFO",tr("No se puede generar imagen %1").arg(ImagenXaar),QMessageBox::Ok);

            return;
        }
        ui->m_pbEndSGL->setEnabled(true);
        ui->SendIni->setEnabled(true);
        m_ActualHeight=dummyImage.height();
        //Carga la vision
        sltLoadVisionProgram(memoryMap->m_mapa.m_variables.NombreRutaIni);
        QTimer::singleShot(0,this,SLOT(_sltCheckVision()));
        //MuestraImagen(m_memoryMap->m_Imagen);
        //if (!m_memoryMap->ImagenVariable)
         memoryMap->m_Imagen=QPixmap();
        //MuestraImagenFromFile();

        //Imagen al color 1
        QString dummyString=ImagenXaar;
        if (QFile::exists(dummyString)) ui->m_leFile1->setText(dummyString);//m_leFileColor1->setText(dummyString);
        else ui->m_leFile1->setText("");//m_leFileColor1->setText("");
        MuestraImagenColor1();

        //Imagen al color 2
        dummyString=ImagenXaar;
        dummyString.remove(QString("_C1_comp.bmp"));
        dummyString.append(QString("_C2_comp.bmp"));
        if (QFile::exists(dummyString))ui->m_leFile2->setText(dummyString);
        else ui->m_leFile2->setText("");
        MuestraImagenColor2();

        //Imagen al color 3
        dummyString=ImagenXaar;
        dummyString.remove(QString("_C1_comp.bmp"));
        dummyString.append(QString("_C3_comp.bmp"));
        if (QFile::exists(dummyString))ui->m_leFile3->setText(dummyString);
        else ui->m_leFile3->setText("");

        //Imagen al color 4
        dummyString=ImagenXaar;
        dummyString.remove(QString("_C1_comp.bmp"));
        dummyString.append(QString("_C4_comp.bmp"));
        if (QFile::exists(dummyString))ui->m_leFile4->setText(dummyString);
        else ui->m_leFile4->setText("");



        QTimer::singleShot(0,this,SLOT(MuestraImagenCentral()));

        (memoryMap->m_mapa.m_campos[0].OnOffCampo)? ui->m_leCampo1->setEnabled(true):ui->m_leCampo1->setEnabled(false);
        (memoryMap->m_mapa.m_campos[1].OnOffCampo)? ui->m_leCampo2->setEnabled(true):ui->m_leCampo2->setEnabled(false);
        (memoryMap->m_mapa.m_campos[2].OnOffCampo)? ui->m_leCampo3->setEnabled(true):ui->m_leCampo3->setEnabled(false);
        (memoryMap->m_mapa.m_campos[3].OnOffCampo)? ui->m_leCampo4->setEnabled(true):ui->m_leCampo4->setEnabled(false);

        sltSetRefMaculas();

        //Carga la velocidad y la paletta si hay
        AceptarVelocidad(QString("%1").arg(memoryMap->m_mapa.m_impresion.iVelocidad));
        QStringList drops=memoryMap->m_mapa.m_impresion.qstrPal1.split("@");
        if (drops.count()==8){
            for (int y=0;y<NUM_ROWS_PALETA;y++){
                QTableWidgetItem *p=ui->m_TablaPaletas->item(y,0);
                if (p){
                    Qt::ItemFlags Flags=p->flags();
                    Flags|=(Qt::ItemIsEditable)|(Qt::ItemIsEnabled);
                    p->setFlags(Flags);
                    p->setText(QString("%1").arg(drops.at(y)));
                }
            }
        sltChangeParameter(ROW_GOTA1,COL_REF);
        }


    }
    else{

        qDebug() << "No hay imagen en memoria";
    }
    //Si hay imagen variable rellenamos algun campo
    if (memoryMap->ImagenVariable){
        float sup=(float)memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.limiteSuperior;
        float inf=(float)memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.valorInicialFila;
        int incr=memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.incremento;
        if ((incr)&&(memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas)){
            int num=qCeil((sup-inf)/incr);
            num=qCeil((float)num/(float)memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas);
            ///ui->m_leTotalVariableToPrint->setText(QString("%1").arg(num));
        }

    }

}
/** Carga las referencias de macula*/
void Maquina::sltSetRefMaculas(){
    //if (m_memoryMap->m_mapa.m_impresion.m_fltrMacula.bMonitorMacula){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_machinedefaults.filtroMaculas){
        MemoryMap *memoryMap=MemoryMap::request(0);
        double Distancia=memoryMap->m_mapa.m_impresion.m_fltrMacula.fDistancia;
        double height=memoryMap->m_mapa.m_variables.WebHeight;
        int iMaculaCounter=1;
        if (height){
            iMaculaCounter=Distancia/height;
            double newDistancia=height*(iMaculaCounter);
            memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax=newDistancia+memoryMap->m_mapa.m_impresion.m_fltrMacula.fError;
            memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin=newDistancia-memoryMap->m_mapa.m_impresion.m_fltrMacula.fError;
            sltSetRefMaculasLimits();
            emit SendDebugInfo(LOGGER_STORE,tr("MaculaCounter=%1;Height=%2;Max=%3;Min=%4").arg(iMaculaCounter).arg(height).arg(memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax).arg(memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin));
            ClienteOPC *opc=ClienteOPC::request(0);
            opc->EscribirDato(ITEM_NUMERO_MACULAS,QVariant(iMaculaCounter));
        }
    }
}

void Maquina::sltSetRefMaculasLimits(){
    //Actualiza referencias de macula
    MemoryMap *memoryMap=MemoryMap::request(0);
    QTableWidgetItem *p=ui->m_twData->item(ROW_MACULAMAX,COL_REF);
    if (p) p->setText(QString("%1").arg(memoryMap->m_mapa.m_impresion.m_fltrMacula.fMax));
    p=ui->m_twData->item(ROW_MACULAMIN,COL_REF);
    if (p) p->setText(QString("%1").arg(memoryMap->m_mapa.m_impresion.m_fltrMacula.fMin));
}

void Maquina::GeneratePD(){
    for (int Card=0;Card<m_iTotalXUSB;Card++){
        //bXaarScorpionSetPDInternal(Card,1);
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
    QPixmap lienzo=Imagen;
    QPainter painter(&lienzo);

    QPen pen(Qt::blue, 8, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setCosmetic(true);
    painter.setPen(pen);


    painter.drawRect(Imagen.rect());
    painter.end();

    MemoryMap *memoryMap=MemoryMap::request(0);
    In2GraphicsScene *scene=m_Scene.at(0);
    In2GraphicsView *view=m_View.at(0);
    if (scene==NULL){
        scene = new In2GraphicsScene(0, 0,memoryMap->m_Imagen.width(), memoryMap->m_Imagen.height());
        In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
        p->setPixmap(lienzo);
        scene->addItem(p);
        view=new In2GraphicsView(ui->m_ImagenCentral);
        ui->m_ImagenCentral->setScene(scene);
        ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
        ui->m_ImagenCentral->centerOn(p);
        ui->m_ImagenCentral->show();
        m_Scene.replace(0,scene);
        m_View.replace(0,view);
    }
    else{
        QList<QGraphicsItem*> items=scene->items();
        In2GraphicsPixmapItem *p;
        if (items.count()){
            p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
            p->setPixmap(lienzo);
        }
        else{
            p=new In2GraphicsPixmapItem(0,0,false);
            p->setPixmap(lienzo);
            scene->addItem(p);
        }
        scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
        ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
        //ui->m_ImagenCentral->centerOn(p);
        //ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
    }
}



//Muestra la imagen central
void Maquina::MuestraImagenFromFile(){
    //Primero la imagen central...hago una compo
    In2ImageCreator imgCreator;
    QPixmap lienzo=imgCreator.returnCompoImage(m_nombrefichero);
    QPainter painter(&lienzo);
    QPen pen(Qt::blue, 8, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(lienzo.rect());
    painter.end();
    In2GraphicsScene *scene=m_Scene.at(0);
    In2GraphicsView *view=m_View.at(0);
    if (scene==NULL){
        scene = new In2GraphicsScene(0, 0,lienzo.width(), lienzo.height());
        In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
        p->setPixmap(lienzo);
        p->setTransformationMode(Qt::SmoothTransformation);
        scene->addItem(p);
        view=new In2GraphicsView(ui->m_ImagenCentral);
        ui->m_ImagenCentral->setScene(scene);
        ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
        ui->m_ImagenCentral->centerOn(p);
        ui->m_ImagenCentral->show();
        m_Scene.replace(0,scene);
        m_View.replace(0,view);



    }
    else{
        QList<QGraphicsItem*> items=scene->items();
        In2GraphicsPixmapItem *p;
        if (items.count()){
            p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
            p->setPixmap(lienzo);
        }
        else{
            p=new In2GraphicsPixmapItem(0,0,false);
            p->setPixmap(lienzo);
            scene->addItem(p);
        }
        scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
        ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
        ui->m_ImagenCentral->centerOn(p);
    }
    MuestraImagenColor1();
    MuestraImagenColor2();

    ui->m_TabCentral->setCurrentIndex(0);
}

/** Muestra Imagen central*/
void Maquina::MuestraImagenCentral(){
    QString name1=ui->m_leFile1->text();//m_leFileColor1->text();
    QString name2=ui->m_leFile2->text();

    In2ImageCreator imgCreator;
    QPixmap lienzo=imgCreator.returnCompoImage(name1,name2);
    if (!lienzo.isNull()){
        m_TrabajoNuevo=true;
        ui->SendIni->setEnabled(true);
        QPainter painter(&lienzo);
        QPen pen(Qt::blue, 8, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
        pen.setCosmetic(true);
        painter.setPen(pen);
        painter.drawRect(lienzo.rect());
        painter.end();
        In2GraphicsScene *scene=m_Scene.at(0);
        In2GraphicsView *view=m_View.at(0);
        if (scene==NULL){
            scene = new In2GraphicsScene(0, 0,lienzo.width(), lienzo.height());
            In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
            p->setPixmap(lienzo);
            p->setTransformationMode(Qt::SmoothTransformation);
            scene->addItem(p);
            view=new In2GraphicsView(ui->m_ImagenCentral);
            ui->m_ImagenCentral->setScene(scene);
            ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
            ui->m_ImagenCentral->centerOn(p);
            ui->m_ImagenCentral->show();
            m_Scene.replace(0,scene);
            m_View.replace(0,view);
        }
        else{
            QList<QGraphicsItem*> items=scene->items();
            In2GraphicsPixmapItem *p;
            if (items.count()){
                p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                p->setPixmap(lienzo);
            }
            else{
                p=new In2GraphicsPixmapItem(0,0,false);
                p->setPixmap(lienzo);
                scene->addItem(p);
            }
            scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
            ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
            ui->m_ImagenCentral->centerOn(p);
        }
        ui->m_TabCentral->setCurrentIndex(0);
    }
    else {
        ui->SendIni->setEnabled(false);
        QMessageBox::warning(this,"ERROR","No se pudo mostrar la imagen",QMessageBox::Ok);
    }
}

/** Muestra Imagen del color 1*/
void Maquina::MuestraImagenColor1(){
    QString dummyString=ui->m_leFile1->text();//m_leFileColor1->text();
    if (!dummyString.isEmpty()){
        //Luego en negro
        In2GraphicsScene *scene=m_Scene.at(1);
        In2GraphicsView *view=m_View.at(1);
        QPixmap lienzo=QPixmap(dummyString);
        QPainter painter(&lienzo);
        QPen pen(Qt::blue, 8, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
        pen.setCosmetic(true);
        painter.setPen(pen);
        painter.drawRect(lienzo.rect());
        painter.end();

        if (!lienzo.isNull()){
            QPainter painter(&lienzo);
            painter.drawRect(lienzo.rect());
            painter.end();
            QGraphicsView *uiView=NULL;
            uiView=ui->m_ImagenNegro;
            ui->m_TabCentral->setCurrentIndex(1);
            if (scene==NULL){
                scene = new In2GraphicsScene(0, 0,lienzo.width(), lienzo.height());
                In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
                p->setPixmap(lienzo);
                scene->addItem(p);
                view=new In2GraphicsView(uiView);
                uiView->setScene(scene);
                uiView->fitInView(p,Qt::KeepAspectRatio);
                uiView->centerOn(p);
                uiView->show();
                m_Scene.replace(1,scene);
                m_View.replace(1,view);

            }
            else{
                QList<QGraphicsItem*> items=scene->items();
                In2GraphicsPixmapItem *p;
                if (items.count()){
                    p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                    p->setPixmap(lienzo);
                }
                else{
                    p=new In2GraphicsPixmapItem(0,0,false);
                    p->setPixmap(lienzo);
                    scene->addItem(p);
                }
                scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
                uiView->fitInView(p,Qt::KeepAspectRatio);
                uiView->centerOn(p);
            }
        }
        else{
            if (scene) scene->clear();
        }
    }
    ui->m_TabCentral->setCurrentIndex(0);
}

/** Muestra Imagen del color 2*/
void Maquina::MuestraImagenColor2(){
    bool showColor2=true;
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->getClient().compare(CLIENT_ESTEVE)) showColor2=false;
    if (showColor2){
     QString dummyString=ui->m_leFile2->text();
        if (!dummyString.isEmpty()){
            //Luego en negro
            In2GraphicsScene* scene=m_Scene.at(2);
            In2GraphicsView* view=m_View.at(2);
            QPixmap lienzo=QPixmap(dummyString);
            QPainter painter(&lienzo);
            QPen pen(Qt::blue, 8, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
            pen.setCosmetic(true);
            painter.setPen(pen);
            painter.drawRect(lienzo.rect());
            painter.end();

            if (!lienzo.isNull()){
                QPainter painter(&lienzo);
                painter.drawRect(lienzo.rect());
                painter.end();
                QGraphicsView *uiView=NULL;
                uiView=ui->m_ImagenRojo;
                ui->m_TabCentral->setCurrentIndex(2);
                if (scene==NULL){
                    scene = new In2GraphicsScene(0, 0,lienzo.width(), lienzo.height());
                    In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
                    p->setPixmap(lienzo);
                    scene->addItem(p);
                    view=new In2GraphicsView(uiView);
                    uiView->setScene(scene);
                    uiView->fitInView(p,Qt::KeepAspectRatio);
                    uiView->centerOn(p);
                    uiView->show();
                    m_Scene.replace(2,scene);
                    m_View.replace(2,view);

                }
                else{
                    QList<QGraphicsItem*> items=scene->items();
                    In2GraphicsPixmapItem *p;
                    if (items.count()){
                        p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                        p->setPixmap(lienzo);
                    }
                    else{
                        p=new In2GraphicsPixmapItem(0,0,false);
                        p->setPixmap(lienzo);
                        scene->addItem(p);
                    }
                    scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
                    uiView->fitInView(p,Qt::KeepAspectRatio);
                    uiView->centerOn(p);
                }
            }
            else{
                if (scene) scene->clear();
            }
        }
        ui->m_TabCentral->setCurrentIndex(0);
    }
}




 /** Solo para simulacion :-S*/
void Maquina::sltSimulateMaculas(){
}
/** Paramos por softstop la impresion variable*/
void Maquina::sltVariableDataSoftStop(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->m_mapa.m_maquina.softstopear=true;
    //Insertar_Evento(ERROR_GUI,QString("Parada desde interfaz"));
    emit PauseGenerating();
    ui->m_AceptarButton->setEnabled(true);
}

/** Paramos por emergencia la impresion variable*/
void Maquina::sltVariableDataEmergencyStop(){
    for (int Card=0;Card<m_iTotalXUSB;Card++)
        bXaarScorpionEnablePrintMode(Card,0xFFFF,false);

    ChangeStyle(THEME_EMERGENCY);
    emit PauseGenerating();
    QMessageBox::warning(this,"ERROR","Ejecutada parada de emergencia!",QMessageBox::Ok);
    ui->m_AceptarButton->setEnabled(true);

}

/** Vamos mostrando imagen de vision*/
void Maquina::sltUpdateImagenVision(){
}

/** Actualiza la foto de pantalla*/
void Maquina::sltShowFoto(const QPixmap & dib){
}

