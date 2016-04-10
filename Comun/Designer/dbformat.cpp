/** @addtogroup DbFormat
 *  @{*/
/** @file DbFormat.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbFormat.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#include "dbformat.h"
#include "dbformatpage.h"
#include "dbimpresion.h"
#include "globals.h"
#include "principal.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
#include "principal.h"
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
//#include "dbgmem.h"
#include "counterStyle.h"

#define POS_LABEL_W 50
#define POS_LABEL_H 25


struct m_Campos m_campos_temp[NUM_CAMPOS];
struct m_CamposVariables m_camposvariables_temp[NUM_CAMPOS_VARIABLES];

static unsigned char NumeroEstiloFuente;
static int peso_campo;

#define MOVE_STEP 0.1

DbFormat::DbFormat(QWidget *parent)
    : ui(new Ui::EditCamposUI)
{
    QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
    QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    ui->setupUi(this);
    GlobalSetScreenSize(this);
    SetDataCommon();
    ui->m_NombreCampo->setReadOnly(true);
    ui->m_TipoCampo->setCurrentIndex(0);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_TEXTO);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_BARCODE);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_DATAMATRIX);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_IDEOGRAMA);
    //ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_CONTADOR);

    //m_TipoCampo->setVisible(false);
    //escalado=IMAGEN_SINESCALAR;
    offset_x=0;
    offset_y=0;

    //if (memoryMap->ImagenVariable==0x00) ui->m_TipoCampo->setVisible(false);

    for (int j=0;j<NUM_CAMPOS;j++){
        QString tmp_j=QString::number(j+1,10);
        ui->m_NumeroCampo->addItem("Campo "+tmp_j);
    }

    /*Con campos variables*/
    if (memoryMap->ImagenVariable==0x01){
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

    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        ui->m_ColorCampo->addItem(QString("%1").arg(i+1)); 
    }
    ui->m_ColorCampo->setCurrentIndex((memoryMap->m_mapa.m_campos[0].color)-1);

    ui->m_Fuente->setEditable(false);
    ui->m_Estilo->setEditable(false);


    if (memoryMap->m_mapa.m_variables.NumColores==1){
        ui->m_ColorCampo->setVisible(false);
        ui->m_ColorSeleccionado->setVisible(false);
        ui->lbl_ColorCampo->setVisible(false);
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

    UpdateNumeroCampo();

    PasarDeMapATemp();
    if (memoryMap->ImagenVariable==1) PasarDeMapATempVariables();

    //Lo clavamos
    ui->m_ImagenUnitaria->setMaximumWidth(ui->m_ImagenUnitaria->width());
    ui->m_ImagenUnitaria->setMaximumHeight(ui->m_ImagenUnitaria->height());

    rotatedpixmap.load(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
    if (memoryMap->ImagenVariable==0x00) CreaImagenMuestra();
    else CreaImagenMuestraVariable();
    MuestraImagen(rotatedpixmap);

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_CancelButton,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));

    connect(ui->m_NumeroCampo,SIGNAL(activated (QString)),this,SLOT(UpdateNumeroCampo()));
    connect(ui->m_NumeroMatriz,SIGNAL(activated (QString)),this,SLOT(UpdateNumeroMatriz()));
    connect(ui->m_ColorCampo,SIGNAL(activated (QString)),this,SLOT(UpdateColorCampo()));
    connect(ui->m_MatrizOnOff,SIGNAL(clicked()),this,SLOT(UpdateOnOffMatriz()));
    connect(ui->m_TipoCampo,SIGNAL(activated (QString)),this,SLOT(UpdateTipoCampo()));

    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_CampoHorizontal,SIGNAL(selectionChanged()),this,SLOT(UpdateCampoHorizontal()));
    connect(ui->m_CampoVertical,SIGNAL(selectionChanged()),this,SLOT(UpdateCampoVertical()));
    connect(ui->m_Tamano,SIGNAL(selectionChanged()),this,SLOT(UpdateTamanoFuente()));
    connect(ui->m_Orientacion,SIGNAL(selectionChanged()),this,SLOT(UpdateOrientacionFuente()));
    connect(ui->m_FilasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateFilasMatriz()));
    connect(ui->m_ColumnasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateColumnasMatriz()));
    connect(ui->m_FrecuenciaFilasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateFrecuenciaFilasMatriz()));
    connect(ui->m_FrecuenciaColumnasMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateFrecuenciaColumnasMatriz()));
    connect(ui->m_ExcepcionesMatriz,SIGNAL(selectionChanged()),this,SLOT(UpdateExcepcionesMatriz()));
    #endif

    connect(ui->m_AceptarValores,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));
    connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    connect(ui->m_ImagenUnitaria,SIGNAL(LabelPos(QPoint)),this,SLOT(UpdateMousePos(QPoint)));
    connect(ui->m_ImagenUnitaria,SIGNAL(SetItemPos(QPoint)),this,SLOT(UpdateComposition(QPoint)));
    connect(ui->m_MoveDown,SIGNAL(clicked()),this,SLOT(sltMoveDown()));
    connect(ui->m_MoveUp,SIGNAL(clicked()),this,SLOT(sltMoveUp()));
    connect(ui->m_MoveLeft,SIGNAL(clicked()),this,SLOT(sltMoveLeft()));
    connect(ui->m_MoveRight,SIGNAL(clicked()),this,SLOT(sltMoveRight()));
    //qDebug() << "OnOffCampo1: " << memoryMap->m_mapa.m_campos[0].OnOffCampo;

    connect(ui->m_ConfigTipo,SIGNAL(clicked()),this,SLOT(ConfigurarElemento()));

    connect(ui->m_ConfigOrigen,SIGNAL(clicked()),this,SLOT(ConfigurarOrigen()));

    if (memoryMap->m_mapa.m_campos[0].OnOffCampo==0x00){
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

    //qDebug() << "OnOffCampo1: " << memoryMap->m_mapa.m_campos[0].OnOffCampo;

    if (memoryMap->m_mapa.m_campos[0].m_matrices[0].OnOffMatriz==1){
        ui->m_MatrizOnOff->setChecked(true);
    }
    else{
        ui->m_MatrizOnOff->setChecked(false);
    }

    UpdateOnOffMatriz();
    TestRoot();
    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));
    connect(ui->m_MoveItem,SIGNAL(clicked()),this,SLOT(sltMoveItem()));
    ui->tabWidget->setCurrentIndex(0);




}

void DbFormat::UpdateComposition(const QPoint &point){
    QPoint dummyPoint=UpdateMousePos(point);
    ui->m_CampoHorizontal->setText(QString("%1").arg(dummyPoint.x()));
    ui->m_CampoVertical->setText(QString("%1").arg(dummyPoint.y()));

    AceptarValoresCampos();
}

/** Generamos un item para mover*/
void DbFormat::sltMoveItem(){
    if (ui->m_MatrizOnOff->isChecked()==true){
        int i=ui->m_NumeroCampo->currentIndex();
        ActualizaDatosItem();
        QPixmap tmp;
        if (i<NUM_CAMPOS){
            if (memoryMap->m_mapa.m_campos[i].ValorActualCampo!=""){
                tmp=DrawFixedItem(i);
                ui->m_ImagenUnitaria->setNewMimePixmap(tmp);
            }
            else{
                QMessageBox::warning(0,"Warning","El item no tiene valor",QMessageBox::Ok);
            }
        }
        else{
            if (memoryMap->m_mapa.m_camposvariables[i-NUM_CAMPOS].ValorActualCampo!=""){
                tmp=DrawVariableItem(i-NUM_CAMPOS);
                ui->m_ImagenUnitaria->setNewMimePixmap(tmp);
            }
            else{
                QMessageBox::warning(0,"Warning","El item no tiene valor",QMessageBox::Ok);
            }
        }
    }
    else QMessageBox::warning(0,"Warning","El item no está habilitado",QMessageBox::Ok);
}

/** Actualiza la posición del cursor*/
QPoint DbFormat::UpdateMousePos(const QPoint &p ){

    //Depende si el escalado es horizontal, vertical o no hay
    QPoint ModP=p;

    if (offset_x>0) ModP.setX(p.x()-offset_x);
    if (offset_y>0) ModP.setY(p.y()-offset_y);
    //Queda mejor sin negativos
    if (ModP.x()<0) ModP.setX(0);
    if (ModP.y()<0) ModP.setY(0);

    int RealPosx=(ModP.x()*ui->m_ImagenUnitaria->OriginalPixmap().width())/((ui->m_ImagenUnitaria->width()-2*offset_x)*FACTOR_PIXELS_MM);
    int RealPosy=(ModP.y()*ui->m_ImagenUnitaria->OriginalPixmap().height())/((ui->m_ImagenUnitaria->height()-2*offset_y)*FACTOR_PIXELS_MM);
    ui->m_labelPos->setText(QString("%1x%2mm").arg(RealPosx).arg(RealPosy));

    /*ui->m_CampoHorizontal->setText(QString("%1").arg(RealPosx));
    ui->m_CampoVertical->setText(QString("%1").arg(RealPosy));*/
    return QPoint(RealPosx,RealPosy);
}

void DbFormat::TestRoot(){
    if (memoryMap->getUserRights()<ACCESS_ROOT){
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

DbFormat::~DbFormat(){

}


void DbFormat::PasarDeMapATemp(){

    for (int i=0;i<NUM_CAMPOS;i++){
        /*strncpy(m_campos_temp[i].NombreCampo,memoryMap->m_mapa.m_campos[i].NombreCampo,CAMPOS_LEN);
        strncpy(m_campos_temp[i].ValorActualCampo,memoryMap->m_mapa.m_campos[i].ValorActualCampo,CAMPOS_LEN);*/
        m_campos_temp[i].NombreCampo=memoryMap->m_mapa.m_campos[i].NombreCampo;
        m_campos_temp[i].ValorActualCampo=memoryMap->m_mapa.m_campos[i].ValorActualCampo;

        m_campos_temp[i].OnOffCampo=memoryMap->m_mapa.m_campos[i].OnOffCampo;
        m_campos_temp[i].TamanoFuenteCampo=memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo;
        /*strncpy(m_campos_temp[i].TipoLetraCampo,memoryMap->m_mapa.m_campos[i].TipoLetraCampo,CAMPOS_LEN);
        strncpy(m_campos_temp[i].EstiloLetraCampo,memoryMap->m_mapa.m_campos[i].EstiloLetraCampo,CAMPOS_LEN);*/
        m_campos_temp[i].TipoLetraCampo=memoryMap->m_mapa.m_campos[i].TipoLetraCampo;
        m_campos_temp[i].EstiloLetraCampo=memoryMap->m_mapa.m_campos[i].EstiloLetraCampo;

        m_campos_temp[i].color=memoryMap->m_mapa.m_campos[i].color;
        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            m_campos_temp[i].m_matrices[j].OnOffMatriz=memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz;
            m_campos_temp[i].m_matrices[j].PosicionXCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo;
            m_campos_temp[i].m_matrices[j].PosicionYCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo;
            m_campos_temp[i].m_matrices[j].OrientacionCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
            m_campos_temp[i].m_matrices[j].FilasMatrizCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;
            m_campos_temp[i].m_matrices[j].ColumnasMatrizCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;
            m_campos_temp[i].m_matrices[j].FreqFilasMatrizCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo;
            m_campos_temp[i].m_matrices[j].FreqColumnasMatrizCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo;
            m_campos_temp[i].m_matrices[j].ExcepcionesMatrizCampo=memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo;
        }
    }

}

void DbFormat::PasarDeMapATempVariables(){

    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        m_camposvariables_temp[i].NombreCampo=memoryMap->m_mapa.m_camposvariables[i].NombreCampo;
        m_camposvariables_temp[i].TipoCampo=memoryMap->m_mapa.m_camposvariables[i].TipoCampo;
        m_camposvariables_temp[i].ValorActualCampo=memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo;
        m_camposvariables_temp[i].OnOffCampo=memoryMap->m_mapa.m_camposvariables[i].OnOffCampo;
        m_camposvariables_temp[i].TamanoFuenteCampo=memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo;
        m_camposvariables_temp[i].TipoLetraCampo=memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo;
        m_camposvariables_temp[i].EstiloLetraCampo=memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo;
        m_camposvariables_temp[i].color=memoryMap->m_mapa.m_camposvariables[i].color;
        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            m_camposvariables_temp[i].m_matrices[j].OnOffMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz;
            m_camposvariables_temp[i].m_matrices[j].PosicionXCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo;
            m_camposvariables_temp[i].m_matrices[j].PosicionYCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo;
            m_camposvariables_temp[i].m_matrices[j].OrientacionCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
            m_camposvariables_temp[i].m_matrices[j].FilasMatrizCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].ColumnasMatrizCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].FreqFilasMatrizCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].FreqColumnasMatrizCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo;
            m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo;
        }
    }

}

void DbFormat::PasarDeTempAMap(){

    for (int i=0;i<NUM_CAMPOS;i++){
        /*strncpy(memoryMap->m_mapa.m_campos[i].NombreCampo,m_campos_temp[i].NombreCampo,CAMPOS_LEN);
        strncpy(memoryMap->m_mapa.m_campos[i].ValorActualCampo,m_campos_temp[i].ValorActualCampo,CAMPOS_LEN);*/
        memoryMap->m_mapa.m_campos[i].NombreCampo=m_campos_temp[i].NombreCampo;
        memoryMap->m_mapa.m_campos[i].ValorActualCampo=m_campos_temp[i].ValorActualCampo;

        memoryMap->m_mapa.m_campos[i].OnOffCampo=m_campos_temp[i].OnOffCampo;
        memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo=m_campos_temp[i].TamanoFuenteCampo;
        /*strncpy(memoryMap->m_mapa.m_campos[i].TipoLetraCampo,m_campos_temp[i].TipoLetraCampo,CAMPOS_LEN);
        strncpy(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo,m_campos_temp[i].EstiloLetraCampo,CAMPOS_LEN);*/
        memoryMap->m_mapa.m_campos[i].TipoLetraCampo=m_campos_temp[i].TipoLetraCampo;
        memoryMap->m_mapa.m_campos[i].EstiloLetraCampo=m_campos_temp[i].EstiloLetraCampo;

        memoryMap->m_mapa.m_campos[i].NumCaracteresCampo=m_campos_temp[i].NumCaracteresCampo;
        memoryMap->m_mapa.m_campos[i].color=m_campos_temp[i].color;
        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=m_campos_temp[i].m_matrices[j].OnOffMatriz;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=m_campos_temp[i].m_matrices[j].PosicionXCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=m_campos_temp[i].m_matrices[j].PosicionYCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=m_campos_temp[i].m_matrices[j].OrientacionCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=m_campos_temp[i].m_matrices[j].FilasMatrizCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=m_campos_temp[i].m_matrices[j].ColumnasMatrizCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=m_campos_temp[i].m_matrices[j].FreqFilasMatrizCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=m_campos_temp[i].m_matrices[j].FreqColumnasMatrizCampo;
            memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo=m_campos_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
        }
    }

}



void DbFormat::PasarDeTempAMapVariables(){

    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        memoryMap->m_mapa.m_camposvariables[i].NombreCampo=m_camposvariables_temp[i].NombreCampo;
        memoryMap->m_mapa.m_camposvariables[i].TipoCampo=m_camposvariables_temp[i].TipoCampo;
        memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo=m_camposvariables_temp[i].ValorActualCampo;
        memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=m_camposvariables_temp[i].OnOffCampo;
        memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo=m_camposvariables_temp[i].TamanoFuenteCampo;
        memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo=m_camposvariables_temp[i].TipoLetraCampo;
        memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo=m_camposvariables_temp[i].EstiloLetraCampo;
        memoryMap->m_mapa.m_camposvariables[i].NumCaracteresCampo=m_campos_temp[i].NumCaracteresCampo;
        memoryMap->m_mapa.m_camposvariables[i].color=m_camposvariables_temp[i].color;
        for(int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=m_camposvariables_temp[i].m_matrices[j].OnOffMatriz;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo=m_camposvariables_temp[i].m_matrices[j].PosicionXCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo=m_camposvariables_temp[i].m_matrices[j].PosicionYCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo=m_camposvariables_temp[i].m_matrices[j].OrientacionCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FilasMatrizCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ColumnasMatrizCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FreqFilasMatrizCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo=m_camposvariables_temp[i].m_matrices[j].FreqColumnasMatrizCampo;
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
            //memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo=m_camposvariables_temp[i].m_matrices[j].ExcepcionesMatrizCampo;
        }
    }

}

//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void DbFormat::AceptarSeleccion(){

	unsigned char indice;
    unsigned char indice_matriz;
    unsigned char indice_color;

    indice=ui->m_NumeroCampo->currentIndex();
    indice_matriz=ui->m_NumeroMatriz->currentIndex();
    indice_color=ui->m_ColorCampo->currentIndex();

    if (indice<=NUM_CAMPOS-1){
        /*strncpy(memoryMap->m_mapa.m_campos[indice].TipoLetraCampo,ui->m_Fuente->currentText().toAscii().data(),CAMPOS_LEN);
        strncpy(memoryMap->m_mapa.m_campos[indice].EstiloLetraCampo,ui->m_Estilo->currentText().toAscii().data(),CAMPOS_LEN);*/
        memoryMap->m_mapa.m_campos[indice].TipoLetraCampo=ui->m_Fuente->currentText();
        memoryMap->m_mapa.m_campos[indice].EstiloLetraCampo=ui->m_Estilo->currentText();

        memoryMap->m_mapa.m_campos[indice].TamanoFuenteCampo=ui->m_Tamano->toPlainText().toInt();
        //#ifdef COLOR_PLENOS
        //if (memoryMap->m_mapa.m_variables.NumColores>1){
            memoryMap->m_mapa.m_campos[indice].color=indice_color+1;
        //}
        //#endif

        if (ui->m_MatrizOnOff->isChecked()) memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OnOffMatriz=1;
        else memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OnOffMatriz=0;
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].PosicionXCampo=ui->m_CampoHorizontal->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].PosicionYCampo=ui->m_CampoVertical->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->toPlainText().toInt();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->toPlainText();
    }
    else{
        indice=indice-NUM_CAMPOS;
        memoryMap->m_mapa.m_camposvariables[indice].TipoLetraCampo=ui->m_Fuente->currentText().toAscii().data();
        memoryMap->m_mapa.m_camposvariables[indice].EstiloLetraCampo=ui->m_Estilo->currentText().toAscii().data();
        memoryMap->m_mapa.m_camposvariables[indice].TamanoFuenteCampo=ui->m_Tamano->toPlainText().toInt();
        //#ifdef COLOR_PLENOS
        if (memoryMap->m_mapa.m_variables.NumColores>1){
            memoryMap->m_mapa.m_camposvariables[indice].color=(ui->m_ColorCampo->currentIndex())+1;
        }
        //#endif

        if (ui->m_MatrizOnOff->isChecked()) memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OnOffMatriz=1;
        else memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OnOffMatriz=0;
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionXCampo=ui->m_CampoHorizontal->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionYCampo=ui->m_CampoVertical->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->toPlainText();
        //memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=m_ExcepcionesMatriz->toPlainText().toAscii().data();
    }

    LaunchDbImpresion();

}


void DbFormat::CancelarSeleccion(){

    /*PasarDeMapATemp();
    if (memoryMap->ImagenVariable==1) PasarDeMapATempVariables();*/

    LaunchDbImpresion();
}


// Actualiza los datos de pantalla
void DbFormat::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}


void DbFormat::UpdateNumeroCampo(){
    unsigned char indice=0;
    unsigned char indice_matriz=0;
    unsigned char indice_color=0;
    int indice_selecc=0;
    QPalette dummyPaletteDisabled=GetPaletaTextEditDisabled();
    indice=ui->m_NumeroCampo->currentIndex();
    indice_matriz=ui->m_NumeroMatriz->currentIndex();
    ui->m_OrigenCampo->setText("");

    if (indice<=NUM_CAMPOS-1){
        //Campos fijos
        ui->m_NombreCampo->setText(QString(memoryMap->m_mapa.m_campos[indice].NombreCampo));
        ui->m_Fuente->setCurrentFont(QString(memoryMap->m_mapa.m_campos[indice].TipoLetraCampo));
        ui->m_Tamano->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].TamanoFuenteCampo));

        if (!memoryMap->m_mapa.m_campos[indice].TipoCampo.compare(TXT_CAMPO_TIPO_TEXTO)) ui->m_TipoCampo->setCurrentIndex(0);
        else if (!memoryMap->m_mapa.m_campos[indice].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)) ui->m_TipoCampo->setCurrentIndex(1);
        else if (!memoryMap->m_mapa.m_campos[indice].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)) ui->m_TipoCampo->setCurrentIndex(2);
        else if (!memoryMap->m_mapa.m_campos[indice].TipoCampo.compare(TXT_CAMPO_TIPO_IDEOGRAMA)) ui->m_TipoCampo->setCurrentIndex(3);
        //else if (!memoryMap->m_mapa.m_campos[indice].TipoCampo.compare("Contador")) ui->m_TipoCampo->setCurrentIndex(4);

        UpdateTipoCampo();

        indice_selecc=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_campos[indice].EstiloLetraCampo));
        indice_color=memoryMap->m_mapa.m_campos[indice].color;
        ui->m_ColorCampo->setCurrentIndex(indice_color-1);
        ui->m_Estilo->setCurrentIndex(indice_selecc);
        QPalette paleta_color_pleno;
        paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[indice_color-1].valor));
        ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);

        if (memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
        else ui->m_MatrizOnOff->setChecked(false);
        ui->m_CampoHorizontal->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].PosicionXCampo));
        ui->m_CampoVertical->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].PosicionYCampo));
        ui->m_Orientacion->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OrientacionCampo));
        ui->m_FilasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FilasMatrizCampo));
        ui->m_ColumnasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].ColumnasMatrizCampo));
        ui->m_FrecuenciaFilasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo));
        ui->m_FrecuenciaColumnasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo));
        ui->m_ExcepcionesMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo));

		if (memoryMap->m_mapa.m_campos[indice].OnOffCampo==0x00){
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
            if (memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
            else ui->m_MatrizOnOff->setChecked(false);
            UpdateOnOffMatriz();
            ui->m_Tamano->setEnabled(true);
            ui->m_Fuente->setEnabled(true);
            ui->m_Estilo->setEnabled(true);
            ui->m_AceptarValores->setEnabled(true);
            ui->m_NombreCampo->setEnabled(true);
            ui->m_ColorCampo->setEnabled(true);
		}
    }
    else{
        //Campos variables
        indice=indice-NUM_CAMPOS;
        ui->m_NombreCampo->setText(QString(memoryMap->m_mapa.m_camposvariables[indice].NombreCampo));
        ui->m_Fuente->setCurrentFont(QString(memoryMap->m_mapa.m_camposvariables[indice].TipoLetraCampo));
        ui->m_Tamano->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].TamanoFuenteCampo));
        ui->m_OrigenCampo->setText(QString(memoryMap->m_mapa.m_camposvariables[indice].OrigenValorCampo));

        if (!memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare(TXT_CAMPO_TIPO_TEXTO)) ui->m_TipoCampo->setCurrentIndex(0);
        else if (!memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)) ui->m_TipoCampo->setCurrentIndex(1);
        else if (!memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)) ui->m_TipoCampo->setCurrentIndex(2);
        else if (!memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare(TXT_CAMPO_TIPO_IDEOGRAMA)) ui->m_TipoCampo->setCurrentIndex(3);
        //else if (!memoryMap->m_mapa.m_camposvariables[indice].TipoCampo.compare("Contador")) ui->m_TipoCampo->setCurrentIndex(4);

        UpdateTipoCampo();

        indice_selecc=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_camposvariables[indice].EstiloLetraCampo));
        ui->m_ColorCampo->setCurrentIndex((memoryMap->m_mapa.m_camposvariables[indice].color)-1);
        ui->m_Estilo->setCurrentIndex(indice_selecc);

        /*if (int x=ui->m_TipoCampo->findText(memoryMap->m_mapa.m_camposvariables[indice].TipoCampo)!=-1)
            ui->m_TipoCampo->setCurrentIndex(x);*/

        if (memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
        else ui->m_MatrizOnOff->setChecked(false);
        ui->m_CampoHorizontal->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionXCampo));
        ui->m_CampoVertical->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionYCampo));
        ui->m_Orientacion->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OrientacionCampo));
        ui->m_FilasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FilasMatrizCampo));
        ui->m_ColumnasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ColumnasMatrizCampo));
        ui->m_FrecuenciaFilasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo));
        ui->m_FrecuenciaColumnasMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo));
        ui->m_ExcepcionesMatriz->setText(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo));

        if (memoryMap->m_mapa.m_camposvariables[indice].OnOffCampo==0x00){
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
            if (memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OnOffMatriz==1) ui->m_MatrizOnOff->setChecked(true);
            else ui->m_MatrizOnOff->setChecked(false);
            UpdateOnOffMatriz();
            ui->m_Tamano->setEnabled(true);
            ui->m_Fuente->setEnabled(true);
            ui->m_Estilo->setEnabled(true);
            ui->m_AceptarValores->setEnabled(true);
            ui->m_NombreCampo->setEnabled(true);
            ui->m_OrigenCampo->setEnabled(true);
            ui->m_TipoCampo->setEnabled(true);
            ui->m_ColorCampo->setEnabled(true);
        }

    }
	TestRoot();
}

void DbFormat::UpdateNumeroMatriz(){

    //QString dummy;

    UpdateNumeroCampo();
}

void DbFormat::UpdateCampoHorizontal(){
    InputData *input = InputData::request(this, "Posicion X Campo [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateHorizontal(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateHorizontal(QString value){
    ui->m_CampoHorizontal->setText(value);
    ui->m_CampoHorizontal->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormat::UpdateCampoVertical(){
    InputData *input = InputData::request(this,"Posicion Y Campo [mm]");
    if (input) {
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateVertical(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateVertical(QString value){
    ui->m_CampoVertical->setText(value);
    ui->m_CampoVertical->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormat::UpdateTamanoFuente(){
    InputData *input = InputData::request(this, "Tamaño Fuente [ptos]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateTamano(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateTamano(QString value){
    ui->m_Tamano->setText(value);
    ui->m_Tamano->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormat::UpdateOrientacionFuente(){
    InputData *input = InputData::request(this,"Orientacion Fuente");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateOrientacion(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateOrientacion(QString value){
    ui->m_Orientacion->setText(value);
    ui->m_Orientacion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormat::UpdateFilasMatriz(){
    InputData *input = InputData::request(this,"Filas matriz");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFilas(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateFilas(QString value){
    ui->m_FilasMatriz->setText(value);
    ui->m_FilasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormat::UpdateColumnasMatriz(){
    InputData *input = InputData::request(this,"Columnas matriz");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateColumnas(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateColumnas(QString value){
    ui->m_ColumnasMatriz->setText(value);
    ui->m_ColumnasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void DbFormat::UpdateFrecuenciaFilasMatriz(){
    InputData *input = InputData::request(this,"Frecuencia Filas matriz [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFrecuenciaFilas(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateFrecuenciaFilas(QString value){
    ui->m_FrecuenciaFilasMatriz->setText(value);
    ui->m_FrecuenciaFilasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormat::UpdateFrecuenciaColumnasMatriz(){
    InputData *input = InputData::request(this,"Frecuencia Columnas matriz [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateFrecuenciaColumnas(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateFrecuenciaColumnas(QString value){
    ui->m_FrecuenciaColumnasMatriz->setText(value);
    ui->m_FrecuenciaColumnasMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormat::UpdateExcepcionesMatriz(){
    InputData *input = InputData::request(this,"Excepciones matriz");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateExcepciones(QString)));
        input->setFocus();
    }
}

void DbFormat::UpdateExcepciones(QString value){
    ui->m_ExcepcionesMatriz->setText(value);
    ui->m_ExcepcionesMatriz->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void DbFormat::UpdateOnOffMatriz(){
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
    }

}

void DbFormat::UpdateColorCampo(){
    unsigned char indice;
    indice=ui->m_ColorCampo->currentIndex();
    QPalette paleta_color_pleno;
    paleta_color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[indice].valor));
    ui->m_ColorSeleccionado->setPalette(paleta_color_pleno);
}

/** Actualiza los datos del item en el mapa de memoria con los datos de pantalla*/
void DbFormat::ActualizaDatosItem(){
    unsigned char indice;
    unsigned char indice_matriz;
    indice=ui->m_NumeroCampo->currentIndex();
    indice_matriz=ui->m_NumeroMatriz->currentIndex();

    if (indice<=NUM_CAMPOS-1){
        //qDebug() << "Es un campo fijo";
        memoryMap->m_mapa.m_campos[indice].TamanoFuenteCampo=ui->m_Tamano->toPlainText().toInt();
        /*strncpy(memoryMap->m_mapa.m_campos[indice].TipoLetraCampo,ui->m_Fuente->currentText().toAscii().data(),CAMPOS_LEN);
        strncpy(memoryMap->m_mapa.m_campos[indice].EstiloLetraCampo,ui->m_Estilo->currentText().toAscii().data(),CAMPOS_LEN);*/
        memoryMap->m_mapa.m_campos[indice].TipoLetraCampo=ui->m_Fuente->currentText();
        memoryMap->m_mapa.m_campos[indice].EstiloLetraCampo=ui->m_Estilo->currentText();

        //#ifdef COLOR_PLENOS
        if (memoryMap->m_mapa.m_variables.NumColores>1){
            memoryMap->m_mapa.m_campos[indice].color=(ui->m_ColorCampo->currentIndex())+1;
        }
        //#endif

        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].PosicionXCampo=ui->m_CampoHorizontal->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].PosicionYCampo=ui->m_CampoVertical->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->toPlainText().toInt();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->toPlainText();

        if (ui->m_MatrizOnOff->isChecked()) memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OnOffMatriz=1;
        else memoryMap->m_mapa.m_campos[indice].m_matrices[indice_matriz].OnOffMatriz=0;
        memoryMap->m_mapa.m_campos[indice].TipoCampo=ui->m_TipoCampo->currentText();
    }
    else{
        //qDebug() << "Es un campo variable";
        indice=indice-NUM_CAMPOS;
        memoryMap->m_mapa.m_camposvariables[indice].TamanoFuenteCampo=ui->m_Tamano->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].TipoLetraCampo=ui->m_Fuente->currentText().toAscii().data();
        memoryMap->m_mapa.m_camposvariables[indice].EstiloLetraCampo=ui->m_Estilo->currentText().toAscii().data();
        //#ifdef COLOR_PLENOS
        if (memoryMap->m_mapa.m_variables.NumColores>1){
            memoryMap->m_mapa.m_camposvariables[indice].color=(ui->m_ColorCampo->currentIndex())+1;
        }
        //#endif

        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionXCampo=ui->m_CampoHorizontal->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].PosicionYCampo=ui->m_CampoVertical->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OrientacionCampo=ui->m_Orientacion->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FilasMatrizCampo=ui->m_FilasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ColumnasMatrizCampo=ui->m_ColumnasMatriz->toPlainText().toInt();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FreqFilasMatrizCampo=ui->m_FrecuenciaFilasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].FreqColumnasMatrizCampo=ui->m_FrecuenciaColumnasMatriz->toPlainText().toDouble();
        memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].ExcepcionesMatrizCampo=ui->m_ExcepcionesMatriz->toPlainText();

        if (ui->m_MatrizOnOff->isChecked()) memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OnOffMatriz=1;
        else memoryMap->m_mapa.m_camposvariables[indice].m_matrices[indice_matriz].OnOffMatriz=0;
        //Vamos a utilizarlo de momento en los campos variables.
        memoryMap->m_mapa.m_camposvariables[indice].TipoCampo=ui->m_TipoCampo->currentText();

    }
}
/** Mueve abajo*/
void DbFormat::sltMoveDown(){
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoVertical->toPlainText().toFloat();
        /*if (dummy<ui->m_ImagenUnitaria->height())*/ dummy+=MOVE_STEP;
        ui->m_CampoVertical->setText(QString("%1").arg(dummy));
        AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning","El item no está habilitado",QMessageBox::Ok);
}
/** Mueve arriba*/
void DbFormat::sltMoveUp(){
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoVertical->toPlainText().toFloat();
        if (dummy>=MOVE_STEP*1.5) dummy-=MOVE_STEP;
        ui->m_CampoVertical->setText(QString("%1").arg(dummy));
        AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning","El item no está habilitado",QMessageBox::Ok);
}
/** Mueve derecha*/
void DbFormat::sltMoveRight(){
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoHorizontal->toPlainText().toFloat();
        /*if (dummy<ui->m_ImagenUnitaria->width())*/  dummy+=MOVE_STEP;
        ui->m_CampoHorizontal->setText(QString("%1").arg(dummy));
        AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning","El item no está habilitado",QMessageBox::Ok);
}
/** Mueve izquierda*/
void DbFormat::sltMoveLeft(){
    if (ui->m_MatrizOnOff->isChecked()){
        float dummy=ui->m_CampoHorizontal->toPlainText().toFloat();
        if (dummy>=MOVE_STEP*1.5) dummy-=MOVE_STEP;
        ui->m_CampoHorizontal->setText(QString("%1").arg(dummy));
        AceptarValoresCampos();
    }
    else QMessageBox::warning(0,"Warning","El item no está habilitado",QMessageBox::Ok);
}



void DbFormat::AceptarValoresCampos(){
	
    QLabel *dummyLabel= ShowMensaje("Rendering");
    ui->m_AceptarButton->setEnabled(false);
    ui->m_CancelButton->setEnabled(false);

    ActualizaDatosItem();

    rotatedpixmap.load(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
    if (memoryMap->ImagenVariable==0x00) CreaImagenMuestra();
    else CreaImagenMuestraVariable();
    MuestraImagen(rotatedpixmap);

    ui->m_AceptarButton->setEnabled(true);
    ui->m_CancelButton->setEnabled(true);
    delete (dummyLabel);

}

void DbFormat::LaunchDbImpresion(){
    DbImpresion *ScrDbImpresion;
    ScrDbImpresion=new DbImpresion(0);
    ScrDbImpresion->show();
    this->deleteLater();
}



//Crea cada una de las imagenes que forma la impresion
void DbFormat::CreaImagenMuestraVariable(){

	unsigned char tamano_fuente_campo;
    unsigned char i,j;
	unsigned int Xtemp,Ytemp;
	unsigned int Xotemp,Yotemp;
	unsigned int dix,diy;
	unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
	QString Vtemp;
	QWidget dummyWidget;

    //qDebug() << "CreaImagenMuestraVariable";

    QPixmap pic(rotatedpixmap.size());
	QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);

    p.drawPixmap(0,0,rotatedpixmap); // and draw your original pixmap on it
	p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

	QFont mifont;
    int indice;

    QStringList lista_excepciones;
    unsigned char indice_color;

    miEstiloContador=new counterStyle();
    QString dummy;

	for (i=0;i<NUM_CAMPOS;i++){
		if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
            for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
                    //qDebug()<< "Dibujando";
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                    tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);

                    Vtemp=QString(memoryMap->m_mapa.m_campos[i].ValorActualCampo);

                    indice=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo));
                    peso_campo=EstiloFuenteQt.at(indice);
                    mifont.setPixelSize(tamano_fuente_campo);
                    mifont.setStyleStrategy(QFont::PreferAntialias);
                    mifont.setWeight(peso_campo);
                    mifont.setFamily(memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
                    p.setFont(mifont);

                    if (memoryMap->m_mapa.m_variables.NumColores==1){
                        p.setPen(Qt::black);
                    }
                    else{
                        indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                        p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
                    }
                    Ytemp=Yotemp;
                    celda=1;
                    for (unsigned int yy=0;yy<memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;yy++){
                        Xtemp=Xotemp;
                        for (unsigned int xx=0;xx<memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;xx++){
                            celda_s=QString("%1").arg(celda);
                            esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                            if (esunaexcepcion==false){
                                p.save();
                                //Ojo, aqui hacemos codigo de barras!!!
                                if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                    //Nos movemos menos...
                                    p.translate(Xtemp,Ytemp);
                                    p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);

                                    struct m_BarcodeParameters bc=memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                    In2BarcodeClass* dummybc=new In2BarcodeClass(bc);
                                    dummybc->SetPixmap(Vtemp);
                                    p.drawPixmap(0,0,dummybc->qpxImagen);
                                }
                                else if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
                                    //Nos movemos menos...
                                    p.translate(Xtemp,Ytemp);
                                    p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);

                                    struct m_BarcodeParameters bc=memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                    In2BarcodeClass* dummybc=new In2BarcodeClass(bc);
                                    dummybc->SetPixmap(Vtemp);
                                    p.drawPixmap(0,0,dummybc->qpxImagen);
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
                        }
                        Ytemp+=diy;
                    }
                } //if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS;i++){
 	

    //Ahora metemos los campos variables

    int offset_columna[NUM_CAMPOS_VARIABLES];
    int valor_inicial_fila=0;
    long incremento[NUM_CAMPOS_VARIABLES];
    long valor_actual=0;

    for (i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            offset_columna[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
            incremento[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.incremento;
            valor_inicial_fila=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial;
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual=valor_inicial_fila;
            for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                    //qDebug()<< "Dibujando";
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                    tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);

                    indice=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo));
                    if (indice>=0) peso_campo=EstiloFuenteQt.at(indice);
                    mifont.setPixelSize(tamano_fuente_campo);
                    mifont.setStyleStrategy(QFont::PreferAntialias);
                    mifont.setWeight(peso_campo);
                    mifont.setFamily(memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo);
                    p.setFont(mifont);

                    if (memoryMap->m_mapa.m_variables.NumColores==1){
                        p.setPen(Qt::black);
                    }
                    else{
                        indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                        p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
                    }

                    valor_actual=valor_inicial_fila;
                    Ytemp=Yotemp;
                    celda=1;
                    for (unsigned int yy=0;yy<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;yy++){
                        Xtemp=Xotemp;
                        if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS) valor_actual=valor_inicial_fila;
                        for (unsigned int xx=0;xx<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;xx++){
                            celda_s=QString("%1").arg(celda);
                            esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                            if (esunaexcepcion==false){
                                p.save();

                                if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                    if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                                        memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual=valor_actual;
                                        int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                        Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);
                                        valor_actual+=offset_columna[i];
                                    }
                                    else if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                                        //valor_actual=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual;
                                        memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual=valor_actual;
                                        int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                        Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);
                                        valor_actual+=incremento[i];
                                    }
                                    //dummy_index++;

                                }
                                else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                    QDateTime fechahora;
                                    QString sFechaHora;
                                    sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato));
                                    Vtemp=QString("%1").arg(sFechaHora);
                                }
                                else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                    int limiteInferior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                                    int limiteSuperior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                                    int numeroCifras=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                                    int valorAleatorioActual=generarNumeroAleatorio(limiteInferior,limiteSuperior);
                                    Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
                                }
                                else{
                                    Vtemp=QString(memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo);
                                }

                                if (Vtemp.isEmpty()) Vtemp=QString("Variable%1").arg(i+1);
                                qDebug() << "Valor campo: " << Vtemp;

                                //Ojo, aqui hacemos codigo de barras!!!
                                if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                    //Nos movemos menos...
                                    p.translate(Xtemp,Ytemp);
                                    p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                    struct m_BarcodeParameters bc=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                    /**In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
                                    dummybc->SetSymbol(BARCODE_CODE128);*/
                                    In2BarcodeClass* dummybc=new In2BarcodeClass(bc);
                                    dummybc->SetPixmap(Vtemp);
                                    p.drawPixmap(0,0,dummybc->qpxImagen);
                                }
                                else if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
                                    //Nos movemos menos...
                                    p.translate(Xtemp,Ytemp);
                                    p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                    struct m_BarcodeParameters bc=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                    /**In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
                                    dummybc->SetSymbol(BARCODE_CODE128);*/
                                    In2BarcodeClass* dummybc=new In2BarcodeClass(bc);
                                    dummybc->SetPixmap(Vtemp);
                                    //In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
                                    //dummybc->SetSymbol(BARCODE_DATAMATRIX);
                                    //dummybc->SetPixmap(Vtemp);
                                    p.drawPixmap(0,0,dummybc->qpxImagen);
                                }
                                else {
                                    p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                    p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                    //p.drawText(0,0,Vtemp);
                                    struct FormatoNumero f=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format;
                                    dummy=miEstiloContador->darformato(memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual,f);
                                    p.drawText(0,0,dummy);
                                }
                                //}

                                p.restore();
                            }
                            Xtemp+=dix;
                            celda++;

                        } //xx--> columnas
                        Ytemp+=diy;
                        valor_inicial_fila+=incremento[i];
                        memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila=valor_inicial_fila;
                    } // yy --> filas
                } //if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){

            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS_VARIABLES;i++){


	p.end();//01122011 Esta linea puede arrglar el problema de los puntos

    rotatedpixmap=pic.transformed(QTransform().rotate(0,Qt::ZAxis));

    //rotatedpixmap.save("c:\\barcode.bmp","BMP");

}

/** Dibujo un item individual*/
QPixmap DbFormat::DrawFixedItem(int i){
    unsigned char tamano_fuente_campo;
    unsigned char j=0;
    unsigned int Xtemp,Ytemp;
    unsigned int celda;
    QString celda_s;
    QString Vtemp;

    QFont mifont;
    int indice;

    unsigned char indice_color;

//qDebug()<< "Dibujando";
    Xtemp=0;
    Ytemp=0;
    tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);
    Vtemp=QString(memoryMap->m_mapa.m_campos[i].ValorActualCampo);
    //if (Vtemp.isEmpty()) Vtemp=QString("Fixed%1").arg(i+1);
    indice=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo));
    peso_campo=EstiloFuenteQt.at(indice);
    mifont.setPixelSize(tamano_fuente_campo);
    mifont.setStyleStrategy(QFont::PreferAntialias);
    mifont.setWeight(peso_campo);
    mifont.setFamily(memoryMap->m_mapa.m_campos[i].TipoLetraCampo);

    QFontMetrics fontMetric(mifont);
    QSize dummySize(fontMetric.width(Vtemp),fontMetric.height());

    if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
        dummySize.setWidth(memoryMap->m_mapa.m_campos[i].m_barcodeParameters.width);
        dummySize.setHeight(memoryMap->m_mapa.m_campos[i].m_barcodeParameters.height);
    }
    else if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
       dummySize.setWidth(memoryMap->m_mapa.m_campos[i].m_barcodeParameters.width);
       dummySize.setHeight(memoryMap->m_mapa.m_campos[i].m_barcodeParameters.height);
    }


    QPixmap pic(dummySize);
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::white);
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
    p.setFont(mifont);

    if (memoryMap->m_mapa.m_variables.NumColores==1){
        p.setPen(Qt::black);
    }
    else{
        indice_color= memoryMap->m_mapa.m_campos[i].color-1;
        p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
    }

    celda=1;
    celda_s=QString("%1").arg(celda);
    p.save();
    //Ojo, aqui hacemos codigo de barras!!!
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
    p.end();//01122011 Esta linea puede arrglar el problema de los puntos

    //En pic tengo la imagen a tamaño completo...testeamos posibles escalados

    int lw=ui->m_ImagenUnitaria->width();
    int lh=ui->m_ImagenUnitaria->height();
    int iw=rotatedpixmap.width();
    int ih=rotatedpixmap.height();
    //Nos salimos si la imagen tiene dimensiones erroneas
    if ((lw==0)||(lh==0)) return pic;

    unsigned char escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);
    QPixmap dummy;
    dummy.fill(Qt::transparent);

    float factor=1;
    switch (escalado){
    case IMAGEN_SINESCALAR:
        dummy=pic;
        break;
    case IMAGEN_S2W:
        factor=pic.width()*((float)ui->m_ImagenUnitaria->width()/(float)rotatedpixmap.width());
        dummy=pic.scaledToWidth(factor,Qt::SmoothTransformation);
        break;
    case IMAGEN_S2H:
        factor=pic.height()*((float)ui->m_ImagenUnitaria->height()/(float)rotatedpixmap.height());
        dummy=pic.scaledToHeight(factor,Qt::SmoothTransformation);
        break;
    }

    return dummy;


}


/** Dibujo un item individual*/
QPixmap DbFormat::DrawVariableItem(int i){
    unsigned char tamano_fuente_campo;
    unsigned char j=0;
    unsigned int Xtemp,Ytemp;
    unsigned int celda;
    QString celda_s;
    QString Vtemp;

    QFont mifont;
    int indice;

    unsigned char indice_color;

//qDebug()<< "Dibujando";
    Xtemp=0;
    Ytemp=0;
    tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
    Vtemp=QString(memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo);
    if (Vtemp.isEmpty()) Vtemp=QString("Variable%1").arg(i+1);
    indice=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo));
    peso_campo=EstiloFuenteQt.at(indice);
    mifont.setPixelSize(tamano_fuente_campo);
    mifont.setStyleStrategy(QFont::PreferAntialias);
    mifont.setWeight(peso_campo);
    mifont.setFamily(memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo);

    QFontMetrics fontMetric(mifont);
    QSize dummySize(fontMetric.width(Vtemp),fontMetric.height());
    QPixmap pic(dummySize);
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::white);
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
    p.setFont(mifont);

    if (memoryMap->m_mapa.m_variables.NumColores==1){
        p.setPen(Qt::black);
    }
    else{
        indice_color= memoryMap->m_mapa.m_camposvariables[i].color-1;
        p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
    }

    celda=1;
    celda_s=QString("%1").arg(celda);
    p.save();
    //Ojo, aqui hacemos codigo de barras!!!
    if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                    //Nos movemos menos...
        p.translate(Xtemp,Ytemp);
        p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
        In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
        dummybc->SetSymbol(BARCODE_CODE128);
        dummybc->SetPixmap(Vtemp);
        p.drawPixmap(0,0,dummybc->qpxImagen);
    }
    else if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
                                    //Nos movemos menos...
        p.translate(Xtemp,Ytemp);
        p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
        In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
        dummybc->SetSymbol(BARCODE_DATAMATRIX);
        dummybc->SetPixmap(Vtemp);
        p.drawPixmap(0,0,dummybc->qpxImagen);
    }
    else{
        p.translate(Xtemp,Ytemp+tamano_fuente_campo);
        p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
        p.drawText(0,0,Vtemp);
    }
    p.restore();
    p.end();//01122011 Esta linea puede arrglar el problema de los puntos

    //En pic tengo la imagen a tamaño completo...testeamos posibles escalados

    int lw=ui->m_ImagenUnitaria->width();
    int lh=ui->m_ImagenUnitaria->height();
    int iw=rotatedpixmap.width();
    int ih=rotatedpixmap.height();
    //Nos salimos si la imagen tiene dimensiones erroneas
    if ((lw==0)||(lh==0)) return pic;

    unsigned char escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);
    QPixmap dummy;
    dummy.fill(Qt::transparent);

    float factor=1;
    switch (escalado){
    case IMAGEN_SINESCALAR:
        dummy=pic;
        break;
    case IMAGEN_S2W:
        factor=pic.width()*((float)ui->m_ImagenUnitaria->width()/(float)rotatedpixmap.width());
        dummy=pic.scaledToWidth(factor,Qt::SmoothTransformation);
        break;
    case IMAGEN_S2H:
        factor=pic.height()*((float)ui->m_ImagenUnitaria->height()/(float)rotatedpixmap.height());
        dummy=pic.scaledToHeight(factor,Qt::SmoothTransformation);
        break;
    }

    return dummy;

}



//Crea cada una de las imagenes que forma la impresion
void DbFormat::CreaImagenMuestra(){

    unsigned char tamano_fuente_campo;
    unsigned char i,j;
    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    unsigned int dix,diy;
    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QString Vtemp;
    QWidget dummyWidget;

    QPixmap pic(rotatedpixmap.size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);

    p.drawPixmap(0,0,rotatedpixmap); // and draw your original pixmap on it
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

    QFont mifont;
    int indice;

    QStringList lista_excepciones;
    unsigned char indice_color;

    for (i=0;i<NUM_CAMPOS;i++){
        if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
            for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
                    //qDebug()<< "Dibujando";
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                    tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);                    
                    Vtemp=QString(memoryMap->m_mapa.m_campos[i].ValorActualCampo);
                    indice=ui->m_Estilo->findText(QString(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo));
                    peso_campo=EstiloFuenteQt.at(indice);
                    mifont.setPixelSize(tamano_fuente_campo);
                    mifont.setStyleStrategy(QFont::PreferAntialias);
                    mifont.setWeight(peso_campo);
                    mifont.setFamily(memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
                    p.setFont(mifont);


                    if (memoryMap->m_mapa.m_variables.NumColores==1){
                        p.setPen(Qt::black);
                    }
                    else{
                        indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                        p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
                    }

                    Ytemp=Yotemp;
                    celda=1;
                    for (unsigned int yy=0;yy<memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;yy++){
                        Xtemp=Xotemp;
                        for (unsigned int xx=0;xx<memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;xx++){
                            celda_s=QString("%1").arg(celda);
                            esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                            if (esunaexcepcion==false){
                                /*p.save();
                                p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                p.drawText(0,0,Vtemp);
                                p.restore();*/
                                p.save();
                                //Ojo, aqui hacemos codigo de barras!!!
                                if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                    //Nos movemos menos...
                                    p.translate(Xtemp,Ytemp);
                                    p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                    //In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
                                    In2BarcodeClass *dummybc=new In2BarcodeClass(true,i);
                                    //dummybc->SetSymbol(BARCODE_CODE128);
                                    dummybc->SetPixmap(Vtemp);
                                    p.drawPixmap(0,0,dummybc->qpxImagen);
                                }
                                else if (!memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_DATAMATRIX)){
                                    //Nos movemos menos...
                                    p.translate(Xtemp,Ytemp);
                                    p.rotate(memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                    //In2Barcode *dummybc=new In2Barcode(2*tamano_fuente_campo,tamano_fuente_campo);
                                    In2BarcodeClass *dummybc=new In2BarcodeClass(true,i);
                                    //dummybc->SetSymbol(BARCODE_DATAMATRIX);
                                    dummybc->SetPixmap(Vtemp);
                                    p.drawPixmap(0,0,dummybc->qpxImagen);
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
                        }
                        Ytemp+=diy;
                    }
                } //if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS;i++){

    p.end();//01122011 Esta linea puede arrglar el problema de los puntos

    rotatedpixmap=pic.transformed(QTransform().rotate(0,Qt::ZAxis));

}


//Muestra la imagen central
void DbFormat::MuestraImagen(QPixmap Imagen){


        int lw=ui->m_ImagenUnitaria->width();
        int lh=ui->m_ImagenUnitaria->height();
        int iw=Imagen.width();
        int ih=Imagen.height();
        //Nos salimos si la imagen tiene dimensiones erroneas
        if ((lw==0)||(lh==0)) return;

        unsigned char escalado=EscaladoDeImagenEnQLabel(lw,lh,iw,ih);

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


void DbFormat::ConfigurarOrigen(void){

    QString texto=ui->m_OrigenCampo->toPlainText();

    if (!texto.compare(TXT_ORIGEN_CONTADOR))
      ConfigurarContador();

    if (!texto.compare(TXT_ORIGEN_FECHA))
      ConfigurarFechaHora();

    if (!texto.compare(TXT_ORIGEN_RANDOM))
      ConfigurarRandom();
}



void DbFormat::ConfigurarElemento(void){

    int indice=ui->m_TipoCampo->currentIndex();
    QString texto=ui->m_TipoCampo->itemText(indice);

    /*ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_TEXTO);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_BARCODE);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_DATAMATRIX);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_IDEOGRAMA);
    ui->m_TipoCampo->addItem(TXT_CAMPO_TIPO_CONTADOR);*/

    if ( (!texto.compare(TXT_CAMPO_TIPO_DATAMATRIX)) || (!texto.compare(TXT_CAMPO_TIPO_BARCODE))  )
        ConfigurarCodigoBarras();

}


void DbFormat::ConfigurarCodigoBarras(void){
    struct m_BarcodeParameters data;
    int i=ui->m_NumeroCampo->currentIndex();
    if (i<NUM_CAMPOS){
        data.NumeroCampo=i;
        data.CampoFijo=0x01;
        data.texto=memoryMap->m_mapa.m_campos[data.NumeroCampo].ValorActualCampo;
    }
    else{
        data.NumeroCampo=i-NUM_CAMPOS;
        data.CampoFijo=0x00;
        data.texto=memoryMap->m_mapa.m_camposvariables[data.NumeroCampo].ValorActualCampo;
    }

    //data.NumeroCampo=ui->m_NumeroCampo->currentIndex();



    formatBarcode *ScrformatBarcode;
    ScrformatBarcode=new formatBarcode(this,data);
    ScrformatBarcode->show();

    QObject::connect(ScrformatBarcode, SIGNAL(sglBarcodeConfigured(const m_BarcodeParameters & )),this, SLOT(sltBarcodeConfigured(const m_BarcodeParameters &)));

}

void DbFormat::UpdateTipoCampo(void){
    //JORGE: 21/10/2013
    //Cambio para que aparezca la pantalla de configuración del código de barras
    int indice_actual_tipo_campo;
    indice_actual_tipo_campo=ui->m_TipoCampo->currentIndex();
    QString texto=ui->m_TipoCampo->itemText(indice_actual_tipo_campo);
    if ( (!texto.compare(TXT_CAMPO_TIPO_DATAMATRIX)) || (!texto.compare(TXT_CAMPO_TIPO_BARCODE))){
        ui->m_ConfigTipo->setEnabled(true);
    }
    else{
       ui->m_ConfigTipo->setEnabled(false);
    }
}


void DbFormat::UpdateOrigenCampo(void){

    QString texto=ui->m_OrigenCampo->toPlainText();
    if ( (!texto.compare(TXT_ORIGEN_CONTADOR)) ||
         (!texto.compare(TXT_ORIGEN_FECHA)) ||
         (!texto.compare(TXT_ORIGEN_RANDOM)) ){
        ui->m_ConfigOrigen->setEnabled(true);
    }
    else{
       ui->m_ConfigOrigen->setEnabled(false);
    }
}




void DbFormat::sltBarcodeConfigured(const m_BarcodeParameters & data){
    struct m_BarcodeParameters  barcode=data;
    int campo=barcode.NumeroCampo;

    if (barcode.CampoFijo==0x01){
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
    else{
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.NumeroCampo=barcode.NumeroCampo;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.width=barcode.width;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.height=barcode.height;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.width_mm=barcode.width_mm;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.height_mm=barcode.height_mm;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.codificacion=barcode.codificacion;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.option2=barcode.option2;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.inputMode=barcode.inputMode;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.texto=barcode.texto;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.whitespace=barcode.whitespace;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.borderWidth=barcode.borderWidth;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.borderType=barcode.borderType;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.hideText=barcode.hideText;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.escala=barcode.escala;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.ar=barcode.ar;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.fontSizeLarge=barcode.fontSizeLarge;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.fontSizeSmall=barcode.fontSizeSmall;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.fontType=barcode.fontType;
        memoryMap->m_mapa.m_camposvariables[campo].m_barcodeParameters.nivelSeguridad=barcode.nivelSeguridad;

    }

}

void DbFormat::ConfigurarContador(void){

    int i=ui->m_NumeroCampo->currentIndex();
    if (i>=NUM_CAMPOS) i=i-NUM_CAMPOS;
    struct m_CounterParameters data=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters;
    formatCounter *ScrformatCounter;
    ScrformatCounter=new formatCounter(this,data);
    ScrformatCounter->show();

    connect(ScrformatCounter, SIGNAL(sglCounterConfigured(const struct m_CounterParameters &)),this, SLOT(sltCounterConfigured(const struct m_CounterParameters &)));

}

void DbFormat::sltCounterConfigured(const struct m_CounterParameters & data){

    int numeroCampo=data.numeroCampo;

    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.valorInicial=data.valorInicial;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.limiteInferior=data.limiteInferior;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.limiteSuperior=data.limiteSuperior;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.incremento=data.incremento;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.offset=data.offset;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.configuracion=data.configuracion;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.indexConfiguracion=data.indexConfiguracion;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.valorInicialFila=data.valorInicial;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.numeroCifras=data.numeroCifras;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.numeroCampo=data.numeroCampo;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.m_format.bFormatEnabled=data.m_format.bFormatEnabled;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.m_format.zeroPadding=data.m_format.zeroPadding;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.m_format.FormatSeparador=data.m_format.FormatSeparador;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.m_format.numeroCifras=data.m_format.numeroCifras;
    /*memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.thousandsSeparator=data.thousandsSeparator;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_counterParameters.decimalSeparator=data.decimalSeparator;*/
}

void DbFormat::sltDateTimeConfigured(const m_DateTimeParameters & data){

    int numeroCampo=ui->m_NumeroCampo->currentIndex();
    if (numeroCampo<NUM_CAMPOS){
        memoryMap->m_mapa.m_campos[numeroCampo].m_dateTimeParameters.formato=data.formato;
        memoryMap->m_mapa.m_campos[numeroCampo].m_dateTimeParameters.numeroCampo=numeroCampo;
    }
    else { //if (i>=NUM_CAMPOS)
        numeroCampo=numeroCampo-NUM_CAMPOS;
        memoryMap->m_mapa.m_camposvariables[numeroCampo].m_dateTimeParameters.formato=data.formato;
        memoryMap->m_mapa.m_camposvariables[numeroCampo].m_dateTimeParameters.numeroCampo=numeroCampo;
    }

}

void DbFormat::sltRandomConfigured(const m_RandomNumberParameters & data){

    int numeroCampo=ui->m_NumeroCampo->currentIndex();

    numeroCampo=numeroCampo-NUM_CAMPOS;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_randomNumberParameters.formato=data.formato;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_randomNumberParameters.limiteInferior=data.limiteInferior;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_randomNumberParameters.limiteSuperior=data.limiteSuperior;
    memoryMap->m_mapa.m_camposvariables[numeroCampo].m_randomNumberParameters.numeroCampo=numeroCampo;

}

void DbFormat::ConfigurarFechaHora(void){

    int i=ui->m_NumeroCampo->currentIndex();
    if (i>=NUM_CAMPOS) i=i-NUM_CAMPOS;
    struct m_DateTimeParameters data=memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters;
    data.numeroCampo=i;
    formatDateTime *ScrformatDateTime;
    ScrformatDateTime=new formatDateTime(this,data);
    ScrformatDateTime->show();

    connect(ScrformatDateTime, SIGNAL(sglDateTimeConfigured(const struct m_DateTimeParameters &)),this, SLOT(sltDateTimeConfigured(const struct m_DateTimeParameters &)));

}

void DbFormat::ConfigurarRandom(void){

    int i=ui->m_NumeroCampo->currentIndex();
    if (i>=NUM_CAMPOS) i=i-NUM_CAMPOS;
    struct m_RandomNumberParameters data=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters;
    data.numeroCampo=i;
    formatRandom *ScrformatRandom;
    ScrformatRandom=new formatRandom(this,data);
    ScrformatRandom->show();

    connect(ScrformatRandom, SIGNAL(sglRandomConfigured(const struct m_RandomNumberParameters &)),this, SLOT(sltRandomConfigured(const struct m_RandomNumberParameters &)));

}
/*
//Crea cada una de las imagenes que forma la impresion
void DbFormat::CreaImagenMuestra(){

    unsigned char tamano_fuente_campo;
    unsigned char i;
    unsigned int Xtemp,Ytemp;
    QString Vtemp;
    //unsigned char *temp;
    QWidget dummyWidget;

    QPixmap pic(rotatedpixmap.size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);

    p.drawPixmap(0,0,rotatedpixmap); // and draw your original pixmap on it
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

    QFont mifont;
    int indice;

    for (i=0;i<NUM_CAMPOS;i++){
        if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
            Xtemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].PosicionXCampo);
            Ytemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_campos[i].PosicionYCampo);
            tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);
            Vtemp=QString(memoryMap->m_mapa.m_campos[i].ValorActualCampo);
            indice=m_Estilo->findText(QString(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo));
            peso_campo=EstiloFuenteQt.at(indice);
            //p.setFont(QFont(memoryMap->m_mapa.m_campos[i].TipoLetraCampo, tamano_fuente_campo,QFont::PreferAntialias));
            mifont.setPixelSize(tamano_fuente_campo);
            mifont.setStyleStrategy(QFont::PreferAntialias);
            mifont.setWeight(peso_campo);
            mifont.setFamily(memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
            p.setFont(mifont);
            p.save();
            p.translate(Xtemp,Ytemp+tamano_fuente_campo);
            p.rotate(memoryMap->m_mapa.m_campos[i].OrientacionCampo);
            p.drawText(0,0,Vtemp);
            p.restore();
        }
    }

    p.end();//01122011 Esta linea puede arrglar el problema de los puntos

    rotatedpixmap=pic.transformed(QTransform().rotate(0,Qt::ZAxis));

}

//return pic;
}
*/
/** Cargamos datos comunes*/
void DbFormat::SetDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=this->ui->m_ImagenUnitaria;
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
