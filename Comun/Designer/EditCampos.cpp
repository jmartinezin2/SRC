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

#include "EditCampos.h"
#include "Composicion.h"
#include "in2defines.h"
//#include "principal.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
//#include "principal.h"
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
#include <QLocale>
#include "In2Barcode.h"
#include "formatBarcode.h"
#include "formatCounter.h"
#include "formatDateTime.h"
#include "formatRandom.h"
#include "in2imagecreator.h"
//#include "dbgmem.h"
#include "counterStyle.h"
#include "inkjetdebug.h"
#include "in2fieldpixmapitem.h"

#define POS_LABEL_W 50
#define POS_LABEL_H 25


struct m_Campos m_campos_temp[NUM_CAMPOS];
struct m_CamposVariables m_camposvariables_temp[NUM_CAMPOS_VARIABLES];
static unsigned char NumeroEstiloFuente;



#define MOVE_STEP 0.1

#define BACKGROUND_ID 0x0

EditCampos::EditCampos(QWidget *parent)
    : ui(new Ui::EditCamposUI)
{
    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    m_memoryMap=MemoryMap::request(0);
    ui->setupUi(this);
    GlobalSetScreenSize(this);
    ui->m_NombreCampo->setReadOnly(true);
    ui->m_TipoCampo->setCurrentIndex(0);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_TEXTO);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_BARCODE);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_IDEOGRAMA);
    //ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_CONTADOR);
    //m_TipoCampo->setVisible(false);
    //escalado=IMAGEN_SINESCALAR;
    offset_x=0;
    offset_y=0;
    m_margin=0;



    //if (m_memoryMap->ImagenVariable==0x00) ui->m_TipoCampo->setVisible(false);

    for (int j=0;j<NUM_CAMPOS;j++){
        QString tmp_j=QString::number(j+1,10);
        ui->m_NumeroCampo->addItem("Campo "+tmp_j);
    }

    /*Con campos variables*/
    if (m_memoryMap->ImagenVariable==0x01){
        for (int j=0;j<NUM_CAMPOS_VARIABLES;j++){
            QString tmp_j=QString::number(j+1,10);
            ui->m_NumeroCampo->addItem("Variable "+tmp_j);
        }
    }

    ui->m_NumeroCampo->setCurrentIndex(0);
    ui->m_NumeroCampo->setFocus();

    for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        QString tmp_j=QString::number(j+1,10);
        ui->m_NumeroMatriz->addItem("Matriz "+tmp_j);
    }
    ui->m_NumeroMatriz->setCurrentIndex(0);

    ui->m_ColorSeleccionado->setReadOnly(true);

    for (int i=0;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
        ui->m_ColorCampo->addItem(QString("%1").arg(i+1)); 
    }
    ui->m_ColorCampo->setCurrentIndex((m_memoryMap->m_mapa.m_campos[0].color)-1);

    ui->m_Fuente->setEditable(false);
    ui->m_Estilo->setEditable(false);


    if (m_memoryMap->m_mapa.m_variables.NumColores==1){
        ui->m_ColorCampo->setVisible(false);
        ui->m_ColorSeleccionado->setVisible(false);

    }

    NumeroEstiloFuente=EstiloFuente.count();
    ui->m_Estilo->clear();
    QString dummy;
    for (int i=0;i<NumeroEstiloFuente;i++){
        dummy=EstiloFuente.at(i);
        ui->m_Estilo->addItem(dummy);
    }


    ui->m_ColorCampo->setPalette(dummyPaletteEnabled);
    ui->m_CampoHorizontal->setPalette(dummyPaletteEnabled);
    ui->m_CampoVertical->setPalette(dummyPaletteEnabled);
    ui->m_Tamano->setPalette(dummyPaletteEnabled);
    ui->m_Fuente->setPalette(dummyPaletteEnabled);
    ui->m_Estilo->setPalette(dummyPaletteEnabled);
    ui->m_Orientacion->setPalette(dummyPaletteEnabled);
    ui->m_FilasMatriz->setPalette(dummyPaletteEnabled);
    ui->m_ColumnasMatriz->setPalette(dummyPaletteEnabled);
    ui->m_FrecuenciaFilasMatriz->setPalette(dummyPaletteEnabled);
    ui->m_FrecuenciaColumnasMatriz->setPalette(dummyPaletteEnabled);
    ui->m_ExcepcionesMatriz->setPalette(dummyPaletteEnabled);

    m_OrdenCapas.resize((NUM_CAMPOS+NUM_CAMPOS_VARIABLES)*NUM_MATRIZ_POR_CAMPO);
    m_OrdenCapas.fill(NULL);

    UpdateNumeroCampo();

    PasarDeMapATemp();
    if (m_memoryMap->ImagenVariable==1) PasarDeMapATempVariables();



    QTimer::singleShot(0,this,SLOT(MuestraImagen()));


    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccionAndExit()));
    connect(ui->m_CancelButton,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));

    connect(ui->m_NumeroCampo,SIGNAL(activated (QString)),this,SLOT(UpdateNumeroCampo()));
    connect(ui->m_NumeroMatriz,SIGNAL(activated (QString)),this,SLOT(UpdateNumeroMatriz()));
    connect(ui->m_ColorCampo,SIGNAL(activated (QString)),this,SLOT(UpdateColorCampo()));
    connect(ui->m_MatrizOnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMatriz()));
    connect(ui->m_TipoCampo,SIGNAL(activated (QString)),this,SLOT(UpdateTipoCampo()));

    #ifdef CON_TECLADOVIRTUAL
    /*connect(ui->m_CampoHorizontal,SIGNAL(selectionChanged()),this,SLOT(UpdateCampoHorizontal()));
    connect(ui->m_CampoVertical,SIGNAL(selectionChanged()),this,SLOT(UpdateCampoVertical()));
    connect(ui->m_Tamano,SIGNAL(selectionChanged()),this,SLOT(UpdateTamanoFuente()));
    connect(ui->m_Orientacion,SIGNAL(selectionChanged()),this,SLOT(UpdateOrientacionFuente()));
    connect(ui->m_FilasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateFilasMatriz()));
    connect(ui->m_ColumnasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateColumnasMatriz()));
    connect(ui->m_FrecuenciaFilasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateFrecuenciaFilasMatriz()));
    connect(ui->m_FrecuenciaColumnasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateFrecuenciaColumnasMatriz()));
    connect(ui->m_ExcepcionesMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateExcepcionesMatriz()));
    connect(ui->m_ValorCampo,SIGNAL(selectionChanged()),this,SLOT(UpdateValorCampo()));*/
    #endif

    connect(ui->m_AceptarValores,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));
    connect(ui->m_MoveDown,SIGNAL(clicked()),this,SLOT(sltMoveDown()));
    connect(ui->m_MoveUp,SIGNAL(clicked()),this,SLOT(sltMoveUp()));
    connect(ui->m_MoveLeft,SIGNAL(clicked()),this,SLOT(sltMoveLeft()));
    connect(ui->m_MoveRight,SIGNAL(clicked()),this,SLOT(sltMoveRight()));
    //qDebug() << "OnOffCampo1: " << m_memoryMap->m_mapa.m_campos[0].OnOffCampo;
    connect(ui->m_pbGridMas,SIGNAL(clicked()),this,SLOT(sltGridMas()));
    connect(ui->m_pbGridMenos,SIGNAL(clicked()),this,SLOT(sltGridMenos()));

    connect(ui->m_ConfigTipo,SIGNAL(clicked()),this,SLOT(ConfigurarElemento()));

    connect(ui->m_ConfigOrigen,SIGNAL(clicked()),this,SLOT(ConfigurarOrigen()));

    if (m_memoryMap->m_mapa.m_campos[0].OnOffCampo==0x00){
        ui->m_NombreCampo->setEnabled(false);
        ui->m_NombreCampo->setText("");
        ui->m_Tamano->setEnabled(false);
        ui->m_Fuente->setEnabled(false);
        ui->m_Estilo->setEnabled(false);
        ui->m_Orientacion->setEnabled(false);
        //ui->m_AceptarValores->setEnabled(false);
        ui->m_ColorCampo->setEnabled(false);
        ui->m_ColorSeleccionado->setPalette(dummyPaletteDisabled);
    }

    //qDebug() << "OnOffCampo1: " << m_memoryMap->m_mapa.m_campos[0].OnOffCampo;

    if (m_memoryMap->m_mapa.m_campos[0].m_matrices[0].OnOffMatriz==1){
        ui->m_MatrizOnOff->setChecked(true);

    }
    else{
        ui->m_MatrizOnOff->setChecked(false);
    }

    UpdateOnOffMatriz();
    TestRoot();
    //InkjetDebug *info=InkjetDebug::request(0);
    //connect(this, SIGNAL(SendDebugInfo(unsigned char,QString,bool)),info, SLOT(ProcessMsg(unsigned char,QString,bool)));
    //connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
    connect(ui->m_MoveItem,SIGNAL(clicked()),this,SLOT(sltMoveItem()));
    connect(ui->m_showGrid,SIGNAL(clicked()),this,SLOT(sltShowGrid()));
    //connect(this,SIGNAL(repaintIntem(int)),this,SLOT(drawItem(int)));
    ui->tabWidget->setCurrentIndex(0);
    QTimer::singleShot(0,this,SLOT(sltShowGrid()));

}
/** Muestra/Oculta el grid*/
void EditCampos::sltShowGrid(){
    bool enable=ui->m_showGrid->isChecked();
    m_Scene->enableGrid(enable);

}

void EditCampos::UpdateComposition(const QPoint &point){

    AceptarValoresCampos();
}

/** Generamos un item para mover*/
void EditCampos::sltMoveItem(){
    int indice_matriz=ui->m_NumeroMatriz->currentIndex();
    if (ui->m_MatrizOnOff->isChecked()==true){
        //AceptarValoresCampos();
        //AceptarSeleccion();
        int i=ui->m_NumeroCampo->currentIndex();
        ActualizaDatosItem();
        drawItem(i,indice_matriz);
    }
    else QMessageBox::warning(0,"Warning",tr("Item no habilitado"),QMessageBox::Ok);
}

/** Posiciona un item por capas*/
void EditCampos::drawItem(int i,int matriz){

    In2ImageCreator p;
    if (i<NUM_CAMPOS){        
        m_campos_temp[i].ValorActualCampo=ui->m_ValorCampo->text();
           PasarDeTempAMap(i);
            QPixmap tmp=p.DrawFixedItem(i,matriz);
            In2FieldPixmapItem *pixmap=m_OrdenCapas.at(i*NUM_MATRIZ_POR_CAMPO+matriz);
            if (pixmap==NULL){ //Es la primera vez..añado el pixmap
                //pixmap=(In2GraphicsPixmapItem*)m_Scene->addPixmap(tmp);
                In2FieldPixmapItem *pixmap=new In2FieldPixmapItem(0,i*NUM_MATRIZ_POR_CAMPO+matriz);
                connect(pixmap,SIGNAL(sgnFieldPressed(int)),this,SLOT(UpdateNumeroCampo(int)));
                connect(pixmap,SIGNAL(sgnFieldReleased(int)),this,SLOT(sltItemSelected(int)));

                //pixmap->setStyle(CUT_STYLE);
                pixmap->setPixmap(tmp);
                m_Scene->addItem(pixmap);
                pixmap->setFlag(QGraphicsItem::ItemIsMovable, true);
                //pixmap->setFlag(QGraphicsItem::ItemIsSelectable, true);
                double x=m_campos_temp[i].m_matrices[matriz].PosicionXCampo;
                double y=m_campos_temp[i].m_matrices[matriz].PosicionYCampo;

                //El pixmap viene en la posicion (0,0)...lo muevo
                /*if ((x==0)&&(y==0)){
                    y=i*25*FACTOR_PIXELS_MM;
                    m_campos_temp[i].m_matrices[matriz].PosicionYCampo=i*25;
                } */
                pixmap->setPos(x,y);
                m_OrdenCapas.replace(i*NUM_MATRIZ_POR_CAMPO+matriz,pixmap);

            }
            else{
                pixmap->setPixmap(tmp); //Aqui sólo hace falta reemplazar
                double x=m_campos_temp[i].m_matrices[matriz].PosicionXCampo;
                double y=m_campos_temp[i].m_matrices[matriz].PosicionYCampo;
                if ((x==0)&&(y==0)){
                    y=i*10;
                    m_campos_temp[i].m_matrices[matriz].PosicionYCampo=i*10;
                }
                pixmap->setPos(x,y);
            }
            PasarDeMapATemp(i);

    }
    else{
        PasarDeTempAMapVariables(i-NUM_CAMPOS);
        QPixmap tmp=p.DrawVariableItem(i-NUM_CAMPOS,matriz);
        //QGraphicsPixmapItem *pixmap=m_Scene->addPixmap(tmp);
        In2FieldPixmapItem *pixmap=m_OrdenCapas.at(i*NUM_MATRIZ_POR_CAMPO+matriz);
        if (pixmap==NULL){ //Es la primera vez..añado el pixmap
            In2FieldPixmapItem *pixmap=new In2FieldPixmapItem(0,i*NUM_MATRIZ_POR_CAMPO+matriz);
            connect(pixmap,SIGNAL(sgnFieldPressed(int)),this,SLOT(ActualizaDatosItem(int)));
            connect(pixmap,SIGNAL(sgnFieldReleased(int)),this,SLOT(sltItemSelected(int)));

            pixmap->setPixmap(tmp);
            m_Scene->addItem(pixmap);            
            pixmap->setFlag(QGraphicsItem::ItemIsMovable, true);
            //pixmap->setFlag(QGraphicsItem::ItemIsSelectable, true);
            double x=m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[matriz].PosicionXCampo;
            double y=m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[matriz].PosicionYCampo;
            pixmap->setPos(x,y);
                //El pixmap viene en la posicion (0,0)...lo muevo
            m_OrdenCapas.replace(i*NUM_MATRIZ_POR_CAMPO+matriz,pixmap);
            m_Scene->update(m_Scene->sceneRect());

        }
        else{
            pixmap->setPixmap(tmp); //Aqui sólo hace falta reemplazar
            double x=m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[matriz].PosicionXCampo;
            double y=m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[matriz].PosicionYCampo;
            pixmap->setPos(x,y);
        }
        PasarDeMapATempVariables(i-NUM_CAMPOS);

    }
}

/** Actualiza la posición del cursor*/
QPoint EditCampos::UpdateMousePos(const QPointF &p ){
    QPoint foo;
    //m_margin=0;
    foo.setX(int((p.x()-m_margin)));
    foo.setY(int((p.y()-m_margin)));
    /*ui->m_CampoHorizontal->setText(QString("%1").arg(foo.x()));
    ui->m_CampoVertical->setText(QString("%1").arg(foo.y()));*/
    ui->m_labelPos->setText(QString("%1x%2").arg(foo.x()).arg(foo.y()));

    return foo;
}

void EditCampos::TestRoot(){
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_CampoHorizontal->setEnabled(false);
        ui->m_CampoVertical->setEnabled(false);
        ui->m_Tamano->setEnabled(false);
        ui->m_Fuente->setEnabled(false);
        ui->m_Estilo->setEnabled(false);
        ui->m_FilasMatriz->setEnabled(false);
        ui->m_ColumnasMatriz->setEnabled(false);
        ui->m_FrecuenciaFilasMatriz->setEnabled(false);
        ui->m_FrecuenciaColumnasMatriz->setEnabled(false);
        ui->m_ExcepcionesMatriz->setEnabled(false);
        ui->m_Orientacion->setEnabled(false);
        ui->m_AceptarValores->setEnabled(false);
        ui->m_ColorCampo->setEnabled(false);
        ui->m_NumeroMatriz->setEnabled(false);
    }
}

EditCampos::~EditCampos(){
}


void EditCampos::PasarDeMapATemp(){
    for (int i=0;i<NUM_CAMPOS;i++){
        PasarDeMapATemp(i);
    }
   /* for (int i=0;i<NUM_CAMPOS;i++){
        m_campos_temp[i].NombreCampo=m_memoryMap->m_mapa.m_campos[i].NombreCampo;
        m_campos_temp[i].ValorActualCampo=m_memoryMap->m_mapa.m_campos[i].ValorActualCampo;
        m_campos_temp[i].OnOffCampo=m_memoryMap->m_mapa.m_campos[i].OnOffCampo;
        m_campos_temp[i].TamanoFuenteCampo=m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo;
        m_campos_temp[i].TipoLetraCampo=m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo;
        m_campos_temp[i].EstiloLetraCampo=m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo;
        m_campos_temp[i].m_barcodeParameters=m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
        m_campos_temp[i].m_dateTimeParameters=m_memoryMap->m_mapa.m_campos[i].m_dateTimeParameters;


        m_campos_temp[i].color=m_memoryMap->m_mapa.m_campos[i].color;
        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            m_campos_temp[i].m_matrices[j].OnOffMatriz=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz;
            m_campos_temp[i].m_matrices[j].PosicionXCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo;
            m_campos_temp[i].m_matrices[j].PosicionYCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo;
            m_campos_temp[i].m_matrices[j].OrientacionCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
            m_campos_temp[i].m_matrices[j].FilasMatrizCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;
            m_campos_temp[i].m_matrices[j].ColumnasMatrizCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;
            m_campos_temp[i].m_matrices[j].FreqFilasMatrizCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo;
            m_campos_temp[i].m_matrices[j].FreqColumnasMatrizCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo;
            m_campos_temp[i].m_matrices[j].ExcepcionesMatrizCampo=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo;
            m_campos_temp[i].m_matrices[j].incColumnas=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas;
            m_campos_temp[i].m_matrices[j].incFilas=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas;

        }
    }*/

}

void EditCampos::PasarDeMapATempVariables(){
    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        PasarDeMapATempVariables(i);
    }

   /* for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        m_camposvariables_temp[i].NombreCampo=m_memoryMap->m_mapa.m_camposvariables[i].NombreCampo;
        m_camposvariables_temp[i].TipoCampo=m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo;
        m_camposvariables_temp[i].ValorActualCampo=m_memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo;
        m_camposvariables_temp[i].OnOffCampo=m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo;
        m_camposvariables_temp[i].TamanoFuenteCampo=m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo;
        m_camposvariables_temp[i].TipoLetraCampo=m_memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo;
        m_camposvariables_temp[i].EstiloLetraCampo=m_memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo;
        m_camposvariables_temp[i].color=m_memoryMap->m_mapa.m_camposvariables[i].color;
        m_camposvariables_temp[i].m_barcodeParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
        m_camposvariables_temp[i].m_dateTimeParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters;
        m_camposvariables_temp[i].m_counterParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_counterParameters;
        m_camposvariables_temp[i].m_randomNumberParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters;

        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            m_camposvariables_temp[i].m_matrices[j].OnOffMatriz=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz;
            m_camposvariables_temp[i].m_matrices[j].PosicionXCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo;
            m_camposvariables_temp[i].m_matrices[j].PosicionYCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo;
            m_camposvariables_temp[i].m_matrices[j].OrientacionCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
            m_camposvariables_temp[i].m_matrices[j].FilasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].ColumnasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].FreqFilasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].FreqColumnasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].incColumnas=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas;
            m_camposvariables_temp[i].m_matrices[j].incFilas=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas;

        }
    }*/
}

void EditCampos::PasarDeMapATemp(int i){
    m_campos_temp[i]=m_memoryMap->m_mapa.m_campos[i];
    /*m_campos_temp[i].NombreCampo=m_memoryMap->m_mapa.m_campos[i].NombreCampo;
    m_campos_temp[i].ValorActualCampo=m_memoryMap->m_mapa.m_campos[i].ValorActualCampo;
    m_campos_temp[i].OnOffCampo=m_memoryMap->m_mapa.m_campos[i].OnOffCampo;
    m_campos_temp[i].TamanoFuenteCampo=m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo;
    m_campos_temp[i].TipoLetraCampo=m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo;
    m_campos_temp[i].EstiloLetraCampo=m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo;
    m_campos_temp[i].m_barcodeParameters=m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
    m_campos_temp[i].m_dateTimeParameters=m_memoryMap->m_mapa.m_campos[i].m_dateTimeParameters;*/

}


/** por indice*/
void EditCampos::PasarDeMapATempVariables(int i){
   m_camposvariables_temp[i]=m_memoryMap->m_mapa.m_camposvariables[i];
    /* m_camposvariables_temp[i].NombreCampo=m_memoryMap->m_mapa.m_camposvariables[i].NombreCampo;
    m_camposvariables_temp[i].TipoCampo=m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo;
    m_camposvariables_temp[i].ValorActualCampo=m_memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo;
    m_camposvariables_temp[i].OnOffCampo=m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo;
    m_camposvariables_temp[i].TamanoFuenteCampo=m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo;
    m_camposvariables_temp[i].TipoLetraCampo=m_memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo;
    m_camposvariables_temp[i].EstiloLetraCampo=m_memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo;
    m_camposvariables_temp[i].color=m_memoryMap->m_mapa.m_camposvariables[i].color;
    m_camposvariables_temp[i].m_barcodeParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
    m_camposvariables_temp[i].m_dateTimeParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters;
    m_camposvariables_temp[i].m_counterParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_counterParameters;
    m_camposvariables_temp[i].m_randomNumberParameters=m_memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters;
    for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        m_camposvariables_temp[i].m_matrices[j].OnOffMatriz=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz;
        m_camposvariables_temp[i].m_matrices[j].PosicionXCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo;
        m_camposvariables_temp[i].m_matrices[j].PosicionYCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo;
        m_camposvariables_temp[i].m_matrices[j].OrientacionCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
        m_camposvariables_temp[i].m_matrices[j].FilasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
        m_camposvariables_temp[i].m_matrices[j].ColumnasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;
        m_camposvariables_temp[i].m_matrices[j].FreqFilasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo;
        m_camposvariables_temp[i].m_matrices[j].FreqColumnasMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo;
        m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo;
        m_camposvariables_temp[i].m_matrices[j].incColumnas=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas;
        m_camposvariables_temp[i].m_matrices[j].incFilas=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas;
    }*/
}

/** Por indice*/
void EditCampos::PasarDeTempAMap(int i){
    m_memoryMap->m_mapa.m_campos[i]=m_campos_temp[i];
    /*m_memoryMap->m_mapa.m_campos[i].NombreCampo=m_campos_temp[i].NombreCampo;
    m_memoryMap->m_mapa.m_campos[i].ValorActualCampo=m_campos_temp[i].ValorActualCampo;

    m_memoryMap->m_mapa.m_campos[i].OnOffCampo=m_campos_temp[i].OnOffCampo;
    m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo=m_campos_temp[i].TamanoFuenteCampo;
    m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo=m_campos_temp[i].TipoLetraCampo;
    m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo=m_campos_temp[i].EstiloLetraCampo;

    m_memoryMap->m_mapa.m_campos[i].NumCaracteresCampo=m_campos_temp[i].NumCaracteresCampo;
    m_memoryMap->m_mapa.m_campos[i].color=m_campos_temp[i].color;
    m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters=m_campos_temp[i].m_barcodeParameters;
    m_memoryMap->m_mapa.m_campos[i].m_dateTimeParameters=m_campos_temp[i].m_dateTimeParameters;

    for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=m_campos_temp[i].m_matrices[j].OnOffMatriz;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=m_campos_temp[i].m_matrices[j].PosicionXCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=m_campos_temp[i].m_matrices[j].PosicionYCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=m_campos_temp[i].m_matrices[j].OrientacionCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=m_campos_temp[i].m_matrices[j].FilasMatrizCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=m_campos_temp[i].m_matrices[j].ColumnasMatrizCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=m_campos_temp[i].m_matrices[j].FreqFilasMatrizCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=m_campos_temp[i].m_matrices[j].FreqColumnasMatrizCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo=m_campos_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas=m_campos_temp[i].m_matrices[j].incColumnas;
        m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas=m_campos_temp[i].m_matrices[j].incFilas;

    }*/

}

void EditCampos::PasarDeTempAMapVariables(int i){
    m_memoryMap->m_mapa.m_camposvariables[i]=m_camposvariables_temp[i];
    /*m_memoryMap->m_mapa.m_camposvariables[i].NombreCampo=m_camposvariables_temp[i].NombreCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo=m_camposvariables_temp[i].TipoCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo=m_camposvariables_temp[i].ValorActualCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=m_camposvariables_temp[i].OnOffCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo=m_camposvariables_temp[i].TamanoFuenteCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo=m_camposvariables_temp[i].TipoLetraCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo=m_camposvariables_temp[i].EstiloLetraCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].NumCaracteresCampo=m_campos_temp[i].NumCaracteresCampo;
    m_memoryMap->m_mapa.m_camposvariables[i].color=m_camposvariables_temp[i].color;
    m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters=m_camposvariables_temp[i].m_barcodeParameters;
    m_memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters=m_camposvariables_temp[i].m_dateTimeParameters;
    m_memoryMap->m_mapa.m_camposvariables[i].m_counterParameters=m_camposvariables_temp[i].m_counterParameters;
    m_memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters=m_camposvariables_temp[i].m_randomNumberParameters;

    for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=m_camposvariables_temp[i].m_matrices[j].OnOffMatriz;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo=m_camposvariables_temp[i].m_matrices[j].PosicionXCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo=m_camposvariables_temp[i].m_matrices[j].PosicionYCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo=m_camposvariables_temp[i].m_matrices[j].OrientacionCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FilasMatrizCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ColumnasMatrizCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FreqFilasMatrizCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FreqColumnasMatrizCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas=m_camposvariables_temp[i].m_matrices[j].incColumnas;
        m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas=m_camposvariables_temp[i].m_matrices[j].incFilas;
    }*/
}



void EditCampos::PasarDeTempAMap(){
    for (int i=0;i<NUM_CAMPOS;i++){
        PasarDeTempAMap(i);
    }

    /*for (int i=0;i<NUM_CAMPOS;i++){
        m_memoryMap->m_mapa.m_campos[i].NombreCampo=m_campos_temp[i].NombreCampo;
        m_memoryMap->m_mapa.m_campos[i].ValorActualCampo=m_campos_temp[i].ValorActualCampo;

        m_memoryMap->m_mapa.m_campos[i].OnOffCampo=m_campos_temp[i].OnOffCampo;
        m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo=m_campos_temp[i].TamanoFuenteCampo;
        m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo=m_campos_temp[i].TipoLetraCampo;
        m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo=m_campos_temp[i].EstiloLetraCampo;

        m_memoryMap->m_mapa.m_campos[i].NumCaracteresCampo=m_campos_temp[i].NumCaracteresCampo;        
        m_memoryMap->m_mapa.m_campos[i].color=m_campos_temp[i].color;
        m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters=m_campos_temp[i].m_barcodeParameters;
        m_memoryMap->m_mapa.m_campos[i].m_dateTimeParameters=m_campos_temp[i].m_dateTimeParameters;

        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=m_campos_temp[i].m_matrices[j].OnOffMatriz;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=m_campos_temp[i].m_matrices[j].PosicionXCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=m_campos_temp[i].m_matrices[j].PosicionYCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=m_campos_temp[i].m_matrices[j].OrientacionCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=m_campos_temp[i].m_matrices[j].FilasMatrizCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=m_campos_temp[i].m_matrices[j].ColumnasMatrizCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=m_campos_temp[i].m_matrices[j].FreqFilasMatrizCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=m_campos_temp[i].m_matrices[j].FreqColumnasMatrizCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo=m_campos_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas=m_campos_temp[i].m_matrices[j].incColumnas;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas=m_campos_temp[i].m_matrices[j].incFilas;

        }
    }*/

}

void EditCampos::PasarDeTempAMapVariables(){
    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        PasarDeTempAMapVariables(i);
    }

    /*for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        m_memoryMap->m_mapa.m_camposvariables[i].NombreCampo=m_camposvariables_temp[i].NombreCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo=m_camposvariables_temp[i].TipoCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo=m_camposvariables_temp[i].ValorActualCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=m_camposvariables_temp[i].OnOffCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo=m_camposvariables_temp[i].TamanoFuenteCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo=m_camposvariables_temp[i].TipoLetraCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo=m_camposvariables_temp[i].EstiloLetraCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].NumCaracteresCampo=m_campos_temp[i].NumCaracteresCampo;
        m_memoryMap->m_mapa.m_camposvariables[i].color=m_camposvariables_temp[i].color;
        m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters=m_camposvariables_temp[i].m_barcodeParameters;
        m_memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters=m_camposvariables_temp[i].m_dateTimeParameters;
        m_memoryMap->m_mapa.m_camposvariables[i].m_counterParameters=m_camposvariables_temp[i].m_counterParameters;
        m_memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters=m_camposvariables_temp[i].m_randomNumberParameters;

        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=m_camposvariables_temp[i].m_matrices[j].OnOffMatriz;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo=m_camposvariables_temp[i].m_matrices[j].PosicionXCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo=m_camposvariables_temp[i].m_matrices[j].PosicionYCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo=m_camposvariables_temp[i].m_matrices[j].OrientacionCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FilasMatrizCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ColumnasMatrizCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FreqFilasMatrizCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FreqColumnasMatrizCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas=m_camposvariables_temp[i].m_matrices[j].incColumnas;
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas=m_camposvariables_temp[i].m_matrices[j].incFilas;

            //m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
        }
    }*/

}




//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void EditCampos::AceptarSeleccionAndExit(){

    //Volvemos hacia atrás
    PasarDeTempAMap();
    if (m_memoryMap->ImagenVariable==1) PasarDeTempAMapVariables();
    //AceptarSeleccion();
    LaunchComposicion();

}


void EditCampos::CancelarSeleccion(){
    /*PasarDeMapATemp();
    if (m_memoryMap->ImagenVariable==1) PasarDeMapATempVariables();*/

    LaunchComposicion();
}

void EditCampos::UpdateNumeroCampo(){
    int indice=0;
    unsigned char indice_matriz=0;
    unsigned char indice_color=0;
    int indice_selecc=0;
    QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    indice=ui->m_NumeroCampo->currentIndex();
    indice_matriz=ui->m_NumeroMatriz->currentIndex();
    ui->m_OrigenCampo->setText("");

    if (indice<0) return;

    if (indice<=NUM_CAMPOS-1){
        //Campos fijos
        ui->m_NombreCampo->setText(QString(m_campos_temp[indice].NombreCampo));
        ui->m_Fuente->setCurrentFont(QString(m_campos_temp[indice].TipoLetraCampo));
        ui->m_Tamano->setText(QString("%1").arg(m_campos_temp[indice].TamanoFuenteCampo));
        ui->m_ValorCampo->setText(QString("%1").arg(m_campos_temp[indice].ValorActualCampo));
        ui->m_leLetterSpace->setText(QString("%1").arg(m_campos_temp[indice].letterSpace));
        if (!m_campos_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_TEXTO)) ui->m_TipoCampo->setCurrentIndex(0);
        else if (!m_campos_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)) ui->m_TipoCampo->setCurrentIndex(1);
        else if (!m_campos_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_IDEOGRAMA)) ui->m_TipoCampo->setCurrentIndex(2);
        //else if (!m_memoryMap->m_mapa.m_campos[indice].TipoCampo.compare("Contador")) ui->m_TipoCampo->setCurrentIndex(4);
        UpdateTipoCampo();

        indice_selecc=ui->m_Estilo->findText(QString(m_campos_temp[indice].EstiloLetraCampo));
        indice_color=m_campos_temp[indice].color;
        ui->m_ColorCampo->setCurrentIndex(indice_color-1);
        ui->m_Estilo->setCurrentIndex(indice_selecc);
        QPalette paleta_color_pleno;
        paleta_color_pleno.setColor(QPalette::Base,QColor(m_memoryMap->m_mapa.m_colores[indice_color-1].valor));
        ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);

        if (m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
        else ui->m_MatrizOnOff->setChecked(false);
        ui->m_CampoHorizontal->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].PosicionXCampo));
        ui->m_CampoVertical->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].PosicionYCampo));

        ui->m_Orientacion->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].OrientacionCampo));
        ui->m_FilasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo));
        ui->m_ColumnasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo));
        ui->m_FrecuenciaFilasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo));
        ui->m_FrecuenciaColumnasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo));
        ui->m_ExcepcionesMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo));
        ui->m_IncCols->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].incColumnas));
        ui->m_IncFilas->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].incFilas));

        if (m_campos_temp[indice].OnOffCampo==0x00){
            ui->m_MatrizOnOff->setChecked(false);
            ui->m_CampoHorizontal->setText(" ");
            ui->m_CampoVertical->setText(" ");
            ui->m_FilasMatriz->setText(" ");
            ui->m_ColumnasMatriz->setText(" ");
            ui->m_FrecuenciaFilasMatriz->setText(" ");
            ui->m_FrecuenciaColumnasMatriz->setText(" ");
            ui->m_ExcepcionesMatriz->setText(" ");
            ui->m_IncCols->setText(" ");
            ui->m_IncFilas->setText(" ");
            ui->m_Orientacion->setText(" ");
            ui->m_CampoHorizontal->setEnabled(false);
            ui->m_CampoVertical->setEnabled(false);
            ui->m_Tamano->setText(" ");
            ui->m_Tamano->setEnabled(false);
            ui->m_Fuente->setEnabled(false);
            ui->m_Estilo->setEnabled(false);
            //ui->m_AceptarValores->setEnabled(false);
            ui->m_NombreCampo->setEnabled(false);
            ui->m_NombreCampo->setText("");
            ui->m_OrigenCampo->setEnabled(false);
            ui->m_OrigenCampo->setText("");
            ui->m_TipoCampo->setEnabled(false);
            ui->m_TipoCampo->setCurrentIndex(-1);
            ui->m_Orientacion->setEnabled(false);
            ui->m_FilasMatriz->setEnabled(false);
            ui->m_ColumnasMatriz->setEnabled(false);
            ui->m_FrecuenciaFilasMatriz->setEnabled(false);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(false);
            ui->m_ExcepcionesMatriz->setEnabled(false);
            ui->m_ColorCampo->setEnabled(false);
            ui->m_ColorSeleccionado->setPalette(dummyPaletteDisabled);
            ui->m_MatrizOnOff->setEnabled(false);
            ui->m_NumeroMatriz->setEnabled(false);
		}
		else{
            if (m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
            else ui->m_MatrizOnOff->setChecked(false);
            UpdateOnOffMatriz();
            ui->m_MatrizOnOff->setChecked(true);
            ui->m_CampoHorizontal->setEnabled(true);
            ui->m_CampoVertical->setEnabled(true);
            ui->m_Tamano->setEnabled(true);
            ui->m_Fuente->setEnabled(true);
            ui->m_Estilo->setEnabled(true);
            ui->m_NombreCampo->setEnabled(true);
            ui->m_OrigenCampo->setEnabled(false);
            ui->m_TipoCampo->setEnabled(true);
            ui->m_Orientacion->setEnabled(true);
            ui->m_FilasMatriz->setEnabled(true);
            ui->m_ColumnasMatriz->setEnabled(true);
            ui->m_FrecuenciaFilasMatriz->setEnabled(true);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(true);
            ui->m_ExcepcionesMatriz->setEnabled(true);
            ui->m_ColorCampo->setEnabled(true);
            ui->m_MatrizOnOff->setEnabled(true);
            ui->m_NumeroMatriz->setEnabled(true);
        }
    }
    else{
        //Campos variables
        indice=indice-NUM_CAMPOS;
        ui->m_NombreCampo->setText(QString(m_camposvariables_temp[indice].NombreCampo));
        ui->m_Fuente->setCurrentFont(QString(m_camposvariables_temp[indice].TipoLetraCampo));
        ui->m_Tamano->setText(QString("%1").arg(m_camposvariables_temp[indice].TamanoFuenteCampo));
        ui->m_OrigenCampo->setText(QString(m_camposvariables_temp[indice].OrigenValorCampo));
        ui->m_ValorCampo->setText(QString("%1").arg(m_camposvariables_temp[indice].ValorActualCampo));


        if (!m_camposvariables_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_TEXTO)) ui->m_TipoCampo->setCurrentIndex(0);
        else if (!m_camposvariables_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)) ui->m_TipoCampo->setCurrentIndex(1);
        else if (!m_camposvariables_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_IDEOGRAMA)) ui->m_TipoCampo->setCurrentIndex(2);
        //else if (!m_memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare("Contador")) ui->m_TipoCampo->setCurrentIndex(4);

        UpdateTipoCampo();

        indice_selecc=ui->m_Estilo->findText(QString(m_camposvariables_temp[indice].EstiloLetraCampo));
        ui->m_ColorCampo->setCurrentIndex((m_camposvariables_temp[indice].color)-1);
        ui->m_Estilo->setCurrentIndex(indice_selecc);

        /*if (int x=ui->m_TipoCampo->findText(m_memoryMap->m_mapa.m_camposvariables[indice].TipoCampo)!=-1)
            ui->m_TipoCampo->setCurrentIndex(x);*/

        if (m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
        else ui->m_MatrizOnOff->setChecked(false);
        ui->m_CampoHorizontal->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionXCampo));
        ui->m_CampoVertical->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionYCampo));
        ui->m_Orientacion->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].OrientacionCampo));
        ui->m_FilasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo));
        ui->m_ColumnasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo));
        ui->m_FrecuenciaFilasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo));
        ui->m_FrecuenciaColumnasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo));
        ui->m_ExcepcionesMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo));
        ui->m_IncCols->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].incColumnas));
        ui->m_IncFilas->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].incFilas));

        if (m_camposvariables_temp[indice].OnOffCampo==0x00){
            ui->m_MatrizOnOff->setChecked(false);
            ui->m_CampoHorizontal->setText(" ");
            ui->m_CampoVertical->setText(" ");
            ui->m_FilasMatriz->setText(" ");
            ui->m_ColumnasMatriz->setText(" ");
            ui->m_FrecuenciaFilasMatriz->setText(" ");
            ui->m_FrecuenciaColumnasMatriz->setText(" ");
            ui->m_ExcepcionesMatriz->setText(" ");
            ui->m_Orientacion->setText(" ");
            ui->m_CampoHorizontal->setEnabled(false);
            ui->m_CampoVertical->setEnabled(false);
            ui->m_Tamano->setText(" ");
            ui->m_Tamano->setEnabled(false);
            ui->m_Fuente->setEnabled(false);
            ui->m_Estilo->setEnabled(false);
            //ui->m_AceptarValores->setEnabled(false);
            ui->m_NombreCampo->setEnabled(false);
            ui->m_NombreCampo->setText("");
            ui->m_OrigenCampo->setEnabled(false);
            ui->m_OrigenCampo->setText("");
            ui->m_TipoCampo->setEnabled(false);
            ui->m_TipoCampo->setCurrentIndex(-1);
            ui->m_Orientacion->setEnabled(false);
            ui->m_FilasMatriz->setEnabled(false);
            ui->m_ColumnasMatriz->setEnabled(false);
            ui->m_FrecuenciaFilasMatriz->setEnabled(false);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(false);
            ui->m_ExcepcionesMatriz->setEnabled(false);
            ui->m_ColorCampo->setEnabled(false);
            ui->m_ColorSeleccionado->setPalette(dummyPaletteDisabled);
            ui->m_MatrizOnOff->setEnabled(false);
            ui->m_NumeroMatriz->setEnabled(false);
        }
        else{
            if (m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
            else ui->m_MatrizOnOff->setChecked(false);
            UpdateOnOffMatriz();
            ui->m_MatrizOnOff->setChecked(true);
            ui->m_CampoHorizontal->setEnabled(true);
            ui->m_CampoVertical->setEnabled(true);
            ui->m_Tamano->setEnabled(true);
            ui->m_Fuente->setEnabled(true);
            ui->m_Estilo->setEnabled(true);
            ui->m_NombreCampo->setEnabled(true);
            ui->m_OrigenCampo->setEnabled(true);
            ui->m_TipoCampo->setEnabled(true);
            ui->m_Orientacion->setEnabled(true);
            ui->m_FilasMatriz->setEnabled(true);
            ui->m_ColumnasMatriz->setEnabled(true);
            ui->m_FrecuenciaFilasMatriz->setEnabled(true);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(true);
            ui->m_ExcepcionesMatriz->setEnabled(true);
            ui->m_ColorCampo->setEnabled(true);
            ui->m_MatrizOnOff->setEnabled(true);
            ui->m_NumeroMatriz->setEnabled(true);
        }

    }
	TestRoot();
}

void EditCampos::UpdateNumeroCampo(int index){
    int indice=index;

    unsigned char indice_matriz=0;
    unsigned char indice_color=0;
    int indice_selecc=0;
    QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    //indice_matriz=ui->m_NumeroMatriz->currentIndex();
    indice_matriz=index%2;
    ui->m_OrigenCampo->setText("");

    if (indice<0) return;
    ui->m_NumeroCampo->setCurrentIndex(indice);
    ui->m_NumeroMatriz->setCurrentIndex(indice_matriz);
    if (indice<=NUM_CAMPOS-1){
        //Campos fijos
        ui->m_NombreCampo->setText(QString(m_campos_temp[indice].NombreCampo));
        ui->m_Fuente->setCurrentFont(QString(m_campos_temp[indice].TipoLetraCampo));
        ui->m_Tamano->setText(QString("%1").arg(m_campos_temp[indice].TamanoFuenteCampo));
        ui->m_ValorCampo->setText(QString("%1").arg(m_campos_temp[indice].ValorActualCampo));
        if (!m_campos_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_TEXTO)) ui->m_TipoCampo->setCurrentIndex(0);
        else if (!m_campos_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)) ui->m_TipoCampo->setCurrentIndex(1);
        else if (!m_campos_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_IDEOGRAMA)) ui->m_TipoCampo->setCurrentIndex(2);
        //else if (!m_memoryMap->m_mapa.m_campos[indice].TipoCampo.compare("Contador")) ui->m_TipoCampo->setCurrentIndex(4);
        UpdateTipoCampo();

        indice_selecc=ui->m_Estilo->findText(QString(m_campos_temp[indice].EstiloLetraCampo));
        indice_color=m_campos_temp[indice].color;
        ui->m_ColorCampo->setCurrentIndex(indice_color-1);
        ui->m_Estilo->setCurrentIndex(indice_selecc);
        QPalette paleta_color_pleno;
        paleta_color_pleno.setColor(QPalette::Base,QColor(m_memoryMap->m_mapa.m_colores[indice_color-1].valor));
        ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);

        if (m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
        else ui->m_MatrizOnOff->setChecked(false);
        ui->m_CampoHorizontal->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].PosicionXCampo));
        ui->m_CampoVertical->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].PosicionYCampo));

        ui->m_Orientacion->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].OrientacionCampo));
        ui->m_FilasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo));
        ui->m_ColumnasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo));
        ui->m_FrecuenciaFilasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo));
        ui->m_FrecuenciaColumnasMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo));
        ui->m_ExcepcionesMatriz->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo));
        ui->m_IncCols->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].incColumnas));
        ui->m_IncFilas->setText(QString("%1").arg(m_campos_temp[indice].m_matrices[indice_matriz].incFilas));

        if (m_campos_temp[indice].OnOffCampo==0x00){
            ui->m_MatrizOnOff->setChecked(false);
            ui->m_CampoHorizontal->setText(" ");
            ui->m_CampoVertical->setText(" ");
            ui->m_FilasMatriz->setText(" ");
            ui->m_ColumnasMatriz->setText(" ");
            ui->m_FrecuenciaFilasMatriz->setText(" ");
            ui->m_FrecuenciaColumnasMatriz->setText(" ");
            ui->m_ExcepcionesMatriz->setText(" ");
            ui->m_IncCols->setText(" ");
            ui->m_IncFilas->setText(" ");
            ui->m_Orientacion->setText(" ");
            ui->m_CampoHorizontal->setEnabled(false);
            ui->m_CampoVertical->setEnabled(false);
            ui->m_Tamano->setText(" ");
            ui->m_Tamano->setEnabled(false);
            ui->m_Fuente->setEnabled(false);
            ui->m_Estilo->setEnabled(false);
            //ui->m_AceptarValores->setEnabled(false);
            ui->m_NombreCampo->setEnabled(false);
            ui->m_NombreCampo->setText("");
            ui->m_OrigenCampo->setEnabled(false);
            ui->m_OrigenCampo->setText("");
            ui->m_TipoCampo->setEnabled(false);
            ui->m_TipoCampo->setCurrentIndex(-1);
            ui->m_Orientacion->setEnabled(false);
            ui->m_FilasMatriz->setEnabled(false);
            ui->m_ColumnasMatriz->setEnabled(false);
            ui->m_FrecuenciaFilasMatriz->setEnabled(false);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(false);
            ui->m_ExcepcionesMatriz->setEnabled(false);
            ui->m_ColorCampo->setEnabled(false);
            ui->m_ColorSeleccionado->setPalette(dummyPaletteDisabled);
            ui->m_MatrizOnOff->setEnabled(false);
            ui->m_NumeroMatriz->setEnabled(false);
        }
        else{
            if (m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
            else ui->m_MatrizOnOff->setChecked(false);
            UpdateOnOffMatriz();
            ui->m_MatrizOnOff->setChecked(true);
            ui->m_CampoHorizontal->setEnabled(true);
            ui->m_CampoVertical->setEnabled(true);
            ui->m_Tamano->setEnabled(true);
            ui->m_Fuente->setEnabled(true);
            ui->m_Estilo->setEnabled(true);
            ui->m_NombreCampo->setEnabled(true);
            ui->m_OrigenCampo->setEnabled(false);
            ui->m_TipoCampo->setEnabled(true);
            ui->m_Orientacion->setEnabled(true);
            ui->m_FilasMatriz->setEnabled(true);
            ui->m_ColumnasMatriz->setEnabled(true);
            ui->m_FrecuenciaFilasMatriz->setEnabled(true);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(true);
            ui->m_ExcepcionesMatriz->setEnabled(true);
            ui->m_ColorCampo->setEnabled(true);
            ui->m_MatrizOnOff->setEnabled(true);
            ui->m_NumeroMatriz->setEnabled(true);
        }
    }
    else{
        //Campos variables
        indice=indice-NUM_CAMPOS;
        ui->m_NombreCampo->setText(QString(m_camposvariables_temp[indice].NombreCampo));
        ui->m_Fuente->setCurrentFont(QString(m_camposvariables_temp[indice].TipoLetraCampo));
        ui->m_Tamano->setText(QString("%1").arg(m_camposvariables_temp[indice].TamanoFuenteCampo));
        ui->m_OrigenCampo->setText(QString(m_camposvariables_temp[indice].OrigenValorCampo));
        ui->m_ValorCampo->setText(QString("%1").arg(m_camposvariables_temp[indice].ValorActualCampo));


        if (!m_camposvariables_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_TEXTO)) ui->m_TipoCampo->setCurrentIndex(0);
        else if (!m_camposvariables_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)) ui->m_TipoCampo->setCurrentIndex(1);
        else if (!m_camposvariables_temp[indice].TipoCampo.compare(TXT_CAMPO_TIPO_IDEOGRAMA)) ui->m_TipoCampo->setCurrentIndex(2);
        //else if (!m_memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare("Contador")) ui->m_TipoCampo->setCurrentIndex(4);

        UpdateTipoCampo();

        indice_selecc=ui->m_Estilo->findText(QString(m_camposvariables_temp[indice].EstiloLetraCampo));
        ui->m_ColorCampo->setCurrentIndex((m_camposvariables_temp[indice].color)-1);
        ui->m_Estilo->setCurrentIndex(indice_selecc);

        /*if (int x=ui->m_TipoCampo->findText(m_memoryMap->m_mapa.m_camposvariables[indice].TipoCampo)!=-1)
            ui->m_TipoCampo->setCurrentIndex(x);*/

        if (m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
        else ui->m_MatrizOnOff->setChecked(false);
        ui->m_CampoHorizontal->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionXCampo));
        ui->m_CampoVertical->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionYCampo));
        ui->m_Orientacion->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].OrientacionCampo));
        ui->m_FilasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo));
        ui->m_ColumnasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo));
        ui->m_FrecuenciaFilasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo));
        ui->m_FrecuenciaColumnasMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo));
        ui->m_ExcepcionesMatriz->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo));
        ui->m_IncCols->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].incColumnas));
        ui->m_IncFilas->setText(QString("%1").arg(m_camposvariables_temp[indice].m_matrices[indice_matriz].incFilas));

        if (m_camposvariables_temp[indice].OnOffCampo==0x00){
            ui->m_MatrizOnOff->setChecked(false);
            ui->m_CampoHorizontal->setText(" ");
            ui->m_CampoVertical->setText(" ");
            ui->m_FilasMatriz->setText(" ");
            ui->m_ColumnasMatriz->setText(" ");
            ui->m_FrecuenciaFilasMatriz->setText(" ");
            ui->m_FrecuenciaColumnasMatriz->setText(" ");
            ui->m_ExcepcionesMatriz->setText(" ");
            ui->m_Orientacion->setText(" ");
            ui->m_CampoHorizontal->setEnabled(false);
            ui->m_CampoVertical->setEnabled(false);
            ui->m_Tamano->setText(" ");
            ui->m_Tamano->setEnabled(false);
            ui->m_Fuente->setEnabled(false);
            ui->m_Estilo->setEnabled(false);
            //ui->m_AceptarValores->setEnabled(false);
            ui->m_NombreCampo->setEnabled(false);
            ui->m_NombreCampo->setText("");
            ui->m_OrigenCampo->setEnabled(false);
            ui->m_OrigenCampo->setText("");
            ui->m_TipoCampo->setEnabled(false);
            ui->m_TipoCampo->setCurrentIndex(-1);
            ui->m_Orientacion->setEnabled(false);
            ui->m_FilasMatriz->setEnabled(false);
            ui->m_ColumnasMatriz->setEnabled(false);
            ui->m_FrecuenciaFilasMatriz->setEnabled(false);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(false);
            ui->m_ExcepcionesMatriz->setEnabled(false);
            ui->m_ColorCampo->setEnabled(false);
            ui->m_ColorSeleccionado->setPalette(dummyPaletteDisabled);
            ui->m_MatrizOnOff->setEnabled(false);
            ui->m_NumeroMatriz->setEnabled(false);
        }
        else{
            if (m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
            else ui->m_MatrizOnOff->setChecked(false);
            UpdateOnOffMatriz();
            ui->m_MatrizOnOff->setChecked(true);
            ui->m_CampoHorizontal->setEnabled(true);
            ui->m_CampoVertical->setEnabled(true);
            ui->m_Tamano->setEnabled(true);
            ui->m_Fuente->setEnabled(true);
            ui->m_Estilo->setEnabled(true);
            ui->m_NombreCampo->setEnabled(true);
            ui->m_OrigenCampo->setEnabled(true);
            ui->m_TipoCampo->setEnabled(true);
            ui->m_Orientacion->setEnabled(true);
            ui->m_FilasMatriz->setEnabled(true);
            ui->m_ColumnasMatriz->setEnabled(true);
            ui->m_FrecuenciaFilasMatriz->setEnabled(true);
            ui->m_FrecuenciaColumnasMatriz->setEnabled(true);
            ui->m_ExcepcionesMatriz->setEnabled(true);
            ui->m_ColorCampo->setEnabled(true);
            ui->m_MatrizOnOff->setEnabled(true);
            ui->m_NumeroMatriz->setEnabled(true);
        }

    }
    TestRoot();
}




void EditCampos::UpdateNumeroMatriz(){

    //QString dummy;

    UpdateNumeroCampo();
}

void EditCampos::UpdateCampoHorizontal(){
    InputData *input = InputData::request(this, "Posicion X Campo [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateHorizontal(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateHorizontal(QString value){
    ui->m_CampoHorizontal->setText(value);
    ui->m_CampoHorizontal->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditCampos::UpdateCampoVertical(){
    InputData *input = InputData::request(this,"Posicion Y Campo [mm]");
    if (input) {
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateVertical(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateVertical(QString value){
    ui->m_CampoVertical->setText(value);
    ui->m_CampoVertical->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditCampos::UpdateTamanoFuente(){
    InputData *input = InputData::request(this, "Tamaño Fuente [ptos]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateTamano(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateTamano(QString value){
    ui->m_Tamano->setText(value);
    ui->m_Tamano->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditCampos::UpdateOrientacionFuente(){
    InputData *input = InputData::request(this,"Orientacion Fuente");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateOrientacion(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateOrientacion(QString value){
    ui->m_Orientacion->setText(value);
    ui->m_Orientacion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditCampos::UpdateFilasMatriz(){
    InputData *input = InputData::request(this,"Filas matriz");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFilas(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateFilas(QString value){
    ui->m_FilasMatriz->setText(value);
    ui->m_FilasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditCampos::UpdateValorCampo(){
    InputData *input = InputData::request(this,"Valor campo");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(sltUpdateValorCampo(QString)));
        input->setFocus();
    }
}
void EditCampos::sltUpdateValorCampo(QString value){
    ui->m_ValorCampo->setText(value);
    ui->m_ValorCampo->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}



void EditCampos::UpdateColumnasMatriz(){
    InputData *input = InputData::request(this,"Columnas matriz");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateColumnas(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateColumnas(QString value){
    ui->m_ColumnasMatriz->setText(value);
    ui->m_ColumnasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditCampos::UpdateFrecuenciaFilasMatriz(){
    InputData *input = InputData::request(this,"Frecuencia Filas matriz [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFrecuenciaFilas(QString)));
        input->setFocus();
    }
}

void EditCampos::sltTrackItem(){
    unsigned char indice;
    unsigned char indice_matriz;
    indice=ui->m_NumeroCampo->currentIndex();
    indice_matriz=ui->m_NumeroMatriz->currentIndex();

    In2FieldPixmapItem *pixmap=m_OrdenCapas.at(indice*NUM_MATRIZ_POR_CAMPO+indice_matriz);
    if (pixmap==NULL) return;
    else{
        QPointF p=pixmap->scenePos();
        QPointF pMM=p/FACTOR_PIXELS_MM;
        ui->m_CampoHorizontal->setText(QString("%1").arg(pMM.x()));
        ui->m_CampoVertical->setText(QString("%1").arg(pMM.y()));
        if (indice<=NUM_CAMPOS-1){
            m_campos_temp[indice].m_matrices[indice_matriz].PosicionXCampo=pMM.x();
            m_campos_temp[indice].m_matrices[indice_matriz].PosicionYCampo=pMM.y();
        }
        else{
            indice=indice-NUM_CAMPOS;
            m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionXCampo=pMM.x();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionYCampo=pMM.y();

        }
    }
}


void EditCampos::UpdateFrecuenciaFilas(QString value){
    ui->m_FrecuenciaFilasMatriz->setText(value);
    ui->m_FrecuenciaFilasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditCampos::UpdateFrecuenciaColumnasMatriz(){
    InputData *input = InputData::request(this,"Frecuencia Columnas matriz [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFrecuenciaColumnas(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateFrecuenciaColumnas(QString value){
    ui->m_FrecuenciaColumnasMatriz->setText(value);
    ui->m_FrecuenciaColumnasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditCampos::UpdateExcepcionesMatriz(){
    InputData *input = InputData::request(this,"Excepciones matriz");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateExcepciones(QString)));
        input->setFocus();
    }
}

void EditCampos::UpdateExcepciones(QString value){
    ui->m_ExcepcionesMatriz->setText(value);
    ui->m_ExcepcionesMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditCampos::UpdateOnOffMatriz(){
    int indice=ui->m_NumeroCampo->currentIndex();
    int indice_matriz=ui->m_NumeroMatriz->currentIndex();

    if (ui->m_MatrizOnOff->isChecked())
    {
        ui->m_NumeroMatriz->setEnabled(true);
        ui->m_MatrizOnOff->setEnabled(true);
        ui->m_CampoHorizontal->setEnabled(true);
        ui->m_CampoVertical->setEnabled(true);
        ui->m_Orientacion->setEnabled(true);
        ui->m_FilasMatriz->setEnabled(true);
        ui->m_ColumnasMatriz->setEnabled(true);
        ui->m_FrecuenciaFilasMatriz->setEnabled(true);
        ui->m_FrecuenciaColumnasMatriz->setEnabled(true);
        ui->m_ExcepcionesMatriz->setEnabled(true);
        if ((indice<=NUM_CAMPOS-1)&&(indice>0)){
            m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz=1;
        }
        else{
            if (indice>0){
                indice=indice-NUM_CAMPOS;
                m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz=1;
            }
        }
        AceptarValoresCampos();
    }
    else
    {
        ui->m_NumeroMatriz->setEnabled(true);
        ui->m_MatrizOnOff->setEnabled(true);
        ui->m_CampoHorizontal->setEnabled(false);
        ui->m_CampoVertical->setEnabled(false);
        ui->m_Orientacion->setEnabled(false);
        ui->m_FilasMatriz->setEnabled(false);
        ui->m_ColumnasMatriz->setEnabled(false);
        ui->m_FrecuenciaFilasMatriz->setEnabled(false);
        ui->m_FrecuenciaColumnasMatriz->setEnabled(false);
        ui->m_ExcepcionesMatriz->setEnabled(false);
        if (indice<=NUM_CAMPOS-1){
            m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz=0;
        }
        else{
            indice=indice-NUM_CAMPOS;
            m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz=0;

        }
        AceptarValoresCampos();
    }

}

void EditCampos::UpdateColorCampo(){
    unsigned char indice;
    indice=ui->m_ColorCampo->currentIndex();
    QPalette paleta_color_pleno;
    paleta_color_pleno.setColor(QPalette::Base,QColor(m_memoryMap->m_mapa.m_colores[indice].valor));
    ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);
}

/** Comprueba si se ha seleccionado algun campo y modifica datos de pantalla*/
void EditCampos::sltItemSelected(){
    In2FieldPixmapItem * item=m_Scene->getFieldSelected();
    if (item){
        int id=item->getID();
        if (id!=BACKGROUND_ID){
            this->ui->m_NumeroCampo->setCurrentIndex(id/NUM_MATRIZ_POR_CAMPO);
            this->ui->m_NumeroMatriz->setCurrentIndex(id%NUM_MATRIZ_POR_CAMPO);
            UpdateNumeroCampo();
        }
    }
}

/** Comprueba si se ha seleccionado algun campo y modifica datos de pantalla*/
void EditCampos::sltItemSelected(int id){
    if (id!=BACKGROUND_ID){
        this->ui->m_NumeroCampo->setCurrentIndex(id/NUM_MATRIZ_POR_CAMPO);
        this->ui->m_NumeroMatriz->setCurrentIndex(id%NUM_MATRIZ_POR_CAMPO);
        sltTrackItem();
        //UpdateNumeroCampo();
    }
}


/** Actualiza los datos del item en el mapa de memoria con los datos de pantalla*/
void EditCampos::ActualizaDatosItem(){
    unsigned char indice;
    unsigned char indice_matriz;
    indice=ui->m_NumeroCampo->currentIndex();
    indice_matriz=ui->m_NumeroMatriz->currentIndex();

    In2FieldPixmapItem *pixmap=m_OrdenCapas.at(indice*NUM_MATRIZ_POR_CAMPO+indice_matriz);
    if (pixmap==NULL) return;
    else{
        //int margin=(m_memoryMap->m_Imagen.width()/m_memoryMap->m_mapa.m_variables.DefaultNumHeads)/2;
        //QPoint pMargin(m_margin,m_margin);
        QPointF p=pixmap->scenePos();
        QPointF pMM=p/FACTOR_PIXELS_MM;
        if (indice<=NUM_CAMPOS-1){
        //qDebug() << "Es un campo fijo";
        m_campos_temp[indice].TamanoFuenteCampo=ui->m_Tamano->text().toInt();
        m_campos_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText();
        m_campos_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText();
        m_campos_temp[indice].letterSpace=ui->m_leLetterSpace->text().toInt();
        if (m_memoryMap->m_mapa.m_variables.NumColores>1){
            m_campos_temp[indice].color=(ui->m_ColorCampo->currentIndex())+1;
        }
        ui->m_CampoHorizontal->setText(QString("%1").arg(pMM.x()));
        ui->m_CampoVertical->setText(QString("%1").arg(pMM.y()));
        m_campos_temp[indice].ValorActualCampo=ui->m_ValorCampo->text();
        m_campos_temp[indice].m_matrices[indice_matriz].PosicionXCampo=pMM.x();
        m_campos_temp[indice].m_matrices[indice_matriz].PosicionYCampo=pMM.y();
        m_campos_temp[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->text().toInt();
        m_campos_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->text().toInt();
        m_campos_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->text().toInt();
        m_campos_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->text().toDouble();
        m_campos_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->text().toDouble();
        m_campos_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
        m_campos_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();
        m_campos_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->text();
        m_campos_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
        m_campos_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();


        if (ui->m_MatrizOnOff->isChecked()) m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz=1;
        else m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz=0;
        m_campos_temp[indice].TipoCampo=ui->m_TipoCampo->currentText();
     }
     else{
        //qDebug() << "Es un campo variable";
            indice=indice-NUM_CAMPOS;
/*            m_camposvariables_temp[indice].TamanoFuenteCampo=ui->m_Tamano->text().toInt();
            m_camposvariables_temp[indice].letterSpace=ui->m_leLetterSpace->text().toInt();
#if QT_VERSION>=0x050000
            m_camposvariables_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText().toLatin1().data();
            m_camposvariables_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText().toLatin1().data();
#else
            m_camposvariables_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText().toAscii().data();
            m_camposvariables_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText().toAscii().data();
#endif*/
            m_camposvariables_temp[indice].TamanoFuenteCampo=ui->m_Tamano->text().toInt();
            m_camposvariables_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText();
            m_camposvariables_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText();
            m_camposvariables_temp[indice].letterSpace=ui->m_leLetterSpace->text().toInt();

        //#ifdef COLOR_PLENOS
            if (m_memoryMap->m_mapa.m_variables.NumColores>1){
                m_camposvariables_temp[indice].color=(ui->m_ColorCampo->currentIndex())+1;
            }
        //#endif

            /*m_memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionXCampo=ui->m_CampoHorizontal->toPlainText().toDouble();
            m_memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionYCampo=ui->m_CampoVertical->toPlainText().toDouble();*/

            ui->m_CampoHorizontal->setText(QString("%1").arg(pMM.x()));
            ui->m_CampoVertical->setText(QString("%1").arg(pMM.y()));
            m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionXCampo=pMM.x();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionYCampo=pMM.y();


            m_camposvariables_temp[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->text().toInt();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->text().toInt();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->text().toInt();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();

            m_camposvariables_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->text();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();

            if (ui->m_MatrizOnOff->isChecked()) m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz=1;
            else m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz=0;
            //Vamos a utilizarlo de momento en los campos variables.
            m_camposvariables_temp[indice].TipoCampo=ui->m_TipoCampo->currentText();
        }
    }
}

/** Actualiza los datos del item en el mapa de memoria con los datos de pantalla*/
void EditCampos::ActualizaDatosItem(int id){
    int indice=id/NUM_MATRIZ_POR_CAMPO;
    unsigned char indice_matriz=id%2;
    //indice_matriz=ui->m_NumeroMatriz->currentIndex();

    In2FieldPixmapItem *pixmap=m_OrdenCapas.at(indice*NUM_MATRIZ_POR_CAMPO+indice_matriz);
    if (pixmap==NULL) return;
    else{
        //int margin=(m_memoryMap->m_Imagen.width()/m_memoryMap->m_mapa.m_variables.DefaultNumHeads)/2;
        //QPoint pMargin(m_margin,m_margin);
        QPointF p=pixmap->scenePos();
        QPointF pMM=p;
        if (indice<=NUM_CAMPOS-1){
            m_campos_temp[indice].TamanoFuenteCampo=ui->m_Tamano->text().toInt();
            m_campos_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText();
            m_campos_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText();
            if (m_memoryMap->m_mapa.m_variables.NumColores>1){
                m_campos_temp[indice].color=(ui->m_ColorCampo->currentIndex())+1;
            }
            ui->m_CampoHorizontal->setText(QString("%1").arg(pMM.x()));
            ui->m_CampoVertical->setText(QString("%1").arg(pMM.y()));
            m_campos_temp[indice].ValorActualCampo=ui->m_ValorCampo->text();
            m_campos_temp[indice].m_matrices[indice_matriz].PosicionXCampo=pMM.x();
            m_campos_temp[indice].m_matrices[indice_matriz].PosicionYCampo=pMM.y();

            m_campos_temp[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->text().toInt();
            m_campos_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->text().toInt();
            m_campos_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->text().toInt();
            m_campos_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->text().toDouble();
            m_campos_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->text().toDouble();
            m_campos_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
            m_campos_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();
            m_campos_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->text();
            m_campos_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
            m_campos_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();

            if (ui->m_MatrizOnOff->isChecked()) m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz=1;
            else m_campos_temp[indice].m_matrices[indice_matriz].OnOffMatriz=0;
            m_campos_temp[indice].TipoCampo=ui->m_TipoCampo->currentText();
        }
        else{
        //qDebug() << "Es un campo variable";
            indice=indice-NUM_CAMPOS;
            m_camposvariables_temp[indice].TamanoFuenteCampo=ui->m_Tamano->text().toInt();
#if QT_VERSION>=0x050000
            m_camposvariables_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText().toLatin1().data();
            m_camposvariables_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText().toLatin1().data();
#else
            m_camposvariables_temp[indice].TipoLetraCampo=ui->m_Fuente->currentText().toAscii().data();
            m_camposvariables_temp[indice].EstiloLetraCampo=ui->m_Estilo->currentText().toAscii().data();
#endif
        //#ifdef COLOR_PLENOS
            if (m_memoryMap->m_mapa.m_variables.NumColores>1){
                m_camposvariables_temp[indice].color=(ui->m_ColorCampo->currentIndex())+1;
            }
        //#endif

            /*m_memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionXCampo=ui->m_CampoHorizontal->toPlainText().toDouble();
            m_memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionYCampo=ui->m_CampoVertical->toPlainText().toDouble();*/

            ui->m_CampoHorizontal->setText(QString("%1").arg(pMM.x()));
            ui->m_CampoVertical->setText(QString("%1").arg(pMM.y()));
            m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionXCampo=pMM.x();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].PosicionYCampo=pMM.y();


            m_camposvariables_temp[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->text().toInt();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->text().toInt();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->text().toInt();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();

            m_camposvariables_temp[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->text();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incColumnas=ui->m_IncCols->text().toDouble();
            m_camposvariables_temp[indice].m_matrices[indice_matriz].incFilas=ui->m_IncFilas->text().toDouble();

            if (ui->m_MatrizOnOff->isChecked()) m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz=1;
            else m_camposvariables_temp[indice].m_matrices[indice_matriz].OnOffMatriz=0;
            //Vamos a utilizarlo de momento en los campos variables.
            m_camposvariables_temp[indice].TipoCampo=ui->m_TipoCampo->currentText();
        }
    }
}


/** Mueve abajo*/
void EditCampos::sltMoveDown(){
    int indice_matriz=ui->m_NumeroMatriz->currentIndex();
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoVertical->text().toFloat();
        /*if (dummy<ui->m_ImagenUnitaria->height())*/ dummy+=MOVE_STEP;
        ui->m_CampoVertical->setText(QString("%1").arg(dummy));
        int i=ui->m_NumeroCampo->currentIndex();
        if (i<NUM_CAMPOS){
            m_campos_temp[i].m_matrices[indice_matriz].PosicionYCampo=dummy;
        }
        else{
            m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[indice_matriz].PosicionYCampo=dummy;
        }
        drawItem(i,indice_matriz);
        //AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning",tr("Item no habilitado"),QMessageBox::Ok);
}
/** Mueve arriba*/
void EditCampos::sltMoveUp(){
    int indice_matriz=ui->m_NumeroMatriz->currentIndex();
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoVertical->text().toFloat();
        //if (dummy>=MOVE_STEP*1.5) dummy-=MOVE_STEP;
        dummy-=MOVE_STEP;
        ui->m_CampoVertical->setText(QString("%1").arg(dummy));
        //AceptarValoresCampos();
        int i=ui->m_NumeroCampo->currentIndex();
        if (i<NUM_CAMPOS){
            m_campos_temp[i].m_matrices[indice_matriz].PosicionYCampo=dummy;
        }
        else{
            m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[indice_matriz].PosicionYCampo=dummy;
        }

        drawItem(i,indice_matriz);
    }
    else QMessageBox::warning(0,"Warning",tr("Item no habilitado"),QMessageBox::Ok);
}
/** Mueve derecha*/
void EditCampos::sltMoveRight(){
    int indice_matriz=ui->m_NumeroMatriz->currentIndex();
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoHorizontal->text().toFloat();
        /*if (dummy<ui->m_ImagenUnitaria->width())*/  dummy+=MOVE_STEP;
        ui->m_CampoHorizontal->setText(QString("%1").arg(dummy));
        int i=ui->m_NumeroCampo->currentIndex();

        if (i<NUM_CAMPOS){
            m_campos_temp[i].m_matrices[indice_matriz].PosicionXCampo=dummy;
        }
        else{
            m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[indice_matriz].PosicionXCampo=dummy;
        }

        drawItem(i,indice_matriz);

        //AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning",tr("Item no habilitado"),QMessageBox::Ok);
}
/** Mueve izquierda*/
void EditCampos::sltMoveLeft(){
    int indice_matriz=ui->m_NumeroMatriz->currentIndex();
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoHorizontal->text().toFloat();
        if (dummy>=MOVE_STEP*1.5) dummy-=MOVE_STEP;
        ui->m_CampoHorizontal->setText(QString("%1").arg(dummy));

        int i=ui->m_NumeroCampo->currentIndex();
        if (i<NUM_CAMPOS){
            m_campos_temp[i].m_matrices[indice_matriz].PosicionXCampo=dummy;
        }
        else{
            m_camposvariables_temp[i-NUM_CAMPOS].m_matrices[indice_matriz].PosicionXCampo=dummy;
        }

        drawItem(i,indice_matriz);

        //AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning",tr("Item no habilitado"),QMessageBox::Ok);
}



void EditCampos::AceptarValoresCampos(){

    //Aqui hacemos al reves, y posicionamos la imagen donde le diga en el cuadro

    if ((ui->m_FilasMatriz->text().toInt()>1)&&(ui->m_FrecuenciaFilasMatriz->text().toDouble()==0)){
        QMessageBox::warning(this,"ERROR",tr("La frecuencia de filas no puede ser 0 si el numero de filas es mayor que 1"),QMessageBox::Ok);
        return;
    }
    if ((ui->m_ColumnasMatriz->text().toInt()>1)&&(ui->m_FrecuenciaColumnasMatriz->text().toDouble()==0)){
        QMessageBox::warning(this,"ERROR",tr("La frecuencia de columnas no puede ser 0 si el numero de columnas es mayor que 1"),QMessageBox::Ok);
        return;
    }


    if (ui->m_MatrizOnOff->isChecked()){
        int i=ui->m_NumeroCampo->currentIndex();
        int indice_matriz=ui->m_NumeroMatriz->currentIndex();
        In2FieldPixmapItem *pixmap=m_OrdenCapas.at(i*NUM_MATRIZ_POR_CAMPO+indice_matriz);
        if (pixmap!=NULL){ //Es la primera vez..añado el pixmap
            double x=ui->m_CampoHorizontal->text().toDouble()*FACTOR_PIXELS_MM;
            double y=ui->m_CampoVertical->text().toDouble()*FACTOR_PIXELS_MM;
            pixmap->setPos(x,y);
            ActualizaDatosItem();
            drawItem(i,indice_matriz);
        }
        else{
            //sltMoveItem();
        }

    }
    else{
        int i=ui->m_NumeroCampo->currentIndex();
        int indice_matriz=ui->m_NumeroMatriz->currentIndex();
        In2FieldPixmapItem *pixmap=m_OrdenCapas.at(i*NUM_MATRIZ_POR_CAMPO+indice_matriz);
        if (pixmap!=NULL){ //Es la primera vez..añado el pixmap            
            pixmap=NULL;
            m_OrdenCapas.replace(i*NUM_MATRIZ_POR_CAMPO+indice_matriz,pixmap);
        }

    }

}

void EditCampos::LaunchComposicion(){
    Composicion *ScrComposicion;
    ScrComposicion=new Composicion(0);
    ScrComposicion->show();
    this->deleteLater();
}



//Muestra la imagen central
void EditCampos::MuestraImagen(){

    QPixmap *backImage;
    In2ImageCreator ImgCreator;


    QPixmap tmp=QPixmap(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
    QPixmap *lienzo=new QPixmap(m_memoryMap->m_Imagen.size());

    lienzo->fill(Qt::white);
    backImage= new QPixmap(ImgCreator.returnImageWithMargin(tmp));
        //m_margin=(backImage.width()-tmp.width())/2;
    m_margin=DEFAULT_MARGIN;/*(m_memoryMap->m_Imagen.width()/m_memoryMap->m_mapa.m_variables.DefaultNumHeads)/2;*/
    QPainter painter(lienzo);
    painter.drawPixmap(0,0,*backImage);
    painter.setPen( QPen( Qt::red, 8 ) );
    painter.drawRect(backImage->rect());
    painter.end();
    m_Scene = new In2GraphicsScene(0, 0,m_memoryMap->m_Imagen.width(), m_memoryMap->m_Imagen.height());
    In2FieldPixmapItem *p=new In2FieldPixmapItem(0,BACKGROUND_ID);


    p->setPixmap(/*backImage*/*lienzo);
    m_Scene->addItem(p);
    ui->m_GraphicsView->setScene(m_Scene);
    ui->m_GraphicsView->show();


    //bool test=ui->m_GraphicsView->testAttribute(Qt::WA_TransparentForMouseEvents);
    m_View=new In2GraphicsView(ui->m_GraphicsView);
    //ui->m_GraphicsView->centerOn(p);
    ui->m_GraphicsView->ensureVisible(p,25,25);
    connect(m_View,SIGNAL(trackPoint(QPointF)),this,SLOT(UpdateMousePos(QPointF)));

    //Cargamos los campos fijos/variables...dibujamos solo si ya han sido configurados
    for (int x=0;x<NUM_CAMPOS+NUM_CAMPOS_VARIABLES;x++) {
        for (int matriz=0;matriz<NUM_MATRIZ_POR_CAMPO;matriz++){
            if (x<NUM_CAMPOS){
                /*double xpos=m_memoryMap->m_mapa.m_campos[x].m_matrices[matriz].PosicionXCampo;
                double ypos=m_memoryMap->m_mapa.m_campos[x].m_matrices[matriz].PosicionYCampo;*/
                if ((m_campos_temp[x].OnOffCampo)/*&&((xpos!=0)||(ypos!=0))*/) {
                    ui->m_NumeroCampo->setCurrentIndex(x);
                    UpdateNumeroCampo();
                    drawItem(x,matriz);
                }
            }

            else{
                /*double xpos=m_memoryMap->m_mapa.m_camposvariables[x-NUM_CAMPOS].m_matrices[matriz].PosicionXCampo;
                double ypos=m_memoryMap->m_mapa.m_camposvariables[x-NUM_CAMPOS].m_matrices[matriz].PosicionYCampo;*/
                if ((m_camposvariables_temp[x-NUM_CAMPOS].OnOffCampo)/*&&((xpos!=0)||(ypos!=0))*/) {
                    ui->m_NumeroCampo->setCurrentIndex(x);
                    UpdateNumeroCampo();
                    drawItem(x,matriz);
                }
            }
        }
    }

    ui->m_NumeroCampo->setCurrentIndex(0);
    UpdateNumeroCampo();
    ui->m_GraphicsView->fitInView(p,Qt::KeepAspectRatio);
    UpdateGridInfo();

}

/** Actualiza la info  de grid*/
void EditCampos::UpdateGridInfo(){
    int grid=m_Scene->getGridSize();
    QString txt=QString("%1 mm").arg(grid);
    ui->m_gridSize->setText(txt);
}

/** Grid mas*/
void EditCampos::sltGridMas(){
    int grid=m_Scene->getGridSize();
    grid++;
    m_Scene->setGridSize(grid);
    QVector<QLineF> lines=m_Scene->getGridLines();

    UpdateGridInfo();
}
/** Grid menos*/
void EditCampos::sltGridMenos(){
    int grid=m_Scene->getGridSize();
    grid--;
    m_Scene->setGridSize(grid);
    UpdateGridInfo();
}

void EditCampos::ConfigurarOrigen(void){

    QString texto=ui->m_OrigenCampo->text();

    if (!texto.compare(TXT_ORIGEN_CONTADOR))
      ConfigurarContador();

    if (!texto.compare(TXT_ORIGEN_FECHA))
      ConfigurarFechaHora();

    if (!texto.compare(TXT_ORIGEN_RANDOM))
      ConfigurarRandom();
}



void EditCampos::ConfigurarElemento(void){

    int indice=ui->m_TipoCampo->currentIndex();
    QString texto=ui->m_TipoCampo->itemText(indice);

    /*ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_TEXTO);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_BARCODE);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_IDEOGRAMA);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_CONTADOR);*/

    if (!texto.compare(TXT_CAMPO_TIPO_BARCODE))
        ConfigurarCodigoBarras();
    else if (!texto.compare(TXT_CAMPO_TIPO_TEXTO))
        ConfigurarFechaHora();


}


void EditCampos::ConfigurarCodigoBarras(void){
    struct m_BarcodeParameters data;
    int i=ui->m_NumeroCampo->currentIndex();
    if (i<NUM_CAMPOS){
        data=m_campos_temp[i].m_barcodeParameters;
        data.texto=m_campos_temp[data.NumeroCampo].ValorActualCampo;
        data.NumeroCampo=i;
        data.CampoFijo=0x01;

    }
    else{
        data=m_camposvariables_temp[i-NUM_CAMPOS].m_barcodeParameters;
        data.NumeroCampo=i-NUM_CAMPOS;
        //data.texto=m_camposvariables_temp[i-NUM_CAMPOS].ValorActualCampo;
        data.CampoFijo=0x00;

    }

    //data.NumeroCampo=ui->m_NumeroCampo->currentIndex();



    formatBarcode *ScrformatBarcode;
    ScrformatBarcode=new formatBarcode(this,data);
    ScrformatBarcode->show();

    QObject::connect(ScrformatBarcode, SIGNAL(sglBarcodeConfigured(const m_BarcodeParameters & )),this, SLOT(sltBarcodeConfigured(const m_BarcodeParameters &)));

}

void EditCampos::UpdateTipoCampo(void){
    //JORGE: 21/10/2013
    //Cambio para que aparezca la pantalla de configuración del código de barras
    int indice_actual_tipo_campo;
    indice_actual_tipo_campo=ui->m_TipoCampo->currentIndex();
    QString texto=ui->m_TipoCampo->itemText(indice_actual_tipo_campo);
    if ((!texto.compare(TXT_CAMPO_TIPO_BARCODE))||(!texto.compare(TXT_CAMPO_TIPO_TEXTO))){
        ui->m_ConfigTipo->setEnabled(true);
    }
    else{
       ui->m_ConfigTipo->setEnabled(false);
    }

    //AceptarValoresCampos();
}


void EditCampos::UpdateOrigenCampo(void){

    QString texto=ui->m_OrigenCampo->text();
    if ( (!texto.compare(TXT_ORIGEN_CONTADOR)) ||
         (!texto.compare(TXT_ORIGEN_FECHA)) ||
         (!texto.compare(TXT_ORIGEN_RANDOM)) ){
        ui->m_ConfigOrigen->setEnabled(true);
    }
    else{
       ui->m_ConfigOrigen->setEnabled(false);
    }
}




void EditCampos::sltBarcodeConfigured(const m_BarcodeParameters & data){
    struct m_BarcodeParameters  barcode=data;
    int campo=barcode.NumeroCampo;

    if (barcode.CampoFijo==0x01){
        m_campos_temp[campo].TipoCampo=TXT_CAMPO_TIPO_BARCODE;
        m_campos_temp[campo].m_barcodeParameters.NumeroCampo=barcode.NumeroCampo;
        m_campos_temp[campo].m_barcodeParameters.width=barcode.width;
        m_campos_temp[campo].m_barcodeParameters.width_mm=barcode.width_mm;
        m_campos_temp[campo].m_barcodeParameters.height=barcode.height;
        m_campos_temp[campo].m_barcodeParameters.height_mm=barcode.height_mm;
        m_campos_temp[campo].m_barcodeParameters.codificacion=barcode.codificacion;
        m_campos_temp[campo].m_barcodeParameters.option2=barcode.option2;
        m_campos_temp[campo].m_barcodeParameters.inputMode=barcode.inputMode;
        m_campos_temp[campo].m_barcodeParameters.texto=barcode.texto;
        m_campos_temp[campo].m_barcodeParameters.whitespace=barcode.whitespace;
        m_campos_temp[campo].m_barcodeParameters.borderWidth=barcode.borderWidth;
        m_campos_temp[campo].m_barcodeParameters.borderType=barcode.borderType;
        m_campos_temp[campo].m_barcodeParameters.hideText=barcode.hideText;
        m_campos_temp[campo].m_barcodeParameters.escala=barcode.escala;
        m_campos_temp[campo].m_barcodeParameters.ar=barcode.ar;
        m_campos_temp[campo].m_barcodeParameters.fontSizeLarge=barcode.fontSizeLarge;
        m_campos_temp[campo].m_barcodeParameters.fontSizeSmall=barcode.fontSizeSmall;
        m_campos_temp[campo].m_barcodeParameters.fontType=barcode.fontType;
        m_campos_temp[campo].m_barcodeParameters.nivelSeguridad=barcode.nivelSeguridad;
        m_campos_temp[campo].m_barcodeParameters.textAlignement=barcode.textAlignement;
    }
    else{
        m_camposvariables_temp[campo].TipoCampo=TXT_CAMPO_TIPO_BARCODE;
        m_camposvariables_temp[campo].m_barcodeParameters.NumeroCampo=barcode.NumeroCampo;
        m_camposvariables_temp[campo].m_barcodeParameters.width=barcode.width;
        m_camposvariables_temp[campo].m_barcodeParameters.height=barcode.height;
        m_camposvariables_temp[campo].m_barcodeParameters.width_mm=barcode.width_mm;
        m_camposvariables_temp[campo].m_barcodeParameters.height_mm=barcode.height_mm;
        m_camposvariables_temp[campo].m_barcodeParameters.codificacion=barcode.codificacion;
        m_camposvariables_temp[campo].m_barcodeParameters.option2=barcode.option2;
        m_camposvariables_temp[campo].m_barcodeParameters.inputMode=barcode.inputMode;
        m_camposvariables_temp[campo].m_barcodeParameters.texto=barcode.texto;
        m_camposvariables_temp[campo].m_barcodeParameters.whitespace=barcode.whitespace;
        m_camposvariables_temp[campo].m_barcodeParameters.borderWidth=barcode.borderWidth;
        m_camposvariables_temp[campo].m_barcodeParameters.borderType=barcode.borderType;
        m_camposvariables_temp[campo].m_barcodeParameters.hideText=barcode.hideText;
        m_camposvariables_temp[campo].m_barcodeParameters.escala=barcode.escala;
        m_camposvariables_temp[campo].m_barcodeParameters.ar=barcode.ar;
        m_camposvariables_temp[campo].m_barcodeParameters.fontSizeLarge=barcode.fontSizeLarge;
        m_camposvariables_temp[campo].m_barcodeParameters.fontSizeSmall=barcode.fontSizeSmall;
        m_camposvariables_temp[campo].m_barcodeParameters.fontType=barcode.fontType;
        m_camposvariables_temp[campo].m_barcodeParameters.nivelSeguridad=barcode.nivelSeguridad;
        m_camposvariables_temp[campo].m_barcodeParameters.textAlignement=barcode.textAlignement;

    }

    AceptarValoresCampos();

}

void EditCampos::ConfigurarContador(void){

    int i=ui->m_NumeroCampo->currentIndex();
    if (i>=NUM_CAMPOS) i=i-NUM_CAMPOS;
    struct m_CounterParameters data=m_camposvariables_temp[i].m_counterParameters;
    formatCounter *ScrformatCounter;
    ScrformatCounter=new formatCounter(this,data);
    ScrformatCounter->show();

    connect(ScrformatCounter, SIGNAL(sglCounterConfigured(const struct m_CounterParameters &)),this, SLOT(sltCounterConfigured(const struct m_CounterParameters &)));

}

void EditCampos::sltCounterConfigured(const struct m_CounterParameters & data){

    int numeroCampo=data.numeroCampo;
//Damos un valor a valor actual
    m_camposvariables_temp[numeroCampo].ValorActualCampo=QString("%1").arg(data.valorInicial);
    m_camposvariables_temp[numeroCampo].m_counterParameters.valorInicial=data.valorInicial;
    m_camposvariables_temp[numeroCampo].m_counterParameters.limiteInferior=data.limiteInferior;
    m_camposvariables_temp[numeroCampo].m_counterParameters.limiteSuperior=data.limiteSuperior;
    m_camposvariables_temp[numeroCampo].m_counterParameters.incremento=data.incremento;
    m_camposvariables_temp[numeroCampo].m_counterParameters.offset=data.offset;
    m_camposvariables_temp[numeroCampo].m_counterParameters.configuracion=data.configuracion;
    m_camposvariables_temp[numeroCampo].m_counterParameters.indexConfiguracion=data.indexConfiguracion;
    m_camposvariables_temp[numeroCampo].m_counterParameters.valorInicialFila=data.valorInicial;
    m_camposvariables_temp[numeroCampo].m_counterParameters.numeroCifras=data.numeroCifras;
    m_camposvariables_temp[numeroCampo].m_counterParameters.numeroCampo=data.numeroCampo;
    m_camposvariables_temp[numeroCampo].m_counterParameters.m_format.bFormatEnabled=data.m_format.bFormatEnabled;
    m_camposvariables_temp[numeroCampo].m_counterParameters.m_format.zeroPadding=data.m_format.zeroPadding;
    m_camposvariables_temp[numeroCampo].m_counterParameters.m_format.FormatSeparador=data.m_format.FormatSeparador;
    m_camposvariables_temp[numeroCampo].m_counterParameters.m_format.numeroCifras=data.m_format.numeroCifras;
    /*m_memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.thousandsSeparator=data.thousandsSeparator;
    m_memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.decimalSeparator=data.decimalSeparator;*/
}

void EditCampos::sltDateTimeConfigured(const m_DateTimeParameters & data){

    int numeroCampo=ui->m_NumeroCampo->currentIndex();
    if (numeroCampo<NUM_CAMPOS){
        m_campos_temp[numeroCampo].m_dateTimeParameters=data;
        //m_campos_temp[numeroCampo].m_dateTimeParameters.numeroCampo=numeroCampo;
    }
    else { //if (i>=NUM_CAMPOS)
        numeroCampo=numeroCampo-NUM_CAMPOS;
        m_camposvariables_temp[numeroCampo].m_dateTimeParameters=data;
        //m_camposvariables_temp[numeroCampo].m_dateTimeParameters.numeroCampo=numeroCampo;
    }

}

void EditCampos::sltRandomConfigured(const m_RandomNumberParameters & data){

    int numeroCampo=ui->m_NumeroCampo->currentIndex();

    numeroCampo=numeroCampo-NUM_CAMPOS;

    m_camposvariables_temp[numeroCampo].m_randomNumberParameters.formato=data.formato;
    m_camposvariables_temp[numeroCampo].m_randomNumberParameters.limiteInferior=data.limiteInferior;
    m_camposvariables_temp[numeroCampo].m_randomNumberParameters.limiteSuperior=data.limiteSuperior;
    m_camposvariables_temp[numeroCampo].m_randomNumberParameters.numeroCampo=numeroCampo;

}

void EditCampos::ConfigurarFechaHora(void){

    int i=ui->m_NumeroCampo->currentIndex();
    struct m_DateTimeParameters data;
    if (i>=NUM_CAMPOS) {
        i=i-NUM_CAMPOS;
        data=m_camposvariables_temp[i].m_dateTimeParameters;
    }
    else{
        data=m_campos_temp[i].m_dateTimeParameters;
    }
    data.numeroCampo=i;
    formatDateTime *ScrformatDateTime;
    ScrformatDateTime=new formatDateTime(this,data);
    ScrformatDateTime->show();
    connect(ScrformatDateTime, SIGNAL(sglDateTimeConfigured(const struct m_DateTimeParameters &)),this, SLOT(sltDateTimeConfigured(const struct m_DateTimeParameters &)));
}

void EditCampos::ConfigurarRandom(void){

    int i=ui->m_NumeroCampo->currentIndex();
    if (i>=NUM_CAMPOS) i=i-NUM_CAMPOS;
    struct m_RandomNumberParameters data=m_camposvariables_temp[i].m_randomNumberParameters;
    data.numeroCampo=i;
    formatRandom *ScrformatRandom;
    ScrformatRandom=new formatRandom(this,data);
    ScrformatRandom->show();

    connect(ScrformatRandom, SIGNAL(sglRandomConfigured(const struct m_RandomNumberParameters &)),this, SLOT(sltRandomConfigured(const struct m_RandomNumberParameters &)));

}
