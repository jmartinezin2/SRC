/** @addtogroup DbFormatPlantilla
 *  @{*/
/** @file DbFormatPlantilla.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbFormatPlantilla.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include <QDebug>

#include "dbformat.h"
#include "dbformatplantilla.h"
#include "dbimpresion.h"
#include "globals.h"
#include "principal.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
#include "principal.h"
//#include "EditPosicionImagen.h"
#include "corteimagen.h"
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
#include <QMouseEvent>
#include <QTableView>
//#include "dbgmem.h"

#define MIN_DISTANCE_COLS 1
#define MAX_DISTANCE_COLS 300

// Define los tabs
#define TAB_IMAGEN 0
#define TAB_PLANTILLA 1



DbFormatPlantilla::DbFormatPlantilla(QWidget *parent)
    : ui(new Ui::EditImagenFijaUI)
{

    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    ui->setupUi(this);


     ui->m_hsPlantAlto->setValue(memoryMap->m_mapa.m_variables.fPlantAlto);
     ui->m_hsPlantAncho->setValue(memoryMap->m_mapa.m_variables.fPlantAlto);
     ui->m_PlantAlto->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.fPlantAlto));
     ui->m_PlantAncho->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.fPlantAncho));
     ui->m_hsPlantColsDist->setValue(memoryMap->m_mapa.m_variables.fPlantDistCols);
     ui->m_PlantsColDist->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.fPlantDistCols));
     ui->m_PlantCols->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.iPlantCols));
     ui->m_PlantOffsetX->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.fPlantOffsetX));
     ui->m_PlantOffsetY->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.fPlantOffsetY));
     ui->m_hsPlantOffsetX->setValue(memoryMap->m_mapa.m_variables.fPlantOffsetX);
     ui->m_hsPlantOffsetY->setValue(memoryMap->m_mapa.m_variables.fPlantOffsetY);
     ui->m_twPlantImages->setColumnWidth(0,50);
     ui->m_twPlantImages->horizontalHeader()->setStretchLastSection(true);
     for (int i=0;i<memoryMap->m_mapa.m_variables.strPlantImages.count();i++){
         QTableWidgetItem *p=new QTableWidgetItem(memoryMap->m_mapa.m_variables.strPlantImages.at(i));
         ui->m_twPlantImages->setItem(i,0,p);
     }



    GlobalSetScreenSize(this);
    this->setDataCommon();

    ui->m_Xinicial->setPalette(dummyPaletteEnabled);
    ui->m_Yinicial->setPalette(dummyPaletteEnabled);
    ui->m_Ancho->setPalette(dummyPaletteEnabled);
    ui->m_Alto->setPalette(dummyPaletteEnabled);
    ui->m_NumColor->setPalette(dummyPaletteEnabled);
    ui->m_XColor->setPalette(dummyPaletteEnabled);
    ui->m_YColor->setPalette(dummyPaletteEnabled);


    ui->m_Xinicial->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.XinicialPlantilla));
    ui->m_Yinicial->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.YinicialPlantilla));
    ui->m_Ancho->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.AnchoPlantilla));
    ui->m_Alto->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.AltoPlantilla));


    ui->m_XColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[0].x_pix));
    ui->m_YColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[0].y_pix));
    ui->m_NumTotalColores->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.NumColores));

    ui->m_ColorSeleccionado->setReadOnly(true);
    ui->m_ColorMascara->setReadOnly(true);

    QPalette paleta_color_pleno;
    paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[0].valor));
    ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);

    paleta_color_pleno.setColor(QPalette::Base,QColor(QRgb(memoryMap->m_mapa.m_ColorMascara.valor)));
    ui->m_ColorMascara->setPalette(paleta_color_pleno);


    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        ui->m_NumColor->addItem(QString("Color %1").arg(i+1));
    }
    ui->m_NumColor->setCurrentIndex(0);
    ActualizarControlesColor();

    ui->m_Xinicial->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.XinicialPlantilla));
    ui->m_Yinicial->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.YinicialPlantilla));
    ui->m_Ancho->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.AnchoPlantilla));
    ui->m_Alto->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.AltoPlantilla));
    ui->m_RutaPlantilla->setText(QString(memoryMap->m_mapa.m_variables.NombreRutaPdf));

    PasarValoresDeMapATemp();

    //GeneracionRutaPlantilla();
    pixmapPlantilla.load(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
   //QPixmap ImagenFondo=CreaImagenMuestra();
    /*ui->m_ImagenFormat->setMaximumHeight(ui->m_ImagenFormat->height());
    ui->m_ImagenFormat->setMaximumWidth(ui->m_ImagenFormat->width());*/

    ///MuestraImagen(pixmapPlantilla);

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_BuscarPlantilla,SIGNAL(clicked()),this,SLOT(BuscarPlantilla()));


    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_Xinicial,SIGNAL(selectionChanged()),this,SLOT(UpdateCoordenadaXinicial()));
    connect(ui->m_Yinicial,SIGNAL(selectionChanged()),this,SLOT(UpdateCoordenadaYinicial()));
    connect(ui->m_Ancho,SIGNAL(selectionChanged()),this,SLOT(UpdateAnchoPlantilla()));
    connect(ui->m_Alto,SIGNAL(selectionChanged()),this,SLOT(UpdateAltoPlantilla()));
    connect(ui->m_XColor,SIGNAL(selectionChanged()),this,SLOT(UpdateCoordenadaXcolor()));
    connect(ui->m_YColor,SIGNAL(selectionChanged()),this,SLOT(UpdateCoordenadaYcolor()));
    #endif

    connect(ui->m_AceptarValores,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));
    connect(ui->m_NumColor,SIGNAL(activated (QString)),this,SLOT(UpdateColor()));
    connect(ui->m_ColorMascara,SIGNAL(selectionChanged()),this,SLOT(SelColorMascara()));
    connect(ui->m_ColorSeleccionado,SIGNAL(selectionChanged()),this,SLOT(SelColorPrint()));
    //Selección de color activa
    ui->m_ColorSeleccionado->setFrameShadow(QFrame::Plain);

    connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    connect(ui->m_ImagenFormat,SIGNAL(DobleClick(int,int)),this,SLOT(ActualizarColor(int,int)));
    connect(ui->m_SelCorte,SIGNAL(clicked()),this,SLOT(SelCorteImagen()));

    connect(ui->m_Previsualizacion,SIGNAL(clicked()),this,SLOT(sltPrevisualizacion()));
    connect(ui->m_cbColor,SIGNAL(clicked()),this,SLOT(SelColorPrint()));
    connect(ui->m_cbMascara,SIGNAL(clicked()),this,SLOT(SelColorMascara()));
    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
    //ui->m_rbColor->setVisible(true);
    //ui->m_rbMascara->setVisible(true);
	TestRoot();

    AceptarValoresCampos();
    //Valores de troquel/platilla
    ui->m_hsPlantColsDist->setMinimum(MIN_DISTANCE_COLS);
    ui->m_hsPlantColsDist->setMaximum(MAX_DISTANCE_COLS);
    ui->m_hsPlantAlto->setMinimum(1);
    ui->m_hsPlantAlto->setMaximum(memoryMap->m_mapa.m_variables.WebHeight);
    ui->m_hsPlantAncho->setMinimum(1);
    ui->m_hsPlantAncho->setMaximum(memoryMap->m_mapa.m_variables.WebWidth);
    ui->m_PlantHeight->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.WebHeight));
    ui->m_PlantWidth->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.WebWidth));

    connect(ui->m_PlantEnabled,SIGNAL(clicked()),this,SLOT(sltPlantEnabled()));
    connect(ui->m_hsPlantColsDist,SIGNAL(sliderMoved(int)),this,SLOT(sltUpdateDistanciaColumnas(int)));
    connect(ui->m_hsPlantAlto,SIGNAL(sliderMoved(int)),this,SLOT(sltUpdateAltoPlantilla(int)));
    connect(ui->m_hsPlantAncho,SIGNAL(sliderMoved(int)),this,SLOT(sltUpdateAnchoPlantilla(int)));
    connect(ui->m_hsPlantOffsetX,SIGNAL(sliderMoved(int)),this, SLOT(sltUpdateOffsetX(int)));
    connect(ui->m_hsPlantOffsetY,SIGNAL(sliderMoved(int)),this, SLOT(sltUpdateOffsetY(int)));
    connect(ui->m_twPlantImages,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(sltAddPlantImage(int,int)));


//Se testea el modulo  de plantillas
    #ifndef MODULO_PLANTILLAS
        ui->tabwidget->setTabEnabled(TAB_PLANTILLA,false);
    #endif
    if (memoryMap->m_mapa.m_variables.bPlantEnabled){
        #ifndef MODULO_PLANTILLAS
            QMessageBox::warning(0,"ERROR","El móduilo de generación de plantillas no está incluido en esta versión de software. Póngase en contacto con In2 Printing Solutions para ampliar funcionalidades.",QMessageBox::Ok);
            ui->tabwidget->setCurrentIndex(TAB_IMAGEN);
        #else
            ui->m_PlantEnabled->setChecked(true);
            ui->tabwidget->setCurrentIndex(TAB_PLANTILLA);
            sltRenderPlantilla();
        #endif
    }
    else{
        ui->tabwidget->setCurrentIndex(TAB_IMAGEN);
    }

    ui->m_PlantAlto->installEventFilter(this);
    ui->m_PlantAncho->installEventFilter(this);
    ui->m_PlantCols->installEventFilter(this);
    ui->m_PlantsColDist->installEventFilter(this);
    ui->m_PlantOffsetX->installEventFilter(this);
    ui->m_PlantOffsetY->installEventFilter(this);
}

/** Filtro de eventos*/
bool DbFormatPlantilla::eventFilter(QObject *p, QEvent *e){
    int val=0;
    if (e->type() == QEvent::KeyPress){
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
            if (keyEvent->key() == Qt::Key_Up){
                if (p==ui->m_PlantAlto){
                    val=ui->m_PlantAlto->text().toInt();
                    val++;
                    ui->m_PlantAlto->setText(QString("%1").arg(val));
                    ui->m_hsPlantAlto->setValue(val);
                    sltUpdatePlantilla(0);
                    return true;
                }
                else if (p==ui->m_PlantAncho){
                    val=ui->m_PlantAncho->text().toInt();
                    val++;
                    ui->m_PlantAncho->setText(QString("%1").arg(val));
                    ui->m_hsPlantAncho->setValue(val);
                    sltUpdatePlantilla(0);
                    return true;
                }
                else if (p==ui->m_PlantCols){
                    val=ui->m_PlantCols->text().toInt();
                    val++;
                    ui->m_PlantCols->setText(QString("%1").arg(val));
                    sltUpdatePlantilla(0);
                    return true;
                }
                else if (p==ui->m_PlantOffsetX){
                    val=ui->m_PlantOffsetX->text().toInt();
                    val++;
                    ui->m_hsPlantOffsetX->setValue(val);
                    ui->m_PlantOffsetX->setText(QString("%1").arg(val));
                    sltUpdatePlantilla(0);
                    return true;
                }
                else if (p==ui->m_PlantOffsetY){
                    val=ui->m_PlantOffsetY->text().toInt();
                    val++;
                    ui->m_PlantOffsetY->setText(QString("%1").arg(val));
                    ui->m_hsPlantOffsetY->setValue(val);
                    sltUpdatePlantilla(0);
                    return true;
                }
                else if (p==ui->m_PlantsColDist){
                    val=ui->m_PlantsColDist->text().toInt();
                    val++;
                    ui->m_PlantsColDist->setText(QString("%1").arg(val));
                    sltUpdatePlantilla(0);
                    return true;
                }

            }
            else if(keyEvent->key() == Qt::Key_Down){
                if (p==ui->m_PlantAlto){
                    val=ui->m_PlantAlto->text().toInt();
                    if (val>0){
                        val--;
                        ui->m_hsPlantAlto->setValue(val);
                        ui->m_PlantAlto->setText(QString("%1").arg(val));
                        sltUpdatePlantilla(0);
                    }
                    return true;
                }
                else if (p==ui->m_PlantAncho){
                    val=ui->m_PlantAncho->text().toInt();
                    if (val>0){
                        val--;
                        ui->m_hsPlantAncho->setValue(val);
                        ui->m_PlantAncho->setText(QString("%1").arg(val));
                        sltUpdatePlantilla(0);
                    }
                    return true;
                }
                else if (p==ui->m_PlantCols){
                    val=ui->m_PlantCols->text().toInt();
                    if (val>0){
                        val--;
                        ui->m_PlantCols->setText(QString("%1").arg(val));
                        sltUpdatePlantilla(0);
                    }
                    return true;
                }
                else if (p==ui->m_PlantOffsetX){
                    val=ui->m_PlantOffsetX->text().toInt();
                    if (val>0){
                        val--;
                        ui->m_hsPlantOffsetX->setValue(val);
                        ui->m_PlantOffsetX->setText(QString("%1").arg(val));
                        sltUpdatePlantilla(0);
                    }
                    return true;
                }
                else if (p==ui->m_PlantOffsetY){
                    val=ui->m_PlantOffsetY->text().toInt();
                    if (val>0){
                        val--;
                        ui->m_PlantOffsetY->setText(QString("%1").arg(val));
                        ui->m_hsPlantOffsetY->setValue(val);
                        sltUpdatePlantilla(0);
                    }
                    return true;
                }
                else if (p==ui->m_PlantsColDist){
                    val=ui->m_PlantsColDist->text().toInt();
                    if (val>0){
                        val--;
                        ui->m_PlantsColDist->setText(QString("%1").arg(val));
                        sltUpdatePlantilla(0);
                    }
                    return true;
                }

            }
      }
    return QWidget::eventFilter(p,e);
}

void DbFormatPlantilla::sltAddPlantImage(int x,int y){
    QFileDialog fileName(this);
    QStringList Ficheros;

    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
    fileName.setFilter(tr("ini,bmp Files (*.ini *.bmp)"));


    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
            QTableWidgetItem *p=new QTableWidgetItem(Ficheros.at(0));
            Qt::ItemFlags Flags;
            Flags=p->flags();
            Flags&=~(Qt::ItemIsEditable);
            p->setFlags(Flags);
            ui->m_twPlantImages->setItem(x,y,p);
        }
    }
}

void DbFormatPlantilla::sltPlantEnabled(){
    if (ui->m_PlantEnabled->isChecked()){
        int rtn=QMessageBox::warning(0,"WARNING","Esto borrará la imagen de fondo...desea continuar?",QMessageBox::Ok| QMessageBox::Cancel);
        if (rtn==QMessageBox::Ok) sltRenderPlantilla();
    }
}
/** Actualiza el Offset Y*/
void DbFormatPlantilla::sltUpdateOffsetY(int d){
    ui->m_PlantOffsetY->setText(QString("%1").arg(ui->m_hsPlantOffsetY->value()));
    if (ui->m_PlantEnabled->isChecked())
        sltRenderPlantilla();
}
/** Actualiza el Offset x*/
void DbFormatPlantilla::sltUpdateOffsetX(int d){
    ui->m_PlantOffsetX->setText(QString("%1").arg(ui->m_hsPlantOffsetX->value()));
    if (ui->m_PlantEnabled->isChecked())
        sltRenderPlantilla();
}
/** Actualiza el alto de la plantilla*/
void DbFormatPlantilla::sltUpdateAltoPlantilla(int d){
    ui->m_PlantAlto->setText(QString("%1").arg(ui->m_hsPlantAlto->value()));
    if (ui->m_PlantEnabled->isChecked())
        sltRenderPlantilla();
}
/** Actualiza el ancho de la plantilla*/
void DbFormatPlantilla::sltUpdateAnchoPlantilla(int d){
    ui->m_PlantAncho->setText(QString("%1").arg(ui->m_hsPlantAncho->value()));
    if (ui->m_PlantEnabled->isChecked())
        sltRenderPlantilla();
}
/** Actualiza el ancho de la plantilla*/
void DbFormatPlantilla::sltUpdateDistanciaColumnas(int d){
    ui->m_PlantsColDist->setText(QString("%1").arg(ui->m_hsPlantColsDist->value()));
    if (ui->m_PlantEnabled->isChecked())
        sltRenderPlantilla();
}



/** Actualiza la plantilla*/
void DbFormatPlantilla::sltUpdatePlantilla(int d){   
    if (ui->m_PlantEnabled->isChecked())
        sltRenderPlantilla();
}


/** Hace una previsualizacion de la impresión*/
void DbFormatPlantilla::sltPrevisualizacion(){
    //Esto hay que hacerlo para cada color
    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        QString dummyruta=QString(memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummyruta.replace(".bmp",QString("_C%1.bmp").arg(i+1));
        QPixmap prevPixmap(dummyruta);

        if (prevPixmap.width()){
            unsigned char escalado;
            QLabel *prevImage= new QLabel(this);

            Qt::WindowFlags flags=Qt::Window;
            prevImage->setWindowFlags(flags);

            prevImage->setWindowTitle(dummyruta);
            prevImage->setGeometry(50,50,ui->m_ImagenFormat->width(),ui->m_ImagenFormat->height());


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

        switch (escalado){
        case IMAGEN_SINESCALAR:
            dummy=prevPixmap;
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));

            break;
        case IMAGEN_S2W:
            dummy=prevPixmap.scaledToWidth(prevImage->width()/*,Qt::SmoothTransformation*/);
            offset_x_plantilla=(prevImage->width()-dummy.width())/2;
            offset_y_plantilla=(prevImage->height()-dummy.height())/2;
            p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
            prevImage->setPixmap((QPixmap)(pic));
            break;
        case IMAGEN_S2H:
            dummy=prevPixmap.scaledToHeight(prevImage->height()/*,Qt::SmoothTransformation*/);
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

/** Selecciona color mascara*/
void DbFormatPlantilla::SelColorMascara(){
    /*if (ui->m_ColorMascara->frameShadow()==QFrame::Sunken){
       ui->m_ColorMascara->setFrameShadow(QFrame::Plain);
       ui->m_ColorSeleccionado->setFrameShadow(QFrame::Sunken);
    }
    else {
        ui->m_ColorMascara->setFrameShadow(QFrame::Sunken);
        ui->m_ColorSeleccionado->setFrameShadow(QFrame::Plain);
    }*/
    ui->m_cbColor->setChecked(false);
}

/** Selecciona color impresion*/
void DbFormatPlantilla::SelColorPrint(){
/*    if (ui->m_ColorSeleccionado->frameShadow()==QFrame::Sunken){
       ui->m_ColorSeleccionado->setFrameShadow(QFrame::Plain);
       ui->m_ColorMascara->setFrameShadow(QFrame::Sunken);
    }
    else{
        ui->m_ColorSeleccionado->setFrameShadow(QFrame::Sunken);
        ui->m_ColorMascara->setFrameShadow(QFrame::Plain);
    }*/

    //ui->m_cbMascara->setChecked(false);
    if (ui->m_cbColor->isChecked()){
        ui->m_ColorSeleccionado->setEnabled(true);
    }
    else{
        int i;
        ui->m_ColorSeleccionado->setEnabled(false);
        i=ui->m_NumColor->currentIndex();
        memoryMap->m_mapa.m_colores[i].x_pix=0;
        memoryMap->m_mapa.m_colores[i].y_pix=0;
        ui->m_XColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[i].x_pix));
        ui->m_YColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[i].y_pix));
    }
}


/** Pantalla de selección de corte*/
void DbFormatPlantilla::SelCorteImagen(){
    CorteImagen *p;
    p=new CorteImagen(0);
    p->show();
    this->deleteLater();
}


void DbFormatPlantilla::TestRoot(){
    if (memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_AceptarValores->setEnabled(false);
        ui->m_Xinicial->setEnabled(false);
        ui->m_Yinicial->setEnabled(false);
        ui->m_Ancho->setEnabled(false);
        ui->m_Alto->setEnabled(false);
        ui->m_BuscarPlantilla->setEnabled(false);
        ui->m_RutaPlantilla->setEnabled(false);
        ui->m_XColor->setEnabled(false);
        ui->m_YColor->setEnabled(false);
        ui->m_NumColor->setEnabled(false);
        ui->m_NumTotalColores->setEnabled(false);
	}
}

DbFormatPlantilla::~DbFormatPlantilla(){
}

void DbFormatPlantilla::PasarValoresDeMapATemp(){
    XinicialPlantilla_tmp=memoryMap->m_mapa.m_variables.XinicialPlantilla;
    YinicialPlantilla_tmp=memoryMap->m_mapa.m_variables.YinicialPlantilla;
    AnchoPlantilla_tmp=memoryMap->m_mapa.m_variables.AnchoPlantilla;
    AltoPlantilla_tmp=memoryMap->m_mapa.m_variables.AltoPlantilla;
    RutaPlantilla_tmp=memoryMap->m_mapa.m_variables.RutaPdf;

    NumColores_tmp=memoryMap->m_mapa.m_variables.NumColores;
    for(int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
            m_colores_tmp[i]=memoryMap->m_mapa.m_colores[i];
    }

}

void DbFormatPlantilla::PasarValoresDeTempAMap(){
    memoryMap->m_mapa.m_variables.XinicialPlantilla=XinicialPlantilla_tmp;
    memoryMap->m_mapa.m_variables.YinicialPlantilla=YinicialPlantilla_tmp;
    memoryMap->m_mapa.m_variables.AnchoPlantilla=AnchoPlantilla_tmp;
    memoryMap->m_mapa.m_variables.AltoPlantilla=AltoPlantilla_tmp;
    memoryMap->m_mapa.m_variables.RutaPdf=RutaPlantilla_tmp;

    for(int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        memoryMap->m_mapa.m_colores[i]=m_colores_tmp[i];
    }

}

//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void DbFormatPlantilla::AceptarSeleccion(){
    int rtn=QMessageBox::question(0,"ATENCION","Deseas guardar los cambios?",QMessageBox::Yes,QMessageBox::No);
    if(rtn==QMessageBox::No){
        CancelarSeleccion();
    }
    else{
        if (!ui->m_PlantEnabled->isChecked()){
            QLabel *dummy = ShowMensaje("Rendering");
            memoryMap->m_mapa.m_variables.XinicialPlantilla=ui->m_Xinicial->toPlainText().toDouble();
            memoryMap->m_mapa.m_variables.YinicialPlantilla=ui->m_Yinicial->toPlainText().toDouble();
            memoryMap->m_mapa.m_variables.AnchoPlantilla=ui->m_Ancho->toPlainText().toDouble();
            memoryMap->m_mapa.m_variables.AltoPlantilla=ui->m_Alto->toPlainText().toDouble();
            memoryMap->m_mapa.m_variables.NombreRutaPdf=ui->m_RutaPlantilla->text();
            memoryMap->m_mapa.m_variables.NumColores=ui->m_NumTotalColores->toPlainText().toInt();
            ActualizarControlesColor();
            delete (dummy);
            LaunchDbImpresion();
        }
        else{
            memoryMap->m_mapa.m_variables.fPlantAncho=ui->m_PlantAncho->text().toDouble();
            memoryMap->m_mapa.m_variables.fPlantAlto=ui->m_PlantHeight->text().toDouble();
            memoryMap->m_mapa.m_variables.fPlantOffsetX=ui->m_PlantOffsetX->text().toDouble();
            memoryMap->m_mapa.m_variables.fPlantOffsetY=ui->m_PlantOffsetY->text().toDouble();
            memoryMap->m_mapa.m_variables.fPlantDistCols=ui->m_PlantsColDist->text().toDouble();
            memoryMap->m_mapa.m_variables.iPlantCols=ui->m_PlantCols->text().toInt();
            memoryMap->m_mapa.m_variables.bPlantEnabled=true;
            for (int x=0;x<memoryMap->m_mapa.m_variables.iPlantCols;x++){
                QTableWidgetItem *p=ui->m_twPlantImages->item(0,x);
                if (p){
                    QString filename=p->text();
                    memoryMap->m_mapa.m_variables.strPlantImages.insert(x,filename);
                }
            }
            LaunchDbImpresion();
        }
    }
}


void DbFormatPlantilla::CancelarSeleccion(){
    QLabel *dummy = ShowMensaje("Rendering");
    PasarValoresDeTempAMap();
    CreaImagenMuestra();
    delete (dummy);
	LaunchDbImpresion();
	//this->ShowMainWindow();
	this->deleteLater();
	
}


// Actualiza los datos de pantalla
void DbFormatPlantilla::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}


void DbFormatPlantilla::UpdateCoordenadaXinicial(){

    InputData *input = InputData::request(this,"Posicion X Inicial [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateXinicial(QString)));
}

void DbFormatPlantilla::UpdateXinicial(QString value){

    ui->m_Xinicial->setText(value);
    ui->m_Xinicial->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPlantilla::UpdateCoordenadaYinicial(){

    InputData *input = InputData::request(this,"Posicion Y Inicial [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateYinicial(QString)));
}

void DbFormatPlantilla::UpdateYinicial(QString value){

    ui->m_Yinicial->setText(value);
    ui->m_Yinicial->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}
void DbFormatPlantilla::UpdateAnchoPlantilla(){

    InputData *input = InputData::request(this,"Ancho [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAncho(QString)));
}

void DbFormatPlantilla::UpdateAncho(QString value){

    ui->m_Ancho->setText(value);
    ui->m_Ancho->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPlantilla::UpdateAltoPlantilla(){

    InputData *input = InputData::request(this,"Alto [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAlto(QString)));
}

void DbFormatPlantilla::UpdateAlto(QString value){

    ui->m_Alto->setText(value);
    ui->m_Alto->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormatPlantilla::UpdateNumeroColores(){

    InputData *input = InputData::request(this,"Numero total de colores");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateNumero(QString)));
}

void DbFormatPlantilla::UpdateNumero(QString value){

    ui->m_NumTotalColores->setText(value);
    ui->m_NumTotalColores->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPlantilla::UpdateColor(){
    int i;
    i=ui->m_NumColor->currentIndex();
    ui->m_XColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[i].x_pix));
    ui->m_YColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[i].y_pix));

    QPalette color_pleno;
    color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[i].valor));
    ui-> m_ColorSeleccionado->setPalette(color_pleno);


}

void DbFormatPlantilla::UpdateCoordenadaXcolor(){
    InputData *input = InputData::request(this,"Posicion X Color [pix]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateXcolor(QString)));
}

void DbFormatPlantilla::UpdateCoordenadaYcolor(){
    InputData *input = InputData::request(this,"Posicion Y Color [pix]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateYcolor(QString)));
}

void DbFormatPlantilla::UpdateXcolor(QString value){
    ui->m_XColor->setText(value);
    ui->m_XColor->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPlantilla::UpdateYcolor(QString value){
    ui->m_YColor->setText(value);
    ui->m_YColor->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormatPlantilla::AceptarValoresCampos(){
    //Muestra imagen fija
    if(!ui->m_PlantEnabled->isChecked()){
        QLabel *dummy=ShowMensaje("Rendering");
        ui->m_AceptarButton->setEnabled(false);
        memoryMap->m_mapa.m_variables.XinicialPlantilla=ui->m_Xinicial->toPlainText().toDouble();
        memoryMap->m_mapa.m_variables.YinicialPlantilla=ui->m_Yinicial->toPlainText().toDouble();
        memoryMap->m_mapa.m_variables.AnchoPlantilla=ui->m_Ancho->toPlainText().toDouble();
        memoryMap->m_mapa.m_variables.AltoPlantilla=ui->m_Alto->toPlainText().toDouble();

        memoryMap->m_mapa.m_variables.NumColores=ui->m_NumTotalColores->toPlainText().toInt();
        ActualizarControlesColor();

        CreaImagenMuestra();
        MuestraImagen(pixmapPlantilla);

        ui->m_AceptarButton->setEnabled(true);

        delete (dummy);
    }
    else{//Muestra plantilla
        ui->m_hsPlantAlto->setValue(ui->m_PlantAlto->text().toInt());
        ui->m_hsPlantAncho->setValue(ui->m_PlantAncho->text().toInt());
        ui->m_hsPlantColsDist->setValue(ui->m_PlantsColDist->text().toInt());
        ui->m_hsPlantOffsetX->setValue(ui->m_PlantOffsetX->text().toInt());
        ui->m_hsPlantOffsetY->setValue(ui->m_PlantOffsetY->text().toInt());
        sltRenderPlantilla();
    }

    //QMessageBox::warning(0,"Atención","Cambios realizados");
}


void DbFormatPlantilla::LaunchDbImpresion(){
	DbImpresion *ScrDbImpresion;
	ScrDbImpresion=new DbImpresion(0);
	ScrDbImpresion->show();
	this->deleteLater();
}



//Crea cada una de las imagenes que forma la impresion
void DbFormatPlantilla::CreaImagenMuestra(){

	Convert();
	
    pixmapPlantilla.load(memoryMap->m_mapa.m_variables.NombreRutaBmp);


}


//Genera los troquelados...AQUI HAY QUE GENERAR LOS BMPS!!!
void DbFormatPlantilla::sltRenderPlantilla(){

    float Totalwidth=ui->m_PlantWidth->text().toFloat()*FACTOR_PIXELS_MM;
    float Totalheight=ui->m_PlantHeight->text().toFloat()*FACTOR_PIXELS_MM;
    float width=ui->m_PlantAncho->text().toFloat()*FACTOR_PIXELS_MM;
    float height=ui->m_PlantAlto->text().toFloat()*FACTOR_PIXELS_MM;
    float offsetx=ui->m_PlantOffsetX->text().toFloat()*FACTOR_PIXELS_MM;
    float offsety=ui->m_PlantOffsetY->text().toFloat()*FACTOR_PIXELS_MM;
    float sepx=ui->m_PlantsColDist->text().toFloat()*FACTOR_PIXELS_MM;

    QPixmap RealPixmap(Totalwidth,Totalheight);
    QPainter p(&RealPixmap);   // create a QPainter for it
    RealPixmap.fill(Qt::white);
    int num=ui->m_PlantCols->text().toInt();
    for (int x=0;x<num;x++){
        float distx=offsetx+(width+sepx)*x;
        float disty=offsety;
        QImage img=sltPlantGetImage(0,x);
        if (!img.isNull()) {
            p.drawPixmap(distx,disty,QPixmap::fromImage(img));

        }
        p.drawRect(distx,disty,width,height);
    }
    //ui->m_ImagenFormat->setPixmap(pic);
    p.end();
    MuestraImagen(RealPixmap);
}

//Devuelve la imagen xy
QImage DbFormatPlantilla::sltPlantGetImage(int x,int y){
    QImage rtnImage;
    QTableWidgetItem *p=ui->m_twPlantImages->item(x,y);
    if (p){
        QString fileName=p->text();
        if (!fileName.isEmpty()){
            //Cogemos directamente la imagen
            QImage image (fileName,"BMP");
            rtnImage=image.copy(0,0,ui->m_PlantAncho->text().toInt()*FACTOR_PIXELS_MM,ui->m_PlantAlto->text().toInt()*FACTOR_PIXELS_MM);
        }
    }
    return rtnImage;
}

//Muestra la imagen central
void DbFormatPlantilla::MuestraImagen(QPixmap Imagen){
    int lw=ui->m_ImagenFormat->width();
    int lh=ui->m_ImagenFormat->height();
    int iw=Imagen.width();
    int ih=Imagen.height();

    if ((lw==0)||(lh==0)) return;

    m_escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

    QPixmap pic(ui->m_ImagenFormat->size());
	QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);
    QPixmap dummy;

    switch (m_escalado){
    case IMAGEN_SINESCALAR:
        dummy=Imagen;
        offset_x_plantilla=(ui->m_ImagenFormat->width()-dummy.width())/2;
        offset_y_plantilla=(ui->m_ImagenFormat->height()-dummy.height())/2;
        p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
        ui->m_ImagenFormat->setOriginalPixmap(Imagen);
        ui->m_ImagenFormat->setPixmap((QPixmap)(pic));

        break;
    case IMAGEN_S2W:
        dummy=Imagen.scaledToWidth(ui->m_ImagenFormat->width(),Qt::SmoothTransformation);
        offset_x_plantilla=(ui->m_ImagenFormat->width()-dummy.width())/2;
        offset_y_plantilla=(ui->m_ImagenFormat->height()-dummy.height())/2;
        p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
        ui->m_ImagenFormat->setOriginalPixmap(Imagen);
        ui->m_ImagenFormat->setPixmap((QPixmap)(pic));
        break;
    case IMAGEN_S2H:
        dummy=Imagen.scaledToHeight(ui->m_ImagenFormat->height(),Qt::SmoothTransformation);
        offset_x_plantilla=(ui->m_ImagenFormat->width()-dummy.width())/2;
        offset_y_plantilla=(ui->m_ImagenFormat->height()-dummy.height())/2;
        p.drawPixmap(offset_x_plantilla,offset_y_plantilla,dummy); // and draw your original pixmap on it
        ui->m_ImagenFormat->setOriginalPixmap(Imagen);
        ui->m_ImagenFormat->setPixmap((QPixmap)(pic));
        break;
    }

    p.end();

    ui->m_ImagenFormat->show();
}


void DbFormatPlantilla::BuscarPlantilla(){

    ui->m_AceptarButton->setEnabled(false);

	QFileDialog fileName(this);
	QString tmp;
	QStringList Ficheros;
			
	fileName.setFileMode(QFileDialog::ExistingFile);
	fileName.setViewMode(QFileDialog::List);
	fileName.setFilter(tr("PDF Files (*.pdf)"));
	QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
	foreach (QPushButton *w, widgets){
			w->setAutoDefault(false);
	 }

	if (fileName.exec()){
		Ficheros=fileName.selectedFiles();
		if (!Ficheros.isEmpty()){
            QLabel *dummy =ShowMensaje("Cargando...");
            QString tmp;
            tmp=(QString)Ficheros.at(0);
            memoryMap->m_mapa.m_variables.FilasPlantilla=memoryMap->m_Imagen.height();
            memoryMap->m_mapa.m_variables.ColumnasPlantilla=memoryMap->m_Imagen.width();

            QString ruta,nombre;

            tmp.replace("/","\\");
            nombre=DevuelveNombre(tmp);
            ruta=DevuelveRuta(tmp);

            //Nombre/Ruta del pdf
            memoryMap->m_mapa.m_variables.NombreRutaPdf=tmp;
            memoryMap->m_mapa.m_variables.NombrePdf=nombre;
            memoryMap->m_mapa.m_variables.RutaPdf=ruta;

            ui->m_RutaPlantilla->setText(tmp);
            delete (dummy);

            AceptarValoresCampos();


		}
	}

    ui->m_AceptarButton->setEnabled(true);

}


//void DbFormatPlantilla::mouseDoubleClickEvent(QMouseEvent * event){
//}

void DbFormatPlantilla::ActualizarColor(int x, int y){

    /*if ((memoryMap->m_mapa.m_variables.NumColores>1)||(ui->m_ColorMascara->frameShadow()==QFrame::Plain)){

        UpdateMemoriaColorClicked();
    }*/
    UpdateMemoriaColorClicked();

}

/** Actualiza seleccion de color a partir de un click*/
void DbFormatPlantilla::UpdateMemoriaColorClicked(){

    int indice_color;
    QRgb rgbSelectedColor;
    rgbSelectedColor=ui->m_ImagenFormat->SelectedPixelColor();
    //Buscamos la primera ocurrencia de color en la imagen y la guardamos
    QImage dummyImage(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP");
    int exitFlag=0;
    int x=0,y=0;
    for (x=0;x<dummyImage.width();x++){
        if (exitFlag) break;
        for (y=0;y<dummyImage.height();y++){
            if (rgbSelectedColor==dummyImage.pixel(x,y)){
                exitFlag=1;
                break;
            }
        }
    }
    //Está seleccionado el color de máscara
    //if (ui->m_ColorMascara->frameShadow()==QFrame::Plain){
    if (ui->m_cbMascara->isChecked()){
        memoryMap->m_mapa.m_ColorMascara.valor=rgbSelectedColor;
        memoryMap->m_mapa.m_ColorMascara.x_pix=x;
        memoryMap->m_mapa.m_ColorMascara.y_pix=y;
        QPalette paleta_color_pleno;
        paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_ColorMascara.valor));
        ui->m_ColorMascara->setPalette(paleta_color_pleno);
    }
    else{
        memoryMap->m_mapa.m_ColorMascara.x_pix=DEFAULT_NOMASK_PIXEL;
        memoryMap->m_mapa.m_ColorMascara.y_pix=DEFAULT_NOMASK_PIXEL;
        indice_color=ui->m_NumColor->currentIndex();
        memoryMap->m_mapa.m_colores[indice_color].valor=rgbSelectedColor;
        memoryMap->m_mapa.m_colores[indice_color].x_pix=x;
        memoryMap->m_mapa.m_colores[indice_color].y_pix=y;
        ui->m_XColor->setText(QString("%1").arg(x));
        ui->m_YColor->setText(QString("%1").arg(y));
        QPalette paleta_color_pleno;
        paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[indice_color].valor));
        ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);
    }
}

/** Cargamos datos comunes*/
void DbFormatPlantilla::setDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=this->ui->m_ImagenFormat;
    dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
    dummyDataCommon.lbl_EstadoLampara=ui->lblLamp;
    dummyDataCommon.lbl_EstadoMaquina=ui->lblMaquina;
    dummyDataCommon.lbl_EstadoTemperatura=ui->lblFluid;

    DataCommon=new InkjetCommon(this,dummyDataCommon);

//Actualizamos
    DataCommon->ShowInfo();
    DataCommon->ShowEstadoImpresora();
    DataCommon->ShowComms();

}

void DbFormatPlantilla::ActualizarControlesColor(){

    /*if (memoryMap->m_mapa.m_variables.NumColores==1){
        ui->lbl_Color->setVisible(false);
        ui->m_NumColor->setVisible(false);
        ui->lbl_XColor->setVisible(false);
        ui->lbl_YColor->setVisible(false);
        ui->m_XColor->setVisible(false);
        ui->m_YColor->setVisible(false);
        ui->m_ColorSeleccionado->setVisible(false);

        memoryMap->m_mapa.m_variables.MaculaIzqdaColor=1;
        memoryMap->m_mapa.m_variables.MaculaDchaColor=1;

        for (int x=0;x<NUM_CAMPOS;x++){
            memoryMap->m_mapa.m_campos[x].color=1;
        }

        for (int x=0;x<NUM_CAMPOS_VARIABLES;x++){
            memoryMap->m_mapa.m_camposvariables[x].color=1;
        }
    }
    else{
        ui->lbl_Color->setVisible(true);
        ui->m_NumColor->setVisible(true);
        ui->lbl_XColor->setVisible(true);
        ui->lbl_YColor->setVisible(true);
        ui->m_XColor->setVisible(true);
        ui->m_YColor->setVisible(true);
        ui->m_ColorSeleccionado->setVisible(true);
        int indice_actual=ui->m_NumColor->currentIndex();
        ui->m_NumColor->clear();
        for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
            ui->m_NumColor->addItem(QString("Color %1").arg(i+1));
        }
        ui->m_NumColor->setCurrentIndex(indice_actual);
    }*/

    ui->lbl_Color->setVisible(true);
    ui->m_NumColor->setVisible(true);
    ui->lbl_XColor->setVisible(true);
    ui->lbl_YColor->setVisible(true);
    ui->m_XColor->setVisible(true);
    ui->m_YColor->setVisible(true);
    ui->m_ColorSeleccionado->setVisible(true);
    int indice_actual=ui->m_NumColor->currentIndex();
    ui->m_NumColor->clear();
    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        ui->m_NumColor->addItem(QString("Color %1").arg(i+1));
    }
    ui->m_NumColor->setCurrentIndex(indice_actual);

}
