/** @addtogroup DbFormatPage
 *  @{*/
/** @file DbFormatPage.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbFormatPage.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


//#include "dbformat.h"
#include "dbformatpage.h"
#include "dbimpresion.h"
#include "globals.h"
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
#include "principal.h"
#include <QList>
#include "In2GraphicsView.h"
#include "in2imagecreator.h"

//#include "dbgmem.h"
#include "globals.h"
#include "In2BarcodeClass.h"
#include "formatBarcode.h"

#define OPCION_TAMANO		0
#define OPCION_IMAGEN_FIJA	1
#define OPCION_MATRIZ		2
#define OPCION_MACULAS		3
#define OPCION_MARCAS		4


DbFormatPage::DbFormatPage(QWidget *parent)
    : ui(new Ui::EditValoresUI)
{

    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    ui->setupUi(this);

    this->SetDataCommon();
	
    ui->m_WebWidth->setPalette(dummyPaletteEnabled);
    ui->m_WebHeight->setPalette(dummyPaletteEnabled);
    ui->m_teComentario->setText(memoryMap->m_mapa.m_variables.Comentario);
    ui->m_PuntoInicial_X->setPalette(dummyPaletteEnabled);
    ui->m_PuntoInicial_Y->setPalette(dummyPaletteEnabled);


    ui->m_NumeroFilas->setPalette(dummyPaletteEnabled);
    ui->m_NumeroColumnas->setPalette(dummyPaletteEnabled);
    ui->m_SeparacionFilas->setPalette(dummyPaletteEnabled);
    ui->m_SeparacionColumnas->setPalette(dummyPaletteEnabled);
    ui->m_AnguloPlantilla->setPalette(dummyPaletteEnabled);

    ui->m_SeleccionMacula->addItem("Izqda");
    ui->m_SeleccionMacula->addItem("Dcha");
    ui->m_SeleccionMacula->setCurrentIndex(0);

    ui->m_ColorSeleccionado->setReadOnly(true);

    if (memoryMap->m_mapa.m_variables.NumColores>1) ui->m_Macula_Color->setVisible(true);
    else ui->m_Macula_Color->setVisible(false);

    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
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
    if (memoryMap->m_mapa.m_variables.NumColores==1){
        ui->m_Macula_Color->setVisible(false);
        ui->m_ColorSeleccionado->setVisible(false);
    }
//#endif


    ui->m_SeleccionMarca->addItem("Superior");
    ui->m_SeleccionMarca->addItem("Inferior");
    ui->m_SeleccionMarca->setCurrentIndex(0);

    ui->m_Marca_Tipo->addItem("Texto");
    ui->m_Marca_Tipo->addItem("Barcode");
    ui->m_Marca_Tipo->addItem("Datamatrix");
    ui->m_Marca_Tipo->setCurrentIndex(0);

    ui->m_ColorMarcaSeleccionado->setReadOnly(true);

    if (memoryMap->m_mapa.m_variables.NumColores>1) ui->m_Marca_Color->setVisible(true);
    else ui->m_Marca_Color->setVisible(false);

    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        ui->m_Marca_Color->addItem(QString("%1").arg(i+1));
    }
    ui->m_Marca_Color->setCurrentIndex(0);

    ui->m_Marca_X->setPalette(dummyPaletteEnabled);
    ui->m_Marca_Y->setPalette(dummyPaletteEnabled);
    ui->m_Marca_repeticiones->setPalette(dummyPaletteEnabled);
    ui->m_Marca_frecuencia->setPalette(dummyPaletteEnabled);
    ui->m_Marca_Color->setPalette(dummyPaletteEnabled);

//#ifdef COLOR_MONOCROMO
    if (memoryMap->m_mapa.m_variables.NumColores==1){
        ui->m_Marca_Color->setVisible(false);
        ui->m_ColorMarcaSeleccionado->setVisible(false);
    }
//#endif

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabEnabled(3,false);

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_CancelButton,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
    //connect(ui->m_OpcionConfiguracion,SIGNAL(activated (QString)),this,SLOT(CambiarControles()));
    connect(ui->m_SeleccionMacula,SIGNAL(activated (QString)),this,SLOT(CambiarMacula()));
    connect(ui->m_Macula_Color,SIGNAL(activated (QString)),this,SLOT(UpdateColorMacula()));
    connect(ui->m_SeleccionMarca,SIGNAL(activated (QString)),this,SLOT(CambiarMarca()));
    connect(ui->m_Marca_Color,SIGNAL(activated (QString)),this,SLOT(UpdateColorMarca()));

    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));

    connect(ui->m_pbDelComment,SIGNAL(clicked()),this,SLOT(sltDeleteComentario()));
    connect(ui->m_pbEditComment,SIGNAL(clicked()),this,SLOT(sltEditComentario()));
    connect(ui->m_pbSaveComment,SIGNAL(clicked()),this,SLOT(sltSaveComentario()));


    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_WebWidth,SIGNAL(selectionChanged()),this,SLOT(UpdateWebWidth()));
    connect(ui->m_WebHeight,SIGNAL(selectionChanged()),this,SLOT(UpdateWebHeight()));


    connect(ui->m_NumeroFilas,SIGNAL(selectionChanged()),this,SLOT(UpdateNumeroFilas()));
    connect(ui->m_NumeroColumnas,SIGNAL(selectionChanged()),this,SLOT(UpdateNumeroColumnas()));
    connect(ui->m_SeparacionFilas,SIGNAL(selectionChanged()),this,SLOT(UpdateSeparacionFilas()));
    connect(ui->m_SeparacionColumnas,SIGNAL(selectionChanged()),this,SLOT(UpdateSeparacionColumnas()));
    connect(ui->m_AnguloPlantilla,SIGNAL(selectionChanged()),this,SLOT(UpdateAnguloPlantilla()));

    connect(ui->m_PuntoInicial_X,SIGNAL(selectionChanged()),this,SLOT(UpdatePuntoInicialX()));
    connect(ui->m_PuntoInicial_Y,SIGNAL(selectionChanged()),this,SLOT(UpdatePuntoInicialY()));

    connect(ui->m_Macula_X,SIGNAL(selectionChanged()),this,SLOT(UpdateMaculaX()));
    connect(ui->m_Macula_Y,SIGNAL(selectionChanged()),this,SLOT(UpdateMaculaY()));
    connect(ui->m_Macula_ancho,SIGNAL(selectionChanged()),this,SLOT(UpdateMaculaAncho()));
    connect(ui->m_Macula_alto,SIGNAL(selectionChanged()),this,SLOT(UpdateMaculaAlto()));
    connect(ui->m_Macula_repeticiones,SIGNAL(selectionChanged()),this,SLOT(UpdateMaculaRepeticiones()));
    connect(ui->m_Macula_frecuencia,SIGNAL(selectionChanged()),this,SLOT(UpdateMaculaFrecuencia()));

    connect(ui->m_Marca_X,SIGNAL(selectionChanged()),this,SLOT(UpdateMarcaX()));
    connect(ui->m_Marca_Y,SIGNAL(selectionChanged()),this,SLOT(UpdateMarcaY()));
    connect(ui->m_Marca_repeticiones,SIGNAL(selectionChanged()),this,SLOT(UpdateMarcaRepeticiones()));
    connect(ui->m_Marca_frecuencia,SIGNAL(selectionChanged()),this,SLOT(UpdateMarcaFrecuencia()));

#endif

    connect(ui->m_Macula_OnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMacula()));
    connect(ui->m_Marca_OnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMarca()));
    connect(ui->m_Marca_configuracion,SIGNAL(clicked()),this,SLOT(ConfigurarMarca()));

    connect(ui->m_AceptarValores,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));

    connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));

    //#ifdef CON_OPC
    //connect((Principal*)this->GetMainWindow(), SIGNAL(ActualizaDataOPC()),this, SLOT(ActualizaDataSlotOPC()));
    //#endif

	TestRoot();

    PasarDeMapATemp();
	CargaDatos();

    //GeneracionRutaPlantilla();
    //rotatedpixmap.load(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
    if (!memoryMap->m_mapa.m_variables.bPlantEnabled)
        rotatedpixmap.load(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
    else{
        rotatedpixmap=getPlantImage();
    }
    //CreaImagenPagina();
    //CambiarControles();

    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
    GlobalSetScreenSize(this);
    /*ui->m_ImagenFormatPage->setMaximumHeight(ui->m_ImagenFormatPage->height());
    ui->m_ImagenFormatPage->setMaximumWidth(ui->m_ImagenFormatPage->width());*/

    //MuestraImagen(ImagenUnitariaPagina);

    In2ImageCreator imgCreator;
    imgCreator.sltGenerateImage();
    QPixmap prevPixmap=memoryMap->m_Imagen;
    MuestraImagen(prevPixmap);

}

void DbFormatPage::PasarDeTempAMap(){

    memoryMap->m_mapa.m_variables.WebWidth=WebWidth_tmp;
    memoryMap->m_mapa.m_variables.WebHeight=WebHeight_tmp;
    memoryMap->m_mapa.m_variables.PuntoInicialX=PuntoInicialX_tmp;
    memoryMap->m_mapa.m_variables.PuntoInicialY=PuntoInicialY_tmp;
    memoryMap->m_mapa.m_variables.FilasPlantilla=FilasPlantilla_tmp;
    memoryMap->m_mapa.m_variables.ColumnasPlantilla=ColumnasPlantilla_tmp;
    memoryMap->m_mapa.m_variables.SeparacionX=SeparacionX_tmp;
    memoryMap->m_mapa.m_variables.SeparacionY=SeparacionY_tmp;
    memoryMap->m_mapa.m_variables.AnguloPlantilla=AnguloPlantilla_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaX=MaculaIzqdaX_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaY=MaculaIzqdaY_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=MaculaIzqdaAncho_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=MaculaIzqdaAlto_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaX=MaculaDchaX_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaY=MaculaDchaY_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaAncho=MaculaDchaAncho_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaAlto=MaculaDchaAlto_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=MaculaIzqdaOnOff_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaOnOff=MaculaDchaOnOff_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=MaculaDchaRep_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=MaculaDchaFreq_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=MaculaIzqdaRep_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=MaculaIzqdaFreq_tmp;
    memoryMap->m_mapa.m_variables.MaculaIzqdaColor=MaculaIzqdaColor_tmp;
    memoryMap->m_mapa.m_variables.MaculaDchaColor=MaculaDchaColor_tmp;
    memoryMap->m_mapa.m_variables.MarcaSupX=MarcaSupX_tmp;
    memoryMap->m_mapa.m_variables.MarcaSupY=MarcaSupY_tmp;
    memoryMap->m_mapa.m_variables.MarcaInfX=MarcaInfX_tmp;
    memoryMap->m_mapa.m_variables.MarcaInfY=MarcaInfY_tmp;
    memoryMap->m_mapa.m_variables.MarcaSupOnOff=MarcaSupOnOff_tmp;
    memoryMap->m_mapa.m_variables.MarcaInfOnOff=MarcaInfOnOff_tmp;
    memoryMap->m_mapa.m_variables.MarcaSupRepeticiones=MarcaSupRep_tmp;
    memoryMap->m_mapa.m_variables.MarcaSupFrecuencia=MarcaSupFreq_tmp;
    memoryMap->m_mapa.m_variables.MarcaInfRepeticiones=MarcaInfRep_tmp;
    memoryMap->m_mapa.m_variables.MarcaInfFrecuencia=MarcaInfFreq_tmp;
    memoryMap->m_mapa.m_variables.MarcaSupColor=MarcaSupColor_tmp;
    memoryMap->m_mapa.m_variables.MarcaInfColor=MarcaInfColor_tmp;


}

void DbFormatPage::PasarDeMapATemp(){

    WebWidth_tmp=memoryMap->m_mapa.m_variables.WebWidth;
    WebHeight_tmp=memoryMap->m_mapa.m_variables.WebHeight;
    PuntoInicialX_tmp=memoryMap->m_mapa.m_variables.PuntoInicialX;
    PuntoInicialY_tmp=memoryMap->m_mapa.m_variables.PuntoInicialY;
    FilasPlantilla_tmp=memoryMap->m_mapa.m_variables.FilasPlantilla;
    ColumnasPlantilla_tmp=memoryMap->m_mapa.m_variables.ColumnasPlantilla;
    SeparacionX_tmp=memoryMap->m_mapa.m_variables.SeparacionX;
    SeparacionY_tmp=memoryMap->m_mapa.m_variables.SeparacionY;
    AnguloPlantilla_tmp=memoryMap->m_mapa.m_variables.AnguloPlantilla;
    MaculaIzqdaX_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaX;
    MaculaIzqdaY_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaY;
    MaculaIzqdaAncho_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaAncho;
    MaculaIzqdaAlto_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaAlto;
    MaculaDchaX_tmp=memoryMap->m_mapa.m_variables.MaculaDchaX;
    MaculaDchaY_tmp=memoryMap->m_mapa.m_variables.MaculaDchaY;
    MaculaDchaAncho_tmp=memoryMap->m_mapa.m_variables.MaculaDchaAncho;
    MaculaDchaAlto_tmp=memoryMap->m_mapa.m_variables.MaculaDchaAlto;
    MaculaIzqdaOnOff_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff;
    MaculaDchaOnOff_tmp=memoryMap->m_mapa.m_variables.MaculaDchaOnOff;
    MaculaDchaRep_tmp=memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones;
    MaculaDchaFreq_tmp=memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia;
    MaculaIzqdaRep_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones;
    MaculaIzqdaFreq_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia;
    MaculaIzqdaColor_tmp=memoryMap->m_mapa.m_variables.MaculaIzqdaColor;
    MaculaDchaColor_tmp=memoryMap->m_mapa.m_variables.MaculaDchaColor;

    MarcaSupX_tmp=memoryMap->m_mapa.m_variables.MarcaSupX;
    MarcaSupY_tmp=memoryMap->m_mapa.m_variables.MarcaSupY;
    MarcaInfX_tmp=memoryMap->m_mapa.m_variables.MarcaInfX;
    MarcaInfY_tmp=memoryMap->m_mapa.m_variables.MarcaInfY;
    MarcaSupOnOff_tmp=memoryMap->m_mapa.m_variables.MarcaSupOnOff;
    MarcaInfOnOff_tmp=memoryMap->m_mapa.m_variables.MarcaInfOnOff;
    MarcaInfRep_tmp=memoryMap->m_mapa.m_variables.MarcaInfRepeticiones;
    MarcaInfFreq_tmp=memoryMap->m_mapa.m_variables.MarcaInfFrecuencia;
    MarcaSupRep_tmp=memoryMap->m_mapa.m_variables.MarcaSupRepeticiones;
    MarcaSupFreq_tmp=memoryMap->m_mapa.m_variables.MarcaSupFrecuencia;
    MarcaInfColor_tmp=memoryMap->m_mapa.m_variables.MarcaInfColor;
    MarcaSupColor_tmp=memoryMap->m_mapa.m_variables.MarcaSupColor;

}


void DbFormatPage::TestRoot(){
     if (memoryMap->getUserRights()<ACCESS_ROOT){
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
        ui->m_Marca_X->setEnabled(false);
        ui->m_Marca_Y->setEnabled(false);
        ui->m_Marca_OnOff->setEnabled(false);
        ui->m_Marca_repeticiones->setEnabled(false);
        ui->m_Marca_frecuencia->setEnabled(false);
        ui->m_Marca_Color->setEnabled(false);
	}
}

DbFormatPage::~DbFormatPage(){

}

//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void DbFormatPage::AceptarSeleccion(){

	GuardaDatos();
	LaunchDbImpresion();
	this->deleteLater();

}


void DbFormatPage::CancelarSeleccion(){

    PasarDeTempAMap();
	LaunchDbImpresion();
	this->deleteLater();
	
}


// Actualiza los datos de pantalla
void DbFormatPage::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}


void DbFormatPage::UpdateWebWidth(){
    InputData *input = InputData::request(this,"Web Width [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateWidth(QString)));
    }
}

void DbFormatPage::UpdateWidth(QString value){
    ui->m_WebWidth->setText(value);
    ui->m_WebWidth->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateWebHeight(){
    InputData *input = InputData::request(this,"Alto [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateHeight(QString)));
    }
}

void DbFormatPage::UpdateHeight(QString value){
    ui->m_WebHeight->setText(value);
    ui->m_WebHeight->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateNumeroFilas(){
    InputData *input = InputData::request(this,"Número Filas");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFilas(QString)));
    }
}

void DbFormatPage::UpdateFilas(QString value){
    ui->m_NumeroFilas->setText(value);
    ui->m_NumeroFilas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateNumeroColumnas(){
    InputData *input = InputData::request(this,"Número Columnas");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateColumnas(QString)));
    }
}

void DbFormatPage::UpdateColumnas(QString value){
    ui->m_NumeroColumnas->setText(value);
    ui->m_NumeroColumnas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateSeparacionFilas(){
    InputData *input = InputData::request(this,"Separación Filas [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateSepFilas(QString)));
    }
}

void DbFormatPage::UpdateSepFilas(QString value){
    ui->m_SeparacionFilas->setText(value);
    ui->m_SeparacionFilas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateSeparacionColumnas(){
    InputData *input = InputData::request(this,"Separación Columnas [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateSepColumnas(QString)));
    }
}

void DbFormatPage::UpdateSepColumnas(QString value){
    ui->m_SeparacionColumnas->setText(value);
    ui->m_SeparacionColumnas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateAnguloPlantilla(){
    InputData *input = InputData::request(this,"Angulo Plantilla [deg]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAngulo(QString)));
    }
}

void DbFormatPage::UpdateAngulo(QString value){
    ui->m_AnguloPlantilla->setText(value);
    ui->m_AnguloPlantilla->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdatePuntoInicialX(){
    InputData *input = InputData::request(this,"Coordenada X Punto Inicial [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateInicialX(QString)));
    }
}

void DbFormatPage::UpdateInicialX(QString value){
    ui->m_PuntoInicial_X->setText(value);
    ui->m_PuntoInicial_X->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdatePuntoInicialY(){
    InputData *input = InputData::request(this,"Coordenada Y Punto Inicial [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateInicialY(QString)));
    }
}

void DbFormatPage::UpdateInicialY(QString value){
    ui->m_PuntoInicial_Y->setText(value);
    ui->m_PuntoInicial_Y->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateOnOffMacula(){
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


void DbFormatPage::UpdateMaculaX(){
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


void DbFormatPage::UpdateX(QString value){
    ui->m_Macula_X->setText(value);
    ui->m_Macula_X->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMaculaY(){
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


void DbFormatPage::UpdateY(QString value){
    ui->m_Macula_Y->setText(value);
    ui->m_Macula_Y->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateMaculaAncho(){
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


void DbFormatPage::UpdateAncho(QString value){
    ui->m_Macula_ancho->setText(value);
    ui->m_Macula_ancho->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMaculaAlto(){
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


void DbFormatPage::UpdateAlto(QString value){
    ui->m_Macula_alto->setText(value);
    ui->m_Macula_alto->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMaculaFrecuencia(){
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


void DbFormatPage::UpdateFrecuencia(QString value){
    ui->m_Macula_frecuencia->setText(value);
    ui->m_Macula_frecuencia->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMaculaRepeticiones(){
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

void DbFormatPage::UpdateColorMacula(){
    unsigned char indice;
    indice=ui->m_Macula_Color->currentIndex();
    QPalette paleta_color_pleno;
    paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[indice].valor));
    ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);
}

void DbFormatPage::UpdateRepeticiones(QString value){
    ui->m_Macula_repeticiones->setText(value);
    ui->m_Macula_repeticiones->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::UpdateOnOffMarca(){
    if (ui->m_Marca_OnOff->isChecked()){
        ui->m_Marca_X->setEnabled(true);
        ui->m_Marca_Y->setEnabled(true);
        ui->m_Marca_repeticiones->setEnabled(true);
        ui->m_Marca_frecuencia->setEnabled(true);
        ui->m_Marca_Color->setEnabled(true);
    }
    else{
        ui->m_Marca_X->setEnabled(false);
        ui->m_Marca_Y->setEnabled(false);
        ui->m_Marca_repeticiones->setEnabled(false);
        ui->m_Marca_frecuencia->setEnabled(false);
        ui->m_Marca_Color->setEnabled(false);
    }
}


void DbFormatPage::UpdateMarcaX(){
    int indice_marca;
    indice_marca=ui->m_SeleccionMarca->currentIndex();
    QString titulo;
    switch (indice_marca){
    case 0:
        titulo=QString("Posición X Marca Superior [mm]");
        break;
    case 1:
        titulo=QString("Posición X Marca Inferior [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateXm(QString)));
}


void DbFormatPage::UpdateXm(QString value){
    ui->m_Marca_X->setText(value);
    ui->m_Marca_X->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMarcaY(){
    int indice_marca;
    indice_marca=ui->m_SeleccionMarca->currentIndex();
    QString titulo;
    switch (indice_marca){
    case 0:
        titulo=QString("Posición Y Marca Superior [mm]");
        break;
    case 1:
        titulo=QString("Posición Y Marca Inferior [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateYm(QString)));
}


void DbFormatPage::UpdateYm(QString value){
    ui->m_Marca_Y->setText(value);
    ui->m_Marca_Y->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMarcaFrecuencia(){
    int indice_marca;
    indice_marca=ui->m_SeleccionMarca->currentIndex();
    QString titulo;
    switch (indice_marca){
    case 0:
        titulo=QString("Frecuencia Marca Superior [mm]");
        break;
    case 1:
        titulo=QString("Frecuencia Marca Inferior [mm]");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFrecuencia_m(QString)));
}


void DbFormatPage::UpdateFrecuencia_m(QString value){
    ui->m_Marca_frecuencia->setText(value);
    ui->m_Marca_frecuencia->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPage::UpdateMarcaRepeticiones(){
    int indice_marca;
    indice_marca=ui->m_SeleccionMarca->currentIndex();
    QString titulo;
    switch (indice_marca){
    case 0:
        titulo=QString("Repeticiones Marca Superior");
        break;
    case 1:
        titulo=QString("Repeticiones Marca Inferior");
        break;
    }
    InputData *input = InputData::request(this,titulo);
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateRepeticiones_m(QString)));
}

void DbFormatPage::UpdateColorMarca(){
    unsigned char indice;
    indice=ui->m_Marca_Color->currentIndex();
    QPalette paleta_color_pleno;
    paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[indice].valor));
    ui->m_ColorMarcaSeleccionado->setPalette(paleta_color_pleno);
}

void DbFormatPage::UpdateRepeticiones_m(QString value){
    ui->m_Marca_repeticiones->setText(value);
    ui->m_Marca_repeticiones->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPage::AceptarValoresCampos(){
	
    ui->m_AceptarButton->setEnabled(false);
    ui->m_CancelButton->setEnabled(false);

	GuardaDatos();
    //CreaImagenPagina();

    In2ImageCreator imgCreator;
    imgCreator.sltGenerateImage();
    QPixmap prevPixmap=memoryMap->m_Imagen;
    MuestraImagen(prevPixmap);


    ui->m_AceptarButton->setEnabled(true);
    ui->m_CancelButton->setEnabled(true);
}

void DbFormatPage::LaunchDbImpresion(){
	DbImpresion *ScrDbImpresion;
	ScrDbImpresion=new DbImpresion(0);
	ScrDbImpresion->show();
	this->deleteLater();
}

void DbFormatPage::MuestraImagen(QPixmap Imagen){

    QPixmap lienzo(Imagen.width(),Imagen.height());
    //lienzo.fill(Qt::gray);
    QPainter painter(&lienzo);
    //painter.setPen( QPen( Qt::red, 10 ) );
    painter.drawRect(lienzo.rect());
    painter.drawPixmap(0,0,Imagen);
    painter.end();
    QGraphicsScene* scene = new QGraphicsScene(QRect(0, 0,memoryMap->m_Imagen.width(), memoryMap->m_Imagen.height()));

    scene->addPixmap(lienzo);
    ui->m_graphicsView->setScene(scene);
    ui->m_graphicsView->show();
    In2GraphicsView *p=new In2GraphicsView(ui->m_graphicsView);
        /*unsigned char escalado;

        int offset_x;
        int offset_y;

        int lw=ui->m_ImagenFormatPage->width();
        int lh=ui->m_ImagenFormatPage->height();
        int iw=Imagen.width();
        int ih=Imagen.height();
        //Por si tenemos imagenes mal dimensionadas
        if ((lw==0)||(lh==0)) return;

        escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

        QPixmap pic(ui->m_ImagenFormatPage->size());
        QPainter p(&pic);   // create a QPainter for it
        pic.fill(Qt::gray);
        QPixmap dummy;

        switch (escalado){
        case IMAGEN_SINESCALAR:
            //No hay que escalar
            dummy=Imagen;
            offset_x=(ui->m_ImagenFormatPage->width()-dummy.width())/2;
            offset_y=(ui->m_ImagenFormatPage->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            ui->m_ImagenFormatPage->setOriginalPixmap(Imagen);
            ui->m_ImagenFormatPage->setPixmap((QPixmap)(pic));

            break;
        case IMAGEN_S2W:
            dummy=Imagen.scaledToWidth(ui->m_ImagenFormatPage->width(),Qt::SmoothTransformation);
            offset_x=(ui->m_ImagenFormatPage->width()-dummy.width())/2;
            offset_y=(ui->m_ImagenFormatPage->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            ui->m_ImagenFormatPage->setOriginalPixmap(Imagen);
            ui->m_ImagenFormatPage->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2H:
            dummy=Imagen.scaledToHeight(ui->m_ImagenFormatPage->height(),Qt::SmoothTransformation);
            offset_x=(ui->m_ImagenFormatPage->width()-dummy.width())/2;
            offset_y=(ui->m_ImagenFormatPage->height()-dummy.height())/2;
            p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
            ui->m_ImagenFormatPage->setOriginalPixmap(Imagen);
            ui->m_ImagenFormatPage->setPixmap((QPixmap)(pic));
            break;
        }
        p.end();
    ui->m_ImagenFormatPage->show();*/

}

void DbFormatPage::CreaImagenPagina(){

    unsigned int i,j;
	unsigned int x;
	unsigned int y;
    unsigned int indice;
	QString tmp,tmp_indice;

    unsigned int ipef,ipec;

	double ww_mm,wh_mm;
	double x0_mm,y0_mm;
	double dix_mm,diy_mm;

	unsigned int ww,wh;
    //unsigned int sx,sy;
	unsigned int x0,y0;
	unsigned int dix,diy;

    unsigned int sx=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.SeparacionX);
    unsigned int sy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.SeparacionY);
    unsigned int Xo=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.PuntoInicialX);
    unsigned int Yo=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.PuntoInicialY);

	unsigned int NumImpresiones;
    unsigned char indice_color;
	
	ww_mm=memoryMap->m_mapa.m_variables.WebWidth;
	wh_mm=memoryMap->m_mapa.m_variables.WebHeight;

	dix_mm=memoryMap->m_mapa.m_variables.SeparacionX;
	diy_mm=memoryMap->m_mapa.m_variables.SeparacionY;

    if ((memoryMap->m_mapa.m_variables.AnguloPlantilla!=0) &&
            (memoryMap->m_mapa.m_variables.AnguloPlantilla!=90) &&
            (memoryMap->m_mapa.m_variables.AnguloPlantilla!=180) &&
            (memoryMap->m_mapa.m_variables.AnguloPlantilla!=270)){
        memoryMap->m_mapa.m_variables.AnguloPlantilla=0;
    }
        if ((memoryMap->m_mapa.m_variables.AnguloPlantilla==270) || (memoryMap->m_mapa.m_variables.AnguloPlantilla==90)){
        }

	ipef=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
	ipec=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
	x0_mm=memoryMap->m_mapa.m_variables.PuntoInicialX;
	y0_mm=memoryMap->m_mapa.m_variables.PuntoInicialY;

	NumImpresiones=ipef*ipec;

    ww=(unsigned int) (FACTOR_PIXELS_MM*ww_mm);
    wh=(unsigned int) (FACTOR_PIXELS_MM*wh_mm);

    dix=(unsigned int) (FACTOR_PIXELS_MM*dix_mm);
    diy=(unsigned int) (FACTOR_PIXELS_MM*diy_mm);

    //sx=dix-fp;
    //sy=diy-cp;

    x0=(unsigned int) (FACTOR_PIXELS_MM*x0_mm);
    y0=(unsigned int) (FACTOR_PIXELS_MM*y0_mm);

	QPixmap dummy;
	//QPixmap pic(cp*ipec+sx*(ipec-1)+x0,fp*ipef+sy*(ipef-1)+y0);
	//El ancho de imagen es el webwidth
	//QPixmap pic(ww,fp*ipef+sy*(ipef-1)+y0);
	//El ancho de imagen es el webwidth y el alto WebHeight
	QPixmap pic(ww,wh);

	QPainter p(&pic);   // create a QPainter for it
	pic.fill(Qt::white);
	p.drawPixmap(0,0,dummy); // and draw your original pixmap on it

	x=x0;
	y=y0;
	indice=0;

    QPixmap aux=rotatedpixmap.transformed(QTransform().rotate(memoryMap->m_mapa.m_variables.AnguloPlantilla,Qt::ZAxis));

    for(i=0;i<ipef;i++){
		for(j=0;j<ipec;j++){
            //if (indice<NumImpresiones) p.drawPixmap(x,y, rotatedpixmap);
            if (indice<NumImpresiones) p.drawPixmap(x,y, aux);
            x=x+dix;//cp+sx;
            indice++;
		}
		x=x0;
        y=y+diy;//fp+sy;
	}


	//Dibujar maculas
	double mi_x_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaX;
	double mi_y_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaY;
	double mi_ancho_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaAncho;
	double mi_alto_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaAlto;
	double md_x_mm=memoryMap->m_mapa.m_variables.MaculaDchaX;
	double md_y_mm=memoryMap->m_mapa.m_variables.MaculaDchaY;
	double md_ancho_mm=memoryMap->m_mapa.m_variables.MaculaDchaAncho;
	double md_alto_mm=memoryMap->m_mapa.m_variables.MaculaDchaAlto;

    double mi_frecuencia_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia;
    double md_frecuencia_mm=memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia;

	md_x_mm = ww_mm - (md_x_mm+md_ancho_mm);

    unsigned int mi_xo;
    unsigned int mi_yo;
    unsigned int mi_x;
    unsigned int mi_y;
	unsigned int mi_ancho;
	unsigned int mi_alto;
    unsigned int md_xo;
    unsigned int md_yo;
    unsigned int md_x;
    unsigned int md_y;
	unsigned int md_ancho;
	unsigned int md_alto;
    unsigned int mi_frecuencia;
    unsigned int md_frecuencia;
    unsigned int num_mi;
    unsigned int num_md;

    num_mi=memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones;
    num_md=memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones;

    mi_xo=(unsigned int) (FACTOR_PIXELS_MM*mi_x_mm);
    mi_yo=(unsigned int) (FACTOR_PIXELS_MM*mi_y_mm);
    mi_ancho=(unsigned int) (FACTOR_PIXELS_MM*mi_ancho_mm);
    mi_alto=(unsigned int) (FACTOR_PIXELS_MM*mi_alto_mm);
    mi_frecuencia=(unsigned int) (FACTOR_PIXELS_MM*mi_frecuencia_mm);

    md_xo=(unsigned int) (FACTOR_PIXELS_MM*md_x_mm);
    md_yo=(unsigned int) (FACTOR_PIXELS_MM*md_y_mm);
    md_ancho=(unsigned int) (FACTOR_PIXELS_MM*md_ancho_mm);
    md_alto=(unsigned int) (FACTOR_PIXELS_MM*md_alto_mm);
    md_frecuencia=(unsigned int) (FACTOR_PIXELS_MM*md_frecuencia_mm);

    if (memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff==1){
        mi_x=mi_xo;
        mi_y=mi_yo;
        for (unsigned int n=0;n<num_mi;n++){
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                p.fillRect(mi_x,mi_y,mi_ancho,mi_alto, Qt::black);
            }
            else{
                unsigned char indice_color;
                indice_color= memoryMap->m_mapa.m_variables.MaculaIzqdaColor-1;
                p.fillRect(mi_x,mi_y,mi_ancho,mi_alto,memoryMap->m_mapa.m_colores[indice_color].valor);
            }

            /*#ifdef COLOR_MONOCROMO
            p.fillRect(mi_x,mi_y,mi_ancho,mi_alto, Qt::black);
            #endif
            #ifdef COLOR_PLENOS
            unsigned char indice_color;
            indice_color= memoryMap->m_mapa.m_variables.MaculaIzqdaColor-1;
            p.fillRect(mi_x,mi_y,mi_ancho,mi_alto,memoryMap->m_mapa.m_colores[indice_color].valor);
            #endif*/

            mi_y+=mi_frecuencia;
        }

    }

    if (memoryMap->m_mapa.m_variables.MaculaDchaOnOff==1){
        md_x=md_xo;
        md_y=md_yo;
        for (unsigned int n=0;n<num_md;n++){
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                //#ifdef COLOR_MONOCROMO
                p.fillRect(md_x,md_y,md_ancho,md_alto, Qt::black);
                //#endif
            }
            else{
                //#ifdef COLOR_PLENOS
                indice_color= memoryMap->m_mapa.m_variables.MaculaDchaColor-1;
                p.fillRect(md_x,md_y,md_ancho,md_alto,memoryMap->m_mapa.m_colores[indice_color].valor);
                //#endif
            }
            md_y+=md_frecuencia;
        }
    }

    /* JORGE INICIO */


    //Dibujar marcas
    double minf_x_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaX;
    double minf_y_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaY;
    double msup_x_mm=memoryMap->m_mapa.m_variables.MaculaDchaX;
    double msup_y_mm=memoryMap->m_mapa.m_variables.MaculaDchaY;

    double minf_frecuencia_mm=memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia;
    double msup_frecuencia_mm=memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia;

    msup_x_mm = ww_mm - (msup_x_mm/*+md_ancho_mm*/);

    unsigned int minf_xo;
    unsigned int minf_yo;
    unsigned int minf_x;
    unsigned int minf_y;
    unsigned int msup_xo;
    unsigned int msup_yo;
    unsigned int msup_x;
    unsigned int msup_y;
    unsigned int minf_frecuencia;
    unsigned int msup_frecuencia;
    unsigned int num_minf;
    unsigned int num_msup;

    num_minf=memoryMap->m_mapa.m_variables.MarcaInfRepeticiones;
    num_msup=memoryMap->m_mapa.m_variables.MarcaSupRepeticiones;

    minf_xo=(unsigned int) (FACTOR_PIXELS_MM*minf_x_mm);
    minf_yo=(unsigned int) (FACTOR_PIXELS_MM*minf_y_mm);
    minf_frecuencia=(unsigned int) (FACTOR_PIXELS_MM*minf_frecuencia_mm);

    msup_xo=(unsigned int) (FACTOR_PIXELS_MM*msup_x_mm);
    msup_yo=(unsigned int) (FACTOR_PIXELS_MM*msup_y_mm);
    msup_frecuencia=(unsigned int) (FACTOR_PIXELS_MM*msup_frecuencia_mm);

    if (memoryMap->m_mapa.m_variables.MarcaInfOnOff==1){
        minf_x=minf_xo;
        minf_y=minf_yo;
        for (unsigned int n=0;n<num_minf;n++){
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                //p.fillRect(minf_x,minf_y,mi_ancho,mi_alto, Qt::black);
            }
            else{
                unsigned char indice_color;
                indice_color= memoryMap->m_mapa.m_variables.MarcaInfColor-1;
                //p.fillRect(mi_x,mi_y,mi_ancho,mi_alto,memoryMap->m_mapa.m_colores[indice_color].valor);
            }

            minf_y+=minf_frecuencia;
        }

    }

    if (memoryMap->m_mapa.m_variables.MarcaSupOnOff==1){
        msup_x=msup_xo;
        msup_y=msup_yo;
        for (unsigned int n=0;n<num_msup;n++){
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                //p.fillRect(md_x,md_y,md_ancho,md_alto, Qt::black);
            }
            else{
                indice_color= memoryMap->m_mapa.m_variables.MarcaSupColor-1;
                //p.fillRect(md_x,md_y,md_ancho,md_alto,memoryMap->m_mapa.m_colores[indice_color].valor);
            }
            msup_y+=msup_frecuencia;
        }
    }



    //unsigned char indice_color;
    unsigned char tamano_fuente_campo;
    //int i,j;
    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    //unsigned int dix,diy;
    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QFont mifont;
    int indice_estilo;
    int peso_campo;
    QStringList lista_excepciones;
    QString Vtemp;


    int filasMatriz=0;
    int columnasMatriz=0;
    int filasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    int columnasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    int impresionesPorPagina;

    //long valor_actual=0;
    //int valor_inicial_fila=0;

    for(indice_color=0;indice_color<memoryMap->m_mapa.m_variables.NumColores;indice_color++){
        //QPixmap pic=PixmapUnitario[indice_color];
        //QPainter p(&pic);
        p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
        p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
        for (i=0;i<NUM_CAMPOS;i++){
            if ((memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01) &&
                    (memoryMap->m_mapa.m_campos[i].color==(indice_color+1))) {
                for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                    if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){                                               
                        lista_excepciones.clear();
                        excepciones=QString(memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                        lista_excepciones=excepciones.split(",");
                        Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo);
                        Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo);
                        dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                        diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                        Vtemp=QString(memoryMap->m_mapa.m_campos[i].ValorActualCampo);
                        tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);
                        indice_estilo=EstiloFuente.indexOf((QString(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo)));
                        peso_campo=EstiloFuenteQt.at(indice_estilo);
                        mifont.setPixelSize(tamano_fuente_campo);
                        mifont.setStyleStrategy(QFont::PreferAntialias);
                        mifont.setFamily(memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
                        mifont.setWeight(peso_campo);
                        p.setFont(mifont);

                        for (unsigned int filaComposicion=0;filaComposicion<filasComposicion;filaComposicion++){
                            for (unsigned int columnaComposicion=0;columnaComposicion<columnasComposicion;columnaComposicion++){
                                Ytemp=Yo+Yotemp+filaComposicion*sy;
                                celda=1;
                                for (unsigned int filaMatriz=0;filaMatriz<memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                    Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                    for (unsigned int columnaMatriz=0;columnaMatriz<memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                        celda_s=QString("%1").arg(celda);
                                        esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                        if (esunaexcepcion==false){
                                            p.save();
                                            if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                                //Nos movemos menos...
                                                p.translate(Xtemp,Ytemp);
                                                p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                                struct m_BarcodeParameters bc=memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                                In2BarcodeClass barcodeInst;
                                                barcodeInst.setParams(bc);
                                                barcodeInst.SetPixmap(Vtemp);
                                                p.drawPixmap(0,0,barcodeInst.qpxImagen);
                                            }
                                            else if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
                                                //Nos movemos menos...
                                                p.translate(Xtemp,Ytemp);
                                                p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                                struct m_BarcodeParameters bc=memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                                In2BarcodeClass barcodeInst;
                                                barcodeInst.setParams(bc);
                                                barcodeInst.SetPixmap(Vtemp);
                                                p.drawPixmap(0,0,barcodeInst.qpxImagen);
                                            }
                                            else{
                                                p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                                p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                                p.drawText(0,0,Vtemp);
                                            }
                                             p.restore();
                                        }
                                        Xtemp+=dix;
                                        celda++;
                                    } // columnaMatriz-->columnas
                                    Ytemp+=diy;
                                } // filaMatriz-->filas
                            } //columna
                        } //fila
                    }
                }
            }
        }




        long incremento[NUM_CAMPOS_VARIABLES];
        long offset_columna[NUM_CAMPOS_VARIABLES];
        long dummy_index;
        long valor_actual;
        int index;
        long valorInicialPagina;

        int indicePagina=1;


        for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
            if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
                tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
                offset_columna[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
                incremento[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.incremento;
                QFont mifont;
                tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
                indice_estilo=EstiloFuente.indexOf((QString(memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo)));
                peso_campo=EstiloFuenteQt.at(indice_estilo);
                mifont.setPixelSize(tamano_fuente_campo);
                mifont.setStyleStrategy(QFont::PreferAntialias);
                mifont.setFamily(memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo);
                mifont.setWeight(peso_campo);
                p.setFont(mifont);

                p.setFont(mifont);
                if (memoryMap->m_mapa.m_variables.NumColores==1){
                    p.setPen(Qt::black);
                }
                else{
                    indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                    p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
                }
                dummy_index=1;
                int numMatrices=memoryMap->getNumMatricesCampoVariable(i);


                for (int j=0;j<numMatrices/*NUM_MATRIZ_POR_CAMPO*/;j++){
                    if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                        lista_excepciones.clear();
                        excepciones=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                        lista_excepciones=excepciones.split(",");
                        Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                        Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                        dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                        diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);

                        unsigned int impresiones_realizadas=0;
                        for (unsigned int filaComposicion=0;filaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;filaComposicion++){
                            for (unsigned int columnaComposicion=0;columnaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;columnaComposicion++){
                                Ytemp=Yo+Yotemp+filaComposicion*sy;
                                celda=1;
                                for (unsigned int filaMatriz=0;filaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                    Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                    filasMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
                                    columnasMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;
                                    //if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS) valor_actual=valor_inicial_fila;
                                    for (unsigned int columnaMatriz=0;columnaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                        celda_s=QString("%1").arg(celda);
                                        esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                        if (esunaexcepcion==false){
                                            p.save();
                                            //Si es un campo de tipo CONTADOR
                                            if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                                if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                                                    //Impresion por columnas
                                                    // = $T4+(($V4-1)+($AD4-1)*$AG$11)*$AG$5+$AG$6*((X$3-1)+$AG$12*(X$2-1))
                                                    // = valorInicialPagina+(filaMatriz+(filaComposicion)*filasMatriz)*incremento[i]+offset[i]*(columnaMatriz+columnasMatriz*columnaComposicion)
                                                    impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                    valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                            +incremento[i]*(indicePagina-1)*filasMatriz*filasComposicion;
                                                    valor_actual= valorInicialPagina+(filaMatriz+(filaComposicion)*filasMatriz)*incremento[i]+offset_columna[i]*(columnaMatriz+columnasMatriz*columnaComposicion);
                                                    int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                    Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);
                                                    qDebug() << "valor_actual contador: " << valor_actual;
                                                }
                                                else if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                                                    //Impresión por filas
                                                    // =$T4+(($AG$12*$AG$9*($V4-1)+X$3+$AG$12*(X$2-1)+($AD4-1)*$AG$9*$AG$12*$AG$11)-1)*$AG$5
                                                    // =valorInicialPagina+((columnasMatriz*columnasComposicion*(filaMatriz-1)+columnaMatriz+columnasMatriz*(columnaComposicion-1)+(filaComposicion-1)*columnasComposicion*columnasMatriz*filasMatriz)-1)*incremento
                                                    impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                    valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                            +incremento[i]*(indicePagina-1)*impresionesPorPagina;
                                                    valor_actual=valorInicialPagina+((columnasMatriz*columnasComposicion*(filaMatriz)+columnaMatriz+columnasMatriz*(columnaComposicion)+(filaComposicion)*columnasComposicion*columnasMatriz*filasMatriz))*incremento[i];
                                                   int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                    Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);
                                                    qDebug() << "valor_actual contador: " << valor_actual;
                                                }
                                                //Vtemp=QString("%1").arg(valor_actual).rightJustified(10,'0',true);
                                                //qDebug() << "valor_actual barcode: " << valor_actual;
                                                dummy_index++;

                                            }
                                            //Si es un campo de origen BASE DE DATOS
                                            else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                                //A revisar
                                                int numCamposVariablesDB;
                                                int LocalOffset=index*filasComposicion*columnasComposicion*numCamposVariablesDB;
                                                int LocalIndex=i+(filaComposicion*columnasComposicion+columnaComposicion)*numCamposVariablesDB;
                                                //Vtemp=miDataBaseCampos.m_Tabla[index*numCamposVariables*numMatrices+i];
                                                Vtemp=miDataBaseCampos.m_Tabla[LocalOffset+LocalIndex];

                                            }
                                            //Si es un campo de tipo Fecha/Hora
                                            else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                                QDateTime fechahora;
                                                QString sFechaHora;
                                                sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato));
                                                Vtemp=QString("%1").arg(sFechaHora);
                                            }
                                            //Si es un campo de tipo Aleatorio
                                            else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                                int limiteInferior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                                                int limiteSuperior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                                                int numeroCifras=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                                                int valorAleatorioActual=generarNumeroAleatorio(limiteInferior,limiteSuperior);
                                                Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
                                            }
                                            else{
                                                Vtemp=QString("Origen?");
                                            }

                                            if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                                //Retrocedemos
                                                //Retrocedemos
                                                //p.translate(0,(-1)*tamano_fuente_campo);
                                                p.translate(Xtemp,Ytemp);

                                                p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                                struct m_BarcodeParameters bc=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                                In2BarcodeClass barcodeInst;
                                                barcodeInst.setParams(bc);
                                                barcodeInst.SetPixmap(Vtemp);
                                                p.drawPixmap(0,0,barcodeInst.qpxImagen);
                                            }
                                            else if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
                                                //Retrocedemos
                                                //Retrocedemos
                                                //p.translate(0,(-1)*tamano_fuente_campo);
                                                p.translate(Xtemp,Ytemp);
                                                p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                                struct m_BarcodeParameters bc=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                                In2BarcodeClass barcodeInst;
                                                barcodeInst.setParams(bc);
                                                barcodeInst.SetPixmap(Vtemp);
                                                p.drawPixmap(0,0,barcodeInst.qpxImagen);
                                            }
                                            else {
                                                //p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                                //p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                                //p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                                //p.translate(0,(-1)*tamano_fuente_campo);
                                                p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                                p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                                p.drawText(0,0,Vtemp);

                                            }
                                            p.restore();
                                        }
                                        Xtemp+=dix;
                                        celda++;
                                    }
                                    Ytemp+=diy;
                                }
                                impresiones_realizadas++;
                            }//columnaComposicion
                         }//filaComposicion


                    } //if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            } //if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
        } //for (i=0;i<NUM_CAMPOS_VARIABLES;i++){
    }

    /*JORGE FIN */


	p.end();//01122011 Esta linea puede arrglar el problema de los puntos

    ImagenUnitariaPagina=pic.transformed(QTransform().rotate(0,Qt::ZAxis));

}

void DbFormatPage::CargaDatos(){

	QString tmp;

	tmp.setNum(memoryMap->m_mapa.m_variables.WebWidth);
    ui->m_WebWidth->setText(tmp);

	tmp.setNum(memoryMap->m_mapa.m_variables.WebHeight);
    ui->m_WebHeight->setText(tmp);

	tmp.setNum(memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas);
    ui->m_NumeroFilas->setText(tmp);

	tmp.setNum(memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas);
    ui->m_NumeroColumnas->setText(tmp);
	
	tmp.setNum(memoryMap->m_mapa.m_variables.SeparacionY);
    ui->m_SeparacionFilas->setText(tmp);

	tmp.setNum(memoryMap->m_mapa.m_variables.SeparacionX);
    ui->m_SeparacionColumnas->setText(tmp);

    tmp.setNum(memoryMap->m_mapa.m_variables.AnguloPlantilla);
    ui->m_AnguloPlantilla->setText(tmp);

	tmp.setNum(memoryMap->m_mapa.m_variables.PuntoInicialX);
    ui->m_PuntoInicial_X->setText(tmp);

	tmp.setNum(memoryMap->m_mapa.m_variables.PuntoInicialY);
    ui->m_PuntoInicial_Y->setText(tmp);

    CambiarMacula();


}

void DbFormatPage::CambiarMacula(){
    QString tmp;
    unsigned int indice_macula;

    indice_macula=ui->m_SeleccionMacula->currentIndex();
    switch (indice_macula){
        case 0:
                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaIzqdaX);
                ui->m_Macula_X->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaIzqdaY);
                ui->m_Macula_Y->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaIzqdaAncho);
                ui->m_Macula_ancho->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaIzqdaAlto);
                ui->m_Macula_alto->setText(tmp);


                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia);
                ui->m_Macula_frecuencia->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones);
                ui->m_Macula_repeticiones->setText(tmp);

                ui->m_Macula_Color->setCurrentIndex(memoryMap->m_mapa.m_variables.MaculaIzqdaColor-1);
                UpdateColorMacula();

                if (memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff==1) ui->m_Macula_OnOff->setChecked(true);
                else ui->m_Macula_OnOff->setChecked(false);
                UpdateOnOffMacula();
                break;
        case 1:
                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaDchaX);
                ui->m_Macula_X->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaDchaY);
                ui->m_Macula_Y->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaDchaAncho);
                ui->m_Macula_ancho->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaDchaAlto);
                ui->m_Macula_alto->setText(tmp);


                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia);
                ui->m_Macula_frecuencia->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones);
                ui->m_Macula_repeticiones->setText(tmp);

                ui->m_Macula_Color->setCurrentIndex(memoryMap->m_mapa.m_variables.MaculaDchaColor-1);
                UpdateColorMacula();

                if (memoryMap->m_mapa.m_variables.MaculaDchaOnOff==1) ui->m_Macula_OnOff->setChecked(true);
                else ui->m_Macula_OnOff->setChecked(false);
                UpdateOnOffMacula();
                break;
    }

}



void DbFormatPage::CambiarMarca(){
    QString tmp;
    unsigned int indice_marca;

    indice_marca=ui->m_SeleccionMarca->currentIndex();
    switch (indice_marca){
        case 0:
                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaSupX);
                ui->m_Marca_X->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaSupY);
                ui->m_Marca_Y->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaSupFrecuencia);
                ui->m_Marca_frecuencia->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaSupRepeticiones);
                ui->m_Marca_repeticiones->setText(tmp);

                ui->m_Marca_Color->setCurrentIndex(memoryMap->m_mapa.m_variables.MarcaSupColor-1);
                UpdateColorMarca();

                if (memoryMap->m_mapa.m_variables.MarcaSupOnOff==1) ui->m_Marca_OnOff->setChecked(true);
                else ui->m_Marca_OnOff->setChecked(false);
                UpdateOnOffMarca();
                break;
        case 1:
                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaInfX);
                ui->m_Marca_X->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaInfY);
                ui->m_Marca_Y->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaInfFrecuencia);
                ui->m_Marca_frecuencia->setText(tmp);

                tmp.setNum(memoryMap->m_mapa.m_variables.MarcaInfRepeticiones);
                ui->m_Marca_repeticiones->setText(tmp);

                ui->m_Marca_Color->setCurrentIndex(memoryMap->m_mapa.m_variables.MarcaInfColor-1);
                UpdateColorMarca();

                if (memoryMap->m_mapa.m_variables.MarcaInfOnOff==1) ui->m_Marca_OnOff->setChecked(true);
                else ui->m_Marca_OnOff->setChecked(false);
                UpdateOnOffMarca();
                break;
    }

}

void DbFormatPage::GuardaDatos(){

	unsigned int tmp_ImpresionesPorEnvioFilas;
	unsigned int tmp_ImpresionesPorEnvioColumnas;
    //QString tmp;

    memoryMap->m_mapa.m_variables.WebWidth=ui->m_WebWidth->text().toDouble();
    memoryMap->m_mapa.m_variables.WebHeight=ui->m_WebHeight->text().toDouble();
    memoryMap->m_mapa.m_variables.PuntoInicialX=ui->m_PuntoInicial_X->text().toDouble();
    memoryMap->m_mapa.m_variables.PuntoInicialY=ui->m_PuntoInicial_Y->text().toDouble();
    tmp_ImpresionesPorEnvioFilas=ui->m_NumeroFilas->text().toInt();
    tmp_ImpresionesPorEnvioColumnas=ui->m_NumeroColumnas->text().toInt();
    tmp_ImpresionesPorEnvioFilas=ui->m_NumeroFilas->text().toInt();
    tmp_ImpresionesPorEnvioColumnas=ui->m_NumeroColumnas->text().toInt();
    memoryMap->m_mapa.m_variables.ImpresionesPorEnvio=tmp_ImpresionesPorEnvioFilas*tmp_ImpresionesPorEnvioColumnas;
    memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas=tmp_ImpresionesPorEnvioFilas;
    memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas=tmp_ImpresionesPorEnvioColumnas;
    memoryMap->m_mapa.m_variables.SeparacionY=ui->m_SeparacionFilas->text().toDouble();
    memoryMap->m_mapa.m_variables.SeparacionX=ui->m_SeparacionColumnas->text().toDouble();
    memoryMap->m_mapa.m_variables.AnguloPlantilla=ui->m_AnguloPlantilla->text().toInt();
    unsigned int indice_macula;
    indice_macula=ui->m_SeleccionMacula->currentIndex();
    switch (indice_macula){
        case 0:
            memoryMap->m_mapa.m_variables.MaculaIzqdaX=ui->m_Macula_X->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaIzqdaY=ui->m_Macula_Y->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=ui->m_Macula_ancho->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=ui->m_Macula_alto->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=ui->m_Macula_repeticiones->text().toInt();
            memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=ui->m_Macula_frecuencia->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaIzqdaColor=ui->m_Macula_Color->currentIndex()+1;
            if (ui->m_Macula_OnOff->isChecked()) memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=1;
            else memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=0;
            break;
        case 1:
            memoryMap->m_mapa.m_variables.MaculaDchaX=ui->m_Macula_X->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaDchaY=ui->m_Macula_Y->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaDchaAncho=ui->m_Macula_ancho->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaDchaAlto=ui->m_Macula_alto->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=ui->m_Macula_repeticiones->text().toInt();
            memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=ui->m_Macula_frecuencia->text().toDouble();
            memoryMap->m_mapa.m_variables.MaculaDchaColor=ui->m_Macula_Color->currentIndex()+1;
            if (ui->m_Macula_OnOff->isChecked()) memoryMap->m_mapa.m_variables.MaculaDchaOnOff=1;
            else memoryMap->m_mapa.m_variables.MaculaDchaOnOff=0;
            break;
    }



    int index;
    index=ui->tabWidget->currentIndex();/*ui->m_OpcionConfiguracion->currentIndex();*/
	switch (index){
		case OPCION_TAMANO:
            memoryMap->m_mapa.m_variables.WebWidth=ui->m_WebWidth->text().toDouble();
            memoryMap->m_mapa.m_variables.WebHeight=ui->m_WebHeight->text().toDouble();
			break;
		case OPCION_IMAGEN_FIJA:
            memoryMap->m_mapa.m_variables.PuntoInicialX=ui->m_PuntoInicial_X->text().toDouble();
            memoryMap->m_mapa.m_variables.PuntoInicialY=ui->m_PuntoInicial_Y->text().toDouble();
			break;
		case OPCION_MATRIZ:
            tmp_ImpresionesPorEnvioFilas=ui->m_NumeroFilas->text().toInt();
            tmp_ImpresionesPorEnvioColumnas=ui->m_NumeroColumnas->text().toInt();

			memoryMap->m_mapa.m_variables.ImpresionesPorEnvio=tmp_ImpresionesPorEnvioFilas*tmp_ImpresionesPorEnvioColumnas;
			memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas=tmp_ImpresionesPorEnvioFilas;
			memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas=tmp_ImpresionesPorEnvioColumnas;
			
            memoryMap->m_mapa.m_variables.SeparacionY=ui->m_SeparacionFilas->text().toDouble();
            memoryMap->m_mapa.m_variables.SeparacionX=ui->m_SeparacionColumnas->text().toDouble();

            memoryMap->m_mapa.m_variables.AnguloPlantilla=ui->m_AnguloPlantilla->text().toInt();

			break;
		case OPCION_MACULAS:
            unsigned int indice_macula;
            indice_macula=ui->m_SeleccionMacula->currentIndex();
            switch (indice_macula){
                case 0:
                    memoryMap->m_mapa.m_variables.MaculaIzqdaX=ui->m_Macula_X->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaIzqdaY=ui->m_Macula_Y->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=ui->m_Macula_ancho->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=ui->m_Macula_alto->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=ui->m_Macula_repeticiones->text().toInt();
                    memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=ui->m_Macula_frecuencia->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaIzqdaColor=ui->m_Macula_Color->currentIndex()+1;
                    if (ui->m_Macula_OnOff->isChecked()) memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=1;
                    else memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=0;
                    break;
                case 1:
                    memoryMap->m_mapa.m_variables.MaculaDchaX=ui->m_Macula_X->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaDchaY=ui->m_Macula_Y->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaDchaAncho=ui->m_Macula_ancho->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaDchaAlto=ui->m_Macula_alto->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=ui->m_Macula_repeticiones->text().toInt();
                    memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=ui->m_Macula_frecuencia->text().toDouble();
                    memoryMap->m_mapa.m_variables.MaculaDchaColor=ui->m_Macula_Color->currentIndex()+1;
                    if (ui->m_Macula_OnOff->isChecked()) memoryMap->m_mapa.m_variables.MaculaDchaOnOff=1;
                    else memoryMap->m_mapa.m_variables.MaculaDchaOnOff=0;
                    break;
            }
			break;
        /*case OPCION_MARCAS:
            unsigned int indice_marca;
            indice_marca=ui->m_SeleccionMarca->currentIndex();
            switch (indice_marca){
                case 0:
                    memoryMap->m_mapa.m_variables.MarcaSupX=ui->m_Marca_X->text().toDouble();
                    memoryMap->m_mapa.m_variables.MarcaSupY=ui->m_Marca_Y->text().toDouble();
                    memoryMap->m_mapa.m_variables.MarcaSupRepeticiones=ui->m_Marca_repeticiones->text().toInt();
                    memoryMap->m_mapa.m_variables.MarcaSupFrecuencia=ui->m_Marca_frecuencia->text().toDouble();
                    memoryMap->m_mapa.m_variables.MarcaSupColor=ui->m_Marca_Color->currentIndex()+1;
                    if (ui->m_Marca_OnOff->isChecked()) memoryMap->m_mapa.m_variables.MarcaSupOnOff=1;
                    else memoryMap->m_mapa.m_variables.MarcaSupOnOff=0;
                    break;
                case 1:
                    memoryMap->m_mapa.m_variables.MarcaInfX=ui->m_Marca_X->text().toDouble();
                    memoryMap->m_mapa.m_variables.MarcaInfY=ui->m_Marca_Y->text().toDouble();
                    memoryMap->m_mapa.m_variables.MarcaInfRepeticiones=ui->m_Marca_repeticiones->text().toInt();
                    memoryMap->m_mapa.m_variables.MarcaInfFrecuencia=ui->m_Marca_frecuencia->text().toDouble();
                    memoryMap->m_mapa.m_variables.MarcaInfColor=ui->m_Marca_Color->currentIndex()+1;
                    if (ui->m_Macula_OnOff->isChecked()) memoryMap->m_mapa.m_variables.MarcaInfOnOff=1;
                    else memoryMap->m_mapa.m_variables.MarcaInfOnOff=0;
                    break;
            }
            break;*/
	}
}
/** Cargamos datos comunes*/
void DbFormatPage::SetDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=ui->led_EstadoTemperatura;
    //dummyDataCommon.m_Imagen=ui->m_ImagenFormatPage;
    dummyDataCommon.m_NombreFicheroConfig=ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=ui->m_TextoStatus;
    dummyDataCommon.m_User=ui->m_User;
    dummyDataCommon.usuario_actual=ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=ui->m_MostrarMantenimiento;
    dummyDataCommon.lbl_EstadoLampara=ui->lblLamp;
    dummyDataCommon.lbl_EstadoMaquina=ui->lblMaquina;
    dummyDataCommon.lbl_EstadoTemperatura=ui->lblFluid;
    DataCommon=new InkjetCommon(this,dummyDataCommon);
//Actualizamos
    DataCommon->ShowInfo();
    DataCommon->ShowEstadoImpresora();
    DataCommon->ShowComms();

}
/** Borra comentario*/
void DbFormatPage::sltDeleteComentario(){
    int rtn=QMessageBox::question(0,"ATENCION","¿Desea borrar comentario?",QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        memoryMap->m_mapa.m_variables.Comentario.clear();
        ui->m_teComentario->clear();
        QMessageBox::warning(0,"OPERACION COMPLETADA","Comentario borrado",QMessageBox::Ok);
        ui->m_teComentario->setEnabled(false);
    }
}
/** Edita comentario*/
void DbFormatPage::sltEditComentario(){
    ui->m_teComentario->setEnabled(true);
}
/** Guarda comentario*/
void DbFormatPage::sltSaveComentario(){
    int rtn=QMessageBox::question(0,"ATENCION","¿Desea guardar comentario?",QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        memoryMap->m_mapa.m_variables.Comentario=ui->m_teComentario->toPlainText();
        QMessageBox::warning(0,"OPERACION COMPLETADA","Comentario guardado",QMessageBox::Ok);
        ui->m_teComentario->setEnabled(false);
    }
}


void DbFormatPage::ConfigurarMarca(void){

    int indice=ui->m_Marca_Tipo->currentIndex();
    QString texto=ui->m_Marca_Tipo->itemText(indice);

    if ( (!texto.compare(TXT_CAMPO_TIPO_DATAMATRIX)) || (!texto.compare(TXT_CAMPO_TIPO_BARCODE))  )
        ConfigurarCodigoBarras();

}

void DbFormatPage::ConfigurarCodigoBarras(void){
    struct m_BarcodeParameters data;
    int i=ui->m_SeleccionMarca->currentIndex();

    data.NumeroCampo=i;
    data.CampoFijo=0x01;
    data.texto=memoryMap->m_mapa.m_variables.MarcaInfValor;

    formatBarcode *ScrformatBarcode;
    ScrformatBarcode=new formatBarcode(this,data);
    ScrformatBarcode->show();

    QObject::connect(ScrformatBarcode, SIGNAL(sglBarcodeConfigured(const m_BarcodeParameters & )),this, SLOT(sltBarcodeConfigured(const m_BarcodeParameters &)));

}




void DbFormatPage::sltBarcodeConfigured(const m_BarcodeParameters & data){

    struct m_BarcodeParameters  barcode=data;
    int campo=barcode.NumeroCampo;

    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.NumeroCampo=barcode.NumeroCampo;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.width=barcode.width;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.width_mm=barcode.width_mm;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.height=barcode.height;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.height_mm=barcode.height_mm;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.codificacion=barcode.codificacion;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.option2=barcode.option2;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.inputMode=barcode.inputMode;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.texto=barcode.texto;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.whitespace=barcode.whitespace;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.borderWidth=barcode.borderWidth;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.borderType=barcode.borderType;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.hideText=barcode.hideText;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.escala=barcode.escala;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.ar=barcode.ar;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.fontSizeLarge=barcode.fontSizeLarge;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.fontSizeSmall=barcode.fontSizeSmall;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.fontType=barcode.fontType;
    memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.nivelSeguridad=barcode.nivelSeguridad;

}
