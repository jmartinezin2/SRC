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

#include "formatBarcode.h"
#include "in2defines.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
//#include "EditPosicionImagen.h"
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
#include <QDebug>
#include "In2BarcodeClass.h"

//#include "dbgmem.h"


unsigned int ancho_qlabel_barcode;
unsigned int alto_qlabel_barcode;






QStringList NombreCodificacionesBarcode=(QStringList()<<"CODE11"<<"C25MATRIX"<<"C25INTER"<<"C25IATA"<<"C25LOGIC"
<<"C25IND"<<"CODE39"<<"EXCODE39"<<"EANX"<<"EAN128"
<<"CODABAR"<<"CODE128"<<"DPLEIT"<<"DPIDENT"<<"CODE16K"
<<"CODE49"<<"CODE93"<<"FLAT"<<"RSS14"<<"RSS_LTD"
<<"RSS_EXP"<<"TELEPEN"<<"UPCA"<<"UPCE"<<"POSTNET"
<<"MSI_PLESSEY"<<"FIM"<<"LOGMARS"<<"PHARMA"<<"PZN"
<<"PHARMA_TWO"<<"PDF417"<<"PDF417TRUNC"<<"MAXICODE"<<"QRCODE"
<<"CODE128B"<<"AUSPOST"<<"AUSREPLY"<<"AUSROUTE"<<"AUSREDIRECT"
<<"ISBNX"<<"RM4SCC"<<"DATAMATRIX"<<"EAN14"<<"CODABLOCKF"
<<"NVE18"<<"JAPANPOST"<<"KOREAPOST"<<"RSS14STACK"<<"RSS14STACK_OMNI"
<<"RSS_EXPSTACK"<<"PLANET"<<"MICROPDF417"<<"ONECODE"<<"PLESSEY"
<<"TELEPEN_NUM"<<"ITF14"<<"KIX"<<"AZTEC"<<"DAFT"
<<"MICROQR"<<"HIBC_128"<<"HIBC_39"<<"HIBC_DM"<<"HIBC_QR"
<<"HIBC_PDF"<<"HIBC_MICPDF"<<"HIBC_BLOCKF"<<"HIBC_AZTEC"<<"AZRUNE"
<<"CODE32"<<"EANX_CC"<<"EAN128_CC"<<"RSS14_CC"<<"RSS_LTD_CC"
<<"RSS_EXP_CC"<<"UPCA_CC"<<"UPCE_CC"<<"RSS14STACK_CC"<<"RSS14_OMNI_CC"
<<"RSS_EXPSTACK_CC"<<"CHANNEL"<<"CODEONE"<<"GRIDMATRIX");

QList<int> NumeroCodificacionesBarcode=(QList<int>()<<1<<2<<3<<4<<6
<<7<<8<<9<<13<<16
<<18<<20<<21<<22<<23
<<24<<25<<28<<29<<30
<<31<<32<<34<<37<<40
<<47<<49<<50<<51<<52
<<53<<55<<56<<57<<58
<<60<<63<<66<<67<<68
<<69<<70<<71<<72<<74
<<75<<76<<77<<79<<80
<<81<<82<<84<<85<<86
<<87<<89<<90<<92<<93
<<97<<98<<99<<102<<104
<<106<<108<<110<<112<<128
<<129<<130<<131<<132<<133
<<134<<135<<136<<137<<138
<<139<<140<<141<<142);

formatBarcode::formatBarcode(QWidget *parent,struct m_BarcodeParameters data)
    : ui(new Ui::EditBarcodeUI)
{
    ui->setupUi(this);
    //MemoryMap *memoryMap=MemoryMap::request(0);
    escalado=IMAGEN_SINESCALAR;
    offset_x=0;
    offset_y=0;
    m_Barcode=data;
    //int campo=m_Barcode.NumeroCampo;


    ui->m_TipoBorde->clear();
    ui->m_TipoBorde->addItem("NO_BORDER");
    ui->m_TipoBorde->addItem("BIND");
    ui->m_TipoBorde->addItem("BOX");

    ui->m_InputMode->clear();
    ui->m_InputMode->addItem("DATA_MODE");
    ui->m_InputMode->addItem("UNICODE_MODE");
    ui->m_InputMode->addItem("GS1_MODE");

    ui->m_aspectRatio->clear();
    ui->m_aspectRatio->addItem("IgnoreAspectRatio");
    ui->m_aspectRatio->addItem("KeepAspectRatio");
    ui->m_aspectRatio->addItem("CenterBarCode");

    ui->m_Texto->setText(QString("%1").arg(m_Barcode.texto));

    ui->m_Codificacion->clear();
    for(int i=0;i<NombreCodificacionesBarcode.size();i++){
        ui->m_Codificacion->addItem(NombreCodificacionesBarcode.at(i));
    }

    ui->m_securityLevel->clear();
    ui->m_securityLevel->addItem("L");
    ui->m_securityLevel->addItem("M");
    ui->m_securityLevel->addItem("Q");
    ui->m_securityLevel->addItem("H");


    ui->m_AlturaSimbolo->setText(QString("%1").arg(data.height_mm));
    ui->m_AnchuraSimbolo->setText(QString("%1").arg(data.width_mm));
    ui->m_Margen->setText(QString("%1").arg(data.whitespace));
    ui->m_Option2->setText(QString("%1").arg(data.option2));
    ui->m_aspectRatio->setCurrentIndex(data.ar);
    ui->m_TipoBorde->setCurrentIndex(data.borderType);
    ui->m_GrosorBorde->setText(QString("%1").arg(data.borderWidth));
    ui->m_InputMode->setCurrentIndex(data.inputMode);
    ui->m_Codificacion->setCurrentIndex(NumeroCodificacionesBarcode.indexOf(data.codificacion));
    switch (data.textAlignement){
        case Qt::AlignHCenter:
            ui->m_cbAlineacion->setCurrentIndex(0);
        break;
        case Qt::AlignLeft:
            ui->m_cbAlineacion->setCurrentIndex(1);
        break;
        case Qt::AlignRight:
            ui->m_cbAlineacion->setCurrentIndex(2);
        break;
        default:
            ui->m_cbAlineacion->setCurrentIndex(0);
        break;
    }


    if (data.hideText) ui->m_EsconderTexto->setChecked(true);
    else ui->m_EsconderTexto->setChecked(false);

    ui->m_fontSmall->setText(QString("%1").arg(data.fontSizeSmall));
    ui->m_FontText->setCurrentFont(data.fontType);
    ui->m_securityLevel->setCurrentIndex(data.nivelSeguridad);


    /*if (m_Barcode.CampoFijo==0x01){
        ui->m_AlturaSimbolo->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.height_mm));
        ui->m_AnchuraSimbolo->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.width_mm));
        //ui->m_Escala->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.escala));
        ui->m_Margen->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.whitespace));
        ui->m_Option2->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.option2));
        //ui->m_Texto->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.texto));

        int ar_mode=memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.ar;
        ui->m_aspectRatio->setCurrentIndex(ar_mode);

        int tipoBorde=memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.borderType;
        ui->m_TipoBorde->setCurrentIndex(tipoBorde);

        ui->m_GrosorBorde->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.borderWidth));

        int inputMode=memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.inputMode;
        ui->m_InputMode->setCurrentIndex(inputMode);

        int codificacion=memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.codificacion;
        ui->m_Codificacion->setCurrentIndex(NumeroCodificacionesBarcode.indexOf(codificacion));

        int Alignment=memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.textAlignement;
        switch (Alignment){
            case Qt::AlignHCenter:
                ui->m_cbAlineacion->setCurrentIndex(0);
            break;
            case Qt::AlignLeft:
                ui->m_cbAlineacion->setCurrentIndex(1);
            break;
            case Qt::AlignRight:
                ui->m_cbAlineacion->setCurrentIndex(2);
            break;
            default:
                ui->m_cbAlineacion->setCurrentIndex(0);
            break;
        }

        bool hide=memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.hideText;
        if (hide) ui->m_EsconderTexto->setChecked(true);
        else ui->m_EsconderTexto->setChecked(false);

        ui->m_fontSmall->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.fontSizeSmall));
        ui->m_FontText->setCurrentFont(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.fontType);
        //ui->m_fontLarge->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.fontSizeLarge));

        ui->m_securityLevel->setCurrentIndex(memoryMap->m_mapa.m_campos[campo].m_barcodeParameters.nivelSeguridad);
    }
    else{
        ui->m_AlturaSimbolo->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.height_mm));
        ui->m_AnchuraSimbolo->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.width_mm));
        //ui->m_Escala->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.escala));
        ui->m_Margen->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.whitespace));
        ui->m_Option2->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.option2));
        //ui->m_Texto->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.texto));

        int ar_mode=memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.ar;
        ui->m_aspectRatio->setCurrentIndex(ar_mode);

        int tipoBorde=memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.borderType;
        ui->m_TipoBorde->setCurrentIndex(tipoBorde);

        ui->m_GrosorBorde->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.borderWidth));

        int inputMode=memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.inputMode;
        ui->m_InputMode->setCurrentIndex(inputMode);

        int codificacion=memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.codificacion;
        ui->m_Codificacion->setCurrentIndex(NumeroCodificacionesBarcode.indexOf(codificacion));

        int Alignment=memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.textAlignement;
        switch (Alignment){
            case Qt::AlignHCenter:
                ui->m_cbAlineacion->setCurrentIndex(0);
            break;
            case Qt::AlignLeft:
                ui->m_cbAlineacion->setCurrentIndex(1);
            break;
            case Qt::AlignRight:
                ui->m_cbAlineacion->setCurrentIndex(2);
            break;
            default:
                ui->m_cbAlineacion->setCurrentIndex(0);
            break;
        }

        bool hide=memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.hideText;
        if (hide) ui->m_EsconderTexto->setChecked(true);
        else ui->m_EsconderTexto->setChecked(false);

        ui->m_fontSmall->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.fontSizeSmall));
        ui->m_FontText->setCurrentFont(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.fontType);
        //ui->m_fontLarge->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.fontSizeLarge));

        ui->m_securityLevel->setCurrentIndex(memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.nivelSeguridad);
    }*/
 
    //Lo clavamos
    ui->m_ImagenUnitaria->setMaximumWidth(ui->m_ImagenUnitaria->width());
    ui->m_ImagenUnitaria->setMaximumHeight(ui->m_ImagenUnitaria->height());

	CreaImagenMuestra();
    MuestraImagen(rotatedpixmap);

    connect(ui->m_Aceptar,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_Aplicar,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));
    connect(ui->m_Cancelar,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
	
    //connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    connect(ui->m_ImagenUnitaria,SIGNAL(LabelPos(QPoint)),this,SLOT(UpdateMousePos(QPoint)));

    TestRoot();

    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));

}

/** Actualiza la posición del cursor*/
void formatBarcode::UpdateMousePos(const QPoint &p ){

    //Depende si el escalado es horizontal, vertical o no hay
    QPoint ModP=p;

    if (offset_x>0) ModP.setX(p.x()-offset_x);
    if (offset_y>0) ModP.setY(p.y()-offset_y);
    //Queda mejor sin negativos
    if (ModP.x()<0) ModP.setX(0);
    if (ModP.y()<0) ModP.setY(0);

}



void formatBarcode::TestRoot(){
    /*MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()<ACCESS_ROOT){

    }*/
}

formatBarcode::~formatBarcode(){

}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void formatBarcode::AceptarSeleccion(){

    AceptarValoresCampos();

    /*m_Barcode.height=barcodeParameters.height;
    m_Barcode.width=barcodeParameters.width;
    m_Barcode.codificacion=barcodeParameters.codificacion;
    m_Barcode.option2=barcodeParameters.option2;
    m_Barcode.texto=barcodeParameters.texto;
    m_Barcode.fontSizeLarge=barcodeParameters.fontSizeLarge;
    m_Barcode.fontSizeSmall=barcodeParameters.fontSizeSmall;
    m_Barcode.borderType=barcodeParameters.borderType;
    m_Barcode.borderWidth=barcodeParameters.borderWidth;
    m_Barcode.whitespace=barcodeParameters.whitespace;
    m_Barcode.hideText==barcodeParameters.hideText;
    m_Barcode.inputMode=barcodeParameters.inputMode;
    m_Barcode.escala=barcodeParameters.escala;*/

    emit sglBarcodeConfigured(m_Barcode);

    this->deleteLater();

}


void formatBarcode::CancelarSeleccion(){

    this->deleteLater();
}


void formatBarcode::UpdateAnchoBarcode(){
    InputData *input = InputData::request(this, "Ancho Barcode [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAncho(QString)));
        input->setFocus();
    }
}

void formatBarcode::UpdateAncho(QString value){
    //ui->m_Ancho->setText(value);
    //ui->m_Ancho->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void formatBarcode::UpdateAltoBarcode(){
    InputData *input = InputData::request(this, "Alto Barcode [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAlto(QString)));
        input->setFocus();
    }
}

void formatBarcode::UpdateAlto(QString value){
    //ui->m_Alto->setText(value);
    //ui->m_Alto->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void formatBarcode::AceptarValoresCampos(){
	
    ui->m_Aceptar->setEnabled(false);
    ui->m_Cancelar->setEnabled(false);

    CreaImagenMuestra();
	MuestraImagen(rotatedpixmap);

    ui->m_Aceptar->setEnabled(true);
    ui->m_Cancelar->setEnabled(true);

}


//Crea cada una de las imagenes que forma la impresion
void formatBarcode::CreaImagenMuestra(){


    QString error;

    ui->lbl_error->setText("--");

    double alturaSimbolo_mm=ui->m_AlturaSimbolo->toPlainText().toDouble();
    double anchuraSimbolo_mm=ui->m_AnchuraSimbolo->toPlainText().toDouble();

    m_Barcode.height_mm=alturaSimbolo_mm;
    m_Barcode.width_mm=anchuraSimbolo_mm;

    float escala=1.0;//ui->m_Escala->toPlainText().toFloat();
    int margen=ui->m_Margen->toPlainText().toInt();
    bool hide=false;
    if (ui->m_EsconderTexto->isChecked()) hide=true;
    int ancho_borde=ui->m_GrosorBorde->toPlainText().toInt();

    int indice_codificacion=ui->m_Codificacion->currentIndex();
    if (indice_codificacion<0) indice_codificacion=0;
    int codificacion=NumeroCodificacionesBarcode.at(indice_codificacion);

    int input_mode=ui->m_InputMode->currentIndex();
    int option2=ui->m_Option2->toPlainText().toInt();

    QString texto=ui->m_Texto->toPlainText();

    Zint::QZint::BorderType tipo_borde=Zint::QZint::NO_BORDER;
    int indice_tipo_borde=ui->m_TipoBorde->currentIndex();
    if (indice_tipo_borde==0) tipo_borde=Zint::QZint::NO_BORDER;
    else if (indice_tipo_borde==1) tipo_borde=Zint::QZint::BIND;
    else if (indice_tipo_borde==2) tipo_borde=Zint::QZint::BOX;

    Zint::QZint::AspectRatioMode ar_mode=Zint::QZint::IgnoreAspectRatio;
    int indice_ar=ui->m_aspectRatio->currentIndex();
    if (indice_ar==0) ar_mode=Zint::QZint::IgnoreAspectRatio;
    else if (indice_ar==1) ar_mode=Zint::QZint::KeepAspectRatio;
    else if (indice_ar==2) ar_mode=Zint::QZint::CenterBarCode;

    int nivelSeguridad=ui->m_securityLevel->currentIndex();

    int Alignment=Qt::AlignHCenter;
    switch(ui->m_cbAlineacion->currentIndex()){
        case 0:
            Alignment=Qt::AlignHCenter;
        break;
        case 1:
            Alignment=Qt::AlignLeft;
        break;
        case 2:
            Alignment=Qt::AlignRight;
        break;
    }

    int largeSize=10;//ui->m_fontLarge->toPlainText().toInt();
    int smallSize=ui->m_fontSmall->toPlainText().toInt();
    QString font=ui->m_FontText->currentFont().toString();
    //QPixmap pic(rotatedpixmap.size());
    QPixmap pic;//(ancho,alto);
	QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);



    m_Barcode.texto=texto;

    int altura_pix =(int) (alturaSimbolo_mm * FACTOR_PIXELS_MM);
    int anchura_pix =(int) (anchuraSimbolo_mm * FACTOR_PIXELS_MM);

    m_Barcode.height=altura_pix;
    m_Barcode.width=anchura_pix;

    m_Barcode.escala=escala;
    m_Barcode.whitespace=margen;
    m_Barcode.codificacion=codificacion;
    m_Barcode.inputMode=input_mode;
    m_Barcode.option2=option2;
    m_Barcode.hideText=hide;
    m_Barcode.borderType=tipo_borde;
    m_Barcode.borderWidth=ancho_borde;
    m_Barcode.fontSizeLarge=largeSize;
    m_Barcode.fontSizeSmall=smallSize;
    m_Barcode.fontType=font;
    m_Barcode.ar=ar_mode;
    m_Barcode.nivelSeguridad=nivelSeguridad;
    m_Barcode.textAlignement=Alignment;

    dummybc=new In2BarcodeClass(m_Barcode);

    dummybc->SetPixmap(texto);

    error=dummybc->GetLastError();
    ui->lbl_error->setText(QString("%1").arg(error));

    p.drawPixmap(0,0,dummybc->qpxImagen);
 	
    p.end();

    rotatedpixmap=dummybc->qpxImagen.transformed(QTransform().rotate(0,Qt::ZAxis));
    //rotatedpixmap.save("C:\\barcodegenerado.bmp","BMP");
}



//Muestra la imagen central
void formatBarcode::MuestraImagen(QPixmap Imagen){

    int lw=ui->m_ImagenUnitaria->width();
    int lh=ui->m_ImagenUnitaria->height();
    int iw=Imagen.width();
    int ih=Imagen.height();
    //Nos salimos si la imagen tiene dimensiones erroneas
    if ((lw==0)||(lh==0)) return;

    escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

    QPixmap pic(ui->m_ImagenUnitaria->size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);
    QPixmap dummy;

    switch (escalado){
    case IMAGEN_SINESCALAR:
        dummy=Imagen;
        offset_x=(ui->m_ImagenUnitaria->width()-dummy.width())/2;
        offset_y=(ui->m_ImagenUnitaria->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        ui->m_ImagenUnitaria->setOriginalPixmap(Imagen);
        ui->m_ImagenUnitaria->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2W:
        dummy=Imagen.scaledToWidth(ui->m_ImagenUnitaria->width(),Qt::SmoothTransformation);
        offset_x=(ui->m_ImagenUnitaria->width()-dummy.width())/2;
        offset_y=(ui->m_ImagenUnitaria->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        ui->m_ImagenUnitaria->setOriginalPixmap(Imagen);
        ui->m_ImagenUnitaria->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2H:
        dummy=Imagen.scaledToHeight(ui->m_ImagenUnitaria->height(),Qt::SmoothTransformation);
        offset_x=(ui->m_ImagenUnitaria->width()-dummy.width())/2;
        offset_y=(ui->m_ImagenUnitaria->height()-dummy.height())/2;
        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
        ui->m_ImagenUnitaria->setOriginalPixmap(Imagen);
        ui->m_ImagenUnitaria->setPixmap((QPixmap)(pic));
        break;
}

    p.end();

    ui->m_ImagenUnitaria->show();

}


