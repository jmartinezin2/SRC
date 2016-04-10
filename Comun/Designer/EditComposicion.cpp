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
#include "EditComposicion.h"
#include "in2defines.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
//#include "principal.h"
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
//#include "principal.h"
#include <QList>
#include "In2GraphicsView.h"
#include "in2imagecreator.h"
#include "in2message.h"
#include "inkjetdebug.h"
#include <QMouseEvent>


//#include "dbgmem.h"
#include "globals.h"
#include "In2BarcodeClass.h"
#include "formatBarcode.h"

#define OPCION_TAMANO		0
#define OPCION_IMAGEN_FIJA	1
#define OPCION_MATRIZ		2
#define OPCION_MACULAS		3
#define OPCION_MARCAS		4


EditComposicion::EditComposicion(QWidget *parent)
    : ui(new Ui::EditValoresUI)
{

    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    ui->setupUi(this);
    m_memoryMap=MemoryMap::request(0);
    ui->m_leMessage->setStyleSheet("QLineEdit{background: black;}");
    //this->SetDataCommon();
    ui->m_WebWidth->setPalette(dummyPaletteEnabled);
    ui->m_WebHeight->setPalette(dummyPaletteEnabled);
    ui->m_teComentario->setText(m_memoryMap->m_mapa.m_variables.Comentario);
    ui->m_PuntoInicial_X->setPalette(dummyPaletteEnabled);
    ui->m_PuntoInicial_Y->setPalette(dummyPaletteEnabled);

    //m_memoryMap->m_mapa.m_variables.DefaultNumHeads*70;


    ui->m_NumeroFilas->setPalette(dummyPaletteEnabled);
    ui->m_NumeroColumnas->setPalette(dummyPaletteEnabled);
    ui->m_SeparacionFilas->setPalette(dummyPaletteEnabled);
    ui->m_SeparacionColumnas->setPalette(dummyPaletteEnabled);
    ui->m_AnguloPlantilla->setPalette(dummyPaletteEnabled);

    ui->m_SeleccionMacula->addItem("Izqda");
    ui->m_SeleccionMacula->addItem("Dcha");
    ui->m_SeleccionMacula->setCurrentIndex(0);

    ui->m_ColorSeleccionado->setReadOnly(true);

    if (m_memoryMap->m_mapa.m_variables.NumColores>1) ui->m_Macula_Color->setVisible(true);
    else ui->m_Macula_Color->setVisible(false);

    for (int i=0;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
        ui->m_Macula_Color->addItem(QString("%1").arg(i+1));
    }
    ui->m_Macula_Color->setCurrentIndex(0);

    ui->m_Macula_X->setPalette(dummyPaletteEnabled);
    ui->m_Macula_Y->setPalette(dummyPaletteEnabled);
    ui->m_Macula_ancho->setPalette(dummyPaletteEnabled);
    ui->m_Macula_alto->setPalette(dummyPaletteEnabled);
    ui->m_Macula_repeticiones->setPalette(dummyPaletteEnabled);
    ui->m_Macula_frecuencia->setPalette(dummyPaletteEnabled);
    ui->m_Macula_Color->setPalette(dummyPaletteEnabled);

//#ifdef COLOR_MONOCROMO
    if (m_memoryMap->m_mapa.m_variables.NumColores==1){
        ui->m_Macula_Color->setVisible(false);
        ui->m_ColorSeleccionado->setVisible(false);
    }

    ui->tabWidget->setCurrentIndex(0);
    //ui->tabWidget->setTabEnabled(3,false);

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_CancelButton,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
    //connect(ui->m_OpcionConfiguracion,SIGNAL(activated (QString)),this,SLOT(CambiarControles()));
    connect(ui->m_SeleccionMacula,SIGNAL(activated (QString)),this,SLOT(CambiarMacula()));
    connect(ui->m_Macula_Color,SIGNAL(activated (QString)),this,SLOT(UpdateColorMacula()));

    connect(ui->m_pbAjustar,SIGNAL(clicked()),this,SLOT(sltAjustarW()));
    connect(ui->m_pbAjustarCorte,SIGNAL(clicked(bool)),this,SLOT(sltAjustarCorte()));
    connect(ui->m_pbRowColAuto,SIGNAL(clicked(bool)),this,SLOT(sltAutoRowCol()));
    //InkjetDebug *info=InkjetDebug::request(0);
    //connect(ui->m_ButtonShowLog,SIGNAL(clicked()),info,SLOT(showLog()));
    //connect(this,SIGNAL(SendDebugInfo(unsigned char,QString,bool)),info, SLOT(ProcessMsg(unsigned char,QString)));

    connect(ui->m_pbDelComment,SIGNAL(clicked()),this,SLOT(sltDeleteComentario()));
    connect(ui->m_pbEditComment,SIGNAL(clicked()),this,SLOT(sltEditComentario()));
    connect(ui->m_pbSaveComment,SIGNAL(clicked()),this,SLOT(sltSaveComentario()));
    connect(ui->m_cbEnableGrid,SIGNAL(clicked()),this,SLOT(sltEnableGrid()));


    connect(ui->m_Macula_OnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMacula()));

    connect(ui->m_MarcaI_OnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMarcaIzquierda()));
    connect(ui->m_MarcaI_configuracion,SIGNAL(clicked()),this,SLOT(ConfigurarMarcaIzquierda()));
    connect(ui->m_MarcaI_OnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMarcaDerecha()));
    connect(ui->m_MarcaD_configuracion,SIGNAL(clicked()),this,SLOT(ConfigurarMarcaDerecha()));



    connect(ui->m_AceptarValores,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));



    //#ifdef CON_OPC
    //connect((Principal*)this->GetMainWindow(), SIGNAL(ActualizaDataOPC()),this, SLOT(ActualizaDataSlotOPC()));
    //#endif

    TestRoot();

    PasarDeMapATemp();
    CargaDatos();


    GlobalSetScreenSize(this);


    //m_scene = new In2GraphicsScene(0, 0,m_memoryMap->m_Imagen.width(), m_memoryMap->m_Imagen.height());
    m_scene = new In2GraphicsScene(0, 0,0,0);
    ui->m_graphicsView->setScene(m_scene);
    //ui->m_graphicsView->show();
    m_In2GView=new In2GraphicsView(ui->m_graphicsView);

    MuestraImagen();

    ui->m_AnguloPlantilla->installEventFilter(this);
    ui->m_AnguloPlantilla->show();
    //Ajustamos los cortes
    ui->m_leWCorte->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AnchoPlantilla));
    ui->m_leHCorte->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AltoPlantilla));

    //La de marca la obviamos
    //ui->tabWidget->setTabEnabled(2,false);
    QTimer::singleShot(0,this,SLOT(sltEnableGrid()));
}

/** Ajusta las filas y columnas*/
void EditComposicion::sltAutoRowCol(){
    int filas=ui->m_NumeroFilas->text().toInt();
    int cols=ui->m_NumeroColumnas->text().toInt();
    bool render=false;
    if (filas>0){
        ui->m_SeparacionFilas->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AltoPlantilla));
        render=true;
    }
    else{
        ui->m_SeparacionFilas->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AltoPlantilla));
    }
    if (cols>0){
        ui->m_SeparacionColumnas->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AnchoPlantilla));
        render=true;
    }
    else{
        ui->m_SeparacionColumnas->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AnchoPlantilla));
    }
    if (render){
        AceptarValoresCampos();
    }
}


/** Ajusta la imagen al corte*/
void EditComposicion::sltAjustarCorte(){
    ui->m_WebWidth->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AnchoPlantilla));
    ui->m_WebHeight->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AltoPlantilla));
    AceptarValoresCampos();
}

/** Habilita PharmaCode izquierdo*/
void EditComposicion::UpdateOnOffMarcaIzquierda(){

}
/** Habilita PharmaCode derecho*/
void EditComposicion::UpdateOnOffMarcaDerecha(){

}
/** Configura PharmaCode derecho*/
void EditComposicion::ConfigurarMarcaDerecha(){
        struct m_BarcodeParameters data;
        data=m_memoryMap->m_mapa.m_Marca.barcodeInf;
        data.texto=ui->m_MarcaD_Valor->text();
        formatBarcode *ScrformatBarcode;
        ScrformatBarcode=new formatBarcode(this,data);
        ScrformatBarcode->show();
        QObject::connect(ScrformatBarcode, SIGNAL(sglBarcodeConfigured(const m_BarcodeParameters & )),this, SLOT(sltBarcodeConfiguredMarcaD(const m_BarcodeParameters &)));
}
/** Configura PharmaCode izquierdo*/
void EditComposicion::ConfigurarMarcaIzquierda(){
        struct m_BarcodeParameters data;
        data=m_memoryMap->m_mapa.m_Marca.barcodeSup;
        data.texto=ui->m_MarcaI_Valor->text();
        formatBarcode *ScrformatBarcode;
        ScrformatBarcode=new formatBarcode(this,data);
        ScrformatBarcode->show();
        QObject::connect(ScrformatBarcode, SIGNAL(sglBarcodeConfigured(const m_BarcodeParameters & )),this, SLOT(sltBarcodeConfiguredMarcaI(const m_BarcodeParameters &)));

}


/** Ajusta el ancho máximo*/
void EditComposicion::sltAjustarW(){
    ui->m_WebWidth->setText(ui->m_MaxWebWidth->text());
    AceptarValoresCampos();
}
/** Carga datos de marcas*/
void EditComposicion::_loadDatosMarca(){
    struct strcMarca foo=m_memoryMap->m_mapa.m_Marca;
    ui->m_MarcaI_Valor->setText(foo.MarcaSupValor);
    ui->m_MarcaD_Valor->setText(foo.MarcaInfValor);

    /*if (foo.MarcaInfTipo>1) foo.MarcaInfTipo=0;
    if (foo.MarcaSupTipo>1) foo.MarcaSupTipo=0;*/

    ui->m_MarcaI_Tipo->setCurrentIndex(foo.MarcaSupTipo);
    ui->m_MarcaD_Tipo->setCurrentIndex(foo.MarcaInfTipo);

    if (foo.MarcaInfColor>m_memoryMap->m_mapa.m_variables.NumColores) foo.MarcaInfColor=0;
    if (foo.MarcaSupColor>m_memoryMap->m_mapa.m_variables.NumColores) foo.MarcaSupColor=0;
    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    ui->m_MarcaI_X->setPalette(dummyPaletteEnabled);
    ui->m_MarcaI_Y->setPalette(dummyPaletteEnabled);
    ui->m_MarcaI_repeticiones->setPalette(dummyPaletteEnabled);
    ui->m_MarcaI_frecuencia->setPalette(dummyPaletteEnabled);
    ui->m_MarcaD_X->setPalette(dummyPaletteEnabled);
    ui->m_MarcaD_Y->setPalette(dummyPaletteEnabled);
    ui->m_MarcaD_repeticiones->setPalette(dummyPaletteEnabled);
    ui->m_MarcaD_frecuencia->setPalette(dummyPaletteEnabled);


    ui->m_MarcaI_X->setText(QString("%1").arg(foo.MarcaSupX));
    ui->m_MarcaI_Y->setText(QString("%1").arg(foo.MarcaSupY));
    ui->m_MarcaD_X->setText(QString("%1").arg(foo.MarcaInfX));
    ui->m_MarcaD_Y->setText(QString("%1").arg(foo.MarcaInfY));
    ui->m_MarcaD_frecuencia->setText(QString("%1").arg(foo.MarcaInfFrecuencia));
    ui->m_MarcaI_frecuencia->setText(QString("%1").arg(foo.MarcaSupFrecuencia));
    ui->m_MarcaI_repeticiones->setText(QString("%1").arg(foo.MarcaSupRepeticiones));
    ui->m_MarcaD_repeticiones->setText(QString("%1").arg(foo.MarcaInfRepeticiones));

    ui->m_MarcaD_OnOff->setChecked(foo.MarcaInfOnOff);
    ui->m_MarcaI_OnOff->setChecked(foo.MarcaSupOnOff);


    ui->m_MarcaIFuente->setCurrentFont(foo.MarcaSupFuente);
    ui->m_MarcaDFuente->setCurrentFont(foo.MarcaIInfFuente);
    int index=ui->m_MarcaIEstilo->findText(foo.MarcaSupEstiloLetraCampo);
    ui->m_MarcaIEstilo->setCurrentIndex(index);
    index=ui->m_MarcaDEstilo->findText(foo.MarcaInfEstiloLetraCampo);
    ui->m_MarcaDEstilo->setCurrentIndex(index);

    ui->m_leMarcaILetterSpace->setText(QString("%1").arg(foo.MarcaSupLetterSpace));
    ui->m_leMarcaDLetterSpace->setText(QString("%1").arg(foo.MarcaInfLetterSpace));
    ui->m_MarcaITamano->setText(QString("%1").arg(foo.MarcaSupTamanoFuente));
    ui->m_MarcaDTamano->setText(QString("%1").arg(foo.MarcaInfTamanoFuente));

    m_memoryMap->m_mapa.m_Marca=foo;
}

/** Filtro de eventos*/
bool EditComposicion::eventFilter(QObject *p, QEvent *e){
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
        if (p==ui->m_AnguloPlantilla){
            if (keyEvent->key() == Qt::Key_Up){
                int angulo=ui->m_AnguloPlantilla->text().toInt();
                if (angulo<360) angulo++;
                else angulo=0;
                ui->m_AnguloPlantilla->setText(QString("%1").arg(angulo));
                AceptarValoresCampos();
            }
            else if (keyEvent->key() == Qt::Key_Down){
                int angulo=ui->m_AnguloPlantilla->text().toInt();
                if (angulo>0) angulo--;
                else angulo=0;
                ui->m_AnguloPlantilla->setText(QString("%1").arg(angulo));
                AceptarValoresCampos();
            }

        }
    return QWidget::eventFilter(p,e);
}

/** Activa*desctiva el grid*/
void EditComposicion::sltEnableGrid(){
    bool value=ui->m_cbEnableGrid->isChecked();
    m_scene->enableGrid(value);
}

void EditComposicion::PasarDeTempAMap(){

    m_memoryMap->m_mapa.m_variables.WebWidth=WebWidth_tmp;
    m_memoryMap->m_mapa.m_variables.WebHeight=WebHeight_tmp;
    m_memoryMap->m_mapa.m_variables.PuntoInicialX=PuntoInicialX_tmp;
    m_memoryMap->m_mapa.m_variables.PuntoInicialY=PuntoInicialY_tmp;
    m_memoryMap->m_mapa.m_variables.FilasPlantilla=FilasPlantilla_tmp;
    m_memoryMap->m_mapa.m_variables.ColumnasPlantilla=ColumnasPlantilla_tmp;
    m_memoryMap->m_mapa.m_variables.SeparacionX=SeparacionX_tmp;
    m_memoryMap->m_mapa.m_variables.SeparacionY=SeparacionY_tmp;
    m_memoryMap->m_mapa.m_variables.AnguloPlantilla=AnguloPlantilla_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaX=MaculaIzqdaX_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaY=MaculaIzqdaY_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=MaculaIzqdaAncho_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=MaculaIzqdaAlto_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaX=MaculaDchaX_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaY=MaculaDchaY_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaAncho=MaculaDchaAncho_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaAlto=MaculaDchaAlto_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=MaculaIzqdaOnOff_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff=MaculaDchaOnOff_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=MaculaDchaRep_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=MaculaDchaFreq_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=MaculaIzqdaRep_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=MaculaIzqdaFreq_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor=MaculaIzqdaColor_tmp;
    m_memoryMap->m_mapa.m_variables.MaculaDchaColor=MaculaDchaColor_tmp;

    m_memoryMap->m_mapa.m_Marca=m_marca;


}

void EditComposicion::PasarDeMapATemp(){

    WebWidth_tmp=m_memoryMap->m_mapa.m_variables.WebWidth;
    WebHeight_tmp=m_memoryMap->m_mapa.m_variables.WebHeight;
    PuntoInicialX_tmp=m_memoryMap->m_mapa.m_variables.PuntoInicialX;
    PuntoInicialY_tmp=m_memoryMap->m_mapa.m_variables.PuntoInicialY;
    FilasPlantilla_tmp=m_memoryMap->m_mapa.m_variables.FilasPlantilla;
    ColumnasPlantilla_tmp=m_memoryMap->m_mapa.m_variables.ColumnasPlantilla;
    SeparacionX_tmp=m_memoryMap->m_mapa.m_variables.SeparacionX;
    SeparacionY_tmp=m_memoryMap->m_mapa.m_variables.SeparacionY;
    AnguloPlantilla_tmp=m_memoryMap->m_mapa.m_variables.AnguloPlantilla;
    MaculaIzqdaX_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaX;
    MaculaIzqdaY_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaY;
    MaculaIzqdaAncho_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaAncho;
    MaculaIzqdaAlto_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaAlto;
    MaculaDchaX_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaX;
    MaculaDchaY_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaY;
    MaculaDchaAncho_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaAncho;
    MaculaDchaAlto_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaAlto;
    MaculaIzqdaOnOff_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff;
    MaculaDchaOnOff_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff;
    MaculaDchaRep_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones;
    MaculaDchaFreq_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia;
    MaculaIzqdaRep_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones;
    MaculaIzqdaFreq_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia;
    MaculaIzqdaColor_tmp=m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor;
    MaculaDchaColor_tmp=m_memoryMap->m_mapa.m_variables.MaculaDchaColor;

    m_marca=m_memoryMap->m_mapa.m_Marca;

}


void EditComposicion::TestRoot(){
     if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_WebWidth->setEnabled(false);
        ui->m_WebHeight->setEnabled(false);
        ui->m_PuntoInicial_X->setEnabled(false);
        ui->m_PuntoInicial_Y->setEnabled(false);
        ui->m_NumeroFilas->setEnabled(false);
        ui->m_NumeroColumnas->setEnabled(false);
        ui->m_SeparacionFilas->setEnabled(false);
        ui->m_SeparacionColumnas->setEnabled(false);
        ui->m_AnguloPlantilla->setEnabled(false);
        ui->m_SeleccionMacula->setEnabled(false);
        ui->m_Macula_X->setEnabled(false);
        ui->m_Macula_Y->setEnabled(false);
        ui->m_Macula_ancho->setEnabled(false);
        ui->m_Macula_alto->setEnabled(false);
        ui->m_Macula_OnOff->setEnabled(false);
        ui->m_AceptarValores->setEnabled(false);
        ui->m_Macula_repeticiones->setEnabled(false);
        ui->m_Macula_frecuencia->setEnabled(false);
        ui->m_Macula_Color->setEnabled(false);
        ui->m_MarcaI_X->setEnabled(false);
        ui->m_MarcaI_Y->setEnabled(false);
        ui->m_MarcaI_OnOff->setEnabled(false);
        ui->m_MarcaI_repeticiones->setEnabled(false);
        ui->m_MarcaI_frecuencia->setEnabled(false);
    }
}

EditComposicion::~EditComposicion(){
}

//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void EditComposicion::AceptarSeleccion(){

    GuardaDatos();
    LaunchComposicion();
    this->deleteLater();

}


void EditComposicion::CancelarSeleccion(){

    PasarDeTempAMap();
    LaunchComposicion();
    this->deleteLater();

}


// Actualiza los datos de pantalla
void EditComposicion::ActualizaDataSlot(){
    /*DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();*/
}

void EditComposicion::UpdateWebWidth(){
    InputData *input = InputData::request(this,"Web Width [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateWidth(QString)));
    }
}

void EditComposicion::UpdateWidth(QString value){
    ui->m_WebWidth->setText(value);
    ui->m_WebWidth->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateWebHeight(){
    InputData *input = InputData::request(this,"Alto [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateHeight(QString)));
    }
}

void EditComposicion::UpdateHeight(QString value){
    ui->m_WebHeight->setText(value);
    ui->m_WebHeight->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateNumeroFilas(){
    InputData *input = InputData::request(this,"Número Filas");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFilas(QString)));
    }
}

void EditComposicion::UpdateFilas(QString value){
    ui->m_NumeroFilas->setText(value);
    ui->m_NumeroFilas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateNumeroColumnas(){
    InputData *input = InputData::request(this,"Número Columnas");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateColumnas(QString)));
    }
}

void EditComposicion::UpdateColumnas(QString value){
    ui->m_NumeroColumnas->setText(value);
    ui->m_NumeroColumnas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditComposicion::UpdateSeparacionFilas(){
    InputData *input = InputData::request(this,"Separación Filas [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateSepFilas(QString)));
    }
}

void EditComposicion::UpdateSepFilas(QString value){
    ui->m_SeparacionFilas->setText(value);
    ui->m_SeparacionFilas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateSeparacionColumnas(){
    InputData *input = InputData::request(this,"Separación Columnas [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateSepColumnas(QString)));
    }
}

void EditComposicion::UpdateSepColumnas(QString value){
    ui->m_SeparacionColumnas->setText(value);
    ui->m_SeparacionColumnas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateAnguloPlantilla(){
    InputData *input = InputData::request(this,"Angulo Plantilla [deg]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAngulo(QString)));
    }
}

void EditComposicion::UpdateAngulo(QString value){
    ui->m_AnguloPlantilla->setText(value);
    ui->m_AnguloPlantilla->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdatePuntoInicialX(){
    InputData *input = InputData::request(this,"Coordenada X Punto Inicial [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateInicialX(QString)));
    }
}

void EditComposicion::UpdateInicialX(QString value){
    ui->m_PuntoInicial_X->setText(value);
    ui->m_PuntoInicial_X->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdatePuntoInicialY(){
    InputData *input = InputData::request(this,"Coordenada Y Punto Inicial [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateInicialY(QString)));
    }
}

void EditComposicion::UpdateInicialY(QString value){
    ui->m_PuntoInicial_Y->setText(value);
    ui->m_PuntoInicial_Y->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateOnOffMacula(){
    if (ui->m_Macula_OnOff->isChecked()){
        ui->m_Macula_X->setEnabled(true);
        ui->m_Macula_Y->setEnabled(true);
        ui->m_Macula_ancho->setEnabled(true);
        ui->m_Macula_alto->setEnabled(true);
        ui->m_Macula_repeticiones->setEnabled(true);
        ui->m_Macula_frecuencia->setEnabled(true);
        ui->m_Macula_Color->setEnabled(true);
    }
    else{
        ui->m_Macula_X->setEnabled(false);
        ui->m_Macula_Y->setEnabled(false);
        ui->m_Macula_ancho->setEnabled(false);
        ui->m_Macula_alto->setEnabled(false);
        ui->m_Macula_repeticiones->setEnabled(false);
        ui->m_Macula_frecuencia->setEnabled(false);
        ui->m_Macula_Color->setEnabled(false);
    }
}


void EditComposicion::UpdateMaculaX(){
    int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    QString titulo;
    switch (indice_macula){
    case 0:
        titulo=QString("Posición X Mácula Izqda [mm]");
        break;
    case 1:
        titulo=QString("Posición X Mácula Dcha [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateX(QString)));
}


void EditComposicion::UpdateX(QString value){
    ui->m_Macula_X->setText(value);
    ui->m_Macula_X->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditComposicion::UpdateMaculaY(){
    int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    QString titulo;
    switch (indice_macula){
    case 0:
        titulo=QString("Posición Y Mácula Izqda [mm]");
        break;
    case 1:
        titulo=QString("Posición Y Mácula Dcha [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateY(QString)));
}


void EditComposicion::UpdateY(QString value){
    ui->m_Macula_Y->setText(value);
    ui->m_Macula_Y->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditComposicion::UpdateMaculaAncho(){
    int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    QString titulo;
    switch (indice_macula){
    case 0:
        titulo=QString("Ancho Mácula Izqda [mm]");
        break;
    case 1:
        titulo=QString("Ancho Mácula Dcha [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAncho(QString)));
}


void EditComposicion::UpdateAncho(QString value){
    ui->m_Macula_ancho->setText(value);
    ui->m_Macula_ancho->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditComposicion::UpdateMaculaAlto(){
    int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    QString titulo;
    switch (indice_macula){
    case 0:
        titulo=QString("Alto Mácula Izqda [mm]");
        break;
    case 1:
        titulo=QString("Alto Mácula Dcha [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAlto(QString)));
}


void EditComposicion::UpdateAlto(QString value){
    ui->m_Macula_alto->setText(value);
    ui->m_Macula_alto->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditComposicion::UpdateMaculaFrecuencia(){
    int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    QString titulo;
    switch (indice_macula){
    case 0:
        titulo=QString("Frecuencia Mácula Izqda [mm]");
        break;
    case 1:
        titulo=QString("Frecuencia Mácula Dcha [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFrecuencia(QString)));
}


void EditComposicion::UpdateFrecuencia(QString value){
    ui->m_Macula_frecuencia->setText(value);
    ui->m_Macula_frecuencia->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditComposicion::UpdateMaculaRepeticiones(){
    int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    QString titulo;
    switch (indice_macula){
    case 0:
        titulo=QString("Repeticiones Mácula Izqda");
        break;
    case 1:
        titulo=QString("Repeticiones Mácula Dcha");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateRepeticiones(QString)));
}

void EditComposicion::UpdateColorMacula(){
    unsigned char indice;
    indice=ui->m_Macula_Color->currentIndex();
    QPalette paleta_color_pleno;
    paleta_color_pleno.setColor(QPalette::Base,QColor(m_memoryMap->m_mapa.m_colores[indice].valor));
    ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);
}

void EditComposicion::UpdateRepeticiones(QString value){
    ui->m_Macula_repeticiones->setText(value);
    ui->m_Macula_repeticiones->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}




void EditComposicion::AceptarValoresCampos(){

    m_memoryMap->m_mapa.m_variables.bReloadCompo=true;
    ui->m_AceptarButton->setEnabled(false);
    ui->m_CancelButton->setEnabled(false);

    GuardaDatos();
    checkDimensiones();
    //CreaImagenPagina();

    In2ImageCreator *imgCreator=new In2ImageCreator();
    imgCreator->sltGenerateImage();

    MuestraImagen();


    ui->m_AceptarButton->setEnabled(true);
    ui->m_CancelButton->setEnabled(true);
}

void EditComposicion::LaunchComposicion(){
    m_memoryMap->m_mapa.m_variables.bReloadCompo=false;
    Composicion *ScrComposicion;
    ScrComposicion=new Composicion(0);
    ScrComposicion->show();
    this->deleteLater();
}

void EditComposicion::MuestraImagen(){
    //In2Message *msg=new In2Message(0,"Rendering");

    In2ImageCreator *img=new In2ImageCreator();
    QString path=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
    path.replace(".ini","_OF_C1_comp.bmp");
    QPixmap *lienzo=new QPixmap(/*m_memoryMap->m_Imagen*/img->returnCompoImage(path));
    QPainter *painter=new QPainter(lienzo);

    painter->setPen( QPen( Qt::red, 10 ) );
    int ww=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.WebWidth);
    int wh=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.WebHeight);
    painter->drawRect(0,0,ww,wh);


    painter->end();

    QList<QGraphicsItem*> items=m_scene->items();
    In2GraphicsPixmapItem *p;
    if (items.count()){
        p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
        p->setPixmap(*lienzo);
        m_scene->setSceneRect(lienzo->rect());
    }
    else{
        p=new In2GraphicsPixmapItem(0,0,false);
        p->setTransformationMode(Qt::SmoothTransformation);
        p->setPixmap(*lienzo);
        m_scene->setSceneRect(lienzo->rect());
        m_scene->addItem(p);
    }

    ui->m_graphicsView->fitInView(p,Qt::KeepAspectRatio);
    //msg->deleteLater();

}
/** Renderiza la composicion*/
void EditComposicion::renderComposition(){
    //QPixmap compo=In2Compo.render();
    /*QPixmap compo=In2Compo.getItem(1).getPixmap();
    QGraphicsScene* scene = new QGraphicsScene(QRect(0, 0,m_memoryMap->m_Imagen.width(), m_memoryMap->m_Imagen.height()));
    scene->addPixmap(compo);
    ui->m_graphicsView->setScene(scene);
    ui->m_graphicsView->show();
    In2GraphicsView *p=new In2GraphicsView(ui->m_graphicsView);*/

}
/** Chequea dimensiones*/
void EditComposicion::checkDimensiones(){
    if (m_memoryMap->m_mapa.m_variables.DefaultNumHeads*DEFAULT_HEAD_WIDTH/FACTOR_PIXELS_MM<m_memoryMap->m_mapa.m_variables.WebWidth){
        QPalette palette=ui->m_leMessage->palette();
        palette.setColor(QPalette::Text,QColor(Qt::red));
        ui->m_leMessage->setPalette(palette);
        ui->m_leMessage->setText("Fuera de márgenes");
    }
    else{
        QPalette palette=ui->m_leMessage->palette();
        palette.setColor(QPalette::Text,QColor(Qt::green));
        ui->m_leMessage->setPalette(palette);
        ui->m_leMessage->setText("OK");
    }
}

void EditComposicion::CargaDatos(){

    QString tmp;

    tmp.setNum(m_memoryMap->m_mapa.m_variables.WebWidth);
    ui->m_WebWidth->setText(tmp);

    tmp.setNum(m_memoryMap->m_mapa.m_variables.WebHeight);
    ui->m_WebHeight->setText(tmp);

    ui->m_MaxWebWidth->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.DefaultNumHeads*DEFAULT_HEAD_WIDTH/FACTOR_PIXELS_MM));
    ui->m_MaxWebHeight->setText(QString("%1").arg(tmp));

    checkDimensiones();


    tmp.setNum(m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas);
    ui->m_NumeroFilas->setText(tmp);

    tmp.setNum(m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas);
    ui->m_NumeroColumnas->setText(tmp);

    tmp.setNum(m_memoryMap->m_mapa.m_variables.SeparacionY);
    ui->m_SeparacionFilas->setText(tmp);

    tmp.setNum(m_memoryMap->m_mapa.m_variables.SeparacionX);
    ui->m_SeparacionColumnas->setText(tmp);

    /*tmp.setNum(m_memoryMap->m_mapa.m_variables.AnguloPlantilla);
    ui->m_AnguloPlantilla->setText(tmp);*/
    ui->m_AnguloPlantilla->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AnguloPlantilla));

    tmp.setNum(m_memoryMap->m_mapa.m_variables.PuntoInicialX);
    ui->m_PuntoInicial_X->setText(tmp);

    tmp.setNum(m_memoryMap->m_mapa.m_variables.PuntoInicialY);
    ui->m_PuntoInicial_Y->setText(tmp);

    CambiarMacula();    
    _loadDatosMarca();


}

void EditComposicion::CambiarMacula(){
    QString tmp;
    unsigned int indice_macula;

    indice_macula=ui->m_SeleccionMacula->currentIndex();
    switch (indice_macula){
        case 0:
                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaIzqdaX);
                ui->m_Macula_X->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaIzqdaY);
                ui->m_Macula_Y->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaIzqdaAncho);
                ui->m_Macula_ancho->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaIzqdaAlto);
                ui->m_Macula_alto->setText(tmp);


                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia);
                ui->m_Macula_frecuencia->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones);
                ui->m_Macula_repeticiones->setText(tmp);

                ui->m_Macula_Color->setCurrentIndex(m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor-1);
                UpdateColorMacula();

                if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff==1) ui->m_Macula_OnOff->setChecked(true);
                else ui->m_Macula_OnOff->setChecked(false);
                UpdateOnOffMacula();
                break;
        case 1:
                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaDchaX);
                ui->m_Macula_X->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaDchaY);
                ui->m_Macula_Y->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaDchaAncho);
                ui->m_Macula_ancho->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaDchaAlto);
                ui->m_Macula_alto->setText(tmp);


                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia);
                ui->m_Macula_frecuencia->setText(tmp);

                tmp.setNum(m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones);
                ui->m_Macula_repeticiones->setText(tmp);

                ui->m_Macula_Color->setCurrentIndex(m_memoryMap->m_mapa.m_variables.MaculaDchaColor-1);
                UpdateColorMacula();

                if (m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff==1) ui->m_Macula_OnOff->setChecked(true);
                else ui->m_Macula_OnOff->setChecked(false);
                UpdateOnOffMacula();
                break;
    }

}



void EditComposicion::GuardaDatos(){

    unsigned int tmp_ImpresionesPorEnvioFilas;
    unsigned int tmp_ImpresionesPorEnvioColumnas;
    //QString tmp;

    m_memoryMap->m_mapa.m_variables.WebWidth=ui->m_WebWidth->text().toDouble();
    m_memoryMap->m_mapa.m_variables.WebHeight=ui->m_WebHeight->text().toDouble();
    m_memoryMap->m_mapa.m_variables.PuntoInicialX=ui->m_PuntoInicial_X->text().toDouble();
    m_memoryMap->m_mapa.m_variables.PuntoInicialY=ui->m_PuntoInicial_Y->text().toDouble();
    tmp_ImpresionesPorEnvioFilas=ui->m_NumeroFilas->text().toInt();
    tmp_ImpresionesPorEnvioColumnas=ui->m_NumeroColumnas->text().toInt();
    tmp_ImpresionesPorEnvioFilas=ui->m_NumeroFilas->text().toInt();
    tmp_ImpresionesPorEnvioColumnas=ui->m_NumeroColumnas->text().toInt();
    m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvio=tmp_ImpresionesPorEnvioFilas*tmp_ImpresionesPorEnvioColumnas;
    m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas=tmp_ImpresionesPorEnvioFilas;
    m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas=tmp_ImpresionesPorEnvioColumnas;
    m_memoryMap->m_mapa.m_variables.SeparacionY=ui->m_SeparacionFilas->text().toDouble();
    m_memoryMap->m_mapa.m_variables.SeparacionX=ui->m_SeparacionColumnas->text().toDouble();
    m_memoryMap->m_mapa.m_variables.AnguloPlantilla=ui->m_AnguloPlantilla->text().toFloat();
    unsigned int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    switch (indice_macula){
        case 0:
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaX=ui->m_Macula_X->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaY=ui->m_Macula_Y->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=ui->m_Macula_ancho->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=ui->m_Macula_alto->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=ui->m_Macula_repeticiones->text().toInt();
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=ui->m_Macula_frecuencia->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor=ui->m_Macula_Color->currentIndex()+1;
            if (ui->m_Macula_OnOff->isChecked()) m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=1;
            else m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=0;
            break;
        case 1:
            m_memoryMap->m_mapa.m_variables.MaculaDchaX=ui->m_Macula_X->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaDchaY=ui->m_Macula_Y->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaDchaAncho=ui->m_Macula_ancho->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaDchaAlto=ui->m_Macula_alto->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=ui->m_Macula_repeticiones->text().toInt();
            m_memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=ui->m_Macula_frecuencia->text().toDouble();
            m_memoryMap->m_mapa.m_variables.MaculaDchaColor=ui->m_Macula_Color->currentIndex()+1;
            if (ui->m_Macula_OnOff->isChecked()) m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff=1;
            else m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff=0;
            break;
    }

    struct strcMarca foo=m_memoryMap->m_mapa.m_Marca;
    foo.MarcaInfFrecuencia=ui->m_MarcaD_frecuencia->text().toInt();
    foo.MarcaSupFrecuencia=ui->m_MarcaI_frecuencia->text().toInt();
    foo.MarcaInfOnOff=ui->m_MarcaD_OnOff->isChecked();
    foo.MarcaSupOnOff=ui->m_MarcaI_OnOff->isChecked();
    foo.MarcaInfRepeticiones=ui->m_MarcaD_repeticiones->text().toInt();
    foo.MarcaSupRepeticiones=ui->m_MarcaI_repeticiones->text().toInt();
    foo.MarcaInfTipo=ui->m_MarcaD_Tipo->currentIndex();
    foo.MarcaSupTipo=ui->m_MarcaI_Tipo->currentIndex();
    foo.MarcaInfValor=ui->m_MarcaD_Valor->text();
    foo.MarcaSupValor=ui->m_MarcaI_Valor->text();
    foo.MarcaInfX=ui->m_MarcaD_X->text().toDouble();
    foo.MarcaSupX=ui->m_MarcaI_X->text().toDouble();
    foo.MarcaInfY=ui->m_MarcaD_Y->text().toDouble();
    foo.MarcaSupY=ui->m_MarcaI_Y->text().toDouble();
    foo.anguloInf=ui->m_MarcaAnguloInferior->text().toInt();
    foo.anguloSup=ui->m_MarcaAnguloSuperior->text().toInt();

    foo.MarcaSupFuente=ui->m_MarcaIFuente->currentText();
    foo.MarcaIInfFuente=ui->m_MarcaDFuente->currentText();
    foo.MarcaSupEstiloLetraCampo=ui->m_MarcaIEstilo->currentText();
    foo.MarcaInfEstiloLetraCampo=ui->m_MarcaDEstilo->currentText();;
    foo.MarcaSupLetterSpace=ui->m_leMarcaILetterSpace->text().toInt();
    foo.MarcaInfLetterSpace=ui->m_leMarcaDLetterSpace->text().toInt();
    foo.MarcaSupTamanoFuente=ui->m_MarcaITamano->text().toUInt();
    foo.MarcaInfTamanoFuente=ui->m_MarcaDTamano->text().toUInt();



    m_memoryMap->m_mapa.m_Marca=foo;


}
/** Borra comentario*/
void EditComposicion::sltDeleteComentario(){
    int rtn=QMessageBox::question(0,tr("ATENCION"),tr("Desea borrar comentario?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        m_memoryMap->m_mapa.m_variables.Comentario.clear();
        ui->m_teComentario->clear();
        QMessageBox::warning(0,tr("OPERACION COMPLETADA"),tr("Comentario borrado"),QMessageBox::Ok);
        ui->m_teComentario->setEnabled(false);
    }
}
/** Edita comentario*/
void EditComposicion::sltEditComentario(){
    ui->m_teComentario->setEnabled(true);
}
/** Guarda comentario*/
void EditComposicion::sltSaveComentario(){
    int rtn=QMessageBox::question(0,tr("ATENCION"),tr("Desea guardar comentario?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        m_memoryMap->m_mapa.m_variables.Comentario=ui->m_teComentario->toPlainText();
        QMessageBox::warning(0,tr("OPERACION COMPLETADA"),tr("Comentario guardado"),QMessageBox::Ok);
        ui->m_teComentario->setEnabled(false);
    }
}

/** Barcode izquierdo*/
void EditComposicion::sltBarcodeConfiguredMarcaI(const m_BarcodeParameters & data){
    m_memoryMap->m_mapa.m_Marca.barcodeSup=data;

}
/** Barcode derecho*/
void EditComposicion::sltBarcodeConfiguredMarcaD(const m_BarcodeParameters & data){
    m_memoryMap->m_mapa.m_Marca.barcodeInf=data;

}
