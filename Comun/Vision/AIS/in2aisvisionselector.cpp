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
#include "in2aisvisionselector.h"
#include "in2graphicsscene.h"
#include "in2sceneselection.h"
#include <QRubberBand>
#include <QComboBox>
#include "MemoryMap.h"
#include "in2aisvisionobject.h"
#include "in2aisconfigurator.h"
#include "in2compofile.h"
#include "in2defines.h"
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include "in2gestorvision.h"
#include <QDateTime>
#include <QScrollBar>
#include "in2aisstringgen.h"
#include "in2configuratorvision.h"
#include "in2ping.h"

#define COL_WIDTH 75
#define COL_TYPE 0
#define COL_X 1
#define COL_Y 2
#define COL_ANGLE 3
#define COL_W 4
#define COL_H 5
#define COL_OCR 6

#define STYLE_NOCONN "QLineEdit { background: rgb(255, 0, 0); selection-background-color: rgb(233, 99, 0); }"
#define STYLE_CONN "background-color: rgba(255,255,255,255);"


In2AISVisionSelector::In2AISVisionSelector(QWidget *parent) :
    ui(new Ui::In2AISVisionSelection)
{
    ui->setupUi(this);
    this->showFullScreen();
    m_parent=NULL;
    m_parent=parent;
    m_view=new In2GraphicsView(ui->m_gv);
    //En eset caso añado 2 items
    In2GraphicsPixmapItem *item=new In2GraphicsPixmapItem(0);
    m_scene=new In2GraphicsScene(0,0,0,0);

    m_scene->addItem(item);
    ui->m_gv->setScene(m_scene);

    connect(m_view,SIGNAL(trackPoint(QPointF)),this,SLOT(UpdateMousePos(QPointF)));

#if QT_VERSION>=0x050000
     ui->m_twData->horizontalHeader()->setSectionResizeMode(COL_OCR, QHeaderView::Stretch );
#else
     ui->m_twData->horizontalHeader()->setResizeMode(COL_OCR, QHeaderView::Stretch );
#endif


    //ui->m_twData->horizontalHeader()->setResizeMode(COL_OCR, QHeaderView::Stretch );
    ui->m_twData->setColumnWidth(COL_TYPE,COL_WIDTH*2);
    ui->m_twData->setColumnWidth(COL_X,COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_Y,COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_ANGLE,COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_W,COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_H,COL_WIDTH);
    //ui->m_twData->setColumnWidth(COL_NAME,COL_WIDTH*2);
    ui->m_twData->setColumnWidth(COL_OCR,COL_WIDTH*3);

#if QT_VERSION>=0x050000
     ui->m_twData->horizontalHeader()->setSectionResizeMode(COL_OCR, QHeaderView::Stretch );

#else
     //ui->m_twData->horizontalHeader()->setResizeMode(COL_OCR, QHeaderView::Stretch );
#endif

    QPalette palette = ui->m_twData->palette();
    palette.setColor(QPalette::Highlight,Qt::red);
    ui->m_twData->setPalette(palette);


    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(sltExit()));
    connect(ui->m_pbExit,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->m_pbLoad,SIGNAL(clicked()),this,SLOT(sltLoadFile()));
    connect(ui->m_pbConfig,SIGNAL(clicked()),this,SLOT(sltVisionConfig()));
    connect(ui->m_pbSend,SIGNAL(clicked()),this,SLOT(sltSend()));
    connect(ui->m_pbClearLog,SIGNAL(clicked()),ui->m_teLog,SLOT(clear()));
    connect(ui->m_pbVisionLoad,SIGNAL(clicked()),this,SLOT(sltVisionLoad()));
    connect(ui->m_pbVisionSave,SIGNAL(clicked()),this,SLOT(sltVisionSave()));
    connect(ui->m_twData,SIGNAL(cellClicked(int,int)),this,SLOT(sltCellClicked(int,int)));
    connect(ui->m_pbPing,SIGNAL(clicked()),this,SLOT(sltPing()));
    connect(ui->m_pbWOL,SIGNAL(clicked()),this,SLOT(sltWOL()));
    connect(ui->m_pbApagar,SIGNAL(clicked()),this,SLOT(sltApagar()));
    connect(ui->m_pbReiniciar,SIGNAL(clicked()),this,SLOT(sltReiniciar()));
    connect(ui->m_pbMapear,SIGNAL(clicked()),this,SLOT(sltRemapear()));
    connect(ui->m_pbConectar,SIGNAL(clicked()),this,SLOT(sltConectar()));
    //QTimer::singleShot(0,this,SLOT(sltLoadImage()));
    m_counter=0;
    m_offsetX=0;
    m_offsetY=0;

    In2ConfiguratorVision config;
    config.readConfig();
    ui->m_leIP->setText(config.getIP());
    ui->m_leFactorX->setText(QString("%1").arg(config.getCX()));
    ui->m_leFactorY->setText(QString("%1").arg(config.getCY()));


    In2GestorVision *gestor=In2GestorVision::request(0);
    QObject::connect(gestor,SIGNAL(sgnVisionReceived(QString)),this,SLOT(sltReceivedDatagram(QString)));
    QObject::connect(gestor,SIGNAL(sgnSocketConnected()),this,SLOT(sltConnected()));
    QObject::connect(gestor,SIGNAL(sgnSocketDisconnected()),this,SLOT(sltDisConnected()));
    QObject::connect(gestor,SIGNAL(sgnSocketConnected()),this,SLOT(sltEquipoConnected()));
    QObject::connect(gestor,SIGNAL(sgnSocketConnected()),this,SLOT(sltAplicacionConnected()));
    QObject::connect(gestor,SIGNAL(sgnVisionRemoteAlive()),this,SLOT(sltEquipoConnected()));
    QObject::connect(gestor,SIGNAL(sgnSocketDisconnected()),this,SLOT(sltAplicacionDisConnected()));
    QObject::connect(gestor,SIGNAL(sgnVisionRemoteDead()),this,SLOT(sltEquipoDisConnected()));
    QObject::connect(gestor,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SLOT(sltMessageFromVision(unsigned char,QString)));
    (gestor->socketConnected())?sltAplicacionConnected():sltAplicacionDisConnected();
    (gestor->equipoConnected())?sltEquipoConnected():sltEquipoDisConnected();
    /*if (gestor->isConnected()){
        sltConnected();
    }
    else{
        sltDisConnected();
    }*/
}
/** Mensaje de vision*/
void In2AISVisionSelector::sltMessageFromVision(unsigned char value,QString txt){
    sltPublishInfo(txt);
}

/** Intenta conectar*/
void In2AISVisionSelector::sltConectar(){
    In2GestorVision *gestor=In2GestorVision::request(0);
    if (gestor->connect()){
        sltPublishInfo(tr("Conectado"));
    }
    else {
        sltPublishInfo(tr("No se pudo conectar"));
    }
}

/** Intenta remapear*/
void In2AISVisionSelector::sltRemapear(){
    In2GestorVision *gestor=In2GestorVision::request(0);
    if (gestor->sltMapearUnidad())  sltPublishInfo(tr("Unidad remapeada"));
    else sltPublishInfo(tr("No se pudo remapear unidad"));
}

/** Intenta apagar el sistema*/
void In2AISVisionSelector::sltApagar(){
    QString ip=QString("\\\\%1").arg(ui->m_leIP->text());
    QProcess foo;

    QString txt=QString("shutdown.exe /s /m %1 /t 00").arg(ip);

    QFile myBat("shutdown.bat");
    if (myBat.exists()) myBat.remove();
    if (myBat.open(QIODevice::WriteOnly)){
#if QT_VERSION>=0x050000
  myBat.write(txt.toLatin1().data());
#else
  myBat.write(txt.toAscii().data());
#endif
        myBat.close();
    }


    foo.start("cmd.exe", QStringList() << "/c" << "shutdown.bat");
    if (foo.waitForStarted()){
            //Lo bloqueo o no?
            if (foo.waitForFinished()) {
                sltPublishInfo(tr("Apagando %1").arg(ip));
            }
            else sltPublishInfo(tr("Error, proceso de apagado no completado"));
    }
    else sltPublishInfo(tr("Error, proceso de apagado no iniciado"));

}

/** Intenta apagar el sistema*/
void In2AISVisionSelector::sltReiniciar(){
    //In2ConfiguratorVision config;
    //config.readConfig();
    //QString ip=QString("\\\\%1").arg(config.getIP());
    QString ip=QString("\\\\%1").arg(ui->m_leIP->text());
    QProcess foo;

    QString txt=QString("shutdown.exe /r /m %1 /t 00").arg(ip);

    QFile myBat("shutdown.bat");
    if (myBat.exists()) myBat.remove();
    if (myBat.open(QIODevice::WriteOnly)){
#if QT_VERSION>=0x050000
  myBat.write(txt.toLatin1().data());
#else
  myBat.write(txt.toAscii().data());
#endif
        myBat.close();
    }


    foo.start("cmd.exe", QStringList() << "/c" << "shutdown.bat");
    if (foo.waitForStarted()){
            //Lo bloqueo o no?
            if (foo.waitForFinished()) {
                sltPublishInfo(tr("Apagando %1").arg(ip));
            }
            else sltPublishInfo(tr("Error, proceso de reinicio no completado"));
    }
    else sltPublishInfo(tr("Error, proceso de reinicio no iniciado"));

}


/** Hace un WOL*/
void In2AISVisionSelector::sltWOL(){
    In2ConfiguratorVision config;
    config.readConfig();
    In2GestorVision *gestor=In2GestorVision::request(0);
    gestor->wakeUp();
    QString MAC=config.getMAC();
    sltPublishInfo(tr("WOL to %1 OK...").arg(MAC));
}

/** Hacemos un ping */
void In2AISVisionSelector::sltPing(){
    In2Ping foo;
    QString ip=ui->m_leIP->text();
    bool rtn=foo.ping(ip);
    if (rtn){
        sltPublishInfo(tr("IP to %1 OK").arg(ip));
    }
    else{
        sltPublishInfo(tr("IP to %1 NOK").arg(ip));
    }

}

/** Avisa de conexion*/
void In2AISVisionSelector::sltConnected(){
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::green);
    ui->m_leIP->setPalette(palette);            
}
/** Avisa de conexion*/
void In2AISVisionSelector::sltDisConnected(){
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::red);
    ui->m_leIP->setPalette(palette);
}


/** Carga un bmp*/
void In2AISVisionSelector::loadBMP(QString file){
    m_file=file;
    QTimer::singleShot(0,this,SLOT(sltLoadImage()));
}

/** Carga un programa*/
void In2AISVisionSelector::loadProgram(QString file){
    QFileInfo info(file);
    ui->m_leVisionText->setText(info.baseName());
}


/** Actualiza la posicion del mouse*/
void In2AISVisionSelector::UpdateMousePos(const QPointF &p ){
    int x=p.x();
    int y=p.y();
    ui->m_lblPos->setText(QString("%1x%2").arg(x).arg(y));
}

/** Selecciona una columna*/
void In2AISVisionSelector::sltCellClicked(int x, int y){
    ui->m_twData->selectRow(x);
}



/** Configura un programa de vission*/
void In2AISVisionSelector::sltVisionConfig(){
    In2AISConfigurator p;
    QString txt=p.getStringConfig(ui->m_leVisionText->text());
    ui->m_teString->setText(txt);
    sltSend();
}

/** Carga un programa de vission*/
void In2AISVisionSelector::sltVisionLoad(){
    In2AISConfigurator p;
    QString txt=p.getStringLoad(ui->m_leVisionText->text());
    ui->m_teString->setText(txt);
    sltSend();
}

/** Guarda un programa de vission*/
void In2AISVisionSelector::sltVisionSave(){
    In2AISConfigurator p;
    QString txt=p.getStringSave(ui->m_leVisionText->text());
    ui->m_teString->setText(txt);
    sltSend();
}


/** Carga un fichero .ini*/
void In2AISVisionSelector::sltLoadFile(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QFileDialog fileName(this);
    QStringList Ficheros;

    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("Archivo INI(*.ini)"));
#else
    fileName.setFilter(tr("Archivos(*.ini *bmp)"));
#endif
    fileName.setDirectory(memoryMap->getImagePath());
    Ficheros=fileName.getOpenFileNames(this,"Selecciona un fichero",memoryMap->getImagePath(),tr("Archivos(*.ini *.bmp)"));
    if (!Ficheros.isEmpty()){
        QString foo=Ficheros.at(0);
        QFileInfo info(foo);
        QString visionFile;

        if (!info.suffix().compare("ini",Qt::CaseInsensitive)){
            //foo.replace(".ini","_C1.bmp",Qt::CaseInsensitive);
            m_file=foo;
            m_file.replace(".ini","_OF_C1_comp.bmp",Qt::CaseInsensitive);
            sltLoadImage();
            memoryMap->m_mapa.m_variables.NombreRutaIni=foo;
            sltLoadIniFile(foo);
            visionFile=info.baseName();
            ui->m_leVisionText->setText(visionFile);

        }
        else if (!info.suffix().compare("bmp",Qt::CaseInsensitive)){
            m_file=foo;            
            sltLoadImage();
            visionFile=foo.left(foo.indexOf("_"));
            ui->m_leVisionText->setText(visionFile);
        }
    }
}

QVector<In2AISVisionObject> In2AISVisionSelector::getData(){
    int count=ui->m_twData->rowCount();
    QVector<In2AISVisionObject> lista;
    In2AISVisionObject obj;
    for(int z=0;z<count;z++){
        QComboBox *p1=static_cast<QComboBox*>(ui->m_twData->cellWidget(z,COL_TYPE));
        int index=0;
        if (p1) index=p1->currentIndex();
        QTableWidgetItem *p=ui->m_twData->item(z,COL_X);
        QString x=p->text();
        p=ui->m_twData->item(z,COL_Y);
        QString y=p->text();
        p=ui->m_twData->item(z,COL_W);
        QString w=p->text();
        p=ui->m_twData->item(z,COL_H);
        QString h=p->text();
        /*p=ui->m_twData->item(z,COL_NAME);
        QString name=p->text();*/
        p=ui->m_twData->item(z,COL_OCR);
        QString txt=p->text();
        obj.setData(index,x,y,h,w,txt,"");
        lista.append(obj);
    }

    return lista;
}

/** Carga un fichero .ini*/
void In2AISVisionSelector::sltLoadIniFile(QString file){
    if (QFile::exists(file)){
        //Leemos la configuiracion de visión
        In2CompoFile iniConfig;
        iniConfig.AbrirFicheroIni();
        In2AISConfigurator config;
        if (config.readConfig(file)){
            QVector<In2AISVisionObject> lista=config.getAISVisionObjects();
            int count=lista.count();
            for (int x=0;x<count;x++){
                sltAddItem(lista.at(x));
            }
            //Intentamos poner la macula como centro
            MemoryMap *memoryMap=MemoryMap::request(0);
            int cx=(memoryMap->m_mapa.m_variables.MaculaIzqdaX+memoryMap->m_mapa.m_variables.MaculaIzqdaAncho)*FACTOR_PIXELS_MM/2;
            int cy=(memoryMap->m_mapa.m_variables.MaculaIzqdaY+memoryMap->m_mapa.m_variables.MaculaIzqdaAlto)*FACTOR_PIXELS_MM/2;
            ui->m_leCx->setText(QString("%1").arg(cx));
            ui->m_leCy->setText(QString("%1").arg(cy));
        }
        else{
            QMessageBox::warning(0,"Error",tr("No se pudo leer %1").arg(file),QMessageBox::Ok);
        }
    }
}
/** elimina un item*/
void In2AISVisionSelector::sltDeleteItem(){
    int index=ui->m_twData->currentRow();
    int x0=ui->m_twData->item(index,COL_X)->text().toInt();
    int y0=ui->m_twData->item(index,COL_Y)->text().toInt();
    int w0=ui->m_twData->item(index,COL_W)->text().toInt();
    int h0=ui->m_twData->item(index,COL_H)->text().toInt();

    int cx=ui->m_leCx->text().toInt();
    int cy=ui->m_leCy->text().toInt();
    double factorX=ui->m_leFactorX->text().toFloat();
    double factorY=ui->m_leFactorY->text().toFloat();


    if (index!=-1){
        ui->m_twData->removeRow(index);
        QList<QGraphicsItem*> items=m_scene->items();
        for (int x=0;x<items.count();x++){
            In2SceneSelection *p=static_cast<In2SceneSelection*>(items.at(x));
            int x1=(p->scenePos().x()-cx)*factorX;
            int y1=(p->scenePos().y()-cy)*factorY;
            int w1=p->pixmap().width()*factorX;
            int h1=p->pixmap().height()*factorY;
            //QRect rect=p->pixmap().rect();
            if ((x0==x1)&&(y0==y1)&&(w0==w1)&&(h0==h1)){
                delete (p);
                break;
            }
            /*if (p->getID()) {
                delete(p);
                break;
            }*/
        }                //Hay que eliminar la zona marcada
    }
}
/** Genera un color segun el indice*/
QColor In2AISVisionSelector::getItemColor(int index){
    QColor color;
    if (index==0) color=QColor(255,0,0,75);
    else if (index==1) color=QColor(125,125,125,75);
    else if (index==2) color=QColor(0,255,0,75);
    else if (index==3) color=QColor(125,0,255,75);
    else if (index==4) color=QColor(125,255,0,75);
    else if (index==5) color=QColor(0,255,125,75);


    else if (index==6) color=QColor(125,0,0,75);
    else if (index==7) color=QColor(125,0,125,75);
    else if (index==8) color=QColor(125,125,0,75);
    else if (index==9) color=QColor(0,125,0,75);
    else if (index==10) color=QColor(0,125,125,75);
    else if (index==11) color=QColor(0,0,0,75);
    else color=QColor(125,125,125,75);

    return color;
}

/** Añade un item*/
void In2AISVisionSelector::sltAddItem(In2AISVisionObject obj){
    int count=ui->m_twData->rowCount();
    float factorX=ui->m_leFactorX->text().toFloat();
    float factorY=ui->m_leFactorY->text().toFloat();
    float x=(obj.getX().toFloat()+m_offsetX)/factorX;
    float y=(obj.getY().toFloat()+m_offsetY)/factorY;
    float w=obj.getW().toFloat()/factorX;
    float h=obj.getH().toFloat()/factorY;
    float angle=obj.getAngle().toFloat();
    //QString strOCR=obj.getString();
    QString strName=obj.getName();

    QColor color=getItemColor(count);
    In2SceneSelection *select=new In2SceneSelection(0,count);
    QRect rect=QRect(0,0,w,h);    
    select->generatePrintZoneExt(rect,angle,color);
    m_scene->addItem(select);
    select->setPos(x,y);
    select->setFlag(QGraphicsItem::ItemIsMovable, false);

    ui->m_gv->show();
    ui->m_twData->setRowCount(count+1);


    QComboBox *combo=new QComboBox(0);
    combo->addItems(QStringList()<<"OCR"<<"T.GENERAL"<<"T.CABEZAL");
    combo->setCurrentIndex(obj.getType());
    ui->m_twData->setCellWidget(count,COL_TYPE,combo);

    QPalette palette = ui->m_twData->palette();
    palette.setColor(QPalette::Background,color);

    QTableWidgetItem *p=new QTableWidgetItem(obj.getX());
    Qt::ItemFlags Flags;
    Flags=p->flags();
    Flags&=~(Qt::ItemIsEditable);
    p->setFlags(Flags);
    p->setBackgroundColor(color);
    ui->m_twData->setItem(count,COL_X,p);

    p=new QTableWidgetItem(obj.getY());
    Flags=p->flags();
    Flags&=~(Qt::ItemIsEditable);
    p->setFlags(Flags);
    p->setBackgroundColor(color);
    ui->m_twData->setItem(count,COL_Y,p);
    p=new QTableWidgetItem(obj.getAngle());
    Flags=p->flags();
    Flags&=~(Qt::ItemIsEditable);
    p->setFlags(Flags);
    p->setBackgroundColor(color);
    ui->m_twData->setItem(count,COL_ANGLE,p);

    p=new QTableWidgetItem(obj.getW());
    Flags=p->flags();
    Flags&=~(Qt::ItemIsEditable);
    p->setFlags(Flags);
    p->setBackgroundColor(color);
    ui->m_twData->setItem(count,COL_W,p);

    p=new QTableWidgetItem(obj.getH());
    Flags=p->flags();
    Flags&=~(Qt::ItemIsEditable);
    p->setFlags(Flags);
    p->setBackgroundColor(color);
    ui->m_twData->setItem(count,COL_H,p);

    /*p=new QTableWidgetItem(strName);
    Flags=p->flags();
    Flags&=~(Qt::ItemIsEditable);
    p->setFlags(Flags);
    p->setBackgroundColor(color);
    ui->m_twData->setItem(count,COL_NAME,p);*/

    connect(select,SIGNAL(sgnRect(int,QRect&)),this,SLOT(sltUpdateGeometry(int,QRect&)));
    m_counter++;
}



/** Añade un item*/
void In2AISVisionSelector::sltAddItem(){
    int count=ui->m_twData->rowCount();
    In2SceneSelection *select=new In2SceneSelection(0,count);
    QRect rect=QRect(0,0,100,100);
    select->generatePrintZone(rect);
    m_scene->addItem(select);

    ui->m_twData->setRowCount(count+1);
    QComboBox *combo=new QComboBox(0);
    combo->addItems(QStringList()<<"OCR"<<"T.GENERAL"<<"T.CABEZAL");
    ui->m_twData->setCellWidget(count,COL_TYPE,combo);

    QTableWidgetItem *p=new QTableWidgetItem("-");
    ui->m_twData->setItem(count,COL_X,p);
    p=new QTableWidgetItem("-");
    ui->m_twData->setItem(count,COL_Y,p);
    p=new QTableWidgetItem("-");
    ui->m_twData->setItem(count,COL_ANGLE,p);
    p=new QTableWidgetItem("-");
    ui->m_twData->setItem(count,COL_W,p);
    p=new QTableWidgetItem("-");
    ui->m_twData->setItem(count,COL_H,p);
    /*p=new QTableWidgetItem(QString("ITEM%1").arg(count));
    ui->m_twData->setItem(count,COL_NAME,p);*/
    p=new QTableWidgetItem("");
    ui->m_twData->setItem(count,COL_OCR,p);


    connect(select,SIGNAL(sgnRect(int,QRect&)),this,SLOT(sltUpdateGeometry(int,QRect&)));
}
/** Actualiza la geometria de una sleccion*/
void In2AISVisionSelector::sltUpdateGeometry(int id, QRect & rect){
    int cx=ui->m_leCx->text().toInt();
    int cy=ui->m_leCy->text().toInt();
    float factorx=ui->m_leFactorX->text().toFloat();
    float factory=ui->m_leFactorY->text().toFloat();
    int count=ui->m_twData->rowCount();
    if (id<count){
        ui->m_twData->selectRow(id);
        int x=(rect.x()-cx)*factorx;
        int y=(rect.y()-cy)*factory;
        int w=rect.width()*factorx;
        int h=rect.height()*factory;

        QTableWidgetItem *p=ui->m_twData->item(id,COL_X);
        p->setText(QString("%1").arg(x));

        p=ui->m_twData->item(id,COL_Y);
        p->setText(QString("%1").arg(y));

        p=ui->m_twData->item(id,COL_W);
        p->setText(QString("%1").arg(w));

        p=ui->m_twData->item(id,COL_H);
        p->setText(QString("%1").arg(h));

    }
}

/** Salimos*/
void In2AISVisionSelector::sltExit(){
    this->deleteLater();
}

/** Destructor*/
In2AISVisionSelector::~In2AISVisionSelector(){
}

/** Carga la imagen*/
void In2AISVisionSelector::sltLoadImage(){
    QPixmap pixmap(m_file);
    //Necesito girar la imagen 180º
    //QPixmap pixmap=tmp.transformed(QTransform().rotate(180,Qt::ZAxis));
    if (!pixmap.isNull()){
        QList<QGraphicsItem*> items=m_scene->items();
        In2GraphicsPixmapItem *p;
        if (items.count()){
            p=static_cast<In2GraphicsPixmapItem*>(items.at(0));
            QPainter painter(&pixmap);
            QPen pen(Qt::blue, 2 , Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);
            painter.drawRect(pixmap.rect());
            p->setPixmap(pixmap);

            /*QRect rect=QRect(0,0,100,100);
            p->generatePrintZone(rect);*/
            m_scene->setSceneRect(pixmap.rect());
            ui->m_gv->fitInView(p,Qt::KeepAspectRatio);
            ui->m_gv->show();
        }
    }
}

/** Envia datos*/
void In2AISVisionSelector::sltSend(){
    QString data=ui->m_teString->toPlainText();
    In2GestorVision *gestor=In2GestorVision::request(this);
    if (!gestor->sendString(data)){
        sltPublishInfo(tr("NO se pundo enviar STRING"));
    }
    else
        sltPublishInfo(tr("SNT: %1").arg(data));

}

/** Llega un datagrama*/
void In2AISVisionSelector::sltReceivedDatagram(QString txt){
    //Es un string de configuracion
    if (txt.at(0)=='%'){
        sltGetConfigFromAIS(txt);
    }
    sltPublishInfo(txt);
}

/** Publica info*/
void In2AISVisionSelector::sltPublishInfo(QString txt){
    QString time=QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->m_teLog->append(QString("[%1] %2").arg(time).arg(txt));
    QScrollBar *sb=ui->m_teLog->verticalScrollBar();
    if (sb){
        sb->setValue(sb->maximum());
    }
}

/** Parasea la info de AIS*/
void In2AISVisionSelector::sltGetConfigFromAIS(QString txt){
    m_offsetX=0;
    m_offsetY=0;

    /*int count=ui->m_twData->rowCount();
    //Se borra lo que haya
    for (int x=0;x<count;x++){
        ui->m_twData->selectRow(0);
        sltDeleteItem();
    }*/
    //m_scene->clear();
    QList<QGraphicsItem*> items=m_scene->items();
    int count=items.count();
    if (count>1){
        for (int x=0;x<count-1;x++){
            QGraphicsItem *p=items.at(x);
            delete (p);
        }
    }

    ui->m_twData->clearContents();
    ui->m_twData->setRowCount(0);
    m_counter=0;
    In2AISStringGen generator;
    QVector<In2AISVisionObject> ocrs=generator.getOCRList(txt);
    In2AISVisionObject templateGeneral=generator.getTemplateGeneral(txt);
    In2AISVisionObject templateCabezal=generator.getTemplateCabezal(txt);
    m_offsetX-=templateGeneral.getX().toFloat();
    m_offsetY-=templateGeneral.getY().toFloat();
    sltAddItem(templateGeneral);
    sltAddItem(templateCabezal);

    MemoryMap *memoryMap=MemoryMap::request(0);
    for (int x=0;x<ocrs.count();x++){
        In2AISVisionObject ocr=ocrs.at(x);
        sltAddItem(ocr);
        QComboBox *combo=new QComboBox();        
        for (int y=0;y<NUM_CAMPOS;y++){
            if (memoryMap->m_mapa.m_campos[y].OnOffCampo) combo->addItem(memoryMap->m_mapa.m_campos[y].ValorActualCampo);

        }
        connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(sltOCRChange(int)));
        float factorX=ui->m_leFactorX->text().toFloat();
        float factorY=ui->m_leFactorY->text().toFloat();
        float Posx=((ocr.getX().toFloat()+m_offsetX)/factorX)/FACTOR_PIXELS_MM;
        float Posy=((ocr.getY().toFloat()+m_offsetY)/factorY)/FACTOR_PIXELS_MM;

        //QTableWidgetItem *p=new QTableWidgetItem();
        ui->m_twData->setCellWidget(m_counter-1,COL_OCR,combo);
        int closest=memoryMap->getClosestFixedField(Posx,Posy);
        if (closest!=-1){
            if (closest<combo->count()) combo->setCurrentIndex(closest);
        }
        ui->m_teRcvString->setText(txt);
        sltRegenConfigString();
    }

}
/** Ha habido un cambio OCR*/
void In2AISVisionSelector::sltOCRChange(int x){
    sltRegenConfigString();
}

/** Regenera el string de salida*/
void In2AISVisionSelector::sltRegenConfigString(){
    //QString txt=ui->m_teRcvString->toPlainText();
    In2AISStringGen generator;
    QString txt=ui->m_teRcvString->toPlainText();
    QVector<In2AISVisionObject> ocrs=generator.getOCRList(txt);
    In2AISVisionObject templateGeneral=generator.getTemplateGeneral(txt);
    In2AISVisionObject templateCabezal=generator.getTemplateCabezal(txt);
    for (int x=0;x<ocrs.count();x++){
        In2AISVisionObject ocr=ocrs.at(x);
        QComboBox *p=static_cast<QComboBox*>(ui->m_twData->cellWidget(2+x,COL_OCR));
        if (p){
            QString foo=p->currentText();
            ocr.setString(foo);
            ocrs.replace(x,ocr);
        }
    }

    QVector<In2AISVisionObject> newList;
    newList.append(templateGeneral);
    newList.append(templateCabezal);
    newList+=ocrs;
    QString rtn=generator.getString(newList);

    ui->m_teString->setText(rtn);
    ui->m_twData->resizeColumnsToContents();
}
/** Leds de estado de apklicacion*/
void In2AISVisionSelector::sltAplicacionConnected(){
    ui->m_ledAplicacion->setEnabled(true);
}
/** Leds de estado de apklicacion*/
void In2AISVisionSelector::sltAplicacionDisConnected(){
    ui->m_ledAplicacion->setEnabled(false);
}
/** Leds de estado de apklicacion*/
void In2AISVisionSelector::sltEquipoConnected(){
    ui->m_ledEquipo->setEnabled(true);
    sltConnected();
}
/** Leds de estado de apklicacion*/
void In2AISVisionSelector::sltEquipoDisConnected(){
    ui->m_ledEquipo->setEnabled(false);
    sltDisConnected();
}
