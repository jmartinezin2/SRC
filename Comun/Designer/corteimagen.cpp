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
#include "corteimagen.h"
#include "ui_corteimagen.h"
#include "EditImagenFija.h"
#include <QFile>
#include "gsrenderengine.h"
#include <QProcess>
#include <QMessageBox>
#include "in2defines.h"



CorteImagen::CorteImagen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorteImagen)
{
    ui->setupUi(this);
    m_scene=NULL;
    m_In2GView=NULL;
    m_memoryMap=MemoryMap::request(0);
    GlobalSetScreenSize(this);
    //m_parent=parent;
    m_selectedW=0;
    m_selectedH=0;
    m_zWidth=0;
    m_zHeight=0;
    m_initPoint=QPoint(0,0);
    m_initSmallPoint=QPoint(0,0);
    m_verticalZoomOffset=0;
    m_locked=false;
    //m_imageLabel = new QLabelExt();

    deleteTempGSFile();

    connect(ui->m_pbSave,SIGNAL(clicked()),this,SLOT(SaveAndExit()));
    connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(Exit()));

    //connect(m_imageLabel,SIGNAL(press(const QPoint &)),this,SLOT(InitSmallSelection(const QPoint&)));
    //connect(m_imageLabel,SIGNAL(moving(const QPoint&)),this,SLOT(ModifySmallSelection(const QPoint&)));


    //connect(m_imageLabel,SIGNAL(newSelection(const QRect &)),this,SLOT(NewActiveZone(const QRect &)));

    connect(ui->m_xini,SIGNAL(textEdited(QString)),this,SLOT(UpdateXiniPixel(QString)));
    connect(ui->m_yini,SIGNAL(textEdited(QString)),this,SLOT(UpdateYiniPixel(QString)));
    connect(ui->m_xfin,SIGNAL(textEdited(QString)),this,SLOT(UpdateXfinPixel(QString)));
    connect(ui->m_yfin,SIGNAL(textEdited(QString)),this,SLOT(UpdateYfinPixel(QString)));
    connect(ui->m_width,SIGNAL(textEdited(QString)),this,SLOT(UpdateWidthPixel(QString)));
    connect(ui->m_height,SIGNAL(textEdited(QString)),this,SLOT(UpdateHeightPixel(QString)));

    connect(ui->m_xini_2,SIGNAL(textEdited(QString)),this,SLOT(UpdateXiniMm(QString)));
    connect(ui->m_yini_2,SIGNAL(textEdited(QString)),this,SLOT(UpdateYiniMm(QString)));
    connect(ui->m_xfin_2,SIGNAL(textEdited(QString)),this,SLOT(UpdateXfinMm(QString)));
    connect(ui->m_yfin_2,SIGNAL(textEdited(QString)),this,SLOT(UpdateYfinMm(QString)));
    connect(ui->m_width_2,SIGNAL(textEdited(QString)),this,SLOT(UpdateWidthMm(QString)));
    connect(ui->m_height_2,SIGNAL(textEdited(QString)),this,SLOT(UpdateHeightMm(QString)));
    connect(ui->m_cbDobleRes,SIGNAL(clicked()),this,SLOT(GenerateImage()));
    connect(ui->m_pbReload,SIGNAL(clicked()),this,SLOT(GenerateImage()));
    connect(ui->m_cbBitsDithering,SIGNAL(currentIndexChanged(int)),this,SLOT(sltChangeDitheringBits(int)));
    connect(ui->m_pbNewCorte,SIGNAL(clicked()),this,SLOT(sltGenNewSelection()));

    ui->m_cbBitsDithering->setCurrentIndex(ui->m_cbBitsDithering->findText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.iBitsDithering)));


    ui->m_xini_2->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.XinicialPlantilla));
    ui->m_yini_2->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.YinicialPlantilla));
    ui->m_xfin_2->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.XinicialPlantilla+m_memoryMap->m_mapa.m_variables.AnchoPlantilla));
    ui->m_yfin_2->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.YinicialPlantilla+m_memoryMap->m_mapa.m_variables.AltoPlantilla));
    ui->m_height_2->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AltoPlantilla));
    ui->m_width_2->setText(QString("%1").arg(m_memoryMap->m_mapa.m_variables.AnchoPlantilla));

    ui->m_xini->setText(QString("%1").arg(int(m_memoryMap->m_mapa.m_variables.XinicialPlantilla*14.18)));
    ui->m_yini->setText(QString("%1").arg(int(m_memoryMap->m_mapa.m_variables.YinicialPlantilla*14.18)));
    ui->m_height->setText(QString("%1").arg(int(m_memoryMap->m_mapa.m_variables.AltoPlantilla*14.18)));
    ui->m_width->setText(QString("%1").arg(int(m_memoryMap->m_mapa.m_variables.AnchoPlantilla*14.18)));
    ui->m_xfin->setText(QString("%1").arg(int((m_memoryMap->m_mapa.m_variables.XinicialPlantilla+m_memoryMap->m_mapa.m_variables.AnchoPlantilla)*14.18)));
    ui->m_yfin->setText(QString("%1").arg(int((m_memoryMap->m_mapa.m_variables.YinicialPlantilla+m_memoryMap->m_mapa.m_variables.AltoPlantilla)*14.18)));




    QTimer::singleShot(0,this,SLOT(GenerateImage()));
}
/** Cambia los bits de dithering*/
void CorteImagen::sltChangeDitheringBits(int index){
    GenerateImage();
}
/** Genera una nueva seleciión*/
void CorteImagen::sltGenNewSelection(){
    ui->m_xini->setText("0");
    ui->m_xini_2->setText("0");
    ui->m_yini->setText("0");
    ui->m_yini_2->setText("0");
    int w=ui->m_wImagen->text().toInt();
    int h=ui->m_hImagen->text().toInt();

    ui->m_width->setText(QString("%1").arg(w));
    ui->m_width_2->setText(QString("%1").arg(w/14.18));
    ui->m_height->setText(QString("%1").arg(h));
    ui->m_height_2->setText(QString("%1").arg(h/14.18));
    ui->m_xfin->setText(QString("%1").arg(w));
    ui->m_xfin_2->setText(QString("%1").arg(w/14.18));
    ui->m_yfin->setText(QString("%1").arg(h));
    ui->m_yfin_2->setText(QString("%1").arg(h/14.18));

    sltShowNewSelection();

}

/** Regenera el corte*/
void CorteImagen::sltShowNewSelection(){
    int x0=ui->m_xini->text().toInt();//m_memoryMap->m_mapa.m_variables.XinicialPlantilla*14.18;
    int y0=ui->m_yini->text().toInt();
    int width=ui->m_width->text().toInt();
    int height=ui->m_height->text().toInt();
    QList<QGraphicsItem*> items=m_scene->items();
    In2GraphicsPixmapItem *p;
    if (items.count()==2){
        p=static_cast<In2GraphicsPixmapItem*>(items.at(0));

        QRect rect;
        if ((width<=0)||(height<=0))
            rect=p->pixmap().rect();
        else rect=QRect(x0,y0,width,height);
        p->setPos(x0,y0);
        p->generatePrintZone(rect);

    }
}

void CorteImagen::UpdateXiniMm(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        float dummy=ui->m_xfin_2->text().toFloat()-ui->m_xini_2->text().toFloat();
        ui->m_width_2->setText(QString("%1").arg(dummy));
        UpdatePixelData(str);

    }
}
void CorteImagen::UpdateXiniPixel(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        int dummy=ui->m_xfin->text().toInt()-ui->m_xini->text().toInt();
        ui->m_width->setText(QString("%1").arg(dummy));
        UpdateMmData(str);
    }
}
void CorteImagen::UpdateXfinMm(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        float dummy=ui->m_xfin_2->text().toFloat()-ui->m_xini_2->text().toFloat();
        ui->m_width_2->setText(QString("%1").arg(dummy));
        UpdatePixelData(str);
    }
}
void CorteImagen::UpdateXfinPixel(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        int dummy=ui->m_xfin->text().toInt()-ui->m_xini->text().toInt();
        ui->m_width->setText(QString("%1").arg(dummy));
        UpdateMmData(str);
    }
}
void CorteImagen::UpdateYiniMm(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        float dummy=ui->m_yfin_2->text().toFloat()-ui->m_yini_2->text().toFloat();
        ui->m_height_2->setText(QString("%1").arg(dummy));
        UpdatePixelData(str);

    }
}
void CorteImagen::UpdateYiniPixel(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        int dummy=ui->m_yfin->text().toInt()-ui->m_yini->text().toInt();
        ui->m_height->setText(QString("%1").arg(dummy));
        UpdateMmData(str);
    }
}
void CorteImagen::UpdateYfinMm(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        float dummy=ui->m_yfin_2->text().toFloat()-ui->m_yini_2->text().toFloat();
        ui->m_height_2->setText(QString("%1").arg(dummy));
        UpdatePixelData(str);
    }
}
void CorteImagen::UpdateYfinPixel(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        int dummy=ui->m_yfin->text().toInt()-ui->m_yini->text().toInt();
        ui->m_height->setText(QString("%1").arg(dummy));
        UpdateMmData(str);
    }
}
void CorteImagen::UpdateWidthMm(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        float dummy=ui->m_xini_2->text().toFloat()+ui->m_width_2->text().toFloat();
        ui->m_xfin_2->setText(QString("%1").arg(dummy));
        UpdatePixelData(str);
    }
}
void CorteImagen::UpdateWidthPixel(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        int dummy=ui->m_xini->text().toInt()+ui->m_width->text().toInt();
        ui->m_xfin->setText(QString("%1").arg(dummy));
        UpdateMmData(str);
    }
}
void CorteImagen::UpdateHeightMm(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        float dummy=ui->m_yini_2->text().toFloat()+ui->m_height_2->text().toFloat();
        ui->m_yfin_2->setText(QString("%1").arg(dummy));
        UpdatePixelData(str);
    }
}
void CorteImagen::UpdateHeightPixel(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        int dummy=ui->m_yini->text().toInt()+ui->m_height->text().toInt();
        ui->m_yfin->setText(QString("%1").arg(dummy));
        UpdateMmData(str);
    }
}


/** Conmecta los updates*/
void CorteImagen::BlockSignals(bool value){
    //Ajustes de pixel a mm
    ui->m_height->blockSignals(value);
    ui->m_width->blockSignals(value);
    ui->m_xini->blockSignals(value);
    ui->m_yini->blockSignals(value);
    ui->m_xfin->blockSignals(value);
    ui->m_yfin->blockSignals(value);
    //Ajustes de mm a pixel
    ui->m_height_2->blockSignals(value);
    ui->m_width_2->blockSignals(value);
    ui->m_xini_2->blockSignals(value);
    ui->m_yini_2->blockSignals(value);
    ui->m_xfin_2->blockSignals(value);
    ui->m_yfin_2->blockSignals(value);
}

/** Repintamos el rubberBand*/
void CorteImagen::UpdateRubberFromKeypad(){

}

/** Reajustamos de mm a pixel*/
void CorteImagen::UpdatePixelData(QString str){
    bool ok;
    str.toFloat(&ok);
    if (ok){ //Solo si estamos metiendo un numero
        updatepixeldata();
        UpdateRubberFromKeypad();
        sltShowNewSelection();
    }
}
/** Reajustamos de pixel a mm*/
void CorteImagen::UpdateMmData(QString str){
    bool ok;
    str.toInt(&ok);
    if (ok){//Solo si estamos metiendo un numero
        updatemmdata();
        UpdateRubberFromKeypad();
        sltShowNewSelection();
    }
}
/** Actualiza de mm a pixel*/
void CorteImagen::updatepixeldata(){
    int xini=ui->m_xini_2->text().toFloat()*FACTOR_PIXELS_MM;
    int yini=ui->m_yini_2->text().toFloat()*FACTOR_PIXELS_MM;
    int xfin=ui->m_xfin_2->text().toFloat()*FACTOR_PIXELS_MM;
    int yfin=ui->m_yfin_2->text().toFloat()*FACTOR_PIXELS_MM;
    int width=ui->m_width_2->text().toFloat()*FACTOR_PIXELS_MM;
    int height=ui->m_height_2->text().toFloat()*FACTOR_PIXELS_MM;

   // BlockSignals(true);
    ui->m_xini->setText(QString("%1").arg(xini));
    ui->m_yini->setText(QString("%1").arg(yini));
    ui->m_xfin->setText(QString("%1").arg(xfin));
    ui->m_yfin->setText(QString("%1").arg(yfin));
    ui->m_width->setText(QString("%1").arg(width));
    ui->m_height->setText(QString("%1").arg(height));
}
/** Actualiza de pixel a mm*/
void CorteImagen::updatemmdata(){
    float xini=ui->m_xini->text().toInt()/FACTOR_PIXELS_MM;
    float yini=ui->m_yini->text().toInt()/FACTOR_PIXELS_MM;
    float xfin=ui->m_xfin->text().toInt()/FACTOR_PIXELS_MM;
    float yfin=ui->m_yfin->text().toInt()/FACTOR_PIXELS_MM;
    float width=ui->m_width->text().toInt()/FACTOR_PIXELS_MM;
    float height=ui->m_height->text().toInt()/FACTOR_PIXELS_MM;
   // BlockSignals(true);
    ui->m_xini_2->setText(QString("%1").arg(xini));
    ui->m_yini_2->setText(QString("%1").arg(yini));
    ui->m_xfin_2->setText(QString("%1").arg(xfin));
    ui->m_yfin_2->setText(QString("%1").arg(yfin));
    ui->m_width_2->setText(QString("%1").arg(width));
    ui->m_height_2->setText(QString("%1").arg(height));
}

/** Generamos en la zona ancha un a nueva seleccion*/
void CorteImagen::NewActiveZone(const QRect &r){
    int ixini=r.topLeft().x();
    float fxini=ixini/FACTOR_PIXELS_MM;
    int iyini=r.topLeft().y();
    float fyini=iyini/FACTOR_PIXELS_MM;
    int ixfin=r.bottomRight().x();
    float fxfin=ixfin/FACTOR_PIXELS_MM;
    int iyfin=r.bottomRight().y();
    float fyfin=iyfin/FACTOR_PIXELS_MM;
    int iwidth=r.width();
    float fwidth=iwidth/FACTOR_PIXELS_MM;
    int iheight=r.height();
    float fheight=iheight/FACTOR_PIXELS_MM;

    ui->m_xini->setText(QString("%1").arg(ixini));
    ui->m_yini->setText(QString("%1").arg(iyini));
    ui->m_xfin->setText(QString("%1").arg(ixfin));
    ui->m_yfin->setText(QString("%1").arg(iyfin));
    ui->m_width->setText(QString("%1").arg(iwidth));
    ui->m_height->setText(QString("%1").arg(iheight));
    ui->m_xini_2->setText(QString("%1").arg(fxini));
    ui->m_yini_2->setText(QString("%1").arg(fyini));
    ui->m_xfin_2->setText(QString("%1").arg(fxfin));
    ui->m_yfin_2->setText(QString("%1").arg(fyfin));
    ui->m_width_2->setText(QString("%1").arg(fwidth));
    ui->m_height_2->setText(QString("%1").arg(fheight));


}

/** Ajuste fino*/
void CorteImagen::FineAdjust(){
}
/** Inicializa la seleccion*/
void CorteImagen::InitSelection(const QPoint &p){
    //Hay que extrapolar entre las dos imagenes
    QPoint pMod(p);
    if (p.y()>m_verticalZoomOffset) pMod.setY(p.y()-m_verticalZoomOffset);

    m_initPoint.setX((pMod.x()*m_image.width())/(m_zWidth*1000));
    m_initPoint.setY((pMod.y()*m_image.height())/(m_zHeight*1000));
    //Nos desplazamos en la imagen "grande"
    QRect newRect(QRect(m_initPoint,QSize()));
    //m_imageLabel->setRubber(newRect);
    ui->m_xini->setText(QString("%1").arg(m_initPoint.x()));
    ui->m_yini->setText(QString("%1").arg(m_initPoint.y()));
    ui->m_xfin->setText(QString("0"));
    ui->m_yfin->setText(QString("0"));
    ui->m_width->setText(QString("0"));
    ui->m_height->setText(QString("0"));
    updatemmdata();
}
void CorteImagen::ModifySelection(const QPoint &p){
    QPoint newPoint;
    QPoint pMod(p);
    if (p.y()>m_verticalZoomOffset) pMod.setY(p.y()-m_verticalZoomOffset);

    newPoint.setX((pMod.x()*1000*m_image.width())/(m_zWidth*1000));
    newPoint.setY((pMod.y()*1000*m_image.height())/(m_zHeight*1000));
    QRect newRect(QRect(m_initPoint,newPoint));
    //m_imageLabel->setRubber(newRect);
    ui->m_xfin->setText(QString("%1").arg(newPoint.x()));
    ui->m_yfin->setText(QString("%1").arg(newPoint.y()));
    ui->m_width->setText(QString("%1").arg(newPoint.x()-m_initPoint.x()));
    ui->m_height->setText(QString("%1").arg(newPoint.y()-m_initPoint.y()));
    updatemmdata();
    //UpdateMmData("0");
}

/** Inicializa la seleccion*/
void CorteImagen::InitSmallSelection(const QPoint &p){
    //Hay que extrapolar entre las dos imagenes
    m_initPoint=p;
    m_initSmallPoint.setX((p.x()*m_zWidth)/(m_image.width()));
    m_initSmallPoint.setY((p.y()*m_zHeight)/(m_image.height())+m_verticalZoomOffset);
    QRect newRect(QRect(m_initPoint,QSize()));
    ui->m_xini->setText(QString("%1").arg(m_initPoint.x()));
    ui->m_yini->setText(QString("%1").arg(m_initPoint.y()));
    ui->m_xfin->setText(QString("0"));
    ui->m_yfin->setText(QString("0"));
    ui->m_width->setText(QString("0"));
    ui->m_height->setText(QString("0"));
    updatemmdata();
}
void CorteImagen::ModifySmallSelection(const QPoint &p){
    QPoint newPoint;
    newPoint.setX((p.x()*m_zWidth)/(m_image.width()));
    newPoint.setY((p.y()*m_zHeight)/(m_image.height())+m_verticalZoomOffset);
    QRect newRect(QRect(m_initSmallPoint,newPoint));
    ui->m_xfin->setText(QString("%1").arg(p.x()));
    ui->m_yfin->setText(QString("%1").arg(p.y()));
    ui->m_width->setText(QString("%1").arg(p.x()-m_initPoint.x()));
    ui->m_height->setText(QString("%1").arg(p.y()-m_initPoint.y()));
    updatemmdata();
    //UpdateMmData("0");
}



/**Genera la imagen*/
void CorteImagen::GenerateImage(){
    QString dummy_bmp;
    QString comando;
    QLabel *dummyLabel=ShowMensaje("Rendering");
    comando.clear();
    dummy_bmp=QString("%1\\dummy.bmp").arg(m_memoryMap->m_mapa.m_variables.RutaBmp);
    QFile::remove(dummy_bmp);
    GsRenderEngine tmpRender;
    if (ui->m_cbDobleRes->isChecked()) tmpRender.setDoubleResolution();
    tmpRender.setInputFile(m_memoryMap->m_mapa.m_variables.NombreRutaPdf);
    tmpRender.setOutputFile(dummy_bmp);
    int dith=ui->m_cbBitsDithering->currentText().toInt();
    tmpRender.setDitheringBits(dith);
    tmpRender.render();
    //m_image.load(dummy_bmp,"bmp");
    QPixmap pixmap(dummy_bmp,"bmp");

    ui->m_wImagen->setText(QString("%1").arg(pixmap.width()));
    ui->m_hImagen->setText(QString("%1").arg(pixmap.height()));

    if (m_scene==NULL){
        m_scene = new In2GraphicsScene(0, 0,pixmap.width(),pixmap.height());
        m_In2GView=new In2GraphicsView(ui->m_graphicsView);
        In2GraphicsPixmapItem *p=new In2GraphicsPixmapItem(0,0,false);
        p->setPixmap(pixmap);
        m_scene->addItem(p);
        In2GraphicsPixmapItem *p1=new In2GraphicsPixmapItem(0,0,false);
        //p2->setPixmap(dummy);
        p1->setFlag(QGraphicsItem::ItemIsMovable, true);
       // p1->setFlag(QGraphicsItem::ItemPositionChange, true);
        //p1->setFlags(QGraphicsItem::ItemPositionChange|QGraphicsItem::ItemIsMovable);
        p1->enableShowPrintableZone(true);

        p1->setMaximumPrintZone(pixmap.width());
        p1->setStyle(CUT_STYLE);
        //p1->enableShowPrintableZone(true);
        int x0=ui->m_xini->text().toInt();
        int y0=ui->m_yini->text().toInt();
        int width=ui->m_width->text().toInt();
        int height=ui->m_height->text().toInt();
        QRect rect;
        if ((width<=0)||(height<=0))
            rect=pixmap.rect();
        else rect=QRect(x0,y0,width,height);

        //rect=pixmap.rect();
        p1->setPos(x0,y0);
        QRect org=pixmap.rect();
        p1->generatePrintZone(org);
        p1->generatePrintZone(rect);

        connect(p1,SIGNAL(sgnNewRect(QRect&)),this,SLOT(sltTrackPrintableZone(QRect&)));     
        m_scene->addItem(p1);

        ui->m_graphicsView->setScene(m_scene);
        ui->m_graphicsView->fitInView(p,Qt::KeepAspectRatio);
        ui->m_graphicsView->show();
    }
    else{
        QList<QGraphicsItem*> items=m_scene->items();
        In2GraphicsPixmapItem *p;        
        if (items.count()==2){
            p=static_cast<In2GraphicsPixmapItem*>(items.at(1));
            p->setPixmap(pixmap);
        }
        else{
            p=new In2GraphicsPixmapItem(0,0,false);
            p->setPixmap(pixmap);             
            m_scene->addItem(p);
        }
        //m_scene->setSceneRect(0,0,lienzo->width(),lienzo->height());
        ui->m_graphicsView->fitInView(p,Qt::KeepAspectRatio);
        ui->m_graphicsView->centerOn(p);
        //ui->m_ImagenCentral->fitInView(p,Qt::KeepAspectRatio);
    }


}
/** Hace track del corte de imagen*/
void CorteImagen::sltTrackPrintableZone(QRect& pos){

    QList <QGraphicsItem*> itemsPrincipal=m_scene->items();
    In2GraphicsPixmapItem *printable=static_cast<In2GraphicsPixmapItem*>(itemsPrincipal.at(0));
    QRectF printableRectf=printable->sceneBoundingRect();
    QRect rect=printableRectf.toRect();


    ui->m_xini->setText(QString("%1").arg(rect.x()));
    ui->m_yini->setText(QString("%1").arg(rect.y()));
    ui->m_xfin->setText(QString("%1").arg(rect.x()+rect.width()));
    ui->m_yfin->setText(QString("%1").arg(rect.y()+rect.height()));
    ui->m_width->setText(QString("%1").arg(rect.width()));
    ui->m_height->setText(QString("%1").arg(rect.height()));

    ui->m_xini_2->setText(QString("%1").arg(rect.x()/14.18));
    ui->m_yini_2->setText(QString("%1").arg(rect.y()/14.18));
    ui->m_xfin_2->setText(QString("%1").arg((rect.x()+rect.width())/14.18));
    ui->m_yfin_2->setText(QString("%1").arg((rect.y()+rect.height())/14.18));
    ui->m_width_2->setText(QString("%1").arg(rect.width()/14.18));
    ui->m_height_2->setText(QString("%1").arg(rect.height()/14.18));

    /*ui->m_xini=rect.x();
    ui->m_yini=rect.y();
    ui->m_xfin=rect.x()+rect.width();
    ui->m_yfin=rect.y()+rect.height();
    ui->m_width=rect.width();
    ui->m_height=rect.height();*/


}

/** Salvamos y salimos*/
void CorteImagen::SaveAndExit(){
    //Pendiente guardar
    QMessageBox msgBox;
    msgBox.setText("Quieres guardar el corte seleccionado?");

    msgBox.setStandardButtons(QMessageBox::Yes |QMessageBox::No);
    float x0=ui->m_xini_2->text().toFloat();
    float y0=ui->m_yini_2->text().toFloat();
    float height=ui->m_height_2->text().toFloat();
    float width=ui->m_width_2->text().toFloat();

    int ret = msgBox.exec();
    switch (ret){
        case QMessageBox::Yes:
            if (x0<0) x0=0;
            else if (x0>m_scene->width()) x0=m_scene->width()-1;
            if (y0<0) y0=0;
            else if (y0>m_scene->height()) y0=m_scene->height()-1;
            if ((x0+width)>m_scene->width()) width=m_scene->width()-x0;
            if ((y0+height)>m_scene->width()) width=m_scene->width()-x0;

            m_memoryMap->m_mapa.m_variables.XinicialPlantilla=ui->m_xini_2->text().toFloat();
            m_memoryMap->m_mapa.m_variables.YinicialPlantilla=ui->m_yini_2->text().toFloat();
            m_memoryMap->m_mapa.m_variables.AltoPlantilla=ui->m_height_2->text().toFloat();
            m_memoryMap->m_mapa.m_variables.AnchoPlantilla=ui->m_width_2->text().toFloat();
            //Solo lo guardo aqui mismo
            m_memoryMap->m_mapa.m_variables.AlgdobleRes=ui->m_cbDobleRes->isChecked();
            m_memoryMap->m_mapa.m_variables.iBitsDithering=ui->m_cbBitsDithering->currentText().toInt();
        break;
        case QMessageBox::No:
        break;
        default:

        break;
    }

    this->Exit();
}

/** Salimos*/
void CorteImagen::Exit(){
    EditImagenFija *ScrEditImagenFija;
    ScrEditImagenFija=new EditImagenFija(0);
    ScrEditImagenFija->show();
    this->deleteLater();
}

CorteImagen::~CorteImagen()
{

}
