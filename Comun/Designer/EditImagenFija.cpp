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
#include <QDebug>

#include "Composicion.h"
#include "EditImagenFija.h"
#include "in2defines.h"
//#include "globals.h"
//#include "principal.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
//#include "principal.h"
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
#include "in2imagecreator.h"
#include <QToolBar>
#include "inkjetdebug.h"
#include "in2ghostscriptconfig.h"
#include "in2selectpalette.h"
//#include "dbgmem.h"
#include <QSignalMapper>

#define MIN_DISTANCE_COLS 1
#define MAX_DISTANCE_COLS 300

// Define los tabs
#define TAB_IMAGEN 0
#define TAB_PLANTILLA 1

//Columnas de la tabla de color
#define TABCOLOR_COLOR      0
#define TABCOLOR_X          1
#define TABCOLOR_Y          2
#define TABCOLOR_RED        3
#define TABCOLOR_GREEN      4
#define TABCOLOR_BLUE       5
#define TABCOLOR_ENABLED    6
#define TABCOLOR_MULTIPLE   7


//La fila de mascara
#define TABCOLOR_ROWMASK 4

#define MAX_WIDTH_EDIT  50

#define MAX_COLORES 2500

static QLineEdit *m_Xinicial;
static QLineEdit *m_Yinicial;
static QLineEdit *m_Ancho;
static QLineEdit *m_Alto;

//Imagenes central y por colores
#define IMAGEN_COMPLETA 0
#define IMAGEN_COLOR1 1
#define IMAGEN_COLOR2 2
#define IMAGEN_COLOR3 3
#define IMAGEN_COLOR4 4

EditImagenFija::EditImagenFija(QWidget *parent)
    : ui(new Ui::EditImagenFijaUI)
{

     QPalette dummyPaletteEnabled=GetPaletaTextEditEnabled();
     ui->setupUi(this);


     memoryMap=MemoryMap::request(0);
     ui->m_leMessage->setStyleSheet("QLineEdit{background: black;}");



     QToolBar *fileToolBar = this->addToolBar(tr("File"));
     fileToolBar->addAction(QIcon(":res//images//filefind.png"),"Selecciona Imagen",this,SLOT(BuscarPlantilla()));
     fileToolBar->addAction(QIcon(":res//images//editcut.png"),"Corta imagen",this,SLOT(SelCorteImagen()));
     //fileToolBar->addAction(QIcon(":res//images//reload.png"),"Reload",this,SLOT(AceptarValoresCampos()));
     fileToolBar->addAction(QIcon(":res//images//kview.png"),"Previsualizacion",this,SLOT(sltPrevisualizacion()));
     fileToolBar->addAction(QIcon(":res//images//Configuracion.png"),"Configuracion",this,SLOT(sltConfigColors()));
     fileToolBar->addSeparator();
     fileToolBar->addAction(QIcon(":res//images//Ok.png"),"OK",this,SLOT(AceptarSeleccion()));
     fileToolBar->addSeparator();

     QLabel *t1=new QLabel("x",fileToolBar);
     fileToolBar->addWidget(t1);
     m_Xinicial=new QLineEdit(fileToolBar);
     m_Xinicial->setMaximumWidth(MAX_WIDTH_EDIT);
     fileToolBar->addWidget(m_Xinicial);

     QLabel *t2=new QLabel("y",fileToolBar);
     fileToolBar->addWidget(t2);
     m_Yinicial=new QLineEdit(fileToolBar);
     m_Yinicial->setMaximumWidth(MAX_WIDTH_EDIT);
     fileToolBar->addWidget(m_Yinicial);

     QLabel *t3=new QLabel("Ancho(mm)",fileToolBar);
     fileToolBar->addWidget(t3);
     m_Ancho=new QLineEdit(fileToolBar);
     m_Ancho->setMaximumWidth(MAX_WIDTH_EDIT);
     fileToolBar->addWidget(m_Ancho);

     QLabel *t4=new QLabel("Alto(mm)",fileToolBar);
     fileToolBar->addWidget(t4);
     m_Alto=new QLineEdit(fileToolBar);
     m_Alto->setMaximumWidth(MAX_WIDTH_EDIT);
     fileToolBar->addWidget(m_Alto);







     ui->m_twColores->setColumnWidth(TABCOLOR_COLOR,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_X,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_Y,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_RED,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_GREEN,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_BLUE,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_ENABLED,35);
     ui->m_twColores->setColumnWidth(TABCOLOR_MULTIPLE,35);

     QSignalMapper *smPAL=new QSignalMapper(this);
     connect(this,SIGNAL(destroyed(QObject*)),smPAL,SLOT(deleteLater()));
     connect(smPAL,SIGNAL(mapped(QString)),this,SLOT(sltCheckTonos(QString)));


     //Para los 4 colores
     for (int x=0;x<4;x++){
         QPushButton *p=new QPushButton();
         p->setIcon(QIcon(":res//images//paint4.png"));
         connect(this,SIGNAL(destroyed(QObject*)),p,SLOT(deleteLater()));
         ui->m_twColores->setCellWidget(x,TABCOLOR_MULTIPLE,p);
         //connect(p,SIGNAL(clicked(bool)),this,SLOT(sltCheckTonos()));
         connect(p,SIGNAL(clicked()),smPAL,SLOT(map()));
         smPAL->setMapping(p,QString("%1").arg(x));
     }





#if QT_VERSION>=0x050000
     ui->m_twColores->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
     //ui->m_twColores->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
     ui->m_twColores->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
     ui->m_twColores->verticalHeader()->setResizeMode(QHeaderView::Stretch);

#endif

     ui->m_cbBitsDithering->setCurrentIndex(ui->m_cbBitsDithering->findText(QString("%1").arg(memoryMap->m_mapa.m_variables.iBitsDithering)));

     if (memoryMap->m_mapa.m_variables.AlgSelColor==SELECCION_AUTOMATICA) ui->m_rbAlgSelAuto->setChecked(true);
     else ui->m_rbAlgSelManual->setChecked(true);

     ui->m_leGrayTh->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.iGrayTh));

    //Zona central

     //m_Scene = new In2GraphicsScene(0, 0,memoryMap->m_Imagen.width(), memoryMap->m_Imagen.height());

     m_View=new In2GraphicsView(ui->m_GraphicsView);
     m_Scene=NULL;

     m_ViewColor1=new In2GraphicsView(ui->m_GraphicsViewColor1);
     m_SceneColor1=NULL;

     m_ViewColor2=new In2GraphicsView(ui->m_GraphicsViewColor2);
     m_SceneColor2=NULL;

     m_ViewColor3=new In2GraphicsView(ui->m_GraphicsViewColor3);
     m_SceneColor3=NULL;

     m_ViewColor4=new In2GraphicsView(ui->m_GraphicsViewColor4);
     m_SceneColor4=NULL;


     connect(m_View,SIGNAL(selectedPoint(QPointF)),this,SLOT(ActualizarColor(QPointF)));



    GlobalSetScreenSize(this);


    m_Xinicial->setPalette(dummyPaletteEnabled);
    m_Yinicial->setPalette(dummyPaletteEnabled);
    m_Ancho->setPalette(dummyPaletteEnabled);
    m_Alto->setPalette(dummyPaletteEnabled);


    m_Xinicial->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.XinicialPlantilla));
    m_Yinicial->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.YinicialPlantilla));
    m_Ancho->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.AnchoPlantilla));
    m_Alto->setText(QString("%1").arg(memoryMap->m_mapa.m_variables.AltoPlantilla));
    ui->m_RutaPlantilla->setText(QString(memoryMap->m_mapa.m_variables.NombreRutaPdf));

    AceptarValoresCampos();

    CreaImagenMuestra();
    QPixmap pixmapPlantilla=QPixmap(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    if (!pixmapPlantilla.isNull()){
        MuestraImagen(pixmapPlantilla,IMAGEN_COMPLETA);
    }

    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        sltAddColor(i,true);
    }
    //Sólo si hay máscara
    if ((memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL))
        sltAddColor(TABCOLOR_ROWMASK,true);
    //ui->m_NumColor->setCurrentIndex(0);

    PasarValoresDeMapATemp();
    TestRoot();


    connect(ui->m_rbAlgSelAuto,SIGNAL(clicked()),this,SLOT(sltAlgSelColor()));
    connect(ui->m_rbAlgSelManual,SIGNAL(clicked()),this,SLOT(sltAlgSelColor()));
    connect(ui->m_cbBitsDithering,SIGNAL(currentIndexChanged(int)),this,SLOT(sltChangeDithering(int)));
    connect(ui->m_leGrayTh,SIGNAL(editingFinished()),this,SLOT(sltChangeGrayThreshold()));


    QPalette palette = ui->m_twColores->palette();
    palette.setColor(QPalette::Highlight,Qt::red);
    ui->m_twColores->setPalette(palette);

    connect(ui->m_twColores,SIGNAL(cellClicked(int,int)),this,SLOT(sltSelectRow(int,int)));

}
/** Selecciona la columna*/
void EditImagenFija::sltSelectRow(int x,int y){
    ui->m_twColores->selectRow(x);
    if (y==TABCOLOR_ENABLED){
        QTableWidgetItem *p=ui->m_twColores->item(x,y);
        if (p){
            if (p->checkState()==Qt::Unchecked){
                if (x==0) MuestraImagen(QPixmap(),IMAGEN_COLOR1);
                else if (x==1) MuestraImagen(QPixmap(),IMAGEN_COLOR2);
                else if (x==2) MuestraImagen(QPixmap(),IMAGEN_COLOR3);
                else if (x==3) MuestraImagen(QPixmap(),IMAGEN_COLOR4);
            }
            else{
                sltPrevColor(x);
                /*if (x==0) MuestraImagen(QPixmap(),IMAGEN_COLOR1);
                else if (x==1) MuestraImagen(QPixmap(),IMAGEN_COLOR2);
                else if (x==2) MuestraImagen(QPixmap(),IMAGEN_COLOR3);
                else if (x==3) MuestraImagen(QPixmap(),IMAGEN_COLOR4);*/
            }
        }
    }
}

/** Chequea los tonos de la imagen*/
void EditImagenFija::sltCheckTonos(QString pal){
    //QString foo=QString("%1\\tmp\\dummy.bmp").arg(QApplication::applicationDirPath());
    QFileInfo info(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    QString foo=(memoryMap->useLocalCopy())?QString("%1\\tmp\\%2").arg(QApplication::applicationDirPath()).arg(info.fileName()):memoryMap->m_mapa.m_variables.NombreRutaBmp;

    //QImage img=QImage(foo).convertToFormat(QImage::Format_Indexed8);
    QImage img=QImage(foo);
    //img.save("d:\\foo.bmp");
    if (!img.isNull()){
        QVector<QRgb> vector;
        if (img.format()==QImage::Format_Indexed8) vector=img.colorTable();
        //Hay que mirar pixel a pixel
        else{
            int width=img.width();
            int height=img.height();
            //uchar tabla[0xFFFFFF];
            uchar *tabla=(uchar*)malloc(0xFFFFFF);
            memset(tabla,0x00,sizeof(tabla));
            for (int x=0;x<width;x++){
                for(int y=0;y<height;y++){
                    QRgb value=img.pixel(x,y)&(0x00FFFFFF);
                    //if (!vector.contains(value)) vector.append(value);
                    if (!tabla[value]){
                        tabla[value]=1;
                        vector.append(value|0xFF000000);
                    }
                }
            }

        }
        if (vector.count()>MAX_COLORES){
            int question=QMessageBox::warning(this,"INFO",tr("Se han detectado %1 colores diferentes. El uso de más %2 colores conlleva mucho tiempo de procesado. Quiere continuar?").arg(vector.count()).arg(MAX_COLORES),QMessageBox::Ok,QMessageBox::Cancel);
            if (question==QMessageBox::Cancel){
                return;
            }
        }

        In2SelectPalette *p=new In2SelectPalette(this,pal);
        p->showMaximized();
        p->setPalette(vector);
        //Comparamos con lo que hay en memoria
        int x=pal.toInt();
        if ((x>=0)&&(x<4)) {
            QStringList lst=memoryMap->m_mapa.m_colores[x].cadena.split("@");
            QVector <QRgb> vectorComp;
            for (int z=0;z<lst.count();z++){
                vectorComp.append((QRgb)lst.at(z).toUInt());
            }
            p->comparePalette(vectorComp);
        }
        //p->show();



        connect(p,SIGNAL(sgnNewColor(QString&, QString &)),this,SLOT(sltNewColorString(QString &, QString &)));

    }

}

/** Guarda el color*/
void EditImagenFija::sltNewColorString(QString & color,QString &id){
    int sel=id.toInt();
    if ((sel>=0)&&(sel<4)){
        ActualizarControlesColor();
        memoryMap->m_mapa.m_colores[sel].cadena=color;
        memoryMap->m_mapa.m_colores[sel].x_pix=0;
        memoryMap->m_mapa.m_colores[sel].y_pix=0;
        //CreaImagenMuestra();
        QString foo=QString("%1\\dummy.bmp").arg(memoryMap->m_mapa.m_variables.RutaBmp);
        In2ImageCreator imgCreator;
        imgCreator.getColorImage(foo,sel);
        sltAddColor(sel,true);

        //Coloreamos
        QStringList colors=color.split("@");
        if (colors.count()>1){

            QTableWidgetItem *pColor=ui->m_twColores->item(sel,TABCOLOR_COLOR);
            int w=ui->m_twColores->columnWidth(TABCOLOR_COLOR);
            QLinearGradient gradient(0,0,w,w);
            for (int x=0;x<colors.count();x++){
                float stop=((float)x)/(colors.count()-1);
                gradient.setColorAt(stop, QColor((QRgb)colors.at(x).toUInt()));
            }

            QBrush brush(gradient);
            pColor->setBackground(brush);
        }
        else if (colors.count()==1){
            QTableWidgetItem *pColor=ui->m_twColores->item(sel,TABCOLOR_COLOR);
            pColor->setBackgroundColor(QColor((QRgb)colors.at(0).toUInt()));

        }
    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,tr("Error->EditImagenFija[sltNewColorString], id=%1").arg(id),false);
    }


}

/** Rota la Imagen*/
void EditImagenFija::sltRotateImage(){
    /*int orientacion=0;
    if (ui->m_rbZero->isChecked()) orientacion=0;
    else if (ui->m_rb90->isChecked()) orientacion=90;
    else if (ui->m_rb180->isChecked()) orientacion=180;
    else if (ui->m_rb270->isChecked()) orientacion=270;
    memoryMap->m_mapa.m_variables.AnguloCorte=orientacion;
    QPixmap pixmapPlantilla=QPixmap(memoryMap->m_mapa.m_variables.NombreRutaBmp);


    QPixmap dest=pixmapPlantilla.transformed(QTransform().rotate(orientacion));
    dest.save(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
    updatePixelInfo();
    CreaImagenMuestra();
    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        sltAddColor(i,true);
    }
    //Sólo si hay máscara
    if ((memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL))
        sltAddColor(TABCOLOR_ROWMASK,true);


    pixmapPlantilla=QPixmap(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    if (!pixmapPlantilla.isNull()){
        MuestraImagen(pixmapPlantilla,IMAGEN_COMPLETA);
    }*/

}


/** Cambia el threhold gris*/
void EditImagenFija::sltChangeGrayThreshold(){
    memoryMap->m_mapa.m_variables.iGrayTh=ui->m_leGrayTh->text().toInt();
    if (ui->m_rbAlgSelAuto->isChecked()){
        deleteTempGSFile();
        AceptarValoresCampos();
        for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
            sltAddColor(i,true);
        }
    }
}

/** Cambia los bits de dithering*/
void EditImagenFija::sltChangeDithering(int bits){
    deleteTempGSFile();
    memoryMap->m_mapa.m_variables.iBitsDithering=ui->m_cbBitsDithering->currentText().toInt();
    AceptarValoresCampos();
    CreaImagenMuestra();
    QPixmap pixmapPlantilla=QPixmap(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    MuestraImagen(pixmapPlantilla,IMAGEN_COMPLETA);
    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
        sltAddColor(i,true);
    }
}


/** Cambia el algoritmo de selección de color*/
void EditImagenFija::sltAlgSelColor(){
    if (ui->m_rbAlgSelAuto->isChecked()){
        if (memoryMap->m_mapa.m_variables.NumColores<2){
            int count=0;
            for (int x=0;x<NUM_COLORES;x++){
                QTableWidgetItem *p=ui->m_twColores->item(x,TABCOLOR_ENABLED);
                if (p->checkState()==Qt::Unchecked) {
                    p->setCheckState(Qt::Checked);
                    count++;
                }
                else count++;
                if (count==2) break;
            }
            memoryMap->m_mapa.m_variables.NumColores=2;
        }

        memoryMap->m_mapa.m_variables.AlgSelColor=SELECCION_AUTOMATICA;
    }
    else memoryMap->m_mapa.m_variables.AlgSelColor=SELECCION_MANUAL;
    AceptarValoresCampos();
    CreaImagenMuestra();

    for (int x=0;x<NUM_COLORES;x++){
        QTableWidgetItem *p=ui->m_twColores->item(x,TABCOLOR_ENABLED);
        if (p->checkState()==Qt::Checked){
            sltAddColor(x,true);
        }
    }

}

/** Recalcula la imagen*/
void EditImagenFija::sltReload(){
    AceptarValoresCampos();
    CreaImagenMuestra();

    for (int x=0;x<NUM_COLORES;x++){
        QTableWidgetItem *p=ui->m_twColores->item(x,TABCOLOR_ENABLED);
        if (p->checkState()==Qt::Checked){
            sltAddColor(x,true);
        }
    }
}


/** Selecciona la row de la tabla de color*/
void EditImagenFija::sltSelectColorTable(int x,int y){
    ui->m_twColores->selectRow(x);
    if (y==TABCOLOR_ENABLED){
        sltUpdateNumColores();
        QTableWidgetItem *p=ui->m_twColores->item(x,y);
        if (p){
            if (p->checkState()==Qt::Unchecked){
                QTableWidgetItem *px=ui->m_twColores->item(x,TABCOLOR_X);
                px->setText("-");
                QTableWidgetItem *py=ui->m_twColores->item(x,TABCOLOR_Y);
                py->setText("-");
                QTableWidgetItem *pColor=ui->m_twColores->item(x,TABCOLOR_COLOR);
                pColor->setBackgroundColor(QColor(Qt::white));
                if (x==TABCOLOR_ROWMASK){
                    memoryMap->m_mapa.m_ColorMascara.x_pix=DEFAULT_NOMASK_PIXEL;
                    memoryMap->m_mapa.m_ColorMascara.y_pix=DEFAULT_NOMASK_PIXEL;
                    memoryMap->m_mapa.m_ColorMascara.valor=QColor(Qt::white).rgb();
                    //Ojo, con mascara hay que rehacer imagenes
                    //Previsualizamos..si tocamos la mascar hay que rehacer todo
                    AceptarValoresCampos();
                    for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++) sltPrevColor(i);
                    sltPrevColor(x);

                }
                else{
                    memoryMap->m_mapa.m_colores[x].x_pix=DEFAULT_NOMASK_PIXEL;
                    memoryMap->m_mapa.m_colores[x].y_pix=DEFAULT_NOMASK_PIXEL;
                    memoryMap->m_mapa.m_colores[x].valor=QColor(Qt::white).rgb();
                    //Elimino fichero anterior
                    QString dummyruta=QString(memoryMap->m_mapa.m_variables.NombreRutaBmp);
                    dummyruta.replace(".bmp",QString("_C%1.bmp").arg(x+1));
                    QFile file(dummyruta);
                    if (file.exists()) file.remove();
                }

            }//Copio lo que habia antes
            else{
               //sltAddColor(x,false);
            }
        }
    }
}
/** Actualiza el numero de colores*/
void EditImagenFija::sltUpdateNumColores(){
    unsigned char nColores=0;
    for (int x=0;x<4;x++){
        QTableWidgetItem *p=ui->m_twColores->item(x,TABCOLOR_ENABLED);
        if (p){
            if (p->checkState()==Qt::Checked) nColores++;

            else{
                MuestraImagen(QPixmap(),x+1);
            }
        }
    }
    memoryMap->m_mapa.m_variables.NumColores=nColores;
    ui->m_gbColores->setTitle(QString("Colores [%1]").arg(nColores));
}

//Añade un color a la tabla
void EditImagenFija::sltAddColor(int i,bool bActualiza){
    if (bActualiza){
        struct m_Colores color;
        if (i==TABCOLOR_ROWMASK){
            color.x_pix=memoryMap->m_mapa.m_ColorMascara.x_pix;
            color.y_pix=memoryMap->m_mapa.m_ColorMascara.y_pix;
            color.valor=memoryMap->m_mapa.m_ColorMascara.valor;

        }
        else{
            color.x_pix=memoryMap->m_mapa.m_colores[i].x_pix;
            color.y_pix=memoryMap->m_mapa.m_colores[i].y_pix;
            color.valor=memoryMap->m_mapa.m_colores[i].valor;
        }
        QTableWidgetItem *px=ui->m_twColores->item(i,TABCOLOR_X);
        if (px){
            if (color.x_pix!=DEFAULT_NOMASK_PIXEL)
                px->setText(QString("%1").arg(color.x_pix));
            else
                px->setText(QString("-").arg(color.x_pix));
        }
        QTableWidgetItem *py=ui->m_twColores->item(i,TABCOLOR_Y);
        if (py){
            if (color.y_pix!=DEFAULT_NOMASK_PIXEL)
                py->setText(QString("%1").arg(color.y_pix));
            else
                py->setText(QString("-").arg(color.y_pix));
        }

        QTableWidgetItem *pColor=ui->m_twColores->item(i,TABCOLOR_COLOR);
        if (pColor){
            pColor->setBackgroundColor(QColor(color.valor));
        }
        QColor tmpColor=QColor(color.valor);

        QTableWidgetItem *pRed=ui->m_twColores->item(i,TABCOLOR_RED);
        if (pRed) pRed->setText(QString("%1").arg(tmpColor.red()));
        QTableWidgetItem *pGreen=ui->m_twColores->item(i,TABCOLOR_GREEN);
        if (pGreen) pGreen->setText(QString("%1").arg(tmpColor.green()));
        QTableWidgetItem *pBlue=ui->m_twColores->item(i,TABCOLOR_BLUE);
        if (pBlue) pBlue->setText(QString("%1").arg(tmpColor.blue()));


        QTableWidgetItem *pEnabled=ui->m_twColores->item(i,TABCOLOR_ENABLED);
        if (pEnabled){
            pEnabled->setCheckState(Qt::Checked);
        }
    }

    //AceptarValoresCampos();


    //Previsualizamos..si tocamos la mascar hay que rehacer todo
    if (i==TABCOLOR_ROWMASK){
        for (int x=0;x<memoryMap->m_mapa.m_variables.NumColores;x++) sltPrevColor(x);
    }
    else sltPrevColor(i);
    //else sltUpdateSingleColor(i);
}

/** Filtro de eventos*/
bool EditImagenFija::eventFilter(QObject *p, QEvent *e){
    return QWidget::eventFilter(p,e);
}

void EditImagenFija::sltAddPlantImage(int x,int y){
    QFileDialog fileName(this);
    QStringList Ficheros;


    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("ini,bmp Files (*.ini *.bmp)"));
#else
    fileName.setFilter(tr("ini,bmp Files (*.ini *.bmp)"));
#endif
    fileName.setOptions(QFileDialog::DontResolveSymlinks);
    fileName.setDirectory(memoryMap->getImagePath());
    Ficheros=fileName.getOpenFileNames(this,"Selecciona un fichero",memoryMap->getImagePath(),tr("ini,bmp Files (*.ini *.bmp)"));
    if (!Ficheros.isEmpty()){
        QTableWidgetItem *p=new QTableWidgetItem(Ficheros.at(0));
        Qt::ItemFlags Flags;
        Flags=p->flags();
        Flags&=~(Qt::ItemIsEditable);
        p->setFlags(Flags);
     }
    AceptarValoresCampos();
}

void EditImagenFija::sltPlantEnabled(){
}
/** Actualiza el Offset Y*/
void EditImagenFija::sltUpdateOffsetY(int d){
}
/** Actualiza el Offset x*/
void EditImagenFija::sltUpdateOffsetX(int d){
}
/** Actualiza el alto de la plantilla*/
void EditImagenFija::sltUpdateAltoPlantilla(int d){
}
/** Actualiza el ancho de la plantilla*/
void EditImagenFija::sltUpdateAnchoPlantilla(int d){
}
/** Actualiza el ancho de la plantilla*/
void EditImagenFija::sltUpdateDistanciaColumnas(int d){
}
/** Actualiza la plantilla*/
void EditImagenFija::sltUpdatePlantilla(int d){   
}
/** Actualiza un color*/
void EditImagenFija::sltUpdateSingleColor(int index){
    In2ImageCreator creator;
    QPixmap pix=QPixmap::fromImage(creator.getColorImage(memoryMap->m_mapa.m_variables.NombreRutaBmp,index));
    if (!pix.isNull()) MuestraImagen(pix,index+1);
}

/** Hace una previsualizacion de la impresión*/
void EditImagenFija::sltPrevColor(int index){

    QString dummyruta=QString(memoryMap->m_mapa.m_variables.NombreRutaBmp);
    dummyruta.replace(".bmp",QString("_C%1.bmp").arg(index+1));
    QPixmap prevPixmap(dummyruta);
    if (!prevPixmap.isNull()){
        MuestraImagen(prevPixmap,index+1);
    }
}
/** Hace una previsualizacion de la impresión*/
void EditImagenFija::sltConfigColors(){
    In2GhostScriptConfig *p=new In2GhostScriptConfig();
    p->show();
    connect(p,SIGNAL(destroyed()),this,SLOT(sltReload()));
}

/** Hace una previsualizacion de la impresión*/
void EditImagenFija::sltPrevisualizacion(){
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
            prevImage->setGeometry(50,50,ui->m_GraphicsView->width(),ui->m_GraphicsView->height());


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
void EditImagenFija::SelColorMascara(){
    //ui->m_cbColor->setChecked(false);
}

/** Selecciona color impresion*/
void EditImagenFija::SelColorPrint(){
   /* if (ui->m_cbColor->isChecked()){
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
    }*/
}


/** Pantalla de selección de corte*/
void EditImagenFija::SelCorteImagen(){
    deleteTempGSFile();
    CorteImagen *p;
    p=new CorteImagen(0);
    p->show();
    this->deleteLater();



}


void EditImagenFija::TestRoot(){
    if (memoryMap->getUserRights()<ACCESS_ROOT){
        m_Xinicial->setEnabled(false);
        m_Yinicial->setEnabled(false);
        m_Ancho->setEnabled(false);
        m_Alto->setEnabled(false);
        //ui->m_BuscarPlantilla->setEnabled(false);
        ui->m_RutaPlantilla->setEnabled(false);
	}
}

EditImagenFija::~EditImagenFija(){


}

void EditImagenFija::PasarValoresDeMapATemp(){
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

void EditImagenFija::PasarValoresDeTempAMap(){
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
void EditImagenFija::AceptarSeleccion(){
    int rtn=QMessageBox::question(0,tr("ATENCION"),tr("Deseas guardar los cambios?"),QMessageBox::Yes,QMessageBox::No);
    if(rtn==QMessageBox::No){
        CancelarSeleccion();
    }
    else{

            //QLabel *dummy = ShowMensaje("Rendering");
            memoryMap->m_mapa.m_variables.XinicialPlantilla=m_Xinicial->text().toDouble();
            memoryMap->m_mapa.m_variables.YinicialPlantilla=m_Yinicial->text().toDouble();
            memoryMap->m_mapa.m_variables.AnchoPlantilla=m_Ancho->text().toDouble();
            memoryMap->m_mapa.m_variables.AltoPlantilla=m_Alto->text().toDouble();
            memoryMap->m_mapa.m_variables.NombreRutaPdf=ui->m_RutaPlantilla->text();            
            //memoryMap->m_mapa.m_variables.NumColores=ui->m_NumTotalColores->toPlainText().toInt();
            ActualizarControlesColor();
            //delete (dummy);
            LaunchComposicion();                
    }
}


void EditImagenFija::CancelarSeleccion(){
    QLabel *dummy = ShowMensaje("Rendering");
    PasarValoresDeTempAMap();
    CreaImagenMuestra();

	LaunchComposicion();
	//this->ShowMainWindow();
	this->deleteLater();
	
}




void EditImagenFija::UpdateCoordenadaXinicial(){

    InputData *input = InputData::request(this,"Posicion X Inicial [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateXinicial(QString)));
}

void EditImagenFija::UpdateXinicial(QString value){

    m_Xinicial->setText(value);
    m_Xinicial->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditImagenFija::UpdateCoordenadaYinicial(){

    InputData *input = InputData::request(this,"Posicion Y Inicial [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateYinicial(QString)));
}

void EditImagenFija::UpdateYinicial(QString value){

    m_Yinicial->setText(value);
    m_Yinicial->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}
void EditImagenFija::UpdateAnchoPlantilla(){

    InputData *input = InputData::request(this,"Ancho [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAncho(QString)));
}

void EditImagenFija::UpdateAncho(QString value){

    m_Ancho->setText(value);
    m_Ancho->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void EditImagenFija::UpdateAltoPlantilla(){
    InputData *input = InputData::request(this,"Alto [mm]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAlto(QString)));
}

void EditImagenFija::UpdateAlto(QString value){

    m_Alto->setText(value);
    m_Alto->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void EditImagenFija::UpdateNumeroColores(){

    InputData *input = InputData::request(this,"Numero total de colores");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateNumero(QString)));
}

void EditImagenFija::UpdateNumero(QString value){

    /*ui->m_NumTotalColores->setText(value);
    ui->m_NumTotalColores->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);*/
}

void EditImagenFija::UpdateColor(){
    /*int i;
    i=ui->m_NumColor->currentIndex();
    ui->m_XColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[i].x_pix));
    ui->m_YColor->setText(QString("%1").arg(memoryMap->m_mapa.m_colores[i].y_pix));

    QPalette color_pleno;
    color_pleno.setColor(QPalette::Base,QColor(memoryMap->m_mapa.m_colores[i].valor));
    ui-> m_ColorSeleccionado->setPalette(color_pleno);*/


}

void EditImagenFija::UpdateCoordenadaXcolor(){
    InputData *input = InputData::request(this,"Posicion X Color [pix]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateXcolor(QString)));
}

void EditImagenFija::UpdateCoordenadaYcolor(){
    InputData *input = InputData::request(this,"Posicion Y Color [pix]");
    if (input)
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateYcolor(QString)));
}

void EditImagenFija::UpdateXcolor(QString value){
    int sel=ui->m_twColores->currentRow();
    QTableWidgetItem *p=ui->m_twColores->item(sel,TABCOLOR_X);
    if (p){
        p->setText(value);
    }
}

void EditImagenFija::UpdateYcolor(QString value){
    int sel=ui->m_twColores->currentRow();
    QTableWidgetItem *p=ui->m_twColores->item(sel,TABCOLOR_Y);
    if (p){
        p->setText(value);
    }
}


void EditImagenFija::AceptarValoresCampos(){
    //Muestra imagen fija
    deleteTempGSFile();
        memoryMap->m_mapa.m_variables.XinicialPlantilla=m_Xinicial->text().toDouble();
        memoryMap->m_mapa.m_variables.YinicialPlantilla=m_Yinicial->text().toDouble();
        memoryMap->m_mapa.m_variables.AnchoPlantilla=m_Ancho->text().toDouble();
        memoryMap->m_mapa.m_variables.AltoPlantilla=m_Alto->text().toDouble();

}

void EditImagenFija::LaunchComposicion(){

    Composicion *ScrComposicion;
    memoryMap->m_ComposicionGenerada=true;
    ScrComposicion=new Composicion(0);
	ScrComposicion->show();
	this->deleteLater();
}



//Crea cada una de las imagenes que forma la impresion
void EditImagenFija::CreaImagenMuestra(){
    this->setEnabled(false);
    ui->m_leMessage->setEnabled(true);
    QCoreApplication::processEvents();
    In2ImageCreator *img=new In2ImageCreator;
    img->Convert();
    ui->m_leMessage->setEnabled(false);
    this->setEnabled(true);




}


//Genera los troquelados...AQUI HAY QUE GENERAR LOS BMPS!!!
void EditImagenFija::sltRenderPlantilla(){

}


//Muestra la imagen central
void EditImagenFija::MuestraImagen(const QPixmap & Imagen,int color){
    In2GraphicsScene *scene;
    QGraphicsView *view;
    Imagen.save(QString("D:\\Foo%1.bmp").arg(color));
    if (color==IMAGEN_COMPLETA){
        scene=m_Scene;
        view=ui->m_GraphicsView;
    }
    else if (color==IMAGEN_COLOR1){
        scene=m_SceneColor1;
        view=ui->m_GraphicsViewColor1;
    }
    else if (color==IMAGEN_COLOR2){
        scene=m_SceneColor2;
        view=ui->m_GraphicsViewColor2;
    }
    else if (color==IMAGEN_COLOR3){
        scene=m_SceneColor3;
        view=ui->m_GraphicsViewColor3;
    }
    else if (color==IMAGEN_COLOR4){
        scene=m_SceneColor4;
        view=ui->m_GraphicsViewColor4;
    }
    else  return;

    int x=0;
    int y=0;
    QPixmap *lienzo=new QPixmap(Imagen.width(),Imagen.height());
#if QT_VERSION>=0x050000
    lienzo->fill(Qt::white);
#else
    lienzo->fill(QColor(Qt::white).rgb());
#endif
    QPainter painter(lienzo);

    //painter.setPen( QPen( Qt::black,3) );
    QPen pen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setCosmetic(true);

    painter.setPen(pen);
    painter.drawPixmap(x,y,Imagen);
    painter.resetTransform();
    painter.drawRect(x,y,Imagen.width(),Imagen.height());
    painter.end();


    //Eliminamos todo lo que hay
    //qDeleteAll(m_Scene->items());

    if (scene==NULL){
        scene = new In2GraphicsScene(x, y,Imagen.width(), Imagen.height());
        In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
        p->setTransformationMode(Qt::FastTransformation);
        p->setPixmap(*lienzo);
        scene->addItem(p);
        view->setScene(scene);
        view->fitInView(p,Qt::KeepAspectRatio);
        view->centerOn(p);
        view->show();
        if (color==IMAGEN_COMPLETA) m_Scene=scene;
        else if (color==IMAGEN_COLOR1) m_SceneColor1=scene;
        else if (color==IMAGEN_COLOR2) m_SceneColor2=scene;
        else if (color==IMAGEN_COLOR3) m_SceneColor3=scene;
        else if (color==IMAGEN_COLOR4) m_SceneColor4=scene;
    }
    else{
        QList<QGraphicsItem*> items=scene->items();
        In2GraphicsPixmapItem *p;
        if (items.count()){
            p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
            p->setPixmap(*lienzo);
        }
        else{
            p=new In2GraphicsPixmapItem(0,0,false);
            p->setPixmap(*lienzo);
            scene->addItem(p);
        }
        scene->setSceneRect(0,0,lienzo->width(),lienzo->height());
        view->fitInView(p,Qt::KeepAspectRatio);
        view->centerOn(p);
        //ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
    }

}


void EditImagenFija::BuscarPlantilla(){


    QFileDialog fileName(this);
    QStringList Ficheros;
    //static InkjetDebug *dbg=InkjetDebug::request(0);
    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("Archivos PDF(*.pdf);;PNG (*.png);;TIF (*.tif);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)"));
#else
    fileName.setFilter(tr("Archivos PDF(*.pdf);;PNG (*.png);;JPEG (*.jpg *.jpeg);;TIF (*.tif);;BMP (*.bmp)"));
#endif
    fileName.setOptions(QFileDialog::DontResolveSymlinks);
    fileName.setDirectory(memoryMap->getImagePath());
    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
                    w->setAutoDefault(false);
     }*/

    //if (fileName.exec()){
            //Ficheros=fileName.selectedFiles();
        Ficheros=fileName.getOpenFileNames(this,"Selecciona un fichero",memoryMap->getImagePath(),tr("Archivos PDF(*.pdf);;PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;TIFF(*.tif)"));
        if (!Ficheros.isEmpty()){
            deleteTempGSFile();
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

            AceptarValoresCampos();
            CreaImagenMuestra();
            QPixmap pixmapPlantilla=QPixmap(memoryMap->m_mapa.m_variables.NombreRutaBmp);
            MuestraImagen(pixmapPlantilla,IMAGEN_COMPLETA);

            for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
                sltAddColor(i,true);
            }



		}




}


//void EditImagenFija::mouseDoubleClickEvent(QMouseEvent * event){
//}

void EditImagenFija::ActualizarColor(int x, int y){

    //UpdateMemoriaColorClicked();

}

void EditImagenFija::ActualizarColor(const QPointF &p){
    if (ui->m_rbAlgSelAuto->isChecked()){
        QMessageBox::warning(this,"ERROR",tr("Elija primero modo manual"),QMessageBox::Ok);
        return;
    }
    UpdateMemoriaColorClicked(p);
}

/** Actualiza la info de pixel*/
void EditImagenFija::updatePixelInfo(){
    QImage dummyImage(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP");
    int exitFlag=0;
    int x=0,y=0;

    for (int index=0;index<NUM_COLORES;index++){
        QTableWidgetItem *p=ui->m_twColores->item(index,TABCOLOR_ENABLED);
        if (p->checkState()==Qt::Checked) {
            for (x=0;x<dummyImage.width();x++){
                if (exitFlag) {
                    if (x) x--;
                    break;
                }
                for (y=0;y<dummyImage.height();y++){
                    if (memoryMap->m_mapa.m_colores[index].valor==dummyImage.pixel(x,y)){
                        memoryMap->m_mapa.m_colores[index].x_pix=x;
                        memoryMap->m_mapa.m_colores[index].y_pix=y;
                        //memoryMap->m_mapa.m_colores[index].cadena=QString("%1").arg()
                        sltAddColor(index,true);
                        exitFlag=1;
                        break;
                    }
                }
            }
        }
    }
}

/** Actualiza seleccion de color a partir de un click*/
void EditImagenFija::UpdateMemoriaColorClicked(const QPointF &p){
    QRgb rgbSelectedColor;

    //Buscamos la primera ocurrencia de color en la imagen y la guardamos
    QImage buffer = QImage(1,1, QImage::Format_RGB32);
    QPainter painter(&buffer);
    m_Scene->render(&painter, QRectF(0.0, 0.0, 1.0, 1.0), QRectF(p, QSize(1,1)));
    rgbSelectedColor=buffer.pixel(0,0);



    painter.end();

    //QImage dummyImage(memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP");
    QImage dummyImage=QImage(memoryMap->m_mapa.m_variables.NombreRutaBmp).convertToFormat(QImage::Format_RGB32);
    int exitFlag=0;
    int x=0,y=0;
    for (x=0;x<dummyImage.width();x++){
        if (exitFlag) {
            if (x) x--;
            break;
        }
        for (y=0;y<dummyImage.height();y++){
            if (rgbSelectedColor==dummyImage.pixel(x,y)){
                exitFlag=1;
                break;
            }
        }
    }
    int indice=ui->m_twColores->currentRow();
    if (indice>=0){
        if (indice==TABCOLOR_ROWMASK){
            memoryMap->m_mapa.m_ColorMascara.x_pix=x;
            memoryMap->m_mapa.m_ColorMascara.y_pix=y;
            memoryMap->m_mapa.m_ColorMascara.valor=rgbSelectedColor;

        }
        else{
            memoryMap->m_mapa.m_colores[indice].x_pix=x;
            memoryMap->m_mapa.m_colores[indice].y_pix=y;
            memoryMap->m_mapa.m_colores[indice].valor=rgbSelectedColor;
            memoryMap->m_mapa.m_colores[indice].cadena=QString("%1").arg(rgbSelectedColor);
            QTableWidgetItem *p=ui->m_twColores->item(TABCOLOR_ROWMASK,TABCOLOR_ENABLED);
            if (p) p->setCheckState(Qt::Unchecked);
            memoryMap->m_mapa.m_ColorMascara.x_pix=DEFAULT_NOMASK_PIXEL;
            memoryMap->m_mapa.m_ColorMascara.y_pix=DEFAULT_NOMASK_PIXEL;
        }
        //Aqui hay que regenerar solo la imagen del colo tocado
        QString foo=QString("%1\\dummy.bmp").arg(memoryMap->m_mapa.m_variables.RutaBmp);
        In2ImageCreator imgCreator;
        imgCreator.getColorImage(foo,indice);
        sltAddColor(indice,true);
    }
}


void EditImagenFija::ActualizarControlesColor(){
    //Actualizo informacion de numero de colores
    unsigned char nColores=0;
    for (int x=0;x<4;x++){
        QTableWidgetItem *p=ui->m_twColores->item(x,TABCOLOR_ENABLED);
        if (p){
            if (p->checkState()==Qt::Checked){
                memoryMap->m_mapa.m_colores[x].enabled=true;
                nColores++;
            }
            else memoryMap->m_mapa.m_colores[x].enabled=false;
        }
    }
    memoryMap->m_mapa.m_variables.NumColores=nColores;
    ui->m_gbColores->setTitle(QString("Colores [%1]").arg(nColores));

}
