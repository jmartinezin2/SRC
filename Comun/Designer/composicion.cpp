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

#include "Composicion.h"
#include "EditCampos.h"
#include "EditComposicion.h"
#include "EditImagenFija.h"
//#include "globals.h"
#include "in2defines.h"
#include "InputData.h"
#include "memoryMap.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
//#include "principal.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTextCodec>
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
//#include "GestorDB_ODBC.h"
#include "GestorDB_SQLITE.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <in2message.h>
#include <QProcess>
#include "in2imagecreator.h"
#include "In2GraphicsView.h"
#include "in2filedialog.h"
//#include "clienteopc.h"
#include "in2compofile.h"
#include "in2inkcalclauncher.h"
#include "in2configuratorapp.h"
#include "in2previsualizator.h"
#include <QPrintDialog>
//#include "in2managerdiametros.h"
#include "in2managererp.h"
#include "In2ColaImpresion.h"
#include "In2ColaImpresionMono.h"
#include "in2gramarcolaimpresion.h"
#include "inkjetdebug.h"
#ifndef NO_VISION
    #include "in2aisvisionselector.h"
#endif

#include "gsrenderengine.h"
#include "in2configuratordesigner.h"

#define DEFAULT_PIXEL_NUMBER 2000
#define DEFAULT_WIDTH_HEAD 71

#include <QTranslator>

#include "In2BarcodeClass.h"
#include "viewini.h"
#include "compileversion.h"
#include "in2ditheralg.h"
#include "in2gsmonitor.h"

QWidget *Composicion::m_parent  = NULL;

Composicion::Composicion(QWidget *parent)
    : /*InkjetForm(parent)*/ ui(new Ui::ComposicionUI)
{

    ui->setupUi(this);


    ui->m_pbRender->setValue(0);
    /*QPrintDialog p;
    p.show();*/
    m_memoryMap=MemoryMap::request(0);
    //ui->retranslateUi(this);
    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    GlobalSetScreenSize(this);
    InkjetDebug *debug=InkjetDebug::request(0);
    connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),debug,SLOT(ProcessMsg(unsigned char,QString)));

    ui->m_ValorCampo1->setPalette(dummyPaletteEnabled);
    ui->m_ValorCampo2->setPalette(dummyPaletteEnabled);
    ui->m_ValorCampo3->setPalette(dummyPaletteEnabled);
    ui->m_ValorCampo4->setPalette(dummyPaletteEnabled);
    //m_parent=NULL;

    In2ConfiguratorDesigner config;
    QPalette pal;
    pal.setColor(QPalette::Base,QColor((QRgb)config.getColor1()));
    ui->m_leC1->setPalette(pal);
    pal.setColor(QPalette::Base,QColor((QRgb)config.getColor2()));
    ui->m_leC2->setPalette(pal);
    pal.setColor(QPalette::Base,QColor((QRgb)config.getColor3()));
    ui->m_leC3->setPalette(pal);
    pal.setColor(QPalette::Base,QColor((QRgb)config.getColor4()));
    ui->m_leC4->setPalette(pal);
    int numColors=config.getNumColors();
    if (numColors==2){
        ui->m_leP11_2->setEnabled(true);
        ui->m_leP12_2->setEnabled(true);
        ui->m_leP13_2->setEnabled(true);
        ui->m_leP14_2->setEnabled(true);
        ui->m_leP15_2->setEnabled(true);
        ui->m_leP16_2->setEnabled(true);
        ui->m_leP17_2->setEnabled(true);
        ui->m_leP18_2->setEnabled(true);
    }
    else if (numColors==3){
        ui->m_leP11_2->setEnabled(true);
        ui->m_leP12_2->setEnabled(true);
        ui->m_leP13_2->setEnabled(true);
        ui->m_leP14_2->setEnabled(true);
        ui->m_leP15_2->setEnabled(true);
        ui->m_leP16_2->setEnabled(true);
        ui->m_leP17_2->setEnabled(true);
        ui->m_leP18_2->setEnabled(true);

        ui->m_leP11_3->setEnabled(true);
        ui->m_leP12_3->setEnabled(true);
        ui->m_leP13_3->setEnabled(true);
        ui->m_leP14_3->setEnabled(true);
        ui->m_leP15_3->setEnabled(true);
        ui->m_leP16_3->setEnabled(true);
        ui->m_leP17_3->setEnabled(true);
        ui->m_leP18_3->setEnabled(true);
    }
    else if (numColors==4){
        ui->m_leP11_2->setEnabled(true);
        ui->m_leP12_2->setEnabled(true);
        ui->m_leP13_2->setEnabled(true);
        ui->m_leP14_2->setEnabled(true);
        ui->m_leP15_2->setEnabled(true);
        ui->m_leP16_2->setEnabled(true);
        ui->m_leP17_2->setEnabled(true);
        ui->m_leP18_2->setEnabled(true);

        ui->m_leP11_3->setEnabled(true);
        ui->m_leP12_3->setEnabled(true);
        ui->m_leP13_3->setEnabled(true);
        ui->m_leP14_3->setEnabled(true);
        ui->m_leP15_3->setEnabled(true);
        ui->m_leP16_3->setEnabled(true);
        ui->m_leP17_3->setEnabled(true);
        ui->m_leP18_3->setEnabled(true);

        ui->m_leP11_4->setEnabled(true);
        ui->m_leP12_4->setEnabled(true);
        ui->m_leP13_4->setEnabled(true);
        ui->m_leP14_4->setEnabled(true);
        ui->m_leP15_4->setEnabled(true);
        ui->m_leP16_4->setEnabled(true);
        ui->m_leP17_4->setEnabled(true);
        ui->m_leP18_4->setEnabled(true);

    }


    ui->m_leHeight->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.WebHeight));
    ui->m_leWidth->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.WebWidth/*m_memoryMap->m_mapa.m_variables.DefaultNumHeads*DEFAULT_HEAD_WIDTH/FACTOR_PIXELS_MM*/));
    ui->m_leMaxWidth->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.DefaultNumHeads*70));
    connect(ui->m_leHeight,SIGNAL(editingFinished()),this,SLOT(sltRefreshScreen()));
    connect(ui->m_leWidth,SIGNAL(editingFinished()),this,SLOT(sltRefreshScreen()));


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

    ui->m_OrigenCampoVar1->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo);
    ui->m_OrigenCampoVar2->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo);
    ui->m_OrigenCampoVar3->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo);
    ui->m_OrigenCampoVar4->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo);
    ui->m_OrigenCampoVar5->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo);
    ui->m_OrigenCampoVar6->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo);


    connect(ui->m_pbCancelar,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
    connect(ui->m_PrintButton,SIGNAL(clicked()),this,SLOT(ComprobacionesGenerateImage()));
    connect(ui->m_FormatButton,SIGNAL(clicked()),this,SLOT(LaunchEditCampos()));
    connect(ui->m_FormatPageButton,SIGNAL(clicked()),this,SLOT(LaunchEditComposicion()));
    connect(ui->m_FormatPlantillaButton,SIGNAL(clicked()),this,SLOT(LaunchEditImagenFija()));
    connect(ui->m_pbVision,SIGNAL(clicked()),this,SLOT(sltConfigVision()));
    connect(ui->m_CargarFicheroIni,SIGNAL(clicked()),this,SLOT(CargarComposicion()));
    connect(ui->m_GuardarFicheroIni,SIGNAL(clicked()),this,SLOT(GuardarComposicion()));
    connect(ui->m_GuardarFicheroIniNow,SIGNAL(clicked()),this,SLOT(GuardarComposicionNow()));
    connect(ui->m_NuevoFicheroIni,SIGNAL(clicked()),this,SLOT(NuevaComposicion()));
    connect(ui->m_ViewIni,SIGNAL(clicked()),this,SLOT(visualizarIni()));
    connect(ui->m_ColaImpresion,SIGNAL(clicked()),this,SLOT(sltColaImpresion()));
    connect(ui->m_Info,SIGNAL(clicked()),this,SLOT(sltShowVersion()));
    connect(ui->m_config,SIGNAL(clicked()),this,SLOT(sltConfigApp()));
    connect(ui->m_rbSabre0,SIGNAL(clicked()),this,SLOT(sltChangeSabreIndex()));
    connect(ui->m_rbSabre1,SIGNAL(clicked()),this,SLOT(sltChangeSabreIndex()));
    connect(ui->m_rbSabre2,SIGNAL(clicked()),this,SLOT(sltChangeSabreIndex()));
    connect(ui->m_rbSabre3,SIGNAL(clicked()),this,SLOT(sltChangeSabreIndex()));

    In2InkCalcLauncher inkcalc;
    if (inkcalc.exists()){
        connect(ui->m_pbInkCalculator,SIGNAL(clicked()),this,SLOT(sltLaunchInkCalculator()));
    }
    else{
        ui->m_pbInkCalculator->setEnabled(false);
    }

    connect(ui->m_CargarDb,SIGNAL(clicked()),this,SLOT(AbrirBBDD()));
    //connect(ui->m_OrdenFabricacion,SIGNAL(selectionChanged()),this,SLOT(UpdateOrdenFabricacion()));
    connect(ui->m_NombreDb, SIGNAL(selectionChanged()),this, SLOT(UpdateTablaImprimir()));
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
    connect(ui->m_Reload,SIGNAL(clicked()),this,SLOT(sltRefreshScreen()));
    //Algoritmos de color
    //Zona de algoritmos de difusion
    connect(ui->m_cbBlancos1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_cbNegros1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_cbMono1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_rbDiffuse1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_rbFloyd1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_rbNone1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_rbThreshold1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_rbOrdered1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));
    connect(ui->m_cbMonoDiffuse1,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage1()));

    connect(ui->m_cbBlancos1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_cbNegros1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_cbMono1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_rbDiffuse1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_rbFloyd1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_rbNone1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_rbThreshold1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_rbOrdered1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));
    connect(ui->m_cbMonoDiffuse1_2,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage2()));

    connect(ui->m_cbBlancos1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_cbNegros1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_cbMono1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_rbDiffuse1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_rbFloyd1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_rbNone1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_rbThreshold1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_rbOrdered1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));
    connect(ui->m_cbMonoDiffuse1_3,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage3()));

    connect(ui->m_cbBlancos1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_cbNegros1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_cbMono1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_rbDiffuse1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_rbFloyd1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_rbNone1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_rbThreshold1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_rbOrdered1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));
    connect(ui->m_cbMonoDiffuse1_4,SIGNAL(clicked(bool)),this,SLOT(sltUpdateAlgImage4()));

    connect(ui->m_pbConfigDith1,SIGNAL(clicked(bool)),this,SLOT(sltConfigureDith1()));
    connect(ui->m_pbConfigDith2,SIGNAL(clicked(bool)),this,SLOT(sltConfigureDith2()));
    connect(ui->m_pbConfigDith3,SIGNAL(clicked(bool)),this,SLOT(sltConfigureDith3()));
    connect(ui->m_pbConfigDith4,SIGNAL(clicked(bool)),this,SLOT(sltConfigureDith4()));


    m_view=new In2GraphicsView(ui->m_GraphicsView);
    m_scene=NULL;
    GsRenderEngine render;
    bool bGs=render.testGsInstallation();
    if (!bGs){
        int rtn=QMessageBox::warning(this,"ERROR",tr("No se detecta motor de renderizado.Quiere instalarlo?"),QMessageBox::Ok,QMessageBox::Cancel);
        if (rtn==QMessageBox::Ok){
            render.install();
        }

    }

    m_scene=new In2GraphicsScene(0,0,m_memoryMap->m_mapa.m_variables.DefaultNumHeads*1000,1000);
    In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
    QPixmap foo(m_memoryMap->m_mapa.m_variables.DefaultNumHeads*1000,1000);
    foo.fill(QColor(Qt::white));
    p->setPixmap(foo);
    m_scene->addItem(p);
    ui->m_GraphicsView->setScene(m_scene);
    //ui->m_GraphicsView->fitInView(p,Qt::KeepAspectRatio);
    ui->m_GraphicsView->show();
    this->sltEnableGrid(true);


    if ((!m_memoryMap->m_mapa.m_variables.NombreRutaIni.isEmpty())&&(bGs)){
        CargaDatosCampos();
        CargaDatosCamposVariables();
        CargaBBDD();
        QString foo=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
        foo.replace("/","\\");
        ui->m_NombreFicheroIniTmp->setText(foo);
        foo=m_memoryMap->m_mapa.m_variables.savedNombreRutaIni;
        foo.replace("/","\\");
        ui->m_NombreFicheroIni->setText(foo);
        QTimer::singleShot(0,this,SLOT(sltPrevisualizarZoom()));
    }
    UpdateNumCabezales();
	TestRoot();
    ui->tabWidget->setCurrentIndex(0);
//Si no esta definido el modo de variable o depuraci�n, no dejo entrar
#if !defined (MODULO_VARINFO) && !defined(DEPURACION_IN2)
    ui->tabWidget->setTabEnabled(2,false);
#endif

    sltTestERP();

}

/** Actualiza la PB*/
void Composicion::sltUpdatePb(int value, int total){
    ui->m_pbRender->setMaximum(total);
    int actual=ui->m_pbRender->value();
    actual+=value;
    if (actual>=total){
        QTimer::singleShot(1000,this,SLOT(sltResetPb()));
        actual=total;
    }
    ui->m_pbRender->setValue(actual);


}
/** Resetea la PB*/
void Composicion::sltResetPb(){
    ui->m_pbRender->setValue(0);
}


/** Actualiza la imagen 1*/
void Composicion::sltUpdateAlgImage1(){
    m_memoryMap->m_mapa.m_variables.bReloadCompo=true;
    if (ui->m_rbDiffuse1->isChecked()) m_memoryMap->m_mapa.m_colores[0].alg=TXT_DIFFUSE;
    else if (ui->m_rbOrdered1->isChecked()) m_memoryMap->m_mapa.m_colores[0].alg=TXT_ORDERED;
    else if (ui->m_rbThreshold1->isChecked()) m_memoryMap->m_mapa.m_colores[0].alg=TXT_THRESHOLD;
    else if (ui->m_rbFloyd1->isChecked()) m_memoryMap->m_mapa.m_colores[0].alg=TXT_FLOYD;
    else m_memoryMap->m_mapa.m_colores[0].alg.clear();
    if (ui->m_cbBlancos1->isChecked()) m_memoryMap->m_mapa.m_colores[0].bWhiteCorr=true;
    else m_memoryMap->m_mapa.m_colores[0].bWhiteCorr=false;
    if (ui->m_cbMono1->isChecked()) m_memoryMap->m_mapa.m_colores[0].bMonoMode=true;
    else m_memoryMap->m_mapa.m_colores[0].bMonoMode=false;
    if (ui->m_cbNegros1->isChecked()) m_memoryMap->m_mapa.m_colores[0].bBlackCorr=true;
    else m_memoryMap->m_mapa.m_colores[0].bBlackCorr=false;    
    if (ui->m_cbMonoDiffuse1->isChecked())  m_memoryMap->m_mapa.m_colores[0].bMonoCorr=true;
    else m_memoryMap->m_mapa.m_colores[0].bMonoCorr=false;
    sltRefreshScreen();
}

/** Actualiza la imagen 2*/
void Composicion::sltUpdateAlgImage2(){
    m_memoryMap->m_mapa.m_variables.bReloadCompo=true;
    if (ui->m_rbDiffuse1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].alg=TXT_DIFFUSE;
    else if (ui->m_rbOrdered1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].alg=TXT_ORDERED;
    else if (ui->m_rbThreshold1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].alg=TXT_THRESHOLD;
    else if (ui->m_rbFloyd1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].alg=TXT_FLOYD;
    else m_memoryMap->m_mapa.m_colores[1].alg.clear();
    if (ui->m_cbBlancos1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].bWhiteCorr=true;
    else m_memoryMap->m_mapa.m_colores[1].bWhiteCorr=false;
    if (ui->m_cbMono1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].bMonoMode=true;
    else m_memoryMap->m_mapa.m_colores[1].bMonoMode=false;
    if (ui->m_cbNegros1_2->isChecked()) m_memoryMap->m_mapa.m_colores[1].bBlackCorr=true;
    else m_memoryMap->m_mapa.m_colores[1].bBlackCorr=false;

    sltRefreshScreen();
}

/** Actualiza la imagen 3*/
void Composicion::sltUpdateAlgImage3(){
    m_memoryMap->m_mapa.m_variables.bReloadCompo=true;
    if (ui->m_rbDiffuse1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].alg=TXT_DIFFUSE;
    else if (ui->m_rbOrdered1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].alg=TXT_ORDERED;
    else if (ui->m_rbThreshold1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].alg=TXT_THRESHOLD;
    else if (ui->m_rbFloyd1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].alg=TXT_FLOYD;
    else m_memoryMap->m_mapa.m_colores[2].alg.clear();
    if (ui->m_cbBlancos1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].bWhiteCorr=true;
    else m_memoryMap->m_mapa.m_colores[2].bWhiteCorr=false;
    if (ui->m_cbMono1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].bMonoMode=true;
    else m_memoryMap->m_mapa.m_colores[2].bMonoMode=false;
    if (ui->m_cbNegros1_3->isChecked()) m_memoryMap->m_mapa.m_colores[2].bBlackCorr=true;
    else m_memoryMap->m_mapa.m_colores[2].bBlackCorr=false;

    sltRefreshScreen();
}

/** Actualiza la imagen 4*/
void Composicion::sltUpdateAlgImage4(){
    m_memoryMap->m_mapa.m_variables.bReloadCompo=true;
    if (ui->m_rbDiffuse1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].alg=TXT_DIFFUSE;
    else if (ui->m_rbOrdered1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].alg=TXT_ORDERED;
    else if (ui->m_rbThreshold1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].alg=TXT_THRESHOLD;
    else if (ui->m_rbFloyd1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].alg=TXT_FLOYD;
    else m_memoryMap->m_mapa.m_colores[3].alg.clear();
    if (ui->m_cbBlancos1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].bWhiteCorr=true;
    else m_memoryMap->m_mapa.m_colores[3].bWhiteCorr=false;
    if (ui->m_cbMono1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].bMonoMode=true;
    else m_memoryMap->m_mapa.m_colores[3].bMonoMode=false;
    if (ui->m_cbNegros1_4->isChecked()) m_memoryMap->m_mapa.m_colores[3].bBlackCorr=true;
    else m_memoryMap->m_mapa.m_colores[3].bBlackCorr=false;

    sltRefreshScreen();
}

/** Modifica el dither-1*/
void Composicion::sltConfigureDith1(){
    struct m_Colores color=m_memoryMap->m_mapa.m_colores[0];
    In2DitherAlg *p=new In2DitherAlg(0,color);
    connect(p,SIGNAL(sgnReload(m_Colores&)),this,SLOT(sltRefreshDitherInfo1(m_Colores&)));
    connect(this,SIGNAL(sgnImageGenerated()),p,SLOT(sltReloadPalette()));
    p->show();
}

/** Modifica el dither-1*/
void Composicion::sltRefreshDitherInfo1(struct m_Colores & color){
    m_memoryMap->m_mapa.m_colores[0]=color;
    sltUpdateAlgImage1();
}

/** Modifica el dither-2*/
void Composicion::sltConfigureDith2(){
    struct m_Colores color=m_memoryMap->m_mapa.m_colores[1];
    In2DitherAlg *p=new In2DitherAlg(0,color);
    connect(p,SIGNAL(sgnReload(m_Colores&)),this,SLOT(sltRefreshDitherInfo2(m_Colores&)));
    p->show();
}

/** Modifica el dither-2*/
void Composicion::sltRefreshDitherInfo2(struct m_Colores & color){
    m_memoryMap->m_mapa.m_colores[1]=color;
    sltUpdateAlgImage2();
}
/** Modifica el dither-3*/
void Composicion::sltConfigureDith3(){
    struct m_Colores color=m_memoryMap->m_mapa.m_colores[2];
    In2DitherAlg *p=new In2DitherAlg(0,color);
    connect(p,SIGNAL(sgnReload(m_Colores&)),this,SLOT(sltRefreshDitherInfo3(m_Colores&)));
    p->show();
}

/** Modifica el dither-3*/
void Composicion::sltRefreshDitherInfo3(struct m_Colores & color){
    m_memoryMap->m_mapa.m_colores[3]=color;
    sltUpdateAlgImage1();
}
/** Modifica el dither-4*/
void Composicion::sltConfigureDith4(){
    struct m_Colores color=m_memoryMap->m_mapa.m_colores[3];
    In2DitherAlg *p=new In2DitherAlg(0,color);
    connect(p,SIGNAL(sgnReload(m_Colores&)),this,SLOT(sltRefreshDitherInfo4(m_Colores&)));
    p->show();
}

/** Modifica el dither-4*/
void Composicion::sltRefreshDitherInfo4(struct m_Colores & color){
    m_memoryMap->m_mapa.m_colores[3]=color;
    sltUpdateAlgImage4();
}



/** Muestra la version*/
void Composicion::sltShowVersion(){
    CompileVersion *p=new CompileVersion(this);
    p->show();
}

/** Actualiza el Log*/
void Composicion::sltUpdateLog(QString txt){
    QString foo=QTime::currentTime().toString("hh:mm:ss");
    ui->m_tePropiedades->append(QString("[%1] %2").arg(foo).arg(txt));
}

/** Lanza la configuracion de vision*/
void Composicion::sltConfigVision(){
#ifndef NO_VISION
    In2AISVisionSelector *p=new In2AISVisionSelector(this);
    p->show();
#endif
}
/** Testea el ERP*/
void Composicion::sltTestERP(){
    In2ConfiguratorERP confERP;
    confERP.readConfig();
    //Inicializa el gestor de ERP que corresponda
    In2ManagerERP *erp=In2ManagerERP::request(0);
    QString client=erp->getClient();
    if (!client.compare(ERP_ESTEVE)){
        QString val=confERP.getFieldLote();
        ui->m_NombreCampo1->setText(val);
        ui->m_NombreCampo1->setEnabled(false);
        val=confERP.getFieldFchCad();
        ui->m_NombreCampo2->setText(val);
        ui->m_NombreCampo2->setEnabled(false);
        val=confERP.getFieldFchFab();
        ui->m_NombreCampo3->setText(val);
        ui->m_NombreCampo3->setEnabled(false);
        /*ui->m_OnOffCampo1->setChecked(true);
        ui->m_OnOffCampo2->setChecked(true);
        ui->m_OnOffCampo3->setChecked(true);
        ui->m_OnOffCampo1->setEnabled(false);
        ui->m_OnOffCampo2->setEnabled(false);
        ui->m_OnOffCampo3->setEnabled(false);*/
        sltUpdateLog("ERP configurado: SGL");
    }
    else if (!client.compare(ERP_GRAMAR)){
        sltUpdateLog("ERP configurado: GRAMAR");
        QString val=confERP.getFieldLote();
        ui->m_NombreCampo1->setText(val);
        ui->m_NombreCampo1->setEnabled(false);
        val=confERP.getFieldFchCad();
        ui->m_NombreCampo2->setText(val);
        ui->m_NombreCampo2->setEnabled(false);
    }

    else{
        sltUpdateLog("ERP configurado: IN2");
    }
}

/** Cambia el indice Sabre*/
void Composicion::sltChangeSabreIndex(){
    if (ui->m_rbSabre0->isChecked()) m_memoryMap->m_mapa.m_variables.iSabreAlgorithm=0;
    else if (ui->m_rbSabre1->isChecked()) m_memoryMap->m_mapa.m_variables.iSabreAlgorithm=1;
    else if (ui->m_rbSabre2->isChecked()) m_memoryMap->m_mapa.m_variables.iSabreAlgorithm=2;
    else if (ui->m_rbSabre3->isChecked()) m_memoryMap->m_mapa.m_variables.iSabreAlgorithm=3;
    sltPrevisualizarZoom();
}

/** Establece el parent*/
void Composicion::setIn2Parent(QWidget *p){
    m_parent=p;
}
/** Lanza la configuraci�n*/
void Composicion::sltConfigApp(){
    In2ConfiguratorApp *p=new In2ConfiguratorApp(this);
    p->setGridEnabled(m_scene->isGridEnabled());
    connect(p,SIGNAL(sgnEnableGrid(bool)),this,SLOT(sltEnableGrid(bool)));
    connect(this,SIGNAL(destroyed(QObject*)),p,SLOT(deleteLater()));
    p->show();
}
/** Habilita/deshabilita el grid*/
void Composicion::sltEnableGrid(bool enable){
    m_scene->enableGrid(enable);
}

/** Visualiza la cola de impresionn*/
void Composicion::sltColaImpresion(){
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
        connect(p,SIGNAL(sgnWorkLoaded(bool)),this,SLOT(sltLoadFromCola(bool)));
        connect(p,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
        connect(this,SIGNAL(destroyed()),p,SLOT(deleteLater()));
        p->show();

    }
    else{
        In2ColaImpresion *p;
        p=new In2ColaImpresion(this);
        connect(p,SIGNAL(sgnWorkLoaded()),this,SLOT(CargaDatos()));
        p->show();
    }
}

/** Hace un refesco de la pantalla*/
void Composicion::sltRefreshScreen(){
    m_memoryMap->m_mapa.m_variables.bReloadCompo=true;
    QCoreApplication::processEvents();
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    sltPrevisualizarZoom();
}



/** Cambio de pestaña*/
void Composicion::sltChangeTab(int index){
    /*if (index==3){
        In2ImageCreator p;
        p.sltGenerateImage();
        QLabel *imageLabel=new QLabel();
        imageLabel->setPixmap(m_memoryMap->m_Imagen);
        ui->m_scrollArea->setWidget(imageLabel);
    }*/
}

/** Previsualiza la composicion creada*/
void Composicion::sltPrevisualizar(){
    GuardaDatosCampos();
    GuardaDatosCamposVariables();


    for (int i=0;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
        QString nombreruta_bmp=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
        nombreruta_bmp.remove(".ini");
        nombreruta_bmp.append(QString("_OF%1").arg(m_memoryMap->m_mapa.m_variables.OrdenFabricacion));
        nombreruta_bmp.append(QString("_C%1_comp.bmp").arg(i+1));
        In2Previsualizator *scr=new In2Previsualizator(this,nombreruta_bmp);
        scr->show();
    }

}

/** Previsualiza la composicion creada*/
void Composicion::sltRefreshZoom(){

    if (m_memoryMap->m_ComposicionGenerada){
        this->setEnabled(false);
        this->update();
        if (m_scene==NULL){
            In2ImageCreator img;
            QRect rect;
            if (m_memoryMap->m_Imagen.isNull()){
                rect=QRect(0,0,m_memoryMap->m_mapa.m_variables.WebWidth,m_memoryMap->m_mapa.m_variables.WebHeight);
            }
            else rect=m_memoryMap->m_Imagen.rect();
            m_scene=new In2GraphicsScene(0,0,rect.width(),rect.height());
            In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
            QPixmap lienzo(rect.size());
            QPainter *painter=new QPainter(&lienzo);
            QPen pen(Qt::red, 8, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
            pen.setCosmetic(true);
            painter->setPen( pen );
            QString nombreruta=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
            nombreruta.replace(".ini","_OF_C1_comp.bmp");
            painter->drawPixmap(0,0,img.returnCompoImage(nombreruta));
            painter->drawRect(rect);
            painter->end();
            p->setTransformationMode(Qt::FastTransformation);
            p->setPixmap(lienzo);
            m_scene->addItem(p);
            ui->m_GraphicsView->setScene(m_scene);
            ui->m_GraphicsView->fitInView(p,Qt::KeepAspectRatio);
            ui->m_GraphicsView->show();

            //m_scene->enableGrid(true);

        }
        else{
            QList<QGraphicsItem*> items=m_scene->items();
            In2GraphicsPixmapItem *p;
            if (items.count()){

                p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                In2ImageCreator img;
                QPixmap lienzo(m_memoryMap->m_Imagen.size());
                QPainter *painter=new QPainter(&lienzo);
                QPen pen(Qt::red, 8, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
                pen.setCosmetic(true);
                painter->setPen( pen );
                QString nombreruta=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
                nombreruta.replace(".ini","_OF_C1_comp.bmp");
                painter->drawPixmap(0,0,img.returnCompoImage(nombreruta));
                painter->drawRect(m_memoryMap->m_Imagen.rect());
                painter->end();
                p->setTransformationMode(Qt::FastTransformation);
                p->setPixmap(lienzo);
                m_scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
                ui->m_GraphicsView->fitInView(p,Qt::KeepAspectRatio);
                ui->m_GraphicsView->centerOn(p);

            }
        }
    this->setEnabled(true);
    }

}


/** Previsualiza la composicion creada*/
void Composicion::sltPrevisualizarZoom(){
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    //ui->m_pbRender->setValue(0);
    if (m_memoryMap->m_ComposicionGenerada){
        this->setCursor(QCursor(Qt::WaitCursor));
        QCoreApplication::processEvents();
        this->setEnabled(false);
        QCoreApplication::processEvents();
        if (m_scene==NULL){
            In2ImageCreator img;
            connect(&img,SIGNAL(sgnImgCrtProgress(int,int)),this,SLOT(sltUpdatePb(int,int)));
            connect(&img,SIGNAL(sgnImgCrtInit()),this,SLOT(sltResetPb()));
            connect(&img,SIGNAL(sgnImgCrtEnd()),this,SLOT(sltResetPb()));
            if (m_memoryMap->m_mapa.m_variables.bReloadCompo){

                img.sltGenerateImage();

            }
            QRect rect;
            if (m_memoryMap->m_Imagen.isNull()){
                rect=QRect(0,0,m_memoryMap->m_mapa.m_variables.WebWidth,m_memoryMap->m_mapa.m_variables.WebHeight);
            }
            else rect=m_memoryMap->m_Imagen.rect();
            m_scene=new In2GraphicsScene(0,0,rect.width(),rect.height());
            In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
            QPixmap lienzo(rect.size());
            QPainter *painter=new QPainter(&lienzo);
            QPen pen(Qt::red, 8, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
            pen.setCosmetic(true);
            painter->setPen( pen );
            QString nombreruta=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
            nombreruta.replace(".ini","_OF_C1_comp.bmp");
            painter->drawPixmap(0,0,img.returnCompoImage(nombreruta));
            painter->drawRect(rect);
            painter->end();
            p->setTransformationMode(Qt::FastTransformation);
            p->setPixmap(lienzo);
            m_scene->addItem(p);
            ui->m_GraphicsView->setScene(m_scene);
            ui->m_GraphicsView->fitInView(p,Qt::KeepAspectRatio);
            ui->m_GraphicsView->show();



        }
        else{
            QList<QGraphicsItem*> items=m_scene->items();
            In2GraphicsPixmapItem *p;
            if (items.count()){

                p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                In2ImageCreator img;
                connect(&img,SIGNAL(sgnImgCrtProgress(int,int)),this,SLOT(sltUpdatePb(int,int)));
                connect(&img,SIGNAL(sgnImgCrtInit()),this,SLOT(sltResetPb()));
                connect(&img,SIGNAL(sgnImgCrtEnd()),this,SLOT(sltResetPb()));

                if (m_memoryMap->m_mapa.m_variables.bReloadCompo){
                    img.sltGenerateImage();
                }
                QPixmap lienzo(m_memoryMap->m_Imagen.size());
                QPainter *painter=new QPainter(&lienzo);
                QPen pen(Qt::red, 8, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
                pen.setCosmetic(true);
                painter->setPen( pen );
                QString nombreruta=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
                nombreruta.replace(".ini","_OF_C1_comp.bmp");
                painter->drawPixmap(0,0,img.returnCompoImage(nombreruta));
                painter->drawRect(m_memoryMap->m_Imagen.rect());
                painter->end();
                p->setTransformationMode(Qt::FastTransformation);
                p->setPixmap(lienzo);
                m_scene->setSceneRect(0,0,lienzo.width(),lienzo.height());
                ui->m_GraphicsView->fitInView(p,Qt::KeepAspectRatio);
                ui->m_GraphicsView->centerOn(p);

            }
        }
        emit sgnImageGenerated();
        this->setCursor(QCursor(Qt::ArrowCursor));
        this->setEnabled(true);
        //ui->m_pbRender->setValue(0);
    }

}


void Composicion::TestRoot(){

    //QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    //QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();

    if (m_memoryMap->m_ConfiguracionCargada==false){
        ui->m_FormatPlantillaButton->setEnabled(false);
        ui->m_FormatButton->setEnabled(false);
        ui->m_FormatPageButton->setEnabled(false);
        ui->m_PrintButton->setEnabled(false);
        ui->m_OrdenFabricacion->setEnabled(false);
        ui->m_Previsualizar->setEnabled(false);
        //ui->m_ColaImpresion->setEnabled(false);
        //ui->m_pbInkCalculator->setEnabled(false);
        ui->m_Reload->setEnabled(false);
        ui->m_pbVision->setEnabled(false);


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
    else{ //(m_memoryMap->m_ConfiguracionCargada==true)
        if (m_memoryMap->getUserRights()>=ACCESS_PRINTER){
            ui->m_FormatPlantillaButton->setEnabled(true);
            ui->m_FormatButton->setEnabled(true);
            ui->m_FormatPageButton->setEnabled(true);
            ui->m_PrintButton->setEnabled(true);
            ui->m_OrdenFabricacion->setEnabled(true);
            ui->m_Previsualizar->setEnabled(true);
            ui->m_ColaImpresion->setEnabled(true);
            ui->m_pbInkCalculator->setEnabled(true);
            ui->m_Reload->setEnabled(true);
            //ui->m_pbVision->setEnabled(true);


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

Composicion::~Composicion()
{
    //Comprobar_Campos_Variables();
}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void Composicion::AceptarSeleccion(){


    m_memoryMap->ReturnToOriginalWorld();

    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    if (m_memoryMap->m_ConfiguracionCargada){
            GuardarComposicionNow();
    }

	LaunchPrincipal();
    emit finishDesign();
}
/** Lanza la pantalla de seleccion de imagen fija*/
void Composicion::LaunchEditImagenFija(){
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    EditImagenFija *ScrEditImagenFija;
    ScrEditImagenFija=new EditImagenFija(0);
    ScrEditImagenFija->show();
	this->deleteLater(); 
}
/** Lanza la pantalla de modificación de valores de campos*/
void Composicion::LaunchEditCampos(){
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    EditCampos *ScrEditCampos;
    ScrEditCampos=new EditCampos(0);
    ScrEditCampos->show();
	this->deleteLater(); 
}
/** Lanza la pantalla de modificaciond e valores de composicion*/
void Composicion::LaunchEditComposicion(){
    Comprobar_Campos_Variables();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    m_memoryMap->m_mapa.m_variables.bReloadCompo=false;
    EditComposicion *ScrEditComposicion;
    ScrEditComposicion=new EditComposicion(0);
    ScrEditComposicion->show();
	this->deleteLater(); 
}

void Composicion::GuardaDatosCampos(){	

    m_memoryMap->m_mapa.m_variables.WebHeight=ui->m_leHeight->text().toDouble();
    //m_memoryMap->m_mapa.m_variables.WebWidth=ui->m_leWidth->text().toDouble();

    if (ui->m_OnOffCampo1->isChecked()) m_memoryMap->m_mapa.m_campos[0].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_campos[0].OnOffCampo=0;
	
    if (ui->m_OnOffCampo2->isChecked()) m_memoryMap->m_mapa.m_campos[1].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_campos[1].OnOffCampo=0;

    if (ui->m_OnOffCampo3->isChecked()) m_memoryMap->m_mapa.m_campos[2].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_campos[2].OnOffCampo=0;

    if (ui->m_OnOffCampo4->isChecked()) m_memoryMap->m_mapa.m_campos[3].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_campos[3].OnOffCampo=0;

    m_memoryMap->m_mapa.m_variables.OrdenFabricacion=ui->m_OrdenFabricacion->toPlainText();
    m_memoryMap->m_mapa.m_campos[0].NombreCampo=ui->m_NombreCampo1->toPlainText();
    m_memoryMap->m_mapa.m_campos[1].NombreCampo=ui->m_NombreCampo2->toPlainText();
    m_memoryMap->m_mapa.m_campos[2].NombreCampo=ui->m_NombreCampo3->toPlainText();
    m_memoryMap->m_mapa.m_campos[3].NombreCampo=ui->m_NombreCampo4->toPlainText();

    m_memoryMap->m_mapa.m_campos[0].ValorActualCampo=ui->m_ValorCampo1->toPlainText();
    m_memoryMap->m_mapa.m_campos[1].ValorActualCampo=ui->m_ValorCampo2->toPlainText();
    m_memoryMap->m_mapa.m_campos[2].ValorActualCampo=ui->m_ValorCampo3->toPlainText();
    m_memoryMap->m_mapa.m_campos[3].ValorActualCampo=ui->m_ValorCampo4->toPlainText();

    m_memoryMap->m_mapa.m_variables.bDilateAlgorithm=ui->m_dilate->isChecked();

    QString foo=QString("%1@%2@%3@%4@%5@%6@%7@%8")
            .arg(ui->m_leP11->text())
            .arg(ui->m_leP12->text())
            .arg(ui->m_leP13->text())
            .arg(ui->m_leP14->text())
            .arg(ui->m_leP15->text())
            .arg(ui->m_leP16->text())
            .arg(ui->m_leP17->text())
            .arg(ui->m_leP18->text());
    m_memoryMap->m_mapa.m_impresion.qstrPal1=foo;

    foo=QString("%1@%2@%3@%4@%5@%6@%7@%8")
                .arg(ui->m_leP11_2->text())
                .arg(ui->m_leP12_2->text())
                .arg(ui->m_leP13_2->text())
                .arg(ui->m_leP14_2->text())
                .arg(ui->m_leP15_2->text())
                .arg(ui->m_leP16_2->text())
                .arg(ui->m_leP17_2->text())
                .arg(ui->m_leP18_2->text());
     m_memoryMap->m_mapa.m_impresion.qstrPal2=foo;

     foo=QString("%1@%2@%3@%4@%5@%6@%7@%8")
                 .arg(ui->m_leP11_3->text())
                 .arg(ui->m_leP12_3->text())
                 .arg(ui->m_leP13_3->text())
                 .arg(ui->m_leP14_3->text())
                 .arg(ui->m_leP15_3->text())
                 .arg(ui->m_leP16_3->text())
                 .arg(ui->m_leP17_3->text())
                 .arg(ui->m_leP18_3->text());
     m_memoryMap->m_mapa.m_impresion.qstrPal3=foo;

     foo=QString("%1@%2@%3@%4@%5@%6@%7@%8")
                 .arg(ui->m_leP11_4->text())
                 .arg(ui->m_leP12_4->text())
                 .arg(ui->m_leP13_4->text())
                 .arg(ui->m_leP14_4->text())
                 .arg(ui->m_leP15_4->text())
                 .arg(ui->m_leP16_4->text())
                 .arg(ui->m_leP17_4->text())
                 .arg(ui->m_leP18_4->text());
     m_memoryMap->m_mapa.m_impresion.qstrPal4=foo;




    m_memoryMap->m_mapa.m_impresion.iVelocidad=ui->m_leVelocidad->text().toInt();

}

void Composicion::GuardaDatosCamposVariables(){

    if (ui->m_OnOffCampoVar1->isChecked()) m_memoryMap->m_mapa.m_camposvariables[0].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_camposvariables[0].OnOffCampo=0;

    if (ui->m_OnOffCampoVar2->isChecked()) m_memoryMap->m_mapa.m_camposvariables[1].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_camposvariables[1].OnOffCampo=0;

    if (ui->m_OnOffCampoVar3->isChecked()) m_memoryMap->m_mapa.m_camposvariables[2].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_camposvariables[2].OnOffCampo=0;

    if (ui->m_OnOffCampoVar4->isChecked()) m_memoryMap->m_mapa.m_camposvariables[3].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_camposvariables[3].OnOffCampo=0;

    if (ui->m_OnOffCampoVar5->isChecked()) m_memoryMap->m_mapa.m_camposvariables[4].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_camposvariables[4].OnOffCampo=0;

    if (ui->m_OnOffCampoVar6->isChecked()) m_memoryMap->m_mapa.m_camposvariables[5].OnOffCampo=1;
    else m_memoryMap->m_mapa.m_camposvariables[5].OnOffCampo=0;
#if QT_VERSION>=0x050000
    m_memoryMap->m_mapa.m_camposvariables[0].NombreCampo=ui->m_NombreCampoVar1->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[1].NombreCampo=ui->m_NombreCampoVar2->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[2].NombreCampo=ui->m_NombreCampoVar3->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[3].NombreCampo=ui->m_NombreCampoVar4->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[4].NombreCampo=ui->m_NombreCampoVar5->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[5].NombreCampo=ui->m_NombreCampoVar6->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[0].ValorActualCampo=ui->m_ValorCampoVar1->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[1].ValorActualCampo=ui->m_ValorCampoVar2->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[2].ValorActualCampo=ui->m_ValorCampoVar3->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[3].ValorActualCampo=ui->m_ValorCampoVar4->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[4].ValorActualCampo=ui->m_ValorCampoVar5->toPlainText().toLatin1();
    m_memoryMap->m_mapa.m_camposvariables[5].ValorActualCampo=ui->m_ValorCampoVar6->toPlainText().toLatin1();
#else
    m_memoryMap->m_mapa.m_camposvariables[0].NombreCampo=ui->m_NombreCampoVar1->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[1].NombreCampo=ui->m_NombreCampoVar2->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[2].NombreCampo=ui->m_NombreCampoVar3->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[3].NombreCampo=ui->m_NombreCampoVar4->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[4].NombreCampo=ui->m_NombreCampoVar5->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[5].NombreCampo=ui->m_NombreCampoVar6->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[0].ValorActualCampo=ui->m_ValorCampoVar1->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[1].ValorActualCampo=ui->m_ValorCampoVar2->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[2].ValorActualCampo=ui->m_ValorCampoVar3->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[3].ValorActualCampo=ui->m_ValorCampoVar4->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[4].ValorActualCampo=ui->m_ValorCampoVar5->toPlainText().toAscii();
    m_memoryMap->m_mapa.m_camposvariables[5].ValorActualCampo=ui->m_ValorCampoVar6->toPlainText().toAscii();
#endif

    m_memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo=ui->m_OrigenCampoVar1->currentIndex();
    m_memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo=ui->m_OrigenCampoVar2->currentIndex();
    m_memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo=ui->m_OrigenCampoVar3->currentIndex();
    m_memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo=ui->m_OrigenCampoVar4->currentIndex();
    m_memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo=ui->m_OrigenCampoVar5->currentIndex();
    m_memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo=ui->m_OrigenCampoVar6->currentIndex();

    m_memoryMap->m_mapa.m_camposvariables[0].OrigenValorCampo=ui->m_OrigenCampoVar1->currentText();
    m_memoryMap->m_mapa.m_camposvariables[1].OrigenValorCampo=ui->m_OrigenCampoVar2->currentText();
    m_memoryMap->m_mapa.m_camposvariables[2].OrigenValorCampo=ui->m_OrigenCampoVar3->currentText();
    m_memoryMap->m_mapa.m_camposvariables[3].OrigenValorCampo=ui->m_OrigenCampoVar4->currentText();
    m_memoryMap->m_mapa.m_camposvariables[4].OrigenValorCampo=ui->m_OrigenCampoVar5->currentText();
    m_memoryMap->m_mapa.m_camposvariables[5].OrigenValorCampo=ui->m_OrigenCampoVar6->currentText();

    if (m_memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar1->toPlainText().toInt();
        m_memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.valorInicialFila=iniValue;
    }
    if (m_memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar2->toPlainText().toInt();
        m_memoryMap->m_mapa.m_camposvariables[1].m_counterParameters.valorInicialFila=iniValue;
    }
    if (m_memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar3->toPlainText().toInt();
        m_memoryMap->m_mapa.m_camposvariables[2].m_counterParameters.valorInicialFila=iniValue;
    }
    if (m_memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar4->toPlainText().toInt();
        m_memoryMap->m_mapa.m_camposvariables[3].m_counterParameters.valorInicialFila=iniValue;
    }
    if (m_memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar5->toPlainText().toInt();
        m_memoryMap->m_mapa.m_camposvariables[4].m_counterParameters.valorInicialFila=iniValue;
    }
    if (m_memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=ui->m_ValorCampoVar6->toPlainText().toInt();
        m_memoryMap->m_mapa.m_camposvariables[5].m_counterParameters.valorInicialFila=iniValue;
    }

    //Lo dejo por compatibilidad
    m_memoryMap->m_mapa.m_camposvariables[0].OrdenCampo=1;
    m_memoryMap->m_mapa.m_camposvariables[1].OrdenCampo=2;
    m_memoryMap->m_mapa.m_camposvariables[2].OrdenCampo=3;
    m_memoryMap->m_mapa.m_camposvariables[3].OrdenCampo=4;
    m_memoryMap->m_mapa.m_camposvariables[4].OrdenCampo=5;
    m_memoryMap->m_mapa.m_camposvariables[5].OrdenCampo=6;

}


//Cancela la selección y vuelve a la pantalla de Configuracion
void Composicion::CancelarSeleccion(){
    m_memoryMap->ReturnToOriginalWorld();
    int res=QMessageBox::question(this,tr("GUARDAR"),tr("Quieres guardar los cambios?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (res==QMessageBox::Ok)
        GuardarComposicion();

    emit finishDesign();
    LaunchPrincipal();
    //this->deleteLater();

}


// Actualiza los datos de pantalla
void Composicion::ActualizaDataSlot(){
    /*DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();*/
}


void Composicion::UpdateOnOff1(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo1->isChecked())
	{
        ui->m_NombreCampo1->setEnabled(true);
        ui->m_NombreCampo1->setReadOnly(false);
        ui->m_ValorCampo1->setEnabled(true);
        ui->m_NombreCampo1->setPalette(paleteEn);
        ui->m_ValorCampo1->setPalette(paleteEn);
        m_memoryMap->m_mapa.m_campos[0].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
	}
	else
	{
        ui->m_NombreCampo1->setEnabled(false);
        ui->m_NombreCampo1->setReadOnly(true);
        ui->m_ValorCampo1->setEnabled(false);
        ui->m_NombreCampo1->setPalette(paleteDis);
        ui->m_ValorCampo1->setPalette(paleteDis);
        m_memoryMap->m_mapa.m_campos[0].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
	}

}

void Composicion::UpdateOnOff2(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo2->isChecked())
	{
        ui->m_NombreCampo2->setEnabled(true);
        ui->m_NombreCampo2->setReadOnly(false);
        ui->m_ValorCampo2->setEnabled(true);
        ui->m_NombreCampo2->setPalette(paleteEn);
        ui->m_ValorCampo2->setPalette(paleteEn);
        m_memoryMap->m_mapa.m_campos[1].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
        //double xpos=m_memoryMap->m_mapa.m_campos[1].m_matrices[0].
	}
	else
	{
        ui->m_NombreCampo2->setEnabled(false);
        ui->m_NombreCampo2->setReadOnly(true);
        ui->m_ValorCampo2->setEnabled(false);
        ui->m_NombreCampo2->setPalette(paleteDis);
        ui->m_ValorCampo2->setPalette(paleteDis);
        m_memoryMap->m_mapa.m_campos[1].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
	}
}

void Composicion::UpdateOnOff3(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo3->isChecked())
    {
        ui->m_NombreCampo3->setEnabled(true);
        ui->m_NombreCampo3->setReadOnly(false);
        ui->m_ValorCampo3->setEnabled(true);
        ui->m_NombreCampo3->setPalette(paleteEn);
        ui->m_ValorCampo3->setPalette(paleteEn);
        m_memoryMap->m_mapa.m_campos[2].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampo3->setEnabled(false);
        ui->m_NombreCampo3->setReadOnly(true);
        ui->m_ValorCampo3->setEnabled(false);
        ui->m_NombreCampo3->setPalette(paleteDis);
        ui->m_ValorCampo3->setPalette(paleteDis);
        m_memoryMap->m_mapa.m_campos[2].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}

void Composicion::UpdateOnOff4(){
    QPalette paleteEn=GetPaletaTextEditEnabled();
    QPalette paleteDis=GetPaletaTextEditDisabled();

    if (ui->m_OnOffCampo4->isChecked())
    {
        ui->m_NombreCampo4->setEnabled(true);
        ui->m_NombreCampo4->setReadOnly(false);
        ui->m_ValorCampo4->setEnabled(true);
        ui->m_NombreCampo4->setPalette(paleteEn);
        ui->m_ValorCampo4->setPalette(paleteEn);
        m_memoryMap->m_mapa.m_campos[3].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampo4->setEnabled(false);
        ui->m_NombreCampo4->setReadOnly(true);
        ui->m_ValorCampo4->setEnabled(false);
        ui->m_NombreCampo4->setPalette(paleteDis);
        ui->m_ValorCampo4->setPalette(paleteDis);
        m_memoryMap->m_mapa.m_campos[3].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}

void Composicion::UpdateOrdenFabricacion(){
    InputData *input = InputData::request(this,"Orden de fabricacion");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_OrdenFabricacion(QString)));
        input->setFocus();
    }
}

void Composicion::Update_OrdenFabricacion(QString value){
    ui->m_OrdenFabricacion->setText(value);
    ui->m_OrdenFabricacion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateNombreCampo1(){
    InputData *input = InputData::request(this,"Nombre Campo 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo1(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampo1(QString value){
    ui->m_NombreCampo1->setText(value);
    ui->m_NombreCampo1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateNombreCampo2(){
    InputData *input = InputData::request(this,"Nombre Campo 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo2(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampo2(QString value){
    ui->m_NombreCampo2->setText(value);
    ui->m_NombreCampo2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateNombreCampo3(){
    InputData *input = InputData::request(this,"Nombre Campo 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo3(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampo3(QString value){
    ui->m_NombreCampo3->setText(value);
    ui->m_NombreCampo3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateNombreCampo4(){
    InputData *input = InputData::request(this,"Nombre Campo 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampo4(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampo4(QString value){
    ui->m_NombreCampo4->setText(value);
    ui->m_NombreCampo4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampo1(){
    InputData *input = InputData::request(this, "Valor Campo 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo1(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampo1(QString value){
    ui->m_ValorCampo1->setText(value);
    ui->m_ValorCampo1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateValorCampo2(){
    InputData *input = InputData::request(this,"Valor Campo 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo2(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampo2(QString value){
    ui->m_ValorCampo2->setText(value);
    ui->m_ValorCampo2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampo3(){
    InputData *input = InputData::request(this,"Valor Campo 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo3(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampo3(QString value){
    ui->m_ValorCampo3->setText(value);
    ui->m_ValorCampo3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampo4(){
    InputData *input = InputData::request(this,"Valor Campo 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampo4(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampo4(QString value){
    ui->m_ValorCampo4->setText(value);
    ui->m_ValorCampo4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateOnOffVar1(){
    if (ui->m_OnOffCampoVar1->isChecked())
    {
        ui->m_NombreCampoVar1->setEnabled(true);
        ui->m_ValorCampoVar1->setEnabled(true);
        ui->m_OrigenCampoVar1->setEnabled(true);
        m_memoryMap->m_mapa.m_camposvariables[0].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampoVar1->setEnabled(false);
        ui->m_ValorCampoVar1->setEnabled(false);
        ui->m_OrigenCampoVar1->setEnabled(false);
        m_memoryMap->m_mapa.m_camposvariables[0].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }

}

void Composicion::UpdateOnOffVar2(){
    if (ui->m_OnOffCampoVar2->isChecked())
    {
        ui->m_NombreCampoVar2->setEnabled(true);
        ui->m_ValorCampoVar2->setEnabled(true);
        ui->m_OrigenCampoVar2->setEnabled(true);
        m_memoryMap->m_mapa.m_camposvariables[1].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampoVar2->setEnabled(false);
        ui->m_ValorCampoVar2->setEnabled(false);
        ui->m_OrigenCampoVar2->setEnabled(false);
        m_memoryMap->m_mapa.m_camposvariables[1].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}

void Composicion::UpdateOnOffVar3(){
    if (ui->m_OnOffCampoVar3->isChecked())
    {
        ui->m_NombreCampoVar3->setEnabled(true);
        ui->m_ValorCampoVar3->setEnabled(true);
        ui->m_OrigenCampoVar3->setEnabled(true);
        m_memoryMap->m_mapa.m_camposvariables[2].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampoVar3->setEnabled(false);
        ui->m_ValorCampoVar3->setEnabled(false);
        ui->m_OrigenCampoVar3->setEnabled(false);
        m_memoryMap->m_mapa.m_camposvariables[2].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}

void Composicion::UpdateOnOffVar4(){
    if (ui->m_OnOffCampoVar4->isChecked())
    {
        ui->m_NombreCampoVar4->setEnabled(true);
        ui->m_ValorCampoVar4->setEnabled(true);
        ui->m_OrigenCampoVar4->setEnabled(true);
        m_memoryMap->m_mapa.m_camposvariables[3].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampoVar4->setEnabled(false);
        ui->m_ValorCampoVar4->setEnabled(false);
        ui->m_OrigenCampoVar4->setEnabled(false);
        m_memoryMap->m_mapa.m_camposvariables[3].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}
void Composicion::UpdateOnOffVar5(){
    if (ui->m_OnOffCampoVar5->isChecked())
    {
        ui->m_NombreCampoVar5->setEnabled(true);
        ui->m_ValorCampoVar5->setEnabled(true);
        ui->m_OrigenCampoVar5->setEnabled(true);
        m_memoryMap->m_mapa.m_camposvariables[4].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampoVar5->setEnabled(false);
        ui->m_ValorCampoVar5->setEnabled(false);
        ui->m_OrigenCampoVar5->setEnabled(false);
        m_memoryMap->m_mapa.m_camposvariables[4].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}
void Composicion::UpdateOnOffVar6(){
    if (ui->m_OnOffCampoVar6->isChecked())
    {
        ui->m_NombreCampoVar6->setEnabled(true);
        ui->m_ValorCampoVar6->setEnabled(true);
        ui->m_OrigenCampoVar6->setEnabled(true);
        m_memoryMap->m_mapa.m_camposvariables[5].m_matrices[0].OnOffMatriz=1; //Por defecto la matriz 0
    }
    else
    {
        ui->m_NombreCampoVar6->setEnabled(false);
        ui->m_ValorCampoVar6->setEnabled(false);
        ui->m_OrigenCampoVar6->setEnabled(false);
        m_memoryMap->m_mapa.m_camposvariables[5].m_matrices[0].OnOffMatriz=0; //Por defecto la matriz 0
    }
}



void Composicion::UpdateNombreCampoVar1(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar1(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampoVar1(QString value){
    ui->m_NombreCampoVar1->setText(value);
    ui->m_NombreCampoVar1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateNombreCampoVar2(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar2(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampoVar2(QString value){
    ui->m_NombreCampoVar2->setText(value);
    ui->m_NombreCampoVar2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateNombreCampoVar3(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar3(QString)));
        input->setFocus();
    }
}

void Composicion::Update_NombreCampoVar3(QString value){
    ui->m_NombreCampoVar3->setText(value);
    ui->m_NombreCampoVar3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateNombreCampoVar4(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar4(QString)));
        input->setFocus();
     }
}
void Composicion::Update_NombreCampoVar4(QString value){
    ui->m_NombreCampoVar4->setText(value);
    ui->m_NombreCampoVar4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateNombreCampoVar5(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 5");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar5(QString)));
        input->setFocus();
     }
}
void Composicion::Update_NombreCampoVar5(QString value){
    ui->m_NombreCampoVar5->setText(value);
    ui->m_NombreCampoVar5->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateNombreCampoVar6(){
    InputData *input = InputData::request(this,"Nombre Campo Variable 6");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_NombreCampoVar6(QString)));
        input->setFocus();
     }
}
void Composicion::Update_NombreCampoVar6(QString value){
    ui->m_NombreCampoVar6->setText(value);
    ui->m_NombreCampoVar6->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateValorCampoVar1(){
    InputData *input = InputData::request(this, "Valor Campo Variable 1");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar1(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampoVar1(QString value){
    ui->m_ValorCampoVar1->setText(value);
    ui->m_ValorCampoVar1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void Composicion::UpdateValorCampoVar2(){
    InputData *input = InputData::request(this,"Valor Campo Variable 2");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar2(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampoVar2(QString value){
    ui->m_ValorCampoVar2->setText(value);
    ui->m_ValorCampoVar2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampoVar3(){
    InputData *input = InputData::request(this,"Valor Campo Variable 3");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar3(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampoVar3(QString value){
    ui->m_ValorCampoVar3->setText(value);
    ui->m_ValorCampoVar3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampoVar4(){
    InputData *input = InputData::request(this,"Valor Campo Variable 4");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar4(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampoVar4(QString value){
    ui->m_ValorCampoVar4->setText(value);
    ui->m_ValorCampoVar4->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampoVar5(){
    InputData *input = InputData::request(this,"Valor Campo Variable 5");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar5(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampoVar5(QString value){
    ui->m_ValorCampoVar5->setText(value);
    ui->m_ValorCampoVar5->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Composicion::UpdateValorCampoVar6(){
    InputData *input = InputData::request(this,"Valor Campo Variable 6");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_ValorCampoVar6(QString)));
        input->setFocus();
    }
}

void Composicion::Update_ValorCampoVar6(QString value){
    ui->m_ValorCampoVar6->setText(value);
    ui->m_ValorCampoVar6->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}



void Composicion::CargaDatosCampos(){

    //Algoritmo dilate
    ui->m_dilate->setChecked(m_memoryMap->m_mapa.m_variables.bDilateAlgorithm);

    if (m_memoryMap->m_mapa.m_campos[0].OnOffCampo==1) ui->m_OnOffCampo1->setChecked(true);
    else ui->m_OnOffCampo1->setChecked(false);
	UpdateOnOff1();
    if (m_memoryMap->m_mapa.m_campos[1].OnOffCampo==1) ui->m_OnOffCampo2->setChecked(true);
    else ui->m_OnOffCampo2->setChecked(false);
	UpdateOnOff2();
    if (m_memoryMap->m_mapa.m_campos[2].OnOffCampo==1) ui->m_OnOffCampo3->setChecked(true);
    else ui->m_OnOffCampo3->setChecked(false);
    UpdateOnOff3();
    if (m_memoryMap->m_mapa.m_campos[3].OnOffCampo==1) ui->m_OnOffCampo4->setChecked(true);
    else ui->m_OnOffCampo4->setChecked(false);
    UpdateOnOff4();

    ui->m_OrdenFabricacion->setPlainText("");

    ui->m_NombreCampo1->setPlainText(m_memoryMap->m_mapa.m_campos[0].NombreCampo);
    ui->m_NombreCampo2->setPlainText(m_memoryMap->m_mapa.m_campos[1].NombreCampo);
    ui->m_NombreCampo3->setPlainText(m_memoryMap->m_mapa.m_campos[2].NombreCampo);
    ui->m_NombreCampo4->setPlainText(m_memoryMap->m_mapa.m_campos[3].NombreCampo);

    ui->m_ValorCampo1->setPlainText(m_memoryMap->m_mapa.m_campos[0].ValorActualCampo);
    ui->m_ValorCampo2->setPlainText(m_memoryMap->m_mapa.m_campos[1].ValorActualCampo);
    ui->m_ValorCampo3->setPlainText(m_memoryMap->m_mapa.m_campos[2].ValorActualCampo);
    ui->m_ValorCampo4->setPlainText(m_memoryMap->m_mapa.m_campos[3].ValorActualCampo);

    if (m_memoryMap->m_mapa.m_variables.iSabreAlgorithm==0) ui->m_rbSabre0->setChecked(true);
    else if (m_memoryMap->m_mapa.m_variables.iSabreAlgorithm==1) ui->m_rbSabre1->setChecked(true);
    else if (m_memoryMap->m_mapa.m_variables.iSabreAlgorithm==2) ui->m_rbSabre2->setChecked(true);
    else if (m_memoryMap->m_mapa.m_variables.iSabreAlgorithm==3) ui->m_rbSabre3->setChecked(true);


    ui->m_leHeight->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.WebHeight));
    ui->m_leWidth->setText(QString("%1").arg(/*m_memoryMap->m_mapa.m_variables.DefaultNumHeads*DEFAULT_HEAD_WIDTH/FACTOR_PIXELS_MM*/m_memoryMap->m_mapa.m_variables.WebWidth));

    //Algoritmos de color
    m_Colores color=m_memoryMap->m_mapa.m_colores[0];
    ui->m_cbBlancos1->setChecked(color.bWhiteCorr);
    ui->m_cbMono1->setChecked(color.bMonoMode);
    ui->m_cbNegros1->setChecked(color.bBlackCorr);
    QString alg=color.alg;
    if (!alg.compare(TXT_FLOYD)) ui->m_rbFloyd1->setChecked(true);
    else ui->m_rbNone1->setChecked(true);

    color=m_memoryMap->m_mapa.m_colores[1];
    ui->m_cbBlancos1_2->setChecked(color.bWhiteCorr);
    ui->m_cbMono1_2->setChecked(color.bMonoMode);
    ui->m_cbNegros1_2->setChecked(color.bBlackCorr);
    alg=color.alg;
    if (!alg.compare(TXT_FLOYD)) ui->m_rbFloyd1_2->setChecked(true);
    else ui->m_rbNone1_2->setChecked(true);

    color=m_memoryMap->m_mapa.m_colores[2];
    ui->m_cbBlancos1_3->setChecked(color.bWhiteCorr);
    ui->m_cbMono1_3->setChecked(color.bMonoMode);
    ui->m_cbNegros1_3->setChecked(color.bBlackCorr);
    alg=color.alg;
    if (!alg.compare(TXT_FLOYD)) ui->m_rbFloyd1_3->setChecked(true);
    else ui->m_rbNone1_3->setChecked(true);

    color=m_memoryMap->m_mapa.m_colores[3];
    ui->m_cbBlancos1_4->setChecked(color.bWhiteCorr);
    ui->m_cbMono1_4->setChecked(color.bMonoMode);
    ui->m_cbNegros1_4->setChecked(color.bBlackCorr);
    alg=color.alg;
    if (!alg.compare(TXT_FLOYD)) ui->m_rbFloyd1_4->setChecked(true);
    else ui->m_rbNone1_4->setChecked(true);


    QStringList lst=m_memoryMap->m_mapa.m_impresion.qstrPal1.split("@");
    if (lst.count()==8){
        ui->m_leP11->setText(QString("%1").arg(lst.at(0)));
        ui->m_leP12->setText(QString("%1").arg(lst.at(1)));
        ui->m_leP13->setText(QString("%1").arg(lst.at(2)));
        ui->m_leP14->setText(QString("%1").arg(lst.at(3)));
        ui->m_leP15->setText(QString("%1").arg(lst.at(4)));
        ui->m_leP16->setText(QString("%1").arg(lst.at(5)));
        ui->m_leP17->setText(QString("%1").arg(lst.at(6)));
        ui->m_leP18->setText(QString("%1").arg(lst.at(7)));
    }

    lst=m_memoryMap->m_mapa.m_impresion.qstrPal2.split("@");
        if (lst.count()==8){
            ui->m_leP11_2->setText(QString("%1").arg(lst.at(0)));
            ui->m_leP12_2->setText(QString("%1").arg(lst.at(1)));
            ui->m_leP13_2->setText(QString("%1").arg(lst.at(2)));
            ui->m_leP14_2->setText(QString("%1").arg(lst.at(3)));
            ui->m_leP15_2->setText(QString("%1").arg(lst.at(4)));
            ui->m_leP16_2->setText(QString("%1").arg(lst.at(5)));
            ui->m_leP17_2->setText(QString("%1").arg(lst.at(6)));
            ui->m_leP18_2->setText(QString("%1").arg(lst.at(7)));
        }

     lst=m_memoryMap->m_mapa.m_impresion.qstrPal3.split("@");
        if (lst.count()==8){
            ui->m_leP11_3->setText(QString("%1").arg(lst.at(0)));
            ui->m_leP12_3->setText(QString("%1").arg(lst.at(1)));
            ui->m_leP13_3->setText(QString("%1").arg(lst.at(2)));
            ui->m_leP14_3->setText(QString("%1").arg(lst.at(3)));
            ui->m_leP15_3->setText(QString("%1").arg(lst.at(4)));
            ui->m_leP16_3->setText(QString("%1").arg(lst.at(5)));
            ui->m_leP17_3->setText(QString("%1").arg(lst.at(6)));
            ui->m_leP18_3->setText(QString("%1").arg(lst.at(7)));
         }
     lst=m_memoryMap->m_mapa.m_impresion.qstrPal4.split("@");
         if (lst.count()==8){
            ui->m_leP11_4->setText(QString("%1").arg(lst.at(0)));
            ui->m_leP12_4->setText(QString("%1").arg(lst.at(1)));
            ui->m_leP13_4->setText(QString("%1").arg(lst.at(2)));
            ui->m_leP14_4->setText(QString("%1").arg(lst.at(3)));
            ui->m_leP15_4->setText(QString("%1").arg(lst.at(4)));
            ui->m_leP16_4->setText(QString("%1").arg(lst.at(5)));
            ui->m_leP17_4->setText(QString("%1").arg(lst.at(6)));
            ui->m_leP18_4->setText(QString("%1").arg(lst.at(7)));
         }

    ui->m_leVelocidad->setText(QString("%1").arg(m_memoryMap->m_mapa.m_impresion.iVelocidad));

    ShowCompositionProps();

}



void Composicion::CargaDatosCamposVariables(){

    QString tmp;

    //if (token_ideogramas==0){
        //CargaIdeogramas();
        //token_ideogramas=1;
    //}

    if (m_memoryMap->m_mapa.m_camposvariables[0].OnOffCampo==1) ui->m_OnOffCampoVar1->setChecked(true);
    else ui->m_OnOffCampoVar1->setChecked(false);
    UpdateOnOffVar1();
    if (m_memoryMap->m_mapa.m_camposvariables[1].OnOffCampo==1) ui->m_OnOffCampoVar2->setChecked(true);
    else ui->m_OnOffCampoVar2->setChecked(false);
    UpdateOnOffVar2();
    if (m_memoryMap->m_mapa.m_camposvariables[2].OnOffCampo==1) ui->m_OnOffCampoVar3->setChecked(true);
    else ui->m_OnOffCampoVar3->setChecked(false);
    UpdateOnOffVar3();
    if (m_memoryMap->m_mapa.m_camposvariables[3].OnOffCampo==1) ui->m_OnOffCampoVar4->setChecked(true);
    else ui->m_OnOffCampoVar4->setChecked(false);
    UpdateOnOffVar4();
    if (m_memoryMap->m_mapa.m_camposvariables[4].OnOffCampo==1) ui->m_OnOffCampoVar5->setChecked(true);
    else ui->m_OnOffCampoVar5->setChecked(false);
    UpdateOnOffVar5();
    if (m_memoryMap->m_mapa.m_camposvariables[5].OnOffCampo==1) ui->m_OnOffCampoVar6->setChecked(true);
    else ui->m_OnOffCampoVar6->setChecked(false);
    UpdateOnOffVar6();


    ui->m_NombreCampoVar1->setPlainText(m_memoryMap->m_mapa.m_camposvariables[0].NombreCampo);
    ui->m_NombreCampoVar2->setPlainText(m_memoryMap->m_mapa.m_camposvariables[1].NombreCampo);
    ui->m_NombreCampoVar3->setPlainText(m_memoryMap->m_mapa.m_camposvariables[2].NombreCampo);
    ui->m_NombreCampoVar4->setPlainText(m_memoryMap->m_mapa.m_camposvariables[3].NombreCampo);
    ui->m_NombreCampoVar5->setPlainText(m_memoryMap->m_mapa.m_camposvariables[4].NombreCampo);
    ui->m_NombreCampoVar6->setPlainText(m_memoryMap->m_mapa.m_camposvariables[5].NombreCampo);

    ui->m_ValorCampoVar1->setPlainText(m_memoryMap->m_mapa.m_camposvariables[0].ValorActualCampo);
    ui->m_ValorCampoVar2->setPlainText(m_memoryMap->m_mapa.m_camposvariables[1].ValorActualCampo);
    ui->m_ValorCampoVar3->setPlainText(m_memoryMap->m_mapa.m_camposvariables[2].ValorActualCampo);
    ui->m_ValorCampoVar4->setPlainText(m_memoryMap->m_mapa.m_camposvariables[3].ValorActualCampo);
    ui->m_ValorCampoVar5->setPlainText(m_memoryMap->m_mapa.m_camposvariables[4].ValorActualCampo);
    ui->m_ValorCampoVar6->setPlainText(m_memoryMap->m_mapa.m_camposvariables[5].ValorActualCampo);

    ui->m_OrigenCampoVar1->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo);
    ui->m_OrigenCampoVar2->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo);
    ui->m_OrigenCampoVar3->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo);
    ui->m_OrigenCampoVar4->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo);
    ui->m_OrigenCampoVar5->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo);
    ui->m_OrigenCampoVar6->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo);


    ui->m_OrigenCampoVar1->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo);
    if (m_memoryMap->m_mapa.m_camposvariables[0].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=m_memoryMap->m_mapa.m_camposvariables[0].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar1->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar2->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo);
    if (m_memoryMap->m_mapa.m_camposvariables[1].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=m_memoryMap->m_mapa.m_camposvariables[1].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar2->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar3->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo);
    if (m_memoryMap->m_mapa.m_camposvariables[2].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=m_memoryMap->m_mapa.m_camposvariables[2].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar3->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar4->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo);
    if (m_memoryMap->m_mapa.m_camposvariables[3].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=m_memoryMap->m_mapa.m_camposvariables[3].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar4->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar5->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo);
    if (m_memoryMap->m_mapa.m_camposvariables[4].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=m_memoryMap->m_mapa.m_camposvariables[4].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar5->setText(QString("%1").arg(iniValue));
    }
    ui->m_OrigenCampoVar6->setCurrentIndex(m_memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo);
    if (m_memoryMap->m_mapa.m_camposvariables[5].indiceOrigenCampo==ID_ORIGEN_CONTADOR){
        int iniValue=m_memoryMap->m_mapa.m_camposvariables[5].m_counterParameters.valorInicial;
        ui->m_ValorCampoVar6->setText(QString("%1").arg(iniValue));
    }

}

void Composicion::LaunchPrincipal(){

    //GlobalShowMainWindow();
    /*if (m_parent) m_parent->show();
    else{
        InkjetDebug *p=InkjetDebug::request(0);
        p->deleteLater();
    }
    this->deleteLater();*/
    m_parent->show();
    InkjetDebug *p=InkjetDebug::request(0);
    p->deleteLater();
    this->deleteLater();

}


/** Comprobaciones previas a generar la imagen */
void Composicion::ComprobacionesGenerateImage(){

    int respuesta=0;

    GuardaDatosCampos();
    GuardaDatosCamposVariables();

    bool bTest=Comprobar_Campos_Vacios();
    if (bTest){
        respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Alguno de los campos a imprimir\nDesea continuar?"),tr("Si"),tr("No"),QString::null, 0, 1 );
        if (respuesta==1) return;
    }
    bTest=Comprobar_Formatos_Especiales();
    if (bTest){
        respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Alguno de los campos necesitan formato especial\nDesea continuar?"),tr("Si"),tr("No"),QString::null, 0, 1 );
        if (respuesta==1) return;
    }

    bTest=Comprobar_OrdenFabricacion_Vacia();
    if (bTest){
        QMessageBox::warning(0,tr("ERROR"),tr("No hay orden de fabricacion.\nPor favor, introduzca un valor adecuado."));
        return;
    }
    bTest=Comprobar_ExisteComposicion();
    if (bTest){
        respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Ya existe ese trabajo en la cola\nDesea continuar?"),tr("Si"),tr("No"),QString::null, 0, 1 );
        if (respuesta==1) return;
        else{
            QString nombreruta_bmp=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
            nombreruta_bmp.remove(".ini");
            nombreruta_bmp.append(QString("_OF%1_C1_comp.bmp").arg(m_memoryMap->m_mapa.m_variables.OrdenFabricacion));
            QFile dummyFile(nombreruta_bmp);
            dummyFile.remove();
        }

    }
    GenerateImage();


}




/** Genera la imagen*/
void Composicion::GenerateImage(){

    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);
    QCoreApplication::processEvents();
    In2ImageCreator p;
    connect(&p,SIGNAL(sgnImgCrtProgress(int,int)),this,SLOT(sltUpdatePb(int,int)));
    connect(&p,SIGNAL(sgnImgCrtInit()),this,SLOT(sltResetPb()));
    connect(&p,SIGNAL(sgnImgCrtEnd()),this,SLOT(sltResetPb()));

    p.sltGenerateImageToPrint();

    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);

    QCoreApplication::processEvents();


    //Borrar ficheros bmp intermedios

    /*QFile::remove(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
    for(int i=0;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
        QString dummy=QString("%1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummy.replace(".bmp",QString("_C%1.bmp").arg(i+1));
        QFile::remove(QString("%1").arg(dummy));
    }*/
    //Guarda la compo en local...o en remoto si est� habilitado
    GuardaCompoLocal();
    //Con esto copia los ficheros de la zona temporal a la zona OK
    m_memoryMap->JumpToOriginalWorld();
    //GuardarComposicionDirectamente();
    InsertarEnColaImpresion();
    //Por si queremos seguir editando
    m_memoryMap->ReturnToTempWorld();
    QString txt=QString("Generada OT %1").arg(m_memoryMap->m_mapa.m_variables.OrdenFabricacion);
    sltUpdateLog(txt);
    QMessageBox::warning(this,"INFO",txt,QMessageBox::Ok);
}


void Composicion::CargarComposicion(){

    QFileDialog fileName(this);
    QStringList Ficheros;

    deleteTempGSFile();

    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("Archivo de composicion(*.ini)"));
#else
    fileName.setFilter(tr("Archivo de composicion(*.ini)"));
#endif    
    fileName.setDirectory(m_memoryMap->getImagePath());
    Ficheros=fileName.getOpenFileNames(this,"Selecciona un fichero",m_memoryMap->getImagePath(),tr("Archivo de composicion(*.ini)"));
    if (!Ficheros.isEmpty()){
                    //Ficheros=fileName.selectedFiles();
                    this->setCursor(Qt::WaitCursor);
                    this->setEnabled(false);
                    QCoreApplication::processEvents();
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

                    m_memoryMap->InitComposicion();
                    m_memoryMap->m_mapa.m_variables.NombreIni=nombre;
                    m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
                    m_memoryMap->m_mapa.m_variables.NombreRutaIni=nombreruta;

                    //Guardo los originales
                    m_memoryMap->JumpToTempWorld();

                    //AbrirFicheroIni();
                    In2CompoFile ficheroIni;
                    ficheroIni.AbrirFicheroIni();

                    CargaDatosCampos();
                    CargaDatosCamposVariables();
                    CargaBBDD();
                    GeneracionRutaPlantilla();
                    nombreruta=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
                    nombreruta.replace("/","\\");

                    ui->m_NombreFicheroIniTmp->setText(nombreruta);
                    bool aux;
                    QString nombrepdf_aux=QString("%1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaPdf);
                    aux=QFile::exists(nombrepdf_aux);
                    //aux=QFile::exists(m_memoryMap->m_mapa.m_variables.NombreRutaPdf);
                    //QMessageBox::warning(0,"Ruta pdf",QString("%1").arg(nombrepdf_aux),QMessageBox::Ok);
                    if (aux==true){
                        //QMessageBox::warning(0,"ATENCIÓN!","El fichero existe",QMessageBox::Ok);
                        CargaPlantilla();
                        m_memoryMap->m_ConfiguracionCargada=true;
                        m_memoryMap->m_ComposicionGenerada=true;
                        m_memoryMap->m_UltimaAccion=UA_CONFIGURACION_CARGADA;
                        TestRoot();
                        //QTimer::singleShot(0,this,SLOT(sltPrevisualizarZoom()));
                        sltPrevisualizarZoom();
                        QString foo=QString("Composicion cargada: %1").arg(nombreruta);
                        sltUpdateLog(foo);
                        //QMessageBox::warning(0,"ATENCION!","Composicion cargada correctamente",QMessageBox::Ok);
                        emit SendInformerInfo(INFORMER_DEBUG,foo);
                    }
                    else{
                        //Aunque no haya fichero pdf, damos por cargada la compsoicion
                        m_memoryMap->m_ConfiguracionCargada=true;
                        m_memoryMap->m_UltimaAccion=UA_CONFIGURACION_CARGADA;
                        TestRoot();
                        QString mensaje_fichero=tr("No existe el archivo\n%1 ").arg(m_memoryMap->m_mapa.m_variables.NombrePdf);
                        QMessageBox::warning(0,"ERROR!",mensaje_fichero,QMessageBox::Ok);
                        emit SendInformerInfo(INFORMER_DEBUG,tr("Error al cargar composici�n: %1").arg(nombreruta));
                    }

                    //QApplication::restoreOverrideCursor();
                    ui->m_PrintButton->setEnabled(true);
                    ui->m_FormatButton->setEnabled(true);
                    ui->m_FormatPageButton->setEnabled(true);
                    ui->m_FormatPlantillaButton->setEnabled(true);

                    //DataCommon->ShowInfo();
            }
   // }
    this->setCursor(Qt::ArrowCursor);
    this->setEnabled(true);
    sltTestERP();

}

/** Cargamos BBDD*/
void Composicion::CargaBBDD(){
    if (!m_memoryMap->m_mapa.m_database.NombreRutaDb.isEmpty())
        LoadBBDD(m_memoryMap->m_mapa.m_database.NombreRutaDb);
}

void Composicion::CargaPlantilla(){

    unsigned char indice_color;
    In2ImageCreator *img=new In2ImageCreator;
    img->Convert();

    if (m_memoryMap->m_mapa.m_variables.NumColores==1){
        m_PixmapUnitario.load(m_memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::MonoOnly);
    }
    else{
        for(indice_color=0;indice_color<NUM_COLORES;indice_color++){
            QString dummy=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
            dummy.replace(QString(".bmp"),QString("_C%1.bmp").arg(indice_color+1));
            PixmapUnitario[indice_color].load(dummy,"BMP",Qt::MonoOnly);
        }
    }

}
/** Guarda la composicion - step1*/
void Composicion::GuardarComposicion(){
	
	int dummy;

    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);

    if (m_memoryMap->getUserRights()>=ACCESS_ROOT){
		dummy=CuadroDialogoGuardarComposicion();
        if (dummy!=0){
            QString foo=tr("Error en guardado");
            sltUpdateLog(foo);
            QMessageBox::warning(0,tr("ERROR"),foo,QMessageBox::Ok);
            emit SendInformerInfo(INFORMER_DEBUG,tr("Error al guardar composici�n: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));
        }
        else{
            emit SendInformerInfo(INFORMER_DEBUG,tr("Composici�n guardada: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));
        }
	}

    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);


}

/** Guarda la composicion-step2*/
int Composicion::CuadroDialogoGuardarComposicion(){
    int rtn=-1;
    In2FileDialog fileName(this);
    QString Fichero;


    fileName.setAcceptMode(QFileDialog::AcceptSave);
    fileName.setFileMode(QFileDialog::AnyFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("Archivo .ini (*.ini)"));
#else
    fileName.setFilter(tr("Archivo .ini (*.ini)"));
#endif
    fileName.setDefaultSuffix("ini");
    fileName.setDirectory(m_memoryMap->getImagePath());


    //if (fileName.exec()){
        //Ficheros=fileName.selectedFiles();
        Fichero=fileName.getSaveFileName(this,"Selecciona un fichero",m_memoryMap->getImagePath(),tr("Archivo .ini(*.ini)"));
		
        if (!Fichero.isEmpty()){

            QFile File(Fichero);
			if (!File.open(QIODevice::WriteOnly)) return -1;
            Fichero.replace("/","\\");
            m_memoryMap->m_mapa.m_variables.NombreRutaIni=Fichero;

			QString ruta,nombre;

            QFileInfo infoFile(Fichero);
            ruta=infoFile.absolutePath();
            nombre=infoFile.fileName();

            m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
            m_memoryMap->m_mapa.m_variables.NombreIni=nombre;

			QString nombre_bmp=nombre;
			nombre_bmp.replace(".ini",".bmp");

            m_memoryMap->m_mapa.m_variables.NombreIni=nombre;
            m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
            m_memoryMap->m_mapa.m_variables.NombreRutaIni=Fichero;
            QApplication::setOverrideCursor(Qt::BusyCursor);
            QString foo=tr("Guardando...por favor espere");
            sltUpdateLog(foo);
            QCoreApplication::processEvents();

            //File.write((const char*)&m_memoryMap->m_mapa,sizeof(m_memoryMap->m_mapa));
            m_memoryMap->SetCredentialsModified();
            GuardaDatosCampos();
            GuardaDatosCamposVariables();
            In2CompoFile ficheroIni;
            ficheroIni.GuardarFicheroIni();
            //GuardarFicheroIni();
			File.close();
            QApplication::restoreOverrideCursor();
            QCoreApplication::processEvents();

            foo=tr("Cambios guardados");
            sltUpdateLog(foo);
            QMessageBox::information(0,tr("INFO"),foo,QMessageBox::Ok);
            m_memoryMap->JumpToTempWorld();

            QString dummy=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
            dummy.replace("/","\\");
            ui->m_NombreFicheroIniTmp->setPlainText(dummy);
            rtn=0;
            //Aqui debemos hacer un salto al fichero que hemos guardado
		}
        return rtn;

}


/** Guarda la composicion sin abrir cuadro de diálogo - step1*/
void Composicion::GuardarComposicionNow(){

    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);

    if (m_memoryMap->getUserRights()>=ACCESS_ROOT){
        GuardarComposicionDirectamente();
        emit SendInformerInfo(INFORMER_DEBUG,QString(".Ini guardado: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));

    }

    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);

}


/** Guarda la composicion sin abrir cuadro de diálogo - step2*/
void Composicion::GuardarComposicionDirectamente(){


    if (m_memoryMap->m_mapa.m_variables.bUseLocalCopy) m_memoryMap->ReturnToOriginalWorld();

    m_memoryMap->SetCredentialsModified();

    QFile File(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    if (!File.open(QIODevice::WriteOnly)) return ;
    m_memoryMap->m_mapa.m_variables.NombreRutaIni.replace("/","\\");

    QString ruta,nombre;

    QFileInfo infoFile(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    ruta=infoFile.absolutePath();
    nombre=infoFile.fileName();

    m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
    m_memoryMap->m_mapa.m_variables.NombreIni=nombre;

    QString nombre_bmp=nombre;
    nombre_bmp.replace(".ini",".bmp");

    m_memoryMap->m_mapa.m_variables.NombreIni=nombre;
    m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
    //m_memoryMap->m_mapa.m_variables.NombreRutaIni=Fichero;
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QString foo=tr("Guardando...por favor espere");
    sltUpdateLog(foo);
    QCoreApplication::processEvents();

    //File.write((const char*)&m_memoryMap->m_mapa,sizeof(m_memoryMap->m_mapa));
    m_memoryMap->SetCredentialsModified();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    In2CompoFile ficheroIni;
    ficheroIni.GuardarFicheroIni();
    if (m_memoryMap->m_mapa.m_variables.bUseLocalCopy) m_memoryMap->JumpToOriginalWorld();
    //GuardarFicheroIni();
    File.close();
    QApplication::restoreOverrideCursor();
    foo=tr("Cambios guardados");
    sltUpdateLog(foo);
    QMessageBox::information(0,tr("INFO"),foo,QMessageBox::Ok);
    if (m_memoryMap->m_mapa.m_variables.bUseLocalCopy) m_memoryMap->ReturnToTempWorld();

}

void Composicion::GuardaCompoLocal(){
    m_memoryMap->SetCredentialsModified();

    QFile File(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    if (!File.open(QIODevice::WriteOnly)) return ;
    m_memoryMap->m_mapa.m_variables.NombreRutaIni.replace("/","\\");

    QString ruta,nombre;

    QFileInfo infoFile(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    ruta=infoFile.absolutePath();
    nombre=infoFile.fileName();

    m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
    m_memoryMap->m_mapa.m_variables.NombreIni=nombre;

    QString nombre_bmp=nombre;
    nombre_bmp.replace(".ini",".bmp");

    m_memoryMap->m_mapa.m_variables.NombreIni=nombre;
    m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
    //m_memoryMap->m_mapa.m_variables.NombreRutaIni=Fichero;

    //File.write((const char*)&m_memoryMap->m_mapa,sizeof(m_memoryMap->m_mapa));
    m_memoryMap->SetCredentialsModified();
    GuardaDatosCampos();
    GuardaDatosCamposVariables();
    In2CompoFile ficheroIni;
    ficheroIni.GuardarFicheroIni();

    //GuardarFicheroIni();
    File.close();
    QString foo=tr("Cambios guardados");
    sltUpdateLog(foo);
    //QMessageBox::information(0,tr("INFO"),foo,QMessageBox::Ok);

}


void Composicion::NuevaComposicion(){

    int dummy;
    deleteTempGSFile();
    ui->m_PrintButton->setEnabled(false);
    ui->m_FormatButton->setEnabled(false);
    ui->m_FormatPageButton->setEnabled(false);
    ui->m_FormatPlantillaButton->setEnabled(false);

    if (m_memoryMap->getUserRights()>=ACCESS_ROOT){
        dummy=CuadroDialogoNuevaComposicion();
        if (dummy!=0){
            QString foo=tr("Error creando nueva composicion");
            QMessageBox::warning(0,tr("ERROR"),foo,QMessageBox::Ok);
            emit SendDebugInfo(LOGGER_ERROR,foo);
            sltUpdateLog(foo);
        }
        else{
            QString foo=tr("Nueva Composicion");
            emit SendDebugInfo(LOGGER_ERROR,foo);
            sltUpdateLog(foo);
        }
    }

    ui->m_PrintButton->setEnabled(true);
    ui->m_FormatButton->setEnabled(true);
    ui->m_FormatPageButton->setEnabled(true);
    ui->m_FormatPlantillaButton->setEnabled(true);

    QString palette=DEFAULT_XPALETTE;
    QStringList lst=palette.split("@");
    if (lst.count()==8){
        ui->m_leP11->setText(QString("%1").arg(lst.at(0)));
        ui->m_leP12->setText(QString("%1").arg(lst.at(1)));
        ui->m_leP13->setText(QString("%1").arg(lst.at(2)));
        ui->m_leP14->setText(QString("%1").arg(lst.at(3)));
        ui->m_leP15->setText(QString("%1").arg(lst.at(4)));
        ui->m_leP16->setText(QString("%1").arg(lst.at(5)));
        ui->m_leP17->setText(QString("%1").arg(lst.at(6)));
        ui->m_leP18->setText(QString("%1").arg(lst.at(7)));
    }
    ui->m_leVelocidad->setText(QString("%1").arg(DEFAULT_SPEED));
    sltTestERP();
}


int Composicion::CuadroDialogoNuevaComposicion(){

    In2FileDialog fileName(this);
    QString tmp;
    QString Fichero;

    fileName.setAcceptMode(QFileDialog::AcceptSave);
    fileName.setFileMode(QFileDialog::AnyFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("Archivo .ini (*.ini)"));
#else
    fileName.setFilter(tr("Archivo .ini (*.ini)"));
#endif

    fileName.setDefaultSuffix("ini");
    fileName.setDirectory(m_memoryMap->getImagePath());
    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
            w->setAutoDefault(false);
     }*/
    //if (fileName.exec()){
       // Ficheros=fileName.selectedFiles();
        Fichero=fileName.getSaveFileName(this,"Nueva composicion",m_memoryMap->getImagePath(),tr("Archivo .ini(*.ini)"));
    //if (!Ficheros.isEmpty()){
        if (!Fichero.isEmpty()){

            tmp=Fichero;
            QFile File(tmp);
            if (!File.open(QIODevice::WriteOnly)) return -1;

            m_memoryMap->InitComposicion();
            m_memoryMap->m_ComposicionGenerada=true;
            m_memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

            QString ruta,nombre;

            ruta=DevuelveRuta(tmp);
            nombre=DevuelveNombre(tmp);

            QString nombre_bmp=nombre;
            nombre_bmp.replace(".ini",".bmp");
            QString nombreruta_bmp=tmp;
            nombreruta_bmp.replace(".ini",".bmp");


            m_memoryMap->m_mapa.m_variables.NombreIni=nombre;
            m_memoryMap->m_mapa.m_variables.RutaIni=ruta;
            m_memoryMap->m_mapa.m_variables.NombreRutaIni=tmp;

            //Guardo los originales
            ui->m_NombreFicheroIni->setText(QString("%1").arg(tmp));
            m_memoryMap->JumpToTempWorld();

            QString aux=m_memoryMap->m_mapa.m_variables.NombreIni;
            m_memoryMap->m_mapa.m_variables.NombreBmp=aux.replace(".ini",".bmp");/*nombre_bmp*/
            m_memoryMap->m_mapa.m_variables.RutaBmp=m_memoryMap->m_mapa.m_variables.RutaIni;/*ruta*/;
            aux=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
            m_memoryMap->m_mapa.m_variables.NombreRutaBmp=aux.replace(".ini",".bmp");/*nombreruta_bmp;*/

            //Borra ficheros intermedio

            QFile::remove(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
            for(int i=0;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
                QString dummy=QString("%1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
                dummy.replace(".bmp",QString("_C%1.bmp").arg(i+1));
                QFile::remove(QString("%1").arg(dummy));
            }

            m_memoryMap->SetCredentialsCreated();
            //Valores para que la primera visualizacion no sea nada
            m_memoryMap->m_mapa.m_variables.NumColores=1;
            m_memoryMap->m_mapa.m_variables.AnchoPlantilla=m_memoryMap->m_mapa.m_machinedefaults.WebWidth;
            m_memoryMap->m_mapa.m_variables.AltoPlantilla=DEFAULT_WEB_HEIGHT;



            /*QString nombrecampo=QString("Lote");
            m_memoryMap->m_mapa.m_campos[0].NombreCampo=nombrecampo;
            m_memoryMap->m_mapa.m_campos[0].OnOffCampo=1;
            ui->m_OnOffCampo1->setChecked(true);
            ui->m_NombreCampo1->setText(QString("%1").arg(nombrecampo));

            nombrecampo=QString("Fecha caducidad");            
            m_memoryMap->m_mapa.m_campos[1].NombreCampo=nombrecampo;
            m_memoryMap->m_mapa.m_campos[1].OnOffCampo=1;
            ui->m_OnOffCampo2->setChecked(true);
            ui->m_NombreCampo2->setText(QString("%1").arg(nombrecampo));*/


            //Reseteo resto de campos
            this->ui->m_NombreCampo1->clear();
            this->ui->m_NombreCampo2->clear();
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
            this->ui->m_OnOffCampo1->setChecked(false);
            this->ui->m_OnOffCampo2->setChecked(false);
            this->ui->m_OnOffCampo3->setChecked(false);
            this->ui->m_OnOffCampo4->setChecked(false);
            this->ui->m_OnOffCampoVar1->setChecked(false);
            this->ui->m_OnOffCampoVar2->setChecked(false);
            this->ui->m_OnOffCampoVar3->setChecked(false);
            this->ui->m_OnOffCampoVar4->setChecked(false);
            this->ui->m_OnOffCampoVar5->setChecked(false);
            this->ui->m_OnOffCampoVar6->setChecked(false);
            sltPrevisualizarZoom();

            In2CompoFile ficheroIni;
            ficheroIni.GuardarFicheroIni();
            //GuardarFicheroIni();
            File.close();
            tmp.replace("/","\\");
            ui->m_NombreFicheroIniTmp->setText(QString("%1").arg(tmp));

            m_memoryMap->m_ConfiguracionCargada=true;
            m_memoryMap->m_UltimaAccion=UA_CONFIGURACION_CARGADA;

            TestRoot();
            QString foo=tr("Nuevo trabajo generado");
            QMessageBox::information(0,tr("INFO"),foo,QMessageBox::Ok);
            sltUpdateLog(foo);
            return 0;
        }
        else return -1;//JORGE
   // }
   // else return -1;//JORGE

}

/** Comprueba si hay algun formato especial*/
bool Composicion::Comprobar_Formatos_Especiales(){
    bool rtn=false;
    In2ManagerERP *erp=In2ManagerERP::request(0);
    //Tiene que tener configuradas como fechas los datetime de campos 1 y 2
    if (!erp->getClient().compare(ERP_ESTEVE)){
       if (m_memoryMap->m_mapa.m_campos[1].m_dateTimeParameters.formato.isEmpty()){
        sltUpdateLog("Campo 2 no tiene formato de fecha configurado");
        rtn=true;
       }
       if (m_memoryMap->m_mapa.m_campos[2].m_dateTimeParameters.formato.isEmpty()){
           sltUpdateLog("Campo 3 no tiene formato de fecha configurado");
           rtn=true;
       }
    }
    return rtn;
}

/** Comprueba si hay campos vacios*/
bool Composicion::Comprobar_Campos_Vacios(){

    bool aux;

    QString valor_campo1;
    QString valor_campo2;
    QString valor_campo3;
    QString valor_campo4;

    valor_campo1=QString(m_memoryMap->m_mapa.m_campos[0].ValorActualCampo);
    valor_campo2=QString(m_memoryMap->m_mapa.m_campos[1].ValorActualCampo);
    valor_campo3=QString(m_memoryMap->m_mapa.m_campos[2].ValorActualCampo);
    valor_campo4=QString(m_memoryMap->m_mapa.m_campos[3].ValorActualCampo);

    if (
            ((m_memoryMap->m_mapa.m_campos[0].OnOffCampo==1) && (valor_campo1=="")) ||
            ((m_memoryMap->m_mapa.m_campos[1].OnOffCampo==1) && (valor_campo2=="")) ||
            ((m_memoryMap->m_mapa.m_campos[2].OnOffCampo==1) && (valor_campo3=="")) ||
            ((m_memoryMap->m_mapa.m_campos[3].OnOffCampo==1) && (valor_campo4==""))
        )
        aux=true;
    else
        aux=false;

    return (aux);

}
/** Comprueba si la orden de fabricación está vacía*/
bool Composicion::Comprobar_OrdenFabricacion_Vacia(){
    if (m_memoryMap->m_mapa.m_variables.OrdenFabricacion.isEmpty()) return true;
    else return false;
}
/** Comprueba si existe una compisición dada*/
bool Composicion::Comprobar_ExisteComposicion(){

    bool aux;

    QString nombreruta_ini,nombreruta_bmp;

    nombreruta_ini=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    nombreruta_bmp=nombreruta_ini;
    nombreruta_bmp.remove(".ini");
    nombreruta_bmp.append(QString("_OF%1_C1_comp.bmp").arg(m_memoryMap->m_mapa.m_variables.OrdenFabricacion));

    aux=QFile::exists(nombreruta_bmp);

    return (aux);

}

void Composicion::UpdateTablaImprimir(){
    InputData *input = InputData::request(this,"Nombre de la tabla a imprimir");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(Update_TablaImprimir(QString)));
        input->setFocus();
    }
}

void Composicion::Update_TablaImprimir(QString value){
    ui->m_NombreDb->setText(value);
    ui->m_NombreDb->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    //strncpy(m_memoryMap->m_mapa.m_variables.NombreTablaDatosImprimir,m_NombreTablaImprimir->toPlainText().toAscii(),CAMPOS_LEN);
}

void Composicion::AbrirTablaImprimir(){

}
/** Carga BBDD*/
void Composicion::LoadBBDD(QString rutadb){
  /*  miDataBaseCampos.close(); //Por si hay algo abierto
    if (miDataBaseCampos.open(rutadb)!=true){
        emit SendDebugInfo(LOGGER_ERROR,QString("Error abriendo base de datos %1").arg(rutadb));
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,QString("Base de datos %1 abierta correctamente").arg(rutadb));
        ui->m_NombreDb->setText(rutadb);
        m_memoryMap->m_mapa.m_database.NombreRutaDb=rutadb;
        LoadNombresBBDD();
        m_memoryMap->m_ConfiguracionCargada=true;
    }*/
}
/** Abre BBDD*/
void Composicion::AbrirBBDD(){

    QFileDialog fileName;
    QStringList Ficheros;
    QString rutadb;


    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(("Database Files (*.mdb)"));
#else
    fileName.setNameFilter(("Database Files (*.mdb)"));
#endif
    fileName.setDirectory(m_memoryMap->getImagePath());
    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
            w->setAutoDefault(false);
     }*/

    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
            rutadb=QString(Ficheros.at(0));
            int index=rutadb.lastIndexOf("/");
            m_memoryMap->m_mapa.m_database.NombreDb=rutadb.right(rutadb.length()-(index+1));
            m_memoryMap->m_mapa.m_database.RutaDb=rutadb.left(index);
            /*indice1=rutadb.lastIndexOf("/");
            indice2=rutadb.lastIndexOf("\\");
            if (indice1>indice2) indice=indice1;
            else indice=indice2;
            InkjetPathDbCampos=rutadb.left(indice);*/

        }
    }
    LoadBBDD(rutadb);

}


void Composicion::Comprobar_Campos_Variables(){

    if ( ((ui->m_OnOffCampoVar1->isChecked())==true) ||
         ((ui->m_OnOffCampoVar2->isChecked())==true) ||
         ((ui->m_OnOffCampoVar3->isChecked())==true) ||
         ((ui->m_OnOffCampoVar4->isChecked())==true) ||
         ((ui->m_OnOffCampoVar5->isChecked())==true) ||
         ((ui->m_OnOffCampoVar6->isChecked())==true)  )
            m_memoryMap->ImagenVariable=0x01;
    else
            m_memoryMap->ImagenVariable=0x00;

}
//Llama a la base de datos y chequea la configuracion
void Composicion::CheckConfigCampos(){
    /*if (m_memoryMap->m_mapa.m_database.NombreTablaDatosImprimir.isEmpty()) m_memoryMap->m_mapa.m_database.NombreTablaDatosImprimir=QString("DatosImprimir");
    QString tmp_valor=QString("select * from %1").arg(m_memoryMap->m_mapa.m_database.NombreTablaDatosImprimir);
    miDataBaseCampos.ValoresDB(tmp_valor);
    miDataBaseCampos.exec(tmp_valor);
    int temp_campos=0;
        for(unsigned int i=0;i<m_memoryMap->m_mapa.m_database.NumCamposTotalesDB;i++){
            QString tmp=QString(miDataBaseCampos.NombreCampo[i]);
            if (!tmp.isEmpty()){
                if (i==0) {
                    ui->m_NombreCampoVar1->setText(tmp);
                    ui->m_ValorCampoVar1->setText(miDataBaseCampos.m_Tabla[0]);
                    //ui->m_OnOffCampoVar1->setChecked(true);
                    //m_memoryMap->m_mapa.m_camposvariables[0].OnOffCampo=1;
                    m_memoryMap->m_mapa.m_camposvariables[0].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==1) {
                    ui->m_NombreCampoVar2->setText(tmp);
                    ui->m_ValorCampoVar2->setText(miDataBaseCampos.m_Tabla[1]);
                    //ui->m_OnOffCampoVar2->setChecked(true);
                    //m_memoryMap->m_mapa.m_camposvariables[1].OnOffCampo=1;
                    m_memoryMap->m_mapa.m_camposvariables[1].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==2){
                    ui->m_NombreCampoVar3->setText(tmp);
                    ui->m_ValorCampoVar3->setText(miDataBaseCampos.m_Tabla[2]);
                    //ui->m_OnOffCampoVar3->setChecked(true);
                    //m_memoryMap->m_mapa.m_camposvariables[2].OnOffCampo=1;
                    m_memoryMap->m_mapa.m_camposvariables[2].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==3) {
                    ui->m_NombreCampoVar4->setText(tmp);
                    ui->m_ValorCampoVar4->setText(miDataBaseCampos.m_Tabla[3]);
                    //ui->m_OnOffCampoVar4->setChecked(true);
                    //m_memoryMap->m_mapa.m_camposvariables[3].OnOffCampo=1;
                    m_memoryMap->m_mapa.m_camposvariables[3].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==4) {
                    ui->m_NombreCampoVar5->setText(tmp);
                    ui->m_ValorCampoVar5->setText(miDataBaseCampos.m_Tabla[4]);
                    //ui->m_OnOffCampoVar5->setChecked(true);
                    //m_memoryMap->m_mapa.m_camposvariables[4].OnOffCampo=1;
                    m_memoryMap->m_mapa.m_camposvariables[4].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }
                else if (i==5) {
                    ui->m_NombreCampoVar6->setText(tmp);
                    ui->m_ValorCampoVar6->setText(miDataBaseCampos.m_Tabla[5]);
                   // ui->m_OnOffCampoVar6->setChecked(true);
                   // m_memoryMap->m_mapa.m_camposvariables[5].OnOffCampo=1;
                    m_memoryMap->m_mapa.m_camposvariables[5].OrigenValorCampo=QString(TXT_ORIGEN_DB);
                }

                temp_campos++;
            }
        }
        m_memoryMap->m_mapa.m_database.NumCamposTotalesDB=temp_campos;*/
}


/** Carga los nombres de la BBDD*/
void Composicion::LoadNombresBBDD(){

    /*miDataBaseCampos.NumeroCamposDB();
    CheckConfigCampos();*/
}

/** Cargamos datos comunes*/
void Composicion::setDataCommon(){
    /*struct stCommon dummyDataCommon;
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
    DataCommon->ShowComms();*/
}


/** Metemos el trabajo actual en la cola de impresion*/
void Composicion::InsertarEnColaImpresion(){
    In2ManagerERP *erp=In2ManagerERP::request(0);
    erp->insertCurrentWork();
}

/* Devuelve el numero de cabezales
 *Originalmente devolvia todos los cabezales....de momento nos basta con los cabezales de 1 Xusb*/
void Composicion::UpdateNumCabezales(){
}

/** Funcion In2 que intenta eliminar posibles errores de blancos*/
void Composicion::PreprocessImagesIn2(){

    QString StringBlack=m_memoryMap->m_FilePath;
    StringBlack.remove(QString("_C1_comp.bmp"));
    StringBlack.append(QString("_C1_comp.bmp"));
    QString StringRed=m_memoryMap->m_FilePath;
    StringRed.remove(QString("_C1_comp.bmp"));
    StringRed.append(QString("_C2_comp.bmp"));
    QString StringBase=m_memoryMap->m_FilePath;
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
void Composicion::ShowCompositionProps(){
    sltUpdateLog(QString("Creado:%1").arg(m_memoryMap->m_mapa.m_variables.CreadoPor));
    sltUpdateLog(QString("Fecha:%1").arg(m_memoryMap->m_mapa.m_variables.CreadoCuando));
    sltUpdateLog(QString("Modificado:%1").arg(m_memoryMap->m_mapa.m_variables.ModificadoPor));
    sltUpdateLog(QString("Fecha:%1").arg(m_memoryMap->m_mapa.m_variables.ModificadoCuando));

    In2CompoFile ficheroIni;

    QString calculatedHash=ficheroIni.CalculateHashCompoFile();
    QString hashInFile=ficheroIni.GetHashCompoFile();
    if (hashInFile.isEmpty()){
        QString foo=tr("No hay hash en fichero de composicion!");
        sltUpdateLog(foo);
        emit SendDebugInfo(LOGGER_WARNING,foo);
    }
    else{
        if (calculatedHash==hashInFile){
            QString foo=tr("Hash %1 correcto").arg(calculatedHash);
            sltUpdateLog(foo);
            emit SendDebugInfo(LOGGER_WARNING, foo);
        }
        else{
            QString foo=tr("HASH INCORRECTO Leido:%1 Calculado:%2").arg(hashInFile).arg(calculatedHash);
            sltUpdateLog(foo);
            emit SendDebugInfo(LOGGER_WARNING,foo );
        }
    }

}

void Composicion::EnableWidgetsCampo1(){
    ui->m_OnOffCampo1->setEnabled(true);
    UpdateOnOff1();
}

void Composicion::DisableWidgetsCampo1(){
    ui->m_OnOffCampo1->setEnabled(false);
    ui->m_OnOffCampo1->setChecked(false);
    UpdateOnOff1();
}


void Composicion::EnableWidgetsCampo2(){
    ui->m_OnOffCampo2->setEnabled(true);
    UpdateOnOff2();
}

void Composicion::DisableWidgetsCampo2(){
    ui->m_OnOffCampo2->setEnabled(false);
    ui->m_OnOffCampo2->setChecked(false);
    UpdateOnOff2();
}


void Composicion::EnableWidgetsCampo3(){
    ui->m_OnOffCampo3->setEnabled(true);
    UpdateOnOff3();
}

void Composicion::DisableWidgetsCampo3(){
    ui->m_OnOffCampo3->setEnabled(false);
    ui->m_OnOffCampo3->setChecked(false);
    UpdateOnOff3();
}


void Composicion::EnableWidgetsCampo4(){
    ui->m_OnOffCampo4->setEnabled(true);
    UpdateOnOff4();
}

void Composicion::DisableWidgetsCampo4(){
    ui->m_OnOffCampo4->setEnabled(false);
    ui->m_OnOffCampo4->setChecked(false);
    UpdateOnOff4();
}


void Composicion::EnableWidgetsCampoVar1(){
    ui->m_OnOffCampoVar1->setEnabled(true);
    UpdateOnOffVar1();
}

void Composicion::DisableWidgetsCampoVar1(){
    ui->m_OnOffCampoVar1->setEnabled(false);
    ui->m_OnOffCampoVar1->setChecked(false);
    UpdateOnOffVar1();
}


void Composicion::EnableWidgetsCampoVar2(){
    ui->m_OnOffCampoVar2->setEnabled(true);
    UpdateOnOffVar2();
}

void Composicion::DisableWidgetsCampoVar2(){
    ui->m_OnOffCampoVar2->setEnabled(false);
    ui->m_OnOffCampoVar2->setChecked(false);
    UpdateOnOffVar2();
}


void Composicion::EnableWidgetsCampoVar3(){
    ui->m_OnOffCampoVar3->setEnabled(true);
    UpdateOnOffVar3();
}

void Composicion::DisableWidgetsCampoVar3(){
    ui->m_OnOffCampoVar3->setEnabled(false);
    ui->m_OnOffCampoVar3->setChecked(false);
    UpdateOnOffVar3();
}


void Composicion::EnableWidgetsCampoVar4(){
    ui->m_OnOffCampoVar4->setEnabled(true);
    UpdateOnOffVar4();
}

void Composicion::DisableWidgetsCampoVar4(){
    ui->m_OnOffCampoVar4->setEnabled(false);
    ui->m_OnOffCampoVar4->setChecked(false);
    UpdateOnOffVar4();
}


void Composicion::EnableWidgetsCampoVar5(){
    ui->m_OnOffCampoVar5->setEnabled(true);
    UpdateOnOffVar5();
}

void Composicion::DisableWidgetsCampoVar5(){
    ui->m_OnOffCampoVar5->setEnabled(false);
    ui->m_OnOffCampoVar5->setChecked(false);
    UpdateOnOffVar5();
}


void Composicion::EnableWidgetsCampoVar6(){
    ui->m_OnOffCampoVar6->setEnabled(true);
    UpdateOnOffVar6();
}

void Composicion::DisableWidgetsCampoVar6(){
    ui->m_OnOffCampoVar6->setEnabled(false);
    ui->m_OnOffCampoVar6->setChecked(false);
    UpdateOnOffVar6();
}

void Composicion::visualizarIni(){
    viewIni *ScrviewIni;
    ScrviewIni=new viewIni(0);
    ScrviewIni->show();
}
/** Lanza, si existe, la calculadora de tinta*/
void Composicion::sltLaunchInkCalculator(){
    QString tmpFile;
    if (!m_memoryMap->m_Imagen.isNull()){
        tmpFile=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(TMP_INKCALCFILE);
        m_memoryMap->m_Imagen.save(tmpFile);
    }
    In2InkCalcLauncher inkCalc;
    if (inkCalc.exists()){
        inkCalc.start(tmpFile);
    }
}


