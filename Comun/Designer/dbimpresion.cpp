/** @addtogroup DbImpresion
 *  @{*/
/** @file DbImpresion.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbImpresion.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#include "dbimpresion.h"
#include "dbformat.h"
#include "dbformatpage.h"
#include "dbformatplantilla.h"
#include "globals.h"
#include "InputData.h"
#include "MemoryMap.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include "principal.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <assert.h>
#include <QDateTimeEdit>
#include <QSessionManager>
#include <QApplication>
#include <Windows.h>
#include <sql.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <qsqlresult.h> 
#include <QSqlResult>
#include <QTimer>
#include <QHash>
#include <omp.h>
#include <QSettings>
#include <QVariant>
#include <QPalette>
#include <QDebug>
#include <QBrush>
#include "imgfixgenerator.h"
#include "imgvargenerator.h"
#include "InkjetXusbCtrl.h"
#include "GestorDB_ODBC.h"
#include "GestorDB_SQLITE.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <QProcess>
#include "in2imagecreator.h"
#include "In2GraphicsView.h"

#define DEFAULT_PIXEL_NUMBER 2000

#define DEFAULT_WIDTH_HEAD 71

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
#include <QTranslator>

#include "In2BarcodeClass.h"
#include "viewini.h"


DbImpresion::DbImpresion(QWidget *parent)
    : /*InkjetForm(parent)*/ ui(new Ui::ComposicionUI)
{

    ui->setupUi(this);

    //ui->retranslateUi(this);
    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    GlobalSetScreenSize(this);
    /*ui->tabWidget->setMaximumHeight(ui->tabWidget->height());
    ui->tabWidget->setMaximumWidth(ui->tabWidget->width());*/
    ui->m_OrdenCampoVar1->setCurrentIndex(1);
    ui->m_OrdenCampoVar2->setCurrentIndex(2);
    ui->m_OrdenCampoVar3->setCurrentIndex(3);
    ui->m_OrdenCampoVar4->setCurrentIndex(4);
    ui->m_OrdenCampoVar5->setCurrentIndex(5);
    ui->m_OrdenCampoVar6->setCurrentIndex(6);

    ui->m_ValorCampo1->setPalette(dummyPaletteEnabled);
    ui->m_ValorCampo2->setPalette(dummyPaletteEnabled);
    ui->m_ValorCampo3->setPalette(dummyPaletteEnabled);
    ui->m_ValorCampo4->setPalette(dummyPaletteEnabled);


    ui->m_OrdenCampoVar1->addItem(QString("Fijo"));
    ui->m_OrdenCampoVar2->addItem(QString("Fijo"));
    ui->m_OrdenCampoVar3->addItem(QString("Fijo"));
    ui->m_OrdenCampoVar4->addItem(QString("Fijo"));
    ui->m_OrdenCampoVar5->addItem(QString("Fijo"));
    ui->m_OrdenCampoVar6->addItem(QString("Fijo"));
    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        ui->m_OrdenCampoVar1->addItem(QString("%1").arg(i+1));
        ui->m_OrdenCampoVar2->addItem(QString("%1").arg(i+1));
        ui->m_OrdenCampoVar3->addItem(QString("%1").arg(i+1));
        ui->m_OrdenCampoVar4->addItem(QString("%1").arg(i+1));
        ui->m_OrdenCampoVar5->addItem(QString("%1").arg(i+1));
        ui->m_OrdenCampoVar6->addItem(QString("%1").arg(i+1));

    }

    ui->m_OrigenCampoVar1->addItem(TXT_ORIGEN_FIJO);
    ui->m_OrigenCampoVar2->addItem(TXT_ORIGEN_FIJO);
    ui->m_OrigenCampoVar3->addItem(TXT_ORIGEN_FIJO);
    ui->m_OrigenCampoVar4->addItem(TXT_ORIGEN_FIJO);
    ui->m_OrigenCampoVar5->addItem(TXT_ORIGEN_FIJO);
    ui->m_OrigenCampoVar6->addItem(TXT_ORIGEN_FIJO);

    ui->m_OrigenCampoVar1->addItem(TXT_ORIGEN_DB);
    ui->m_OrigenCampoVar2->addItem(TXT_ORIGEN_DB);
    ui->m_OrigenCampoVar3->addItem(TXT_ORIGEN_DB);
    ui->m_OrigenCampoVar4->addItem(TXT_ORIGEN_DB);
    ui->m_OrigenCampoVar5->addItem(TXT_ORIGEN_DB);
    ui->m_OrigenCampoVar6->addItem(TXT_ORIGEN_DB);

    ui->m_OrigenCampoVar1->addItem(TXT_ORIGEN_CONTADOR);
    ui->m_OrigenCampoVar2->addItem(TXT_ORIGEN_CONTADOR);
    ui->m_OrigenCampoVar3->addItem(TXT_ORIGEN_CONTADOR);
    ui->m_OrigenCampoVar4->addItem(TXT_ORIGEN_CONTADOR);
    ui->m_OrigenCampoVar5->addItem(TXT_ORIGEN_CONTADOR);
    ui->m_OrigenCampoVar6->addItem(TXT_ORIGEN_CONTADOR);

    ui->m_OrigenCampoVar1->addItem(TXT_ORIGEN_FECHA);
    ui->m_OrigenCampoVar2->addItem(TXT_ORIGEN_FECHA);
    ui->m_OrigenCampoVar3->addItem(TXT_ORIGEN_FECHA);
    ui->m_OrigenCampoVar4->addItem(TXT_ORIGEN_FECHA);
    ui->m_OrigenCampoVar5->addItem(TXT_ORIGEN_FECHA);
    ui->m_OrigenCampoVar6->addItem(TXT_ORIGEN_FECHA);


    ui->m_OrigenCampoVar1->addItem(TXT_ORIGEN_RANDOM);
    ui->m_OrigenCampoVar2->addItem(TXT_ORIGEN_RANDOM);
    ui->m_OrigenCampoVar3->addItem(TXT_ORIGEN_RANDOM);
    ui->m_OrigenCampoVar4->addItem(TXT_ORIGEN_RANDOM);
    ui->m_OrigenCampoVar5->addItem(TXT_ORIGEN_RANDOM);
    ui->m_OrigenCampoVar6->addItem(TXT_ORIGEN_RANDOM);

    cambiarSemillaNumeroAleatorio();

    ui->m_OrigenCampoVar1->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo);
    ui->m_OrigenCampoVar2->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo);
    ui->m_OrigenCampoVar3->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo);
    ui->m_OrigenCampoVar4->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo);
    ui->m_OrigenCampoVar5->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo);
    ui->m_OrigenCampoVar6->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo);

    //ui->tab->show();

    this->setDataCommon();

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    //connect(ui->m_CancelButton,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
    connect(ui->m_PrintButton,SIGNAL(clicked()),this,SLOT(ComprobacionesGenerateImage()));
    connect(ui->m_FormatButton,SIGNAL(clicked()),this,SLOT(FormatImagen()));
    connect(ui->m_FormatPageButton,SIGNAL(clicked()),this,SLOT(FormatPagina()));
    connect(ui->m_FormatPlantillaButton,SIGNAL(clicked()),this,SLOT(FormatPlantilla()));

    connect(ui->m_CargarFicheroIni,SIGNAL(clicked()),this,SLOT(CargarComposicion()));
    connect(ui->m_GuardarFicheroIni,SIGNAL(clicked()),this,SLOT(GuardarComposicion()));
    connect(ui->m_GuardarFicheroIniNow,SIGNAL(clicked()),this,SLOT(GuardarComposicionNow()));
    connect(ui->m_NuevoFicheroIni,SIGNAL(clicked()),this,SLOT(NuevaComposicion()));
    connect(ui->m_ViewIni,SIGNAL(clicked()),this,SLOT(visualizarIni()));


    connect(ui->m_CargarDb,SIGNAL(clicked()),this,SLOT(AbrirBBDD()));

    connect(ui->m_OrdenFabricacion,SIGNAL(selectionChanged()),this,SLOT(UpdateOrdenFabricacion()));
    connect(ui->m_NombreCampo1,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampo1()));
    connect(ui->m_NombreCampo2,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampo2()));
    connect(ui->m_NombreCampo3,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampo3()));
    connect(ui->m_NombreCampo4,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampo4()));
    connect(ui->m_ValorCampo1,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampo1()));
    connect(ui->m_ValorCampo2,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampo2()));
    connect(ui->m_ValorCampo3,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampo3()));
    connect(ui->m_ValorCampo4,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampo4()));

    connect(ui->m_NombreDb, SIGNAL(selectionChanged()),this, SLOT(UpdateTablaImprimir()));
    connect(ui->m_NombreCampoVar1,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampoVar1()));
    connect(ui->m_NombreCampoVar2,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampoVar2()));
    connect(ui->m_NombreCampoVar3,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampoVar3()));
    connect(ui->m_NombreCampoVar4,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampoVar4()));
    connect(ui->m_NombreCampoVar4,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampoVar5()));
    connect(ui->m_NombreCampoVar4,SIGNAL(selectionChanged()),this,SLOT(UpdateNombreCampoVar6()));
    connect(ui->m_ValorCampoVar1,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampoVar1()));
    connect(ui->m_ValorCampoVar2,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampoVar2()));
    connect(ui->m_ValorCampoVar3,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampoVar3()));
    connect(ui->m_ValorCampoVar4,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampoVar4()));
    connect(ui->m_ValorCampoVar3,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampoVar5()));
    connect(ui->m_ValorCampoVar4,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampoVar6()));

    connect(ui->m_OnOffCampo1,SIGNAL(clicked()),this,SLOT(UpdateOnOff1()));
    connect(ui->m_OnOffCampo2,SIGNAL(clicked()),this,SLOT(UpdateOnOff2()));
    connect(ui->m_OnOffCampo3,SIGNAL(clicked()),this,SLOT(UpdateOnOff3()));
    connect(ui->m_OnOffCampo4,SIGNAL(clicked()),this,SLOT(UpdateOnOff4()));

    connect(ui->m_OnOffCampoVar1,SIGNAL(clicked()),this,SLOT(UpdateOnOffVar1()));
    connect(ui->m_OnOffCampoVar2,SIGNAL(clicked()),this,SLOT(UpdateOnOffVar2()));
    connect(ui->m_OnOffCampoVar3,SIGNAL(clicked()),this,SLOT(UpdateOnOffVar3()));
    connect(ui->m_OnOffCampoVar4,SIGNAL(clicked()),this,SLOT(UpdateOnOffVar4()));
    connect(ui->m_OnOffCampoVar5,SIGNAL(clicked()),this,SLOT(UpdateOnOffVar5()));
    connect(ui->m_OnOffCampoVar6,SIGNAL(clicked()),this,SLOT(UpdateOnOffVar6()));
    connect(ui->m_Previsualizar,SIGNAL(clicked()),this,SLOT(sltPrevisualizar()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(sltChangeTab(int)));

   if (!memoryMap->m_mapa.m_variables.NombreRutaIni.isEmpty()){
        CargaDatosCampos();
        CargaDatosCamposVariables();
        CargaBBDD();
        sltPrevisualizarZoom();
    }
    UpdateNumCabezales();
	TestRoot();
    ui->tabWidget->setCurrentIndex(0);
}
/** Cambio de pestaña*/
void DbImpresion::sltChangeTab(int index){
    if (index==3){
        In2ImageCreator p;
        p.sltGenerateImage();
        QLabel *imageLabel=new QLabel();
        imageLabel->setPixmap(memoryMap->m_Imagen);
        ui->m_scrollArea->setWidget(imageLabel);
    }
}

/** Previsualiza la composicion creada*/
void DbImpresion::sltPrevisualizar(){
    GuardaDatosCampos();
    GuardaDatosCamposVariables();

    GeneratePrev();
    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        QString nombreruta_bmp=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);
        nombreruta_bmp.remove(".ini");
        nombreruta_bmp.append(QString("_OF%1").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));
        nombreruta_bmp.append(QString("_C%1_comp.bmp").arg(i+1));
        QPixmap prevPixmap(nombreruta_bmp);

        if (prevPixmap.width()){
            unsigned char escalado;
            QLabel *prevImage= new QLabel(this);

            Qt::WindowFlags flags=Qt::Window;
            prevImage->setWindowFlags(flags);

            prevImage->setWindowTitle(nombreruta_bmp);
            prevImage->setGeometry(50,50,820,614);

            int lw=prevImage->width();
            int lh=prevImage->height();
            int iw=prevPixmap.width();
            int ih=prevPixmap.height();

            if ((lw==0)||(lh==0)) return;

            escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

            QPixmap pic(prevImage->size());
            QPainter p(&pic);   // create a QPainter for it
            pic.fill(Qt::gray);
            QPixmap dummy;
        int offset_x_plantilla=0;
        int offset_y_plantilla=0;
        switch (escalado){
        case IMAGEN_SINESCALAR:
            dummy=prevPixmap;
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));

            break;
        case IMAGEN_S2W:
            dummy=prevPixmap.scaledToWidth(prevImage->width(),Qt::SmoothTransformation);
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2H:
            dummy=prevPixmap.scaledToHeight(prevImage->height(),Qt::SmoothTransformation);
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));
            break;
        }

        p.end();

        prevImage->show();

       }
    }

}

/** Previsualiza la composicion creada*/
void DbImpresion::sltPrevisualizarZoom(){
    GuardaDatosCampos();
    GuardaDatosCamposVariables();

    unsigned char escalado;
    In2ImageCreator imgCreator;
    imgCreator.sltGenerateImage();
    QPixmap prevPixmap=memoryMap->m_Imagen;

/*  QGraphicsScene* scene = new QGraphicsScene(QRect(0, 0, memoryMap->m_Imagen.width(), memoryMap->m_Imagen.height()));
    scene->addPixmap(prevPixmap);
    ui->m_graphicsView->setScene(scene);
    ui->m_graphicsView->show();
    In2GraphicsView *p=new In2GraphicsView(ui->m_graphicsView);*/
    QLabel *prevImage=this->ui->m_zoomImage;
    int lw=prevImage->width();
    int lh=prevImage->height();
    int iw=prevPixmap.width();
    int ih=prevPixmap.height();

    if ((lw==0)||(lh==0)) return;

    escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

    QPixmap pic(prevImage->size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);
    QPixmap dummy;
    int offset_x_plantilla=0;
    int offset_y_plantilla=0;
        switch (escalado){
        case IMAGEN_SINESCALAR:
            dummy=prevPixmap;
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));

            break;
        case IMAGEN_S2W:
            dummy=prevPixmap.scaledToWidth(prevImage->width(),Qt::SmoothTransformation);
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2H:
            dummy=prevPixmap.scaledToHeight(prevImage->height(),Qt::SmoothTransformation);
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));
            break;
        }

        p.end();

        prevImage->show();


}


void DbImpresion::TestRoot(){

    //QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    //QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();

    if (memoryMap->m_ConfiguracionCargada==false){
        ui->m_FormatPlantillaButton->setEnabled(false);
        ui->m_FormatButton->setEnabled(false);
        ui->m_FormatPageButton->setEnabled(false);
        ui->m_PrintButton->setEnabled(false);
        ui->m_OrdenFabricacion->setEnabled(false);

        DisableWidgetsCampo1();
        DisableWidgetsCampo2();
        DisableWidgetsCampo3();
        DisableWidgetsCampo4();

        DisableWidgetsCampoVar1();
        DisableWidgetsCampoVar2();
        DisableWidgetsCampoVar3();
        DisableWidgetsCampoVar4();
        DisableWidgetsCampoVar5();
        DisableWidgetsCampoVar6();

        ui->m_GuardarFicheroIni->setEnabled(false);
        ui->m_GuardarFicheroIniNow->setEnabled(false);
        ui->m_ViewIni->setEnabled(false);
        ui->m_FormatButton->setEnabled(false);
        ui->m_FormatPageButton->setEnabled(false);
        ui->m_FormatPlantillaButton->setEnabled(false);
	}
	else{ //(memoryMap->m_ConfiguracionCargada==true)
        if (memoryMap->getUserRights()>=ACCESS_PRINTER){
            ui->m_FormatPlantillaButton->setEnabled(true);
            ui->m_FormatButton->setEnabled(true);
            ui->m_FormatPageButton->setEnabled(true);
            ui->m_PrintButton->setEnabled(true);
            ui->m_OrdenFabricacion->setEnabled(true);

            EnableWidgetsCampo1();
            EnableWidgetsCampo2();
            EnableWidgetsCampo3();
            EnableWidgetsCampo4();

            EnableWidgetsCampoVar1();
            EnableWidgetsCampoVar2();
            EnableWidgetsCampoVar3();
            EnableWidgetsCampoVar4();
            EnableWidgetsCampoVar5();
            EnableWidgetsCampoVar6();

            ui->m_GuardarFicheroIni->setEnabled(true);
            ui->m_GuardarFicheroIniNow->setEnabled(true);
            ui->m_ViewIni->setEnabled(true);
            ui->m_FormatButton->setEnabled(true);
            ui->m_FormatPageButton->setEnabled(true);
            ui->m_FormatPlantillaButton->setEnabled(true);
		}
		else{ //(isRoot==0)
            ui->m_FormatPlantillaButton->setEnabled(false);
            ui->m_FormatButton->setEnabled(false);
            ui->m_FormatPageButton->setEnabled(false);
            ui->m_PrintButton->setEnabled(true);
            ui->m_OrdenFabricacion->setEnabled(false);

            DisableWidgetsCampo1();
            DisableWidgetsCampo2();
            DisableWidgetsCampo3();
            DisableWidgetsCampo4();

            DisableWidgetsCampoVar1();
            DisableWidgetsCampoVar2();
            DisableWidgetsCampoVar3();
            DisableWidgetsCampoVar4();
            DisableWidgetsCampoVar5();
            DisableWidgetsCampoVar6();

            QPalette paletteDis=GetPaletaTextEditDisabled();

            ui->m_OrdenFabricacion->setPalette(paletteDis);

            ui->m_GuardarFicheroIni->setEnabled(false);
            ui->m_GuardarFicheroIniNow->setEnabled(false);
            ui->m_ViewIni->setEnabled(false);
            ui->m_FormatButton->setEnabled(false);
            ui->m_FormatPageButton->setEnabled(false);
            ui->m_FormatPlantillaButton->setEnabled(false);
		}
	}

}

DbImpresion::~DbImpresion()
{
    //Comprobar_Campos_Variables();
}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void DbImpresion::AceptarSeleccion(){


    memoryMap->ReturnToOriginalWorld();

    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    if (memoryMap->m_ConfiguracionCargada){
        int res=QMessageBox::question(this,"GUARDAR","Quieres guardar los cambios?",QMessageBox::Ok,QMessageBox::Cancel);
        if (res==QMessageBox::Ok)
            GuardarComposicion();
    }

	LaunchPrincipal();
}

void DbImpresion::FormatPlantilla(){
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
	DbFormatPlantilla *ScrDbFormatPlantilla;
	ScrDbFormatPlantilla=new DbFormatPlantilla(0);
	ScrDbFormatPlantilla->show();
	this->deleteLater(); 
}

void DbImpresion::FormatImagen(){
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
	DbFormat *ScrDbFormat;
	ScrDbFormat=new DbFormat(0);
	ScrDbFormat->show();
	this->deleteLater(); 
}

void DbImpresion::FormatPagina(){
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
	DbFormatPage *ScrDbFormatPage;
	ScrDbFormatPage=new DbFormatPage(0);
	ScrDbFormatPage->show();
	this->deleteLater(); 
}

void DbImpresion::GuardaDatosCampos(){	

    if (ui->m_OnOffCampo1->isChecked()) memoryMap->m_mapa.m_campos[0].OnOffCampo=1;
	else memoryMap->m_mapa.m_campos[0].OnOffCampo=0;
	
    if (ui->m_OnOffCampo2->isChecked()) memoryMap->m_mapa.m_campos[1].OnOffCampo=1;
	else memoryMap->m_mapa.m_campos[1].OnOffCampo=0;

    if (ui->m_OnOffCampo3->isChecked()) memoryMap->m_mapa.m_campos[2].OnOffCampo=1;
    else memoryMap->m_mapa.m_campos[2].OnOffCampo=0;

    if (ui->m_OnOffCampo4->isChecked()) memoryMap->m_mapa.m_campos[3].OnOffCampo=1;
    else memoryMap->m_mapa.m_campos[3].OnOffCampo=0;

    memoryMap->m_mapa.m_variables.OrdenFabricacion=ui->m_OrdenFabricacion->toPlainText();

    /*strncpy(memoryMap->m_mapa.m_campos[0].NombreCampo,ui->m_NombreCampo1->toPlainText().toAscii(),CAMPOS_LEN);
    strncpy(memoryMap->m_mapa.m_campos[1].NombreCampo,ui->m_NombreCampo2->toPlainText().toAscii(),CAMPOS_LEN);
    strncpy(memoryMap->m_mapa.m_campos[2].NombreCampo,ui->m_NombreCampo3->toPlainText().toAscii(),CAMPOS_LEN);
    strncpy(memoryMap->m_mapa.m_campos[3].NombreCampo,ui->m_NombreCampo4->toPlainText().toAscii(),CAMPOS_LEN);

    strncpy(memoryMap->m_mapa.m_campos[0].ValorActualCampo,ui->m_ValorCampo1->toPlainText().toAscii(),CAMPOS_LEN);
    strncpy(memoryMap->m_mapa.m_campos[1].ValorActualCampo,ui->m_ValorCampo2->toPlainText().toAscii(),CAMPOS_LEN);
    strncpy(memoryMap->m_mapa.m_campos[2].ValorActualCampo,ui->m_ValorCampo3->toPlainText().toAscii(),CAMPOS_LEN);
    strncpy(memoryMap->m_mapa.m_campos[3].ValorActualCampo,ui->m_ValorCampo4->toPlainText().toAscii(),CAMPOS_LEN);*/

    memoryMap->m_mapa.m_campos[0].NombreCampo=ui->m_NombreCampo1->toPlainText();
    memoryMap->m_mapa.m_campos[1].NombreCampo=ui->m_NombreCampo2->toPlainText();
    memoryMap->m_mapa.m_campos[2].NombreCampo=ui->m_NombreCampo3->toPlainText();
    memoryMap->m_mapa.m_campos[3].NombreCampo=ui->m_NombreCampo4->toPlainText();

    memoryMap->m_mapa.m_campos[0].ValorActualCampo=ui->m_ValorCampo1->toPlainText();
    memoryMap->m_mapa.m_campos[1].ValorActualCampo=ui->m_ValorCampo2->toPlainText();
    memoryMap->m_mapa.m_campos[2].ValorActualCampo=ui->m_ValorCampo3->toPlainText();
    memoryMap->m_mapa.m_campos[3].ValorActualCampo=ui->m_ValorCampo4->toPlainText();
}

void DbImpresion::GuardaDatosCamposVariables(){

    if (ui->m_OnOffCampoVar1->isChecked()) memoryMap->m_mapa.m_camposvariables[0].OnOffCampo=1;
    else memoryMap->m_mapa.m_camposvariables[0].OnOffCampo=0;

    if (ui->m_OnOffCampoVar2->isChecked()) memoryMap->m_mapa.m_camposvariables[1].OnOffCampo=1;
    else memoryMap->m_mapa.m_camposvariables[1].OnOffCampo=0;

    if (ui->m_OnOffCampoVar3->isChecked()) memoryMap->m_mapa.m_camposvariables[2].OnOffCampo=1;
    else memoryMap->m_mapa.m_camposvariables[2].OnOffCampo=0;

    if (ui->m_OnOffCampoVar4->isChecked()) memoryMap->m_mapa.m_camposvariables[3].OnOffCampo=1;
    else memoryMap->m_mapa.m_camposvariables[3].OnOffCampo=0;

    if (ui->m_OnOffCampoVar5->isChecked()) memoryMap->m_mapa.m_camposvariables[4].OnOffCampo=1;
    else memoryMap->m_mapa.m_camposvariables[4].OnOffCampo=0;

    if (ui->m_OnOffCampoVar6->isChecked()) memoryMap->m_mapa.m_camposvariables[5].OnOffCampo=1;
    else memoryMap->m_mapa.m_camposvariables[5].OnOffCampo=0;

    memoryMap->m_mapa.m_camposvariables[0].NombreCampo=ui->m_NombreCampoVar1->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[1].NombreCampo=ui->m_NombreCampoVar2->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[2].NombreCampo=ui->m_NombreCampoVar3->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[3].NombreCampo=ui->m_NombreCampoVar4->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[4].NombreCampo=ui->m_NombreCampoVar5->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[5].NombreCampo=ui->m_NombreCampoVar6->toPlainText().toAscii();

    memoryMap->m_mapa.m_camposvariables[0].ValorActualCampo=ui->m_ValorCampoVar1->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[1].ValorActualCampo=ui->m_ValorCampoVar2->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[2].ValorActualCampo=ui->m_ValorCampoVar3->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[3].ValorActualCampo=ui->m_ValorCampoVar4->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[4].ValorActualCampo=ui->m_ValorCampoVar5->toPlainText().toAscii();
    memoryMap->m_mapa.m_camposvariables[5].ValorActualCampo=ui->m_ValorCampoVar6->toPlainText().toAscii();

    memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo=ui->m_OrigenCampoVar1->currentIndex();
    memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo=ui->m_OrigenCampoVar2->currentIndex();
    memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo=ui->m_OrigenCampoVar3->currentIndex();
    memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo=ui->m_OrigenCampoVar4->currentIndex();
    memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo=ui->m_OrigenCampoVar5->currentIndex();
    memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo=ui->m_OrigenCampoVar6->currentIndex();

    memoryMap->m_mapa.m_camposvariables[0].OrigenValorCampo=ui->m_OrigenCampoVar1->currentText();
    memoryMap->m_mapa.m_camposvariables[1].OrigenValorCampo=ui->m_OrigenCampoVar2->currentText();
    memoryMap->m_mapa.m_camposvariables[2].OrigenValorCampo=ui->m_OrigenCampoVar3->currentText();
    memoryMap->m_mapa.m_camposvariables[3].OrigenValorCampo=ui->m_OrigenCampoVar4->currentText();
    memoryMap->m_mapa.m_camposvariables[4].OrigenValorCampo=ui->m_OrigenCampoVar5->currentText();
    memoryMap->m_mapa.m_camposvariables[5].OrigenValorCampo=ui->m_OrigenCampoVar6->currentText();

    if (memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar1->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.valorInicialFila=iniValue;
    }
    if (memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar2->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[1].m_counterParameters.valorInicialFila=iniValue;
    }
    if (memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar3->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[2].m_counterParameters.valorInicialFila=iniValue;
    }
    if (memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar4->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[3].m_counterParameters.valorInicialFila=iniValue;
    }
    if (memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar5->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[4].m_counterParameters.valorInicialFila=iniValue;
    }
    if (memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar6->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[5].m_counterParameters.valorInicialFila=iniValue;
    }

    memoryMap->m_mapa.m_camposvariables[0].OrdenCampo=ui->m_OrdenCampoVar1->currentIndex();
    memoryMap->m_mapa.m_camposvariables[1].OrdenCampo=ui->m_OrdenCampoVar2->currentIndex();
    memoryMap->m_mapa.m_camposvariables[2].OrdenCampo=ui->m_OrdenCampoVar3->currentIndex();
    memoryMap->m_mapa.m_camposvariables[3].OrdenCampo=ui->m_OrdenCampoVar4->currentIndex();
    memoryMap->m_mapa.m_camposvariables[4].OrdenCampo=ui->m_OrdenCampoVar5->currentIndex();
    memoryMap->m_mapa.m_camposvariables[5].OrdenCampo=ui->m_OrdenCampoVar6->currentIndex();

}


//Cancela la selección y vuelve a la pantalla de Configuracion
void DbImpresion::CancelarSeleccion(){
    memoryMap->ReturnToOriginalWorld();
    int res=QMessageBox::question(this,"GUARDAR","Quieres guardar los cambios?",QMessageBox::Ok,QMessageBox::Cancel);
    if (res==QMessageBox::Ok)
        GuardarComposicion();

    LaunchPrincipal();
	this->deleteLater();
}


// Actualiza los datos de pantalla
void DbImpresion::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}


void DbImpresion::UpdateOnOff1(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo1->isChecked())
	{
        ui->m_NombreCampo1->setEnabled(true);
        ui->m_NombreCampo1->setReadOnly(false);
        ui->m_ValorCampo1->setEnabled(true);
        ui->m_NombreCampo1->setPalette(paleteEn);
        ui->m_ValorCampo1->setPalette(paleteEn);
	}
	else
	{
        ui->m_NombreCampo1->setEnabled(false);
        ui->m_NombreCampo1->setReadOnly(true);
        ui->m_ValorCampo1->setEnabled(false);
        ui->m_NombreCampo1->setPalette(paleteDis);
        ui->m_ValorCampo1->setPalette(paleteDis);
	}

}

void DbImpresion::UpdateOnOff2(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo2->isChecked())
	{
        ui->m_NombreCampo2->setEnabled(true);
        ui->m_NombreCampo2->setReadOnly(false);
        ui->m_ValorCampo2->setEnabled(true);
        ui->m_NombreCampo2->setPalette(paleteEn);
        ui->m_ValorCampo2->setPalette(paleteEn);
	}
	else
	{
        ui->m_NombreCampo2->setEnabled(false);
        ui->m_NombreCampo2->setReadOnly(true);
        ui->m_ValorCampo2->setEnabled(false);
        ui->m_NombreCampo2->setPalette(paleteDis);
        ui->m_ValorCampo2->setPalette(paleteDis);
	}
}

void DbImpresion::UpdateOnOff3(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo3->isChecked())
    {
        ui->m_NombreCampo3->setEnabled(true);
        ui->m_NombreCampo3->setReadOnly(false);
        ui->m_ValorCampo3->setEnabled(true);
        ui->m_NombreCampo3->setPalette(paleteEn);
        ui->m_ValorCampo3->setPalette(paleteEn);
    }
    else
    {
        ui->m_NombreCampo3->setEnabled(false);
        ui->m_NombreCampo3->setReadOnly(true);
        ui->m_ValorCampo3->setEnabled(false);
        ui->m_NombreCampo3->setPalette(paleteDis);
        ui->m_ValorCampo3->setPalette(paleteDis);
    }
}

void DbImpresion::UpdateOnOff4(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo4->isChecked())
    {
        ui->m_NombreCampo4->setEnabled(true);
        ui->m_NombreCampo4->setReadOnly(false);
        ui->m_ValorCampo4->setEnabled(true);
        ui->m_NombreCampo4->setPalette(paleteEn);
        ui->m_ValorCampo4->setPalette(paleteEn);
    }
    else
    {
        ui->m_NombreCampo4->setEnabled(false);
        ui->m_NombreCampo4->setReadOnly(true);
        ui->m_ValorCampo4->setEnabled(false);
        ui->m_NombreCampo4->setPalette(paleteDis);
        ui->m_ValorCampo4->setPalette(paleteDis);
    }
}

void DbImpresion::UpdateOrdenFabricacion(){
    InputData *input = InputData::request(this,"Orden de fabricación");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_OrdenFabricacion(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_OrdenFabricacion(QString value){
    ui->m_OrdenFabricacion->setText(value);
    ui->m_OrdenFabricacion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateNombreCampo1(){
    InputData *input = InputData::request(this,"Nombre Campo 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo1(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampo1(QString value){
    ui->m_NombreCampo1->setText(value);
    ui->m_NombreCampo1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateNombreCampo2(){
    InputData *input = InputData::request(this,"Nombre Campo 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo2(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampo2(QString value){
    ui->m_NombreCampo2->setText(value);
    ui->m_NombreCampo2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateNombreCampo3(){
    InputData *input = InputData::request(this,"Nombre Campo 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo3(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampo3(QString value){
    ui->m_NombreCampo3->setText(value);
    ui->m_NombreCampo3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateNombreCampo4(){
    InputData *input = InputData::request(this,"Nombre Campo 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo4(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampo4(QString value){
    ui->m_NombreCampo4->setText(value);
    ui->m_NombreCampo4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampo1(){
    InputData *input = InputData::request(this, "Valor Campo 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo1(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampo1(QString value){
    ui->m_ValorCampo1->setText(value);
    ui->m_ValorCampo1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateValorCampo2(){
    InputData *input = InputData::request(this,"Valor Campo 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo2(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampo2(QString value){
    ui->m_ValorCampo2->setText(value);
    ui->m_ValorCampo2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampo3(){
    InputData *input = InputData::request(this,"Valor Campo 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo3(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampo3(QString value){
    ui->m_ValorCampo3->setText(value);
    ui->m_ValorCampo3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampo4(){
    InputData *input = InputData::request(this,"Valor Campo 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo4(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampo4(QString value){
    ui->m_ValorCampo4->setText(value);
    ui->m_ValorCampo4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateOnOffVar1(){
    if (ui->m_OnOffCampoVar1->isChecked())
    {
        ui->m_NombreCampoVar1->setEnabled(true);
        ui->m_ValorCampoVar1->setEnabled(true);
        ui->m_OrdenCampoVar1->setEnabled(true);
        ui->m_OrigenCampoVar1->setEnabled(true);
    }
    else
    {
        ui->m_NombreCampoVar1->setEnabled(false);
        ui->m_ValorCampoVar1->setEnabled(false);
        ui->m_OrdenCampoVar1->setEnabled(false);
        ui->m_OrigenCampoVar1->setEnabled(false);
    }

}

void DbImpresion::UpdateOnOffVar2(){
    if (ui->m_OnOffCampoVar2->isChecked())
    {
        ui->m_NombreCampoVar2->setEnabled(true);
        ui->m_ValorCampoVar2->setEnabled(true);
        ui->m_OrdenCampoVar2->setEnabled(true);
        ui->m_OrigenCampoVar2->setEnabled(true);
    }
    else
    {
        ui->m_NombreCampoVar2->setEnabled(false);
        ui->m_ValorCampoVar2->setEnabled(false);
        ui->m_OrdenCampoVar2->setEnabled(false);
        ui->m_OrigenCampoVar2->setEnabled(false);
    }
}

void DbImpresion::UpdateOnOffVar3(){
    if (ui->m_OnOffCampoVar3->isChecked())
    {
        ui->m_NombreCampoVar3->setEnabled(true);
        ui->m_ValorCampoVar3->setEnabled(true);
        ui->m_OrdenCampoVar3->setEnabled(true);
        ui->m_OrigenCampoVar3->setEnabled(true);
    }
    else
    {
        ui->m_NombreCampoVar3->setEnabled(false);
        ui->m_ValorCampoVar3->setEnabled(false);
        ui->m_OrdenCampoVar3->setEnabled(false);
        ui->m_OrigenCampoVar3->setEnabled(false);
    }
}

void DbImpresion::UpdateOnOffVar4(){
    if (ui->m_OnOffCampoVar4->isChecked())
    {
        ui->m_NombreCampoVar4->setEnabled(true);
        ui->m_ValorCampoVar4->setEnabled(true);
        ui->m_OrdenCampoVar4->setEnabled(true);
        ui->m_OrigenCampoVar4->setEnabled(true);
    }
    else
    {
        ui->m_NombreCampoVar4->setEnabled(false);
        ui->m_ValorCampoVar4->setEnabled(false);
        ui->m_OrdenCampoVar4->setEnabled(false);
        ui->m_OrigenCampoVar4->setEnabled(false);
    }
}
void DbImpresion::UpdateOnOffVar5(){
    if (ui->m_OnOffCampoVar5->isChecked())
    {
        ui->m_NombreCampoVar5->setEnabled(true);
        ui->m_ValorCampoVar5->setEnabled(true);
        ui->m_OrdenCampoVar5->setEnabled(true);
        ui->m_OrigenCampoVar5->setEnabled(true);
    }
    else
    {
        ui->m_NombreCampoVar5->setEnabled(false);
        ui->m_ValorCampoVar5->setEnabled(false);
        ui->m_OrdenCampoVar5->setEnabled(false);
        ui->m_OrigenCampoVar5->setEnabled(false);
    }
}
void DbImpresion::UpdateOnOffVar6(){
    if (ui->m_OnOffCampoVar6->isChecked())
    {
        ui->m_NombreCampoVar6->setEnabled(true);
        ui->m_ValorCampoVar6->setEnabled(true);
        ui->m_OrdenCampoVar6->setEnabled(true);
        ui->m_OrigenCampoVar6->setEnabled(true);
    }
    else
    {
        ui->m_NombreCampoVar6->setEnabled(false);
        ui->m_ValorCampoVar6->setEnabled(false);
        ui->m_OrdenCampoVar6->setEnabled(false);
        ui->m_OrigenCampoVar6->setEnabled(false);
    }
}



void DbImpresion::UpdateNombreCampoVar1(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar1(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampoVar1(QString value){
    ui->m_NombreCampoVar1->setText(value);
    ui->m_NombreCampoVar1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateNombreCampoVar2(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar2(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampoVar2(QString value){
    ui->m_NombreCampoVar2->setText(value);
    ui->m_NombreCampoVar2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateNombreCampoVar3(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar3(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_NombreCampoVar3(QString value){
    ui->m_NombreCampoVar3->setText(value);
    ui->m_NombreCampoVar3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateNombreCampoVar4(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar4(QString)));
        input->setFocus();
     }
}
void DbImpresion::Update_NombreCampoVar4(QString value){
    ui->m_NombreCampoVar4->setText(value);
    ui->m_NombreCampoVar4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateNombreCampoVar5(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 5");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar5(QString)));
        input->setFocus();
     }
}
void DbImpresion::Update_NombreCampoVar5(QString value){
    ui->m_NombreCampoVar5->setText(value);
    ui->m_NombreCampoVar5->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateNombreCampoVar6(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 6");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar6(QString)));
        input->setFocus();
     }
}
void DbImpresion::Update_NombreCampoVar6(QString value){
    ui->m_NombreCampoVar6->setText(value);
    ui->m_NombreCampoVar6->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateValorCampoVar1(){
    InputData *input = InputData::request(this, "Valor Campo Variable 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar1(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampoVar1(QString value){
    ui->m_ValorCampoVar1->setText(value);
    ui->m_ValorCampoVar1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbImpresion::UpdateValorCampoVar2(){
    InputData *input = InputData::request(this,"Valor Campo Variable 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar2(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampoVar2(QString value){
    ui->m_ValorCampoVar2->setText(value);
    ui->m_ValorCampoVar2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampoVar3(){
    InputData *input = InputData::request(this,"Valor Campo Variable 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar3(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampoVar3(QString value){
    ui->m_ValorCampoVar3->setText(value);
    ui->m_ValorCampoVar3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampoVar4(){
    InputData *input = InputData::request(this,"Valor Campo Variable 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar4(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampoVar4(QString value){
    ui->m_ValorCampoVar4->setText(value);
    ui->m_ValorCampoVar4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampoVar5(){
    InputData *input = InputData::request(this,"Valor Campo Variable 5");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar5(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampoVar5(QString value){
    ui->m_ValorCampoVar5->setText(value);
    ui->m_ValorCampoVar5->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbImpresion::UpdateValorCampoVar6(){
    InputData *input = InputData::request(this,"Valor Campo Variable 6");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar6(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_ValorCampoVar6(QString value){
    ui->m_ValorCampoVar6->setText(value);
    ui->m_ValorCampoVar6->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}



void DbImpresion::CargaDatosCampos(){


    if (memoryMap->m_mapa.m_campos[0].OnOffCampo==1) ui->m_OnOffCampo1->setChecked(true);
    else ui->m_OnOffCampo1->setChecked(false);
	UpdateOnOff1();
    if (memoryMap->m_mapa.m_campos[1].OnOffCampo==1) ui->m_OnOffCampo2->setChecked(true);
    else ui->m_OnOffCampo2->setChecked(false);
	UpdateOnOff2();
    if (memoryMap->m_mapa.m_campos[2].OnOffCampo==1) ui->m_OnOffCampo3->setChecked(true);
    else ui->m_OnOffCampo3->setChecked(false);
    UpdateOnOff3();
    if (memoryMap->m_mapa.m_campos[3].OnOffCampo==1) ui->m_OnOffCampo4->setChecked(true);
    else ui->m_OnOffCampo4->setChecked(false);
    UpdateOnOff4();

    ui->m_OrdenFabricacion->setPlainText("");

    ui->m_NombreCampo1->setPlainText(memoryMap->m_mapa.m_campos[0].NombreCampo);
    ui->m_NombreCampo2->setPlainText(memoryMap->m_mapa.m_campos[1].NombreCampo);
    ui->m_NombreCampo3->setPlainText(memoryMap->m_mapa.m_campos[2].NombreCampo);
    ui->m_NombreCampo4->setPlainText(memoryMap->m_mapa.m_campos[3].NombreCampo);

    ui->m_ValorCampo1->setPlainText(memoryMap->m_mapa.m_campos[0].ValorActualCampo);
    ui->m_ValorCampo2->setPlainText(memoryMap->m_mapa.m_campos[1].ValorActualCampo);
    ui->m_ValorCampo3->setPlainText(memoryMap->m_mapa.m_campos[2].ValorActualCampo);
    ui->m_ValorCampo4->setPlainText(memoryMap->m_mapa.m_campos[3].ValorActualCampo);

    QString dummy=memoryMap->m_mapa.m_variables.NombreRutaIni;
    dummy.replace("/","\\");
    ui->m_NombreFicheroIni->setPlainText(dummy);

    ShowCompositionProps();

}



void DbImpresion::CargaDatosCamposVariables(){

    QString tmp;

    //if (token_ideogramas==0){
        //CargaIdeogramas();
        //token_ideogramas=1;
    //}

    if (memoryMap->m_mapa.m_camposvariables[0].OnOffCampo==1) ui->m_OnOffCampoVar1->setChecked(true);
    else ui->m_OnOffCampoVar1->setChecked(false);
    UpdateOnOffVar1();
    if (memoryMap->m_mapa.m_camposvariables[1].OnOffCampo==1) ui->m_OnOffCampoVar2->setChecked(true);
    else ui->m_OnOffCampoVar2->setChecked(false);
    UpdateOnOffVar2();
    if (memoryMap->m_mapa.m_camposvariables[2].OnOffCampo==1) ui->m_OnOffCampoVar3->setChecked(true);
    else ui->m_OnOffCampoVar3->setChecked(false);
    UpdateOnOffVar3();
    if (memoryMap->m_mapa.m_camposvariables[3].OnOffCampo==1) ui->m_OnOffCampoVar4->setChecked(true);
    else ui->m_OnOffCampoVar4->setChecked(false);
    UpdateOnOffVar4();
    if (memoryMap->m_mapa.m_camposvariables[4].OnOffCampo==1) ui->m_OnOffCampoVar5->setChecked(true);
    else ui->m_OnOffCampoVar5->setChecked(false);
    UpdateOnOffVar5();
    if (memoryMap->m_mapa.m_camposvariables[5].OnOffCampo==1) ui->m_OnOffCampoVar6->setChecked(true);
    else ui->m_OnOffCampoVar6->setChecked(false);
    UpdateOnOffVar6();


    ui->m_NombreCampoVar1->setPlainText(memoryMap->m_mapa.m_camposvariables[0].NombreCampo);
    ui->m_NombreCampoVar2->setPlainText(memoryMap->m_mapa.m_camposvariables[1].NombreCampo);
    ui->m_NombreCampoVar3->setPlainText(memoryMap->m_mapa.m_camposvariables[2].NombreCampo);
    ui->m_NombreCampoVar4->setPlainText(memoryMap->m_mapa.m_camposvariables[3].NombreCampo);
    ui->m_NombreCampoVar5->setPlainText(memoryMap->m_mapa.m_camposvariables[4].NombreCampo);
    ui->m_NombreCampoVar6->setPlainText(memoryMap->m_mapa.m_camposvariables[5].NombreCampo);

    ui->m_ValorCampoVar1->setPlainText(memoryMap->m_mapa.m_camposvariables[0].ValorActualCampo);
    ui->m_ValorCampoVar2->setPlainText(memoryMap->m_mapa.m_camposvariables[1].ValorActualCampo);
    ui->m_ValorCampoVar3->setPlainText(memoryMap->m_mapa.m_camposvariables[2].ValorActualCampo);
    ui->m_ValorCampoVar4->setPlainText(memoryMap->m_mapa.m_camposvariables[3].ValorActualCampo);
    ui->m_ValorCampoVar5->setPlainText(memoryMap->m_mapa.m_camposvariables[4].ValorActualCampo);
    ui->m_ValorCampoVar6->setPlainText(memoryMap->m_mapa.m_camposvariables[5].ValorActualCampo);

    ui->m_OrigenCampoVar1->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo);
    ui->m_OrigenCampoVar2->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo);
    ui->m_OrigenCampoVar3->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo);
    ui->m_OrigenCampoVar4->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo);
    ui->m_OrigenCampoVar5->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo);
    ui->m_OrigenCampoVar6->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo);

    ui->m_OrdenCampoVar1->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[0].OrdenCampo);
    ui->m_OrdenCampoVar2->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[1].OrdenCampo);
    ui->m_OrdenCampoVar3->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[2].OrdenCampo);
    ui->m_OrdenCampoVar4->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[3].OrdenCampo);
    ui->m_OrdenCampoVar5->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[4].OrdenCampo);
    ui->m_OrdenCampoVar6->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[5].OrdenCampo);


    ui->m_OrigenCampoVar1->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo);
    if (memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar1->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar2->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo);
    if (memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=memoryMap->m_mapa.m_camposvariables[1].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar2->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar3->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo);
    if (memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=memoryMap->m_mapa.m_camposvariables[2].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar3->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar4->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo);
    if (memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=memoryMap->m_mapa.m_camposvariables[3].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar4->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar5->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo);
    if (memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=memoryMap->m_mapa.m_camposvariables[4].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar5->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar6->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo);
    if (memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=memoryMap->m_mapa.m_camposvariables[5].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar6->setText(QString("%1").arg(iniValue));
    }

}

void DbImpresion::LaunchPrincipal(){

    GlobalShowMainWindow();
    this->deleteLater();

}


/** Comprobaciones previas a generar la imagen */
void DbImpresion::ComprobacionesGenerateImage(){

    int respuesta=0;

    GuardaDatosCampos();
    GuardaDatosCamposVariables();

    bool bTest=Comprobar_Campos_Vacios();
    if (bTest){
        respuesta=QMessageBox::question(this,"ATENCION","Alguno de los campos a imprimir está vacíos\n¿Desea continuar?","Si","No",QString::null, 0, 1 );
        if (respuesta==1) return;
    }

    bTest=Comprobar_OrdenFabricacion_Vacia();
    if (bTest){
        QMessageBox::warning(0,"ERROR","La orden de fabricación está vacía.\nPor favor, introduzca un valor adecuado.");
        return;
    }
    bTest=Comprobar_ExisteComposicion();
    if (bTest){
        respuesta=QMessageBox::question(this,"ATENCION","La composición a generar ya existe\n¿Desea continuar?","Si","No",QString::null, 0, 1 );
        if (respuesta==1) return;

    }
    GenerateImage();


}

/** Copia la BBDD a la carpeta de Composicion*/
void DbImpresion::CopiarBBDD(){
    _CopiarBBDD();
    //_GenerateInternalBBDD();
}

/** Genera Previsualizacion*/
void DbImpresion::GeneratePrev(){

/*    unsigned int ImagenesPorEnvio=0;
    unsigned int j=0;
    unsigned int ImpresionesPosibles;
    unsigned char indice_color;

        for(indice_color=0;indice_color<memoryMap->m_mapa.m_variables.NumColores;indice_color++){
            QString dummy=QString(memoryMap->m_mapa.m_variables.NombreRutaBmp);
            dummy.replace(QString(".bmp"),QString("_C%1.bmp").arg(indice_color+1));
            PixmapUnitario[indice_color].load(dummy,"BMP",Qt::MonoOnly);
        }
    //}

    ImpresionesPosibles=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;

    //Para CINFA no permitimos celdas sin imprimir
    //ImagenesPorEnvio=memoryMap->m_mapa.m_variables.ImpresionesPorEnvio;
    ImagenesPorEnvio=ImpresionesPosibles;


        for(indice_color=0;indice_color<memoryMap->m_mapa.m_variables.NumColores;indice_color++){
            pixMapArray[indice_color].clear();
        }

        for(j=0;j<ImpresionesPosibles;j++){
            if (j<ImagenesPorEnvio)	CreaImagenUnitariaCompleta(j);
            else CreaImagenUnitariaEnBlanco(j);
        }
        CreaImagenEnviar_ColoresPlenos();
    //}

    if ((memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL)){
        GeneraFicheroMascaraComposicion();
    }*/
}


/** Genera la imagen*/
void DbImpresion::GenerateImage(){

    ui->m_AceptarButton->setEnabled(false);
    //ui->m_CancelButton->setEnabled(false);
    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);
    In2ImageCreator p;
    p.sltGenerateImage();
    /*QImage dummy=memoryMap->m_Imagen.toImage();
    QVector<QRgb> table( 256 );
    for( int i = 0; i < 256; ++i ){
        table[i] = qRgb(i,i,i);
    }


    QImage result=dummy.convertToFormat(QImage::Format_Mono,Qt::DiffuseDither);
    result.save("kkMono.bmp");

    QImage result8bit=result.convertToFormat(QImage::Format_Indexed8,Qt::MonoOnly);
    result8bit.save("kk8bit.bmp");

    QImage original(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    original.save("original.bmp");
    QImage original8bit=original.convertToFormat(QImage::Format_Indexed8,table,Qt::MonoOnly);
    original8bit.save("original8bit.bmp");
    QImage trans8bit(original8bit);
    for (int x=1;x<original8bit.width()-1;x++){
        for (int y=1;y<original8bit.height()-1;y++){
            int index=original8bit.pixelIndex(x,y)+original8bit.pixelIndex(x-1,y)+original8bit.pixelIndex(x+1,y)+original8bit.pixelIndex(x,y-1)+original8bit.pixelIndex(x,y+1)+original8bit.pixelIndex(x-1,y-1)+original8bit.pixelIndex(x+1,y-1)+original8bit.pixelIndex(x-1,y+1)+original8bit.pixelIndex(x+1,y+1);
            index=index/9;
            if (index>=234) index=255;
            else if ((index>=162)&&(index<234)) index=150;
            else if ((index>=90)&&(index<162)) index=100;
            else if ((index>=0)&&(index<90)) index=0;

            trans8bit.setPixel(x,y,index);
        }
    }
    trans8bit.save("trans8it.bmp");

    QPixmap pixmapHead1("Mascara.bmp");
    QPixmap pixmapHead2=QPixmap::fromImage(trans8bit);
    QPainter p2(&pixmapHead2);
    p2.setCompositionMode(QPainter::CompositionMode_Multiply);
    p2.drawPixmap(0,0,pixmapHead1);
    pixmapHead2.save("Multiply.bmp");
    p2.end();

    pixmapHead2=QPixmap::fromImage(trans8bit);
    QPainter p3(&pixmapHead2);
    p3.setCompositionMode(QPainter::CompositionMode_Lighten);
    p3.drawPixmap(0,0,pixmapHead1);
    pixmapHead2.save("Lighten.bmp");
    p3.end();

    pixmapHead2=QPixmap::fromImage(trans8bit);
    QPainter p4(&pixmapHead2);
    p4.setCompositionMode(QPainter::CompositionMode_Darken);
    p4.drawPixmap(0,0,pixmapHead1);
    pixmapHead2.save("Darken.bmp");
    p4.end();*/





    ui->m_AceptarButton->setEnabled(true);
    //ui->m_CancelButton->setEnabled(true);
    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);


    //Borrar ficheros bmp intermedios
    QFile::remove(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    for(int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        QString dummy=QString("%1").arg(memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummy.replace(".bmp",QString("_C%1.bmp").arg(i+1));
        QFile::remove(QString("%1").arg(dummy));
    }
    InsertarEnColaImpresion();
    CopiarBBDD();
    //Con esto copia los ficheros de la zona temporal a la zona OK
    memoryMap->JumpToOriginalWorld();
    //Por si queremos seguir editando
    memoryMap->ReturnToTempWorld();
}


void DbImpresion::CargarComposicion(){

    QFileDialog fileName(this);
    QStringList Ficheros;

    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Archivo de composición(*.ini)"));
    fileName.setOptions(QFileDialog::DontResolveSymlinks);
    fileName.setDirectory(memoryMap->getImagePath());
    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
                    w->setAutoDefault(false);
     }*/

    if (fileName.exec()){
            Ficheros=fileName.selectedFiles();
            if (!Ficheros.isEmpty()){                 
                    //QApplication::setOverrideCursor(Qt::waitCursor);
                    QLabel *dummyLabel;
                    dummyLabel=ShowMensaje("Por favor espere");
                    QString tmp;
                    tmp=(QString)Ficheros.at(0);
                    //QMessageBox::warning(0,"Fichero ini",QString("%1").arg(tmp),QMessageBox::Ok);
                    tmp.replace("/","\\");
                    ui->m_NombreFicheroIni->setText(tmp);
                    QFileInfo infoFile(tmp);
                    QString ruta=infoFile.absolutePath();
                    QString nombre=infoFile.fileName();
                    QString nombreruta=tmp;
                    ruta.replace("/","\\");
                    nombre.replace("/","\\");
                    nombreruta.replace("/","\\");

                    memoryMap->InitComposicion();

                    memoryMap->m_mapa.m_variables.NombreIni=nombre;
                    memoryMap->m_mapa.m_variables.RutaIni=ruta;
                    memoryMap->m_mapa.m_variables.NombreRutaIni=nombreruta;

                    //Guardo los originales
                    memoryMap->JumpToTempWorld();

                    AbrirFicheroIni();
                    CargaDatosCampos();
                    CargaDatosCamposVariables();
                    CargaBBDD();
                    GeneracionRutaPlantilla();

                    bool aux;
                    QString nombrepdf_aux=QString("%1").arg(memoryMap->m_mapa.m_variables.NombreRutaPdf);
                    aux=QFile::exists(nombrepdf_aux);
                    //aux=QFile::exists(memoryMap->m_mapa.m_variables.NombreRutaPdf);
                    //QMessageBox::warning(0,"Ruta pdf",QString("%1").arg(nombrepdf_aux),QMessageBox::Ok);
                    if (aux==true){
                        //QMessageBox::warning(0,"ATENCIÓN!","El fichero existe",QMessageBox::Ok);
                        CargaPlantilla();
                        memoryMap->m_ConfiguracionCargada=true;
                        memoryMap->m_UltimaAccion=UA_CONFIGURACION_CARGADA;
                        TestRoot();
                        sltPrevisualizarZoom();
                        delete (dummyLabel);
                        QMessageBox::warning(0,"ATENCIÓN!","Composición cargada correctamente",QMessageBox::Ok);
                        emit SendInformerInfo(INFORMER_DEBUG,QString("Composición cargada: %1").arg(nombreruta));
                    }
                    else{
                        //Aunque no haya fichero pdf, damos por cargada la compsoicion
                        memoryMap->m_ConfiguracionCargada=true;
                        memoryMap->m_UltimaAccion=UA_CONFIGURACION_CARGADA;
                        TestRoot();
                        delete (dummyLabel);
                        QString mensaje_fichero=QString("No existe el archivo\n%1 ").arg(memoryMap->m_mapa.m_variables.NombrePdf);
                        QMessageBox::warning(0,"ERROR!",mensaje_fichero,QMessageBox::Ok);
                        emit SendInformerInfo(INFORMER_DEBUG,QString("Error al cargar composición: %1").arg(nombreruta));
                    }

                    //QApplication::restoreOverrideCursor();
                    ui->m_PrintButton->setEnabled(true);
                    ui->m_FormatButton->setEnabled(true);
                    ui->m_FormatPageButton->setEnabled(true);
                    ui->m_FormatPlantillaButton->setEnabled(true);

                    DataCommon->ShowInfo();
            }
    }

}

/** Cargamos BBDD*/
void DbImpresion::CargaBBDD(){
    if (!memoryMap->m_mapa.m_database.NombreRutaDb.isEmpty())
        LoadBBDD(memoryMap->m_mapa.m_database.NombreRutaDb);
}

void DbImpresion::CargaPlantilla(){

    unsigned char indice_color;
    QString dumy;

	Convert();

    if (memoryMap->m_mapa.m_variables.NumColores==1){
        m_PixmapUnitario.load(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::MonoOnly);
    }
    else{
        for(indice_color=0;indice_color<NUM_COLORES;indice_color++){
            QString dummy=QString(memoryMap->m_mapa.m_variables.NombreRutaBmp);
            dummy.replace(QString(".bmp"),QString("_C%1.bmp").arg(indice_color+1));
            PixmapUnitario[indice_color].load(dummy,"BMP",Qt::MonoOnly);
        }
    }


}
/** Guarda la composicion - step1*/
void DbImpresion::GuardarComposicion(){
	
	int dummy;

    ui->m_AceptarButton->setEnabled(false);
    //ui->m_CancelButton->setEnabled(false);
    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);

    if (memoryMap->getUserRights()>=ACCESS_ROOT){
		dummy=CuadroDialogoGuardarComposicion();
        if (dummy!=0){
            QMessageBox::warning(0,"ERROR","Error durante el proceso\nde guardado de composición",QMessageBox::Ok);
            emit SendInformerInfo(INFORMER_DEBUG,QString("Error al guardar composición: %1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));
        }
        else{
            emit SendInformerInfo(INFORMER_DEBUG,QString("Composición guardada: %1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));
        }
	}

    ui->m_AceptarButton->setEnabled(true);
    //ui->m_CancelButton->setEnabled(true);
    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);


}

/** Guarda la composicion-step2*/
int DbImpresion::CuadroDialogoGuardarComposicion(){
			   
	QFileDialog fileName(this);
	QString tmp,dummy;
	QStringList Ficheros;
	fileName.setAcceptMode(QFileDialog::AcceptSave);		
	fileName.setFileMode(QFileDialog::AnyFile);
	fileName.setViewMode(QFileDialog::List);
	fileName.setFilter(tr("Archivo de composición (*.ini)"));
	fileName.setDefaultSuffix("ini");
    fileName.setDirectory(memoryMap->getImagePath());

	if (fileName.exec()){
		Ficheros=fileName.selectedFiles();
		
		if (!Ficheros.isEmpty()){
			tmp=Ficheros.at(0);
			QFile File(tmp);
			if (!File.open(QIODevice::WriteOnly)) return -1;
            tmp.replace("/","\\");
            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

			QString ruta,nombre;

            QFileInfo infoFile(tmp);
            ruta=infoFile.absolutePath();
            nombre=infoFile.fileName();

            memoryMap->m_mapa.m_variables.RutaIni=ruta;
            memoryMap->m_mapa.m_variables.NombreIni=nombre;

			QString nombre_bmp=nombre;
			nombre_bmp.replace(".ini",".bmp");

            memoryMap->m_mapa.m_variables.NombreIni=nombre;
            memoryMap->m_mapa.m_variables.RutaIni=ruta;
            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

			//File.write((const char*)&memoryMap->m_mapa,sizeof(memoryMap->m_mapa));
            memoryMap->SetCredentialsModified();
            GuardaDatosCampos();
            GuardaDatosCamposVariables();

            GuardarFicheroIni();
			File.close();
			QMessageBox::information(0,"INFO","Configuración guardada",QMessageBox::Ok);
			return 0;
		}
		else return -1;//JORGE
	}
	else return -1;//JORGE

}


/** Guarda la composicion sin abrir cuadro de diálogo - step1*/
void DbImpresion::GuardarComposicionNow(){

    int dummy;

    ui->m_AceptarButton->setEnabled(false);
    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);

    if (memoryMap->getUserRights()>=ACCESS_ROOT){
        GuardarComposicionDirectamente();
        emit SendInformerInfo(INFORMER_DEBUG,QString("Composición guardada: %1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));
    }

    ui->m_AceptarButton->setEnabled(true);
    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);

}


/** Guarda la composicion sin abrir cuadro de diálogo - step2*/
void DbImpresion::GuardarComposicionDirectamente(){

    /*QString tmp;
    tmp=ui->m_NombreFicheroIni->toPlainText();
    memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

    QString ruta,nombre;

    QFileInfo infoFile(tmp);
    ruta=infoFile.absolutePath();
    nombre=infoFile.fileName();

    memoryMap->m_mapa.m_variables.RutaIni=ruta;
    memoryMap->m_mapa.m_variables.NombreIni=nombre;

    QString nombre_bmp=nombre;
    nombre_bmp.replace(".ini",".bmp");

    memoryMap->m_mapa.m_variables.NombreIni=nombre;
    memoryMap->m_mapa.m_variables.RutaIni=ruta;
    memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;*/

     if (memoryMap->m_mapa.m_variables.bUseLocalCopy) memoryMap->ReturnToOriginalWorld();

    memoryMap->SetCredentialsModified();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();

    GuardarFicheroIni();

    if (memoryMap->m_mapa.m_variables.bUseLocalCopy) memoryMap->ReturnToTempWorld();

}


void DbImpresion::NuevaComposicion(){

    int dummy;

    ui->m_AceptarButton->setEnabled(false);
    //ui->m_CancelButton->setEnabled(false);
    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);

    if (memoryMap->getUserRights()>=ACCESS_ROOT){
        dummy=CuadroDialogoNuevaComposicion();
        if (dummy!=0){
            QMessageBox::warning(0,"ERROR","Error creando nueva composición",QMessageBox::Ok);
            emit SendDebugInfo(LOGGER_ERROR,"Error creando nueva composición");
        }
        else
            emit SendDebugInfo(LOGGER_ERROR,"Nueva Composición");
    }

    ui->m_AceptarButton->setEnabled(true);
    //ui->m_CancelButton->setEnabled(true);
    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);


}


int DbImpresion::CuadroDialogoNuevaComposicion(){

    QFileDialog fileName(this);
    QString tmp,dummy;
    QStringList Ficheros;
    fileName.setAcceptMode(QFileDialog::AcceptSave);
    fileName.setFileMode(QFileDialog::AnyFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("Archivo de composición (*.ini)"));
    fileName.setDefaultSuffix("ini");
    fileName.setDirectory(memoryMap->getImagePath());
    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
            w->setAutoDefault(false);
     }*/
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();

        if (!Ficheros.isEmpty()){
            tmp=Ficheros.at(0);
            QFile File(tmp);
            if (!File.open(QIODevice::WriteOnly)) return -1;

            memoryMap->InitComposicion();

            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

            QString ruta,nombre;

            ruta=DevuelveRuta(tmp);
            nombre=DevuelveNombre(tmp);

            QString nombre_bmp=nombre;
            nombre_bmp.replace(".ini",".bmp");
            QString nombreruta_bmp=tmp;
            nombreruta_bmp.replace(".ini",".bmp");


            memoryMap->m_mapa.m_variables.NombreIni=nombre;
            memoryMap->m_mapa.m_variables.RutaIni=ruta;
            memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

            //Guardo los originales
            memoryMap->JumpToTempWorld();

            QString aux=memoryMap->m_mapa.m_variables.NombreIni;
            memoryMap->m_mapa.m_variables.NombreBmp=aux.replace(".ini",".bmp");/*nombre_bmp*/
            memoryMap->m_mapa.m_variables.RutaBmp=memoryMap->m_mapa.m_variables.RutaIni;/*ruta*/;
            aux=memoryMap->m_mapa.m_variables.NombreRutaIni;
            memoryMap->m_mapa.m_variables.NombreRutaBmp=aux.replace(".ini",".bmp");/*nombreruta_bmp;*/

            memoryMap->SetCredentialsCreated();
            //Valores para que la primera visualizacion no sea nada
            memoryMap->m_mapa.m_variables.NumColores=1;
            memoryMap->m_mapa.m_variables.AnchoPlantilla=DEFAULT_WEB_WIDTH;
            memoryMap->m_mapa.m_variables.AltoPlantilla=DEFAULT_WEB_HEIGHT;



            QString nombrecampo=QString("Lote");
            //strncpy(memoryMap->m_mapa.m_campos[0].NombreCampo,nombrecampo.toAscii().data(),CAMPOS_LEN);
            memoryMap->m_mapa.m_campos[0].NombreCampo=nombrecampo;
            memoryMap->m_mapa.m_campos[0].OnOffCampo=1;
            ui->m_OnOffCampo1->setChecked(true);
            ui->m_NombreCampo1->setText(QString("%1").arg(nombrecampo));

            nombrecampo=QString("Fecha caducidad");
            //strncpy(memoryMap->m_mapa.m_campos[1].NombreCampo,nombrecampo.toAscii().data(),CAMPOS_LEN);
            memoryMap->m_mapa.m_campos[1].NombreCampo=nombrecampo;
            memoryMap->m_mapa.m_campos[1].OnOffCampo=1;
            ui->m_OnOffCampo2->setChecked(true);
            ui->m_NombreCampo2->setText(QString("%1").arg(nombrecampo));
            //Reseteo resto de campos
            this->ui->m_NombreCampo3->clear();
            this->ui->m_NombreCampo4->clear();
            this->ui->m_NombreCampoVar1->clear();
            this->ui->m_NombreCampoVar2->clear();
            this->ui->m_NombreCampoVar3->clear();
            this->ui->m_NombreCampoVar4->clear();
            this->ui->m_NombreCampoVar5->clear();
            this->ui->m_NombreCampoVar6->clear();
            this->ui->m_ValorCampo1->clear();
            this->ui->m_ValorCampo2->clear();
            this->ui->m_ValorCampo3->clear();
            this->ui->m_ValorCampo4->clear();
            this->ui->m_ValorCampoVar1->clear();
            this->ui->m_ValorCampoVar2->clear();
            this->ui->m_ValorCampoVar3->clear();
            this->ui->m_ValorCampoVar4->clear();
            this->ui->m_ValorCampoVar5->clear();
            this->ui->m_ValorCampoVar6->clear();
            this->ui->m_OnOffCampo3->setChecked(false);
            this->ui->m_OnOffCampo4->setChecked(false);
            this->ui->m_OnOffCampoVar1->setChecked(false);
            this->ui->m_OnOffCampoVar2->setChecked(false);
            this->ui->m_OnOffCampoVar3->setChecked(false);
            this->ui->m_OnOffCampoVar4->setChecked(false);
            this->ui->m_OnOffCampoVar5->setChecked(false);
            this->ui->m_OnOffCampoVar6->setChecked(false);
            sltPrevisualizarZoom();


            GuardarFicheroIni();
            File.close();

            ui->m_NombreFicheroIni->setText(QString("%1").arg(tmp));
            memoryMap->m_ConfiguracionCargada=true;
            memoryMap->m_UltimaAccion=UA_CONFIGURACION_CARGADA;

            TestRoot();

            QMessageBox::information(0,"INFO","Nueva composición generada",QMessageBox::Ok);
            return 0;
        }
        else return -1;//JORGE
    }
    else return -1;//JORGE

}


/** Comprueba si hay campos vacíos*/
bool DbImpresion::Comprobar_Campos_Vacios(){

    bool aux;

    QString valor_campo1;
    QString valor_campo2;
    QString valor_campo3;
    QString valor_campo4;

    valor_campo1=QString(memoryMap->m_mapa.m_campos[0].ValorActualCampo);
    valor_campo2=QString(memoryMap->m_mapa.m_campos[1].ValorActualCampo);
    valor_campo3=QString(memoryMap->m_mapa.m_campos[2].ValorActualCampo);
    valor_campo4=QString(memoryMap->m_mapa.m_campos[3].ValorActualCampo);

    if (
            ((memoryMap->m_mapa.m_campos[0].OnOffCampo==1) && (valor_campo1=="")) ||
            ((memoryMap->m_mapa.m_campos[1].OnOffCampo==1) && (valor_campo2=="")) ||
            ((memoryMap->m_mapa.m_campos[2].OnOffCampo==1) && (valor_campo3=="")) ||
            ((memoryMap->m_mapa.m_campos[3].OnOffCampo==1) && (valor_campo4==""))
        )
        aux=true;
    else
        aux=false;

    return (aux);

}
/** Comprueba si la orden de fabricación está vacía*/
bool DbImpresion::Comprobar_OrdenFabricacion_Vacia(){
    if (memoryMap->m_mapa.m_variables.OrdenFabricacion.isEmpty()) return true;
    else return false;
}
/** Comprueba si existe una compisición dada*/
bool DbImpresion::Comprobar_ExisteComposicion(){

    bool aux;

    QString nombreruta_ini,nombreruta_bmp;

    nombreruta_ini=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);

    nombreruta_bmp=nombreruta_ini;
    nombreruta_bmp.remove(".ini");
    nombreruta_bmp.append(QString("_OF%1_C1_comp.bmp").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));

    aux=QFile::exists(nombreruta_bmp);

    return (aux);

}

void DbImpresion::UpdateTablaImprimir(){
    InputData *input = InputData::request(this,"Nombre de la tabla a imprimir");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_TablaImprimir(QString)));
        input->setFocus();
    }
}

void DbImpresion::Update_TablaImprimir(QString value){
    ui->m_NombreDb->setText(value);
    ui->m_NombreDb->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    //strncpy(memoryMap->m_mapa.m_variables.NombreTablaDatosImprimir,m_NombreTablaImprimir->toPlainText().toAscii(),CAMPOS_LEN);
}

void DbImpresion::AbrirTablaImprimir(){

}
/** Carga BBDD*/
void DbImpresion::LoadBBDD(QString rutadb){
    miDataBaseCampos.close(); //Por si hay algo abierto
    if (miDataBaseCampos.open(rutadb)!=true){
        emit SendDebugInfo(LOGGER_ERROR,QString("Error abriendo base de datos %1").arg(rutadb));
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,QString("Base de datos %1 abierta correctamente").arg(rutadb));
        ui->m_NombreDb->setText(rutadb);
        memoryMap->m_mapa.m_database.NombreRutaDb=rutadb;
        LoadNombresBBDD();
        memoryMap->m_ConfiguracionCargada=true;
    }
}
/** Abre BBDD*/
void DbImpresion::AbrirBBDD(){

    QFileDialog fileName;
    QStringList Ficheros;
    QString rutadb;


    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(("Database Files (*.mdb)"));
    fileName.setDirectory(memoryMap->getImagePath());
    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
            w->setAutoDefault(false);
     }*/

    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
            rutadb=QString(Ficheros.at(0));
            int index=rutadb.lastIndexOf("/");
            memoryMap->m_mapa.m_database.NombreDb=rutadb.right(rutadb.length()-(index+1));
            memoryMap->m_mapa.m_database.RutaDb=rutadb.left(index);
            /*indice1=rutadb.lastIndexOf("/");
            indice2=rutadb.lastIndexOf("\\");
            if (indice1>indice2) indice=indice1;
            else indice=indice2;
            InkjetPathDbCampos=rutadb.left(indice);*/

        }
    }
    LoadBBDD(rutadb);

}


void DbImpresion::Comprobar_Campos_Variables(){

    if ( ((ui->m_OnOffCampoVar1->isChecked())==true) ||
         ((ui->m_OnOffCampoVar2->isChecked())==true) ||
         ((ui->m_OnOffCampoVar3->isChecked())==true) ||
         ((ui->m_OnOffCampoVar4->isChecked())==true) ||
         ((ui->m_OnOffCampoVar5->isChecked())==true) ||
         ((ui->m_OnOffCampoVar6->isChecked())==true)  )
            memoryMap->ImagenVariable=0x01;
    else
            memoryMap->ImagenVariable=0x00;

}
//Llama a la base de datos y chequea la configuracion
void DbImpresion::CheckConfigCampos(){
    if (memoryMap->m_mapa.m_database.NombreTablaDatosImprimir.isEmpty()) memoryMap->m_mapa.m_database.NombreTablaDatosImprimir=QString("DatosImprimir");
    QString tmp_valor=QString("select * from %1").arg(memoryMap->m_mapa.m_database.NombreTablaDatosImprimir);
    miDataBaseCampos.ValoresDB(tmp_valor);
    miDataBaseCampos.exec(tmp_valor);
    int temp_campos=0;
        for(unsigned int i=0;i<memoryMap->m_mapa.m_database.NumCamposTotalesDB;i++){
            QString tmp=QString(miDataBaseCampos.NombreCampo[i]);
            if (!tmp.isEmpty()){
                if (i==0) {
                    ui->m_NombreCampoVar1->setText(tmp);
                    ui->m_ValorCampoVar1->setText(miDataBaseCampos.m_Tabla[0]);
                    //ui->m_OnOffCampoVar1->setChecked(true);
                    //memoryMap->m_mapa.m_camposvariables[0].OnOffCampo=1;
                    memoryMap->m_mapa.m_camposvariables[0].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==1) {
                    ui->m_NombreCampoVar2->setText(tmp);
                    ui->m_ValorCampoVar2->setText(miDataBaseCampos.m_Tabla[1]);
                    //ui->m_OnOffCampoVar2->setChecked(true);
                    //memoryMap->m_mapa.m_camposvariables[1].OnOffCampo=1;
                    memoryMap->m_mapa.m_camposvariables[1].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==2){
                    ui->m_NombreCampoVar3->setText(tmp);
                    ui->m_ValorCampoVar3->setText(miDataBaseCampos.m_Tabla[2]);
                    //ui->m_OnOffCampoVar3->setChecked(true);
                    //memoryMap->m_mapa.m_camposvariables[2].OnOffCampo=1;
                    memoryMap->m_mapa.m_camposvariables[2].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==3) {
                    ui->m_NombreCampoVar4->setText(tmp);
                    ui->m_ValorCampoVar4->setText(miDataBaseCampos.m_Tabla[3]);
                    //ui->m_OnOffCampoVar4->setChecked(true);
                    //memoryMap->m_mapa.m_camposvariables[3].OnOffCampo=1;
                    memoryMap->m_mapa.m_camposvariables[3].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==4) {
                    ui->m_NombreCampoVar5->setText(tmp);
                    ui->m_ValorCampoVar5->setText(miDataBaseCampos.m_Tabla[4]);
                    //ui->m_OnOffCampoVar5->setChecked(true);
                    //memoryMap->m_mapa.m_camposvariables[4].OnOffCampo=1;
                    memoryMap->m_mapa.m_camposvariables[4].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==5) {
                    ui->m_NombreCampoVar6->setText(tmp);
                    ui->m_ValorCampoVar6->setText(miDataBaseCampos.m_Tabla[5]);
                   // ui->m_OnOffCampoVar6->setChecked(true);
                   // memoryMap->m_mapa.m_camposvariables[5].OnOffCampo=1;
                    memoryMap->m_mapa.m_camposvariables[5].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }

                temp_campos++;
            }
        }
        memoryMap->m_mapa.m_database.NumCamposTotalesDB=temp_campos;
}


/** Carga los nombres de la BBDD*/
void DbImpresion::LoadNombresBBDD(){

    miDataBaseCampos.NumeroCamposDB();
    CheckConfigCampos();

    DataCommon->ShowInfo();
}

/** Cargamos datos comunes*/
void DbImpresion::setDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=NULL;
    dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
    DataCommon=new InkjetCommon(this,dummyDataCommon);

//Actualizamos
    DataCommon->ShowInfo();
    DataCommon->ShowEstadoImpresora();
    DataCommon->ShowComms();
}


/** Metemos el trabajo actual en la cola de impresion*/
void DbImpresion::InsertarEnColaImpresion(){

    //Primero hay que ver si está este trabajo en la cola de impresion
    //y luego hacer un update o un insert

    QString tmp;
    int numero_resultados=0;




    int respuesta=QMessageBox::question(this,"ATENCION","¿Desea incluir el trabajo actual en la cola de impresión?","Si","No",QString::null, 0, 1 );

    if (respuesta==0){
        memoryMap->ReturnToOriginalWorld();
        QString nombreruta,nombre,ruta;
        nombreruta=memoryMap->m_FilePath;
        nombre=DevuelveNombre(nombreruta);
        ruta=DevuelveRuta(nombreruta);

        tmp=QString("select * from ColaImpresion where Composicion='%1' and Path='%2'")
                .arg(nombre)
                .arg(ruta);
        if (miDataBaseColaImpresion.exec(tmp.toAscii().data())!=true){
        }
        else{
            numero_resultados=miDataBaseColaImpresion.m_Tabla.count();
        }

        QString fechahora=QString("%1 %2").arg(QDate::currentDate().toString("yyyy-MM-dd")).arg(QTime::currentTime().toString("hh:mm:ss"));

        if (numero_resultados==0){
            tmp=QString("insert into ColaImpresion ('Composicion','Path','Fecha','OrdenFabricacion') Values ('%1','%2','%3','%4')")
                    .arg(nombre)
                    .arg(ruta)
                    .arg(fechahora)
                    .arg(memoryMap->m_mapa.m_variables.OrdenFabricacion);

            if (miDataBaseColaImpresion.exec(tmp.toAscii().data())!=true){
            }

        }
        else{
            tmp=QString("update ColaImpresion set Fecha='%1' where Composicion='%2' AND Path='%3'")
                    .arg(fechahora)
                    .arg(nombre)
                    .arg(ruta);

            if (miDataBaseColaImpresion.exec(tmp.toAscii().data())!=true){
            }
        }
        memoryMap->ReturnToTempWorld();
    } //(respuesta==0)
}

/* Devuelve el numero de cabezales
 *Originalmente devolvia todos los cabezales....de momento nos basta con los cabezales de 1 Xusb*/

void DbImpresion::UpdateNumCabezales(){
    /*m_NumCabezales=0;
    int m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();
    emit SendDebugInfo(LOGGER_WARNING, QString("%1 XusbDetected").arg(m_iTotalXUSB));
    for (int Card=0;Card<m_iTotalXUSB;Card++){
        InkjetXusbCtrl *newCard=new InkjetXusbCtrl(0,Card);
        //Para cada tarjeta habrá que saber los cabezales (width)
        m_NumCabezales+=newCard->returnNumCabezales();
        delete (newCard);
    }
    emit SendDebugInfo(LOGGER_WARNING, QString("%1 Heads detected").arg(m_NumCabezales));*/
    m_NumCabezales=0;
    int m_iTotalXUSB=(int)XaarScorpionGetXUSBCount();
    emit SendDebugInfo(LOGGER_ERROR, QString("%1 XusbDetected").arg(m_iTotalXUSB));
    int Card=0;
    InkjetXusbCtrl *newCard=new InkjetXusbCtrl(0,Card);
    //Para cada tarjeta habrá que saber los cabezales (width)
    m_NumCabezales+=newCard->returnNumCabezales();
    delete (newCard);
    emit SendDebugInfo(LOGGER_WARNING, QString("%1 Heads detected").arg(m_NumCabezales));
}

/** Funcion In2 que intenta eliminar posibles errores de blancos*/
void DbImpresion::PreprocessImagesIn2(){

    QString StringBlack=memoryMap->m_FilePath;
    StringBlack.remove(QString("_C1_comp.bmp"));
    StringBlack.append(QString("_C1_comp.bmp"));
    QString StringRed=memoryMap->m_FilePath;
    StringRed.remove(QString("_C1_comp.bmp"));
    StringRed.append(QString("_C2_comp.bmp"));
    QString StringBase=memoryMap->m_FilePath;
    StringBase.remove("_C1_comp.bmp");
    StringBase.append("_C1.bmp");
    QFile fileBlack(StringBlack);
    QFile fileRed(StringRed);
    QFile fileBase(StringBase);

    if (fileBlack.exists()&&fileRed.exists()/*&&fileBase.exists()*/){
        QImage ImageBlack(StringBlack);
        QImage ImageRed(StringRed);
        //QImage ImageBase(StringBase);
        QRgb baseColor=QColor(Qt::white).rgb();
        //Buscamos primera ocurrencia en el rojo
        QRgb destColor=QColor(Qt::white).rgb();
        QRgb grdColor=QColor(Qt::white).rgb();
        for (int x=0;x<ImageRed.width();x++){
            if (grdColor!=destColor) break;
            for (int y=0;y<ImageRed.height();y++){
                if (ImageRed.pixel(x,y)!=destColor){
                    destColor=ImageRed.pixel(x,y);
                    break;
                }
            }
        }

        QVector<QRgb> tabla=ImageRed.colorTable();

        int DestIndex=0;
        for(DestIndex=0;DestIndex<tabla.size();DestIndex++){
            if (destColor==tabla.at(DestIndex)) break;
        }

        for (int x=1;x<ImageBlack.width()-1;x++) {
            for (int y=1;y<ImageBlack.height()-1;y++){
                if (ImageBlack.pixel(x,y)!=baseColor){
                    if ((ImageRed.pixel(x-1,y)!=baseColor)||(ImageRed.pixel(x+1,y)!=baseColor)||(ImageRed.pixel(x,y+1)!=baseColor)||(ImageRed.pixel(x,y-1)!=baseColor)){
                        ImageRed.setPixel(x,y,DestIndex);
                    }
                }
            }
        }
        ImageRed.save(StringRed);
    }
}

/** Muestra propiedades de la composición*/
void DbImpresion::ShowCompositionProps(){
    ui->m_tePropiedades->setText(QString("Creado:%1").arg(memoryMap->m_mapa.m_variables.CreadoPor));
    ui->m_tePropiedades->append(QString("Fecha:%1").arg(memoryMap->m_mapa.m_variables.CreadoCuando));
    ui->m_tePropiedades->append(QString("Modificado:%1").arg(memoryMap->m_mapa.m_variables.ModificadoPor));
    ui->m_tePropiedades->append(QString("Fecha:%1").arg(memoryMap->m_mapa.m_variables.ModificadoCuando));

    QString calculatedHash=CalculateHashCompoFile();
    QString hashInFile=GetHashCompoFile();
    if (hashInFile.isEmpty()){
        ui->m_tePropiedades->append("No hay hash en fichero de composicion!");
        emit SendDebugInfo(LOGGER_WARNING, QString("No hay hash en fichero de composicion!"));
    }
    else{
        if (calculatedHash==hashInFile){
            ui->m_tePropiedades->append("Hash correcto");
            emit SendDebugInfo(LOGGER_WARNING, QString("Hash correcto %1").arg(calculatedHash));
        }
        else{
            ui->m_tePropiedades->append("HASH INCORRECTO");
            emit SendDebugInfo(LOGGER_WARNING, QString("HASH INCORRECTO Leido:%1 Calculado:%2").arg(hashInFile).arg(calculatedHash));
        }
    }

}

void DbImpresion::EnableWidgetsCampo1(){
    ui->m_OnOffCampo1->setEnabled(true);
    UpdateOnOff1();
}

void DbImpresion::DisableWidgetsCampo1(){
    ui->m_OnOffCampo1->setEnabled(false);
    ui->m_OnOffCampo1->setChecked(false);
    UpdateOnOff1();
}


void DbImpresion::EnableWidgetsCampo2(){
    ui->m_OnOffCampo2->setEnabled(true);
    UpdateOnOff2();
}

void DbImpresion::DisableWidgetsCampo2(){
    ui->m_OnOffCampo2->setEnabled(false);
    ui->m_OnOffCampo2->setChecked(false);
    UpdateOnOff2();
}


void DbImpresion::EnableWidgetsCampo3(){
    ui->m_OnOffCampo3->setEnabled(true);
    UpdateOnOff3();
}

void DbImpresion::DisableWidgetsCampo3(){
    ui->m_OnOffCampo3->setEnabled(false);
    ui->m_OnOffCampo3->setChecked(false);
    UpdateOnOff3();
}


void DbImpresion::EnableWidgetsCampo4(){
    ui->m_OnOffCampo4->setEnabled(true);
    UpdateOnOff4();
}

void DbImpresion::DisableWidgetsCampo4(){
    ui->m_OnOffCampo4->setEnabled(false);
    ui->m_OnOffCampo4->setChecked(false);
    UpdateOnOff4();
}


void DbImpresion::EnableWidgetsCampoVar1(){
    ui->m_OnOffCampoVar1->setEnabled(true);
    UpdateOnOffVar1();
}

void DbImpresion::DisableWidgetsCampoVar1(){
    ui->m_OnOffCampoVar1->setEnabled(false);
    ui->m_OnOffCampoVar1->setChecked(false);
    UpdateOnOffVar1();
}


void DbImpresion::EnableWidgetsCampoVar2(){
    ui->m_OnOffCampoVar2->setEnabled(true);
    UpdateOnOffVar2();
}

void DbImpresion::DisableWidgetsCampoVar2(){
    ui->m_OnOffCampoVar2->setEnabled(false);
    ui->m_OnOffCampoVar2->setChecked(false);
    UpdateOnOffVar2();
}


void DbImpresion::EnableWidgetsCampoVar3(){
    ui->m_OnOffCampoVar3->setEnabled(true);
    UpdateOnOffVar3();
}

void DbImpresion::DisableWidgetsCampoVar3(){
    ui->m_OnOffCampoVar3->setEnabled(false);
    ui->m_OnOffCampoVar3->setChecked(false);
    UpdateOnOffVar3();
}


void DbImpresion::EnableWidgetsCampoVar4(){
    ui->m_OnOffCampoVar4->setEnabled(true);
    UpdateOnOffVar4();
}

void DbImpresion::DisableWidgetsCampoVar4(){
    ui->m_OnOffCampoVar4->setEnabled(false);
    ui->m_OnOffCampoVar4->setChecked(false);
    UpdateOnOffVar4();
}


void DbImpresion::EnableWidgetsCampoVar5(){
    ui->m_OnOffCampoVar5->setEnabled(true);
    UpdateOnOffVar5();
}

void DbImpresion::DisableWidgetsCampoVar5(){
    ui->m_OnOffCampoVar5->setEnabled(false);
    ui->m_OnOffCampoVar5->setChecked(false);
    UpdateOnOffVar5();
}


void DbImpresion::EnableWidgetsCampoVar6(){
    ui->m_OnOffCampoVar6->setEnabled(true);
    UpdateOnOffVar6();
}

void DbImpresion::DisableWidgetsCampoVar6(){
    ui->m_OnOffCampoVar6->setEnabled(false);
    ui->m_OnOffCampoVar6->setChecked(false);
    UpdateOnOffVar6();
}

void DbImpresion::visualizarIni(){
    viewIni *ScrviewIni;
    ScrviewIni=new viewIni(0);
    ScrviewIni->show();
}


