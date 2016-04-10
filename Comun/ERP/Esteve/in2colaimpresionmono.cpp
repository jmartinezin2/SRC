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
#include "in2colaimpresionmono.h"
#include "in2gestorerp.h"
#include "in2configuratorerp.h"
#include "in2esteveerp.h"
#include "MemoryMap.h"
#include "inkjetdebug.h"
#include "in2defines.h"
#include <QTimer>
#include <QFileInfo>
#include <QMessageBox>
#include "in2store.h"
#include "in2managererp.h"
#include "in2imagecreator.h"

#define UPDATE_TIME 5000

In2ColaImpresionMono::In2ColaImpresionMono(QWidget *parent) :
    ui(new Ui::In2ColaImpresionMono)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    m_parent=parent;
    m_actualOT.clear();
    In2ManagerERP *p=In2ManagerERP::request(0);
    connect(p,SIGNAL(sgnERPWorkLoaded(int)),this,SLOT(sltWorkLoaded(int)));
    connect(p,SIGNAL(sgnMessage(QString)),this,SLOT(sltInfoMessage(QString)));

    ui->m_gbTableIn->setTitle(p->getItem(ERP_TABLE_IN));
    ui->m_gbTableOut->setTitle(p->getItem(ERP_TABLE_OUT));

    ui->m_twData->setColumnCount(1);
    QString name=p->getItem(ERP_TABLE_IN);
    ui->m_twData->setHorizontalHeaderLabels(QStringList()<<name);

    ui->m_twCurrentData->setColumnCount(1);
    name=p->getItem(ERP_TABLE_IN);
    ui->m_twData->setHorizontalHeaderLabels(QStringList()<<name);


    ui->m_twDataOut->setColumnCount(1);
    name=p->getItem(ERP_TABLE_OUT);
    ui->m_twDataOut->setHorizontalHeaderLabels(QStringList()<<name);


#if QT_VERSION>=0x050000
     ui->m_twData->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
     ui->m_twDataOut->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->m_twData->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
    ui->m_twDataOut->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twCurrentData->horizontalHeader()->setResizeMode(QHeaderView::Stretch);


#endif


    m_pendingScan=false;
    m_workLoaded=false;
    //Cargamos el scene y el view
    m_view=new In2GraphicsView(ui->m_gv);
    m_scene=NULL;

    m_scene=new In2GraphicsScene(0,0,0,0);
    In2GraphicsPixmapItem *item=new In2GraphicsPixmapItem(0,0,false);
    m_scene->addItem(item);
    ui->m_gv->setScene(m_scene);
    ui->m_gv->fitInView(item,Qt::KeepAspectRatio);
    ui->m_gv->show();



    connect(ui->m_pbExit,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->m_pbLoad,SIGNAL(clicked()),this,SLOT(sltLoad()));
    connect(ui->m_pbClearInfoMessage,SIGNAL(clicked()),this,SLOT(sltClearInfoMessage()));

    InkjetDebug *inkjetDebug=InkjetDebug::request(0);
    connect(ui->m_pbLog,SIGNAL(clicked()),inkjetDebug,SLOT(showLog()));

    //connect(ui->actionERP)
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(sltExit()));
    connect(ui->actionLoad_Config,SIGNAL(triggered()),this,SLOT(sltLoad()));
    connect(ui->actionShow_Log,SIGNAL(triggered()),inkjetDebug,SLOT(showLog()));
    connect(ui->m_pbLoadScan,SIGNAL(clicked()), this,SLOT(sltFocusScan()));
    connect(ui->m_leScanBarcode,SIGNAL(textEdited(QString)),this,SLOT(sltCheckAndLoad()));

    //ui->m_leScanBarcode->setFocus();
    ui->m_leScanBarcode->setEnabled(false);
    ui->m_pbLoadScan->setEnabled(false);

    Qt::WindowFlags flags=this->windowFlags();
    flags|=Qt::CustomizeWindowHint;
    this->setWindowFlags(flags);
    m_bCheckERP=true;
    m_pedingOrder=false;
    In2ManagerERP *erp=In2ManagerERP::request(0);
    if (erp->orderPending()){
        m_pedingOrder=true;
        QStringList data=erp->lstOrderPending();
        if (data.count()){
            ui->m_twCurrentData->clearContents();
            QStringList strlstHeaders=erp->getItem(ERP_FIELDS_BBDD_IN).split("@");
            int nHeaders=strlstHeaders.count();
            if (nHeaders<=data.count()){
                ui->m_twCurrentData->setRowCount(nHeaders);
                ui->m_twCurrentData->setVerticalHeaderLabels(strlstHeaders);
                for (int x=0;x<nHeaders;x++){
                    QTableWidgetItem *item=new QTableWidgetItem(data.at(x));
                    Qt::ItemFlags flags=0;
                    item->setFlags(flags);
                    ui->m_twCurrentData->setItem(x,0,item);
                }
            }
            else{
                emit SendDebugInfo(LOGGER_ERROR,tr("sltDataFromPendingOrder expected %1 items got %2").arg(nHeaders).arg(data.count()));
                //emit

            }
        }

    }
    else{
        QTimer::singleShot(0,this,SLOT(sltCheckERP()));
        QTimer::singleShot(0,this,SLOT(sltCheckERPOut()));
    }
    this->show();

    //QTimer::singleShot(0,this,SLOT(_sltCheckAndLoad()));

}
void In2ColaImpresionMono::_loadPendingOrder(){
    //QMessageBox::warning(0,"INFO",tr("Hay una orden pendiente"),QMessageBox::Ok);
    In2ManagerERP *erp=In2ManagerERP::request(0);
    QStringList rslt=erp->lstOrderPending();
    sltDataFromPendingOrder(rslt);

}

void In2ColaImpresionMono::setCheckERP(bool val){
    m_bCheckERP=val;
}

/** Pone el foco y limpia la captura del scanner*/
void In2ColaImpresionMono::sltFocusScan(){
    ui->m_leScanBarcode->clear();
    ui->m_leScanBarcode->setFocus();
}

/** Chequea el CB y carga el trabajo si es correcto*/
void In2ColaImpresionMono::sltCheckAndLoad(){
    if (!m_pendingScan){
        QTimer::singleShot(1000,this,SLOT(_sltCheckAndLoad()));
        m_pendingScan=true;
    }
}

/** Chequea el Scan*/
void In2ColaImpresionMono::_sltCheckAndLoad(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString scan=ui->m_leScanBarcode->text();
    In2ManagerERP *erp=In2ManagerERP::request(0);
    QString req=erp->getValueFromERP(in_COD_BULTO_PADRE);
    if (!req.compare(scan)){
        QString txt=QString("Generando Imagen...por favor espere");
        ui->m_pbExit->setEnabled(false);
        ui->m_pbLoad->setEnabled(false);
        ui->m_pbLoadScan->setEnabled(false);
        ui->m_pbLog->setEnabled(false);
        QApplication::setOverrideCursor(Qt::BusyCursor);
        QCoreApplication::processEvents();
        sltInfoMessage(txt);
        setCheckERP(false);
        deleteTempGSFile();
        memoryMap->JumpToTempWorld();
        In2ImageCreator p;
        p.Convert();
        p.sltGenerateImage();
        m_workLoaded=true;
        //if (!erp->orderPending())
        if (!m_pedingOrder)
            erp->sltWriteAnswerToBBDD(SGL_STATE_FIN,"");
        txt=tr("Trabajo %1 cargado").arg(memoryMap->m_mapa.m_variables.NombreRutaIni);
        emit SendDebugInfo(LOGGER_STORE,txt) ;
        QApplication::restoreOverrideCursor();
        ui->m_pbExit->setEnabled(true);
        ui->m_pbLoad->setEnabled(true);
        ui->m_pbLoadScan->setEnabled(true);
        ui->m_pbLog->setEnabled(true);
        QTimer::singleShot(0,this,SLOT(sltShowImage()));
        //QTimer::singleShot(0,this,SLOT(sltExit()));
        //Cargar trabajo
    }
    else{
      QString txt=tr("Error Scan ,leido %1, esperado %2").arg(scan).arg(req);
      sltInfoMessage(txt);
      emit SendDebugInfo(LOGGER_ERROR,txt);
    }
    m_pendingScan=false;
}

/** Muestra los datos del ERP */
void In2ColaImpresionMono::sltShowERPData(){
    /*Qt::ItemFlags flags=0;
    In2GestorERP *gestor=In2GestorERP::request(0);

    QString txt=gestor->getConnection();
    QTableWidgetItem *p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(0,0,p);

    txt=gestor->getUser();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(1,0,p);

    txt=gestor->getPassword();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(2,0,p);

    txt=gestor->getCliente();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(3,0,p);

    txt=QString("%1").arg(gestor->getEnabled());
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(4,0,p);

    txt=gestor->getDriver();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(5,0,p);

    txt=gestor->getODBCEntry();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(6,0,p);

    txt=gestor->getBBDD();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(7,0,p);

    txt=gestor->getTableIn();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(8,0,p);

    txt=gestor->getTableOut();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(9,0,p);

    txt=gestor->getReadBBDD();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(10,0,p);

    txt=gestor->getFieldsIn();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(11,0,p);

    txt=gestor->getFieldsOut();
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(12,0,p);

    txt=QString("%1").arg(gestor->getMultipleItems());
    p=new QTableWidgetItem(txt);
    p->setFlags(flags);
    ui->m_twERP->setItem(13,0,p);*/


}


/** Testea el ERP*/
void In2ColaImpresionMono::sltCheckERP(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    QStringList data;
    /*In2GestorERP *p=In2GestorERP::request(0);
    data=p->checkERP();*/
    In2ManagerERP *p=In2ManagerERP::request(0);
    data=p->checkERP();
    sltDataFromERP(data);

    QApplication::restoreOverrideCursor();

    if (m_bCheckERP) QTimer::singleShot(UPDATE_TIME,this,SLOT(sltCheckERP()));



}

/** Testea el ERP de salida*/
void In2ColaImpresionMono::sltCheckERPOut(){
    QStringList data;
    In2ManagerERP *p=In2ManagerERP::request(0);
    data=p->execSQL(p->getItem(ERP_WRITE_BBDD),false);
    sltDataFromERPOut(data);
}

/** Vienen datos del ERP (tabla de salida)*/
void In2ColaImpresionMono::sltDataFromERPOut(const QStringList & p){
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    QStringList data=p;
    if (data.count()){
        ui->m_twDataOut->clearContents();
        QStringList strlstHeaders=gestor->getItem(ERP_FIELDS_BBDD_OUT).split("@");
        int nHeaders=strlstHeaders.count();
        if (nHeaders<=p.count()){
            ui->m_twDataOut->setRowCount(nHeaders);
            ui->m_twDataOut->setVerticalHeaderLabels(strlstHeaders);
            for (int x=0;x<nHeaders;x++){
                QTableWidgetItem *item=new QTableWidgetItem(p.at(x));
                Qt::ItemFlags flags=0;
                item->setFlags(flags);
                ui->m_twDataOut->setItem(x,0,item);
            }
        }
        else{
            emit SendDebugInfo(LOGGER_ERROR,tr("sltDataFromERPOut expected %1 items got %2").arg(nHeaders).arg(p.count()));
            emit

        }
    }
}

/** Vienen datos del orden pendiente (backup)*/
void In2ColaImpresionMono::sltDataFromPendingOrder(const QStringList & p){
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    gestor->loadPendingOrder();
}




/** Vienen datos del ERP*/
void In2ColaImpresionMono::sltDataFromERP(const QStringList & p){
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    QStringList data=p;
    if (data.count()){
        ui->m_twData->clearContents();
        QStringList strlstHeaders=gestor->getItem(ERP_FIELDS_BBDD_IN).split("@");
        int nHeaders=strlstHeaders.count();
        if (nHeaders==p.count()){
            ui->m_twData->setRowCount(nHeaders);
            ui->m_twData->setVerticalHeaderLabels(strlstHeaders);
            int iFieldOT=gestor->getItemIndex(ERP_FIELD_OT);
            for (int x=0;x<nHeaders;x++){
                QTableWidgetItem *item=new QTableWidgetItem(p.at(x));
                Qt::ItemFlags flags=0;
                item->setFlags(flags);
                ui->m_twData->setItem(x,0,item);
                /*if (x==iFieldOT){
                    if (!m_actualOT.compare(item->text())){
                        QTimer::singleShot(0,this,SLOT(sltShowImage()));
                    }
                }*/
            }
        }        
        //this->setEnabled(true);

    }
    else{
        if (ui->m_twData->rowCount()){
            ui->m_twData->clearContents();
            ui->m_twData->setRowCount(0);
        }
    }
}

/** Previsualiza la imagen si la hay*/
void In2ColaImpresionMono::sltShowImage(){
    QString foo=tr("Previsualizacion");
    sltInfoMessage(foo);
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    MemoryMap *memoryMap=MemoryMap::request(0);
    int iFieldCompo=gestor->getItemIndex(ERP_FIELD_COMPO);
    int iFieldOT=gestor->getItemIndex(ERP_FIELD_OT);
    if ((iFieldCompo!=-1)&&(iFieldOT!=-1)){
        QTableWidgetItem *pCompo=ui->m_twData->item(iFieldCompo,0);
        QTableWidgetItem *pOT=ui->m_twData->item(iFieldOT,0);
        if (((pCompo)&&(pOT))||(gestor->orderPending())){
            //QString file=pCompo->text().replace(QString(".ini"),QString("_OF%1_C1_comp.bmp").arg(pOT->text()));
            QString file=memoryMap->m_FilePath;
            QPixmap pixmap=QPixmap(file);
            if ((!pixmap.isNull())&&(m_scene)){
                QList<QGraphicsItem*> items=m_scene->items();
                if (items.count()){
                    In2GraphicsPixmapItem *item=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                    item->setPixmap(pixmap);
                    m_scene->setSceneRect(0,0,pixmap.width(),pixmap.height());
                    ui->m_gv->fitInView(item,Qt::KeepAspectRatio);
                }
            }
            else{
                foo=tr("No se puede generar imagen %1").arg(file);
                sltInfoMessage(foo);
            }
        }
    }
}

/** Carga el trabajo*/
void In2ColaImpresionMono::sltLoad(){
    In2ManagerERP *erp=In2ManagerERP::request(0);
    if (erp->orderPending()){
        ui->m_pbLoad->setEnabled(false);
        QTimer::singleShot(1000,this,SLOT(_loadPendingOrder()));
        return;
    }
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    if (1){
        QString txt=QString("Cargando...por favor espere");
        QApplication::setOverrideCursor(Qt::BusyCursor);
        sltInfoMessage(txt);
        QApplication::processEvents();
        gestor->processDataIn();
        QApplication::restoreOverrideCursor();
        if (!gestor->getValueFromERP(in_ESTADO).compare(SGL_STATE_PND)){
/*            if (!gestor->getValueFromERP(in_COD_BULTO_PADRE).isNull()){
                ui->m_pbLoadScan->setEnabled(true);
                ui->m_leScanBarcode->setEnabled(true);
                ui->m_leScanBarcode->setStyleSheet("QLineEdit { background: rgb(0, 125, 125); }");
                ui->m_leScanBarcode->setFocus();
            }
            else{
                ui->m_pbLoadScan->setEnabled(false);
                ui->m_leScanBarcode->setEnabled(false);
                ui->m_leScanBarcode->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); }");

            }*/
        }
        else{
            ui->m_pbLoadScan->setEnabled(false);
            ui->m_leScanBarcode->setEnabled(false);
            ui->m_leScanBarcode->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); }");

        }
        QTimer::singleShot(0,this,SLOT(sltCheckERP()));
        //QTimer::singleShot(0,this,SLOT(sltShowImage()));
    }
    else {
        QString txt=tr("No se puede cargar el trabajo");
        //ui->m_pbLoad->setEnabled(false);
        sltInfoMessage(txt);
        emit SendDebugInfo(LOGGER_WARNING,txt);
    }


}


/** Sale de la pantalla*/
void In2ColaImpresionMono::sltExit(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    In2ManagerERP *erp=In2ManagerERP::request(0);
    //Quitamos de memoria
    if (!m_workLoaded){
        memoryMap->InitComposicion();
    }
    else{
        QString txt=tr("Trabajo cargado %1, OT %2").arg(memoryMap->m_mapa.m_variables.NombreRutaIni)
                .arg(memoryMap->m_mapa.m_variables.OrdenFabricacion);
        emit SendDebugInfo(LOGGER_STORE,txt);
        QString originalIni=QString("%1\\%2.ini").arg(erp->getValueFromERP(in_PATH_FICHERO_INI)).arg(erp->getValueFromERP(in_COD_PRODUCTO));
        In2Store p;
        p.createTable(memoryMap->m_mapa.m_variables.Proceso);
        //Meto la orden Vs el INI
        QString foo=QString("insert into OrdenVsIni (Orden,Ini,OT) Values ('%1','%2','%3')").arg(erp->getValueFromERP(in_COD_ORDEN)).arg(originalIni).arg(memoryMap->m_mapa.m_variables.OrdenFabricacion);
        p.execSql(foo);
    }
    if (m_parent) m_parent->show();
    this->deleteLater();

}
/** Publica un mensaje*/
void In2ColaImpresionMono::sltInfoMessage(QString & txt){
    QString date=QDateTime::currentDateTime().toString("hh:mm:ss");
    QString foo=QString("[%1]-%2").arg(date).arg(txt);
    ui->m_teMessage->append(foo);
}
/** Limpia los mensajes*/
void In2ColaImpresionMono::sltClearInfoMessage(){
    ui->m_teMessage->clear();
}


/** recibe el workloaded y sale si es necesario*/
void In2ColaImpresionMono::sltWorkLoaded(int mode){
    QString txt;
    MemoryMap *memoryMap=MemoryMap::request(0);
    In2ManagerERP *manager=In2ManagerERP::request(0);
    if (mode==SGL_WORKLOADED_NOK){
        m_workLoaded=false;        
        txt=manager->getValueFromERP(in_TEXTO_ERROR);
        if (txt.isEmpty())
            txt=tr("Carga de trabajo %1 NOK").arg(memoryMap->m_mapa.m_variables.NombreRutaIni);
    }
    else if (mode==SGL_WORKLOADED_OK){        
        QString txt=QString("Generando Imagen...por favor espere");
        ui->m_pbExit->setEnabled(false);
        ui->m_pbLoad->setEnabled(false);
        ui->m_pbLoadScan->setEnabled(false);
        ui->m_pbLog->setEnabled(false);
        QApplication::setOverrideCursor(Qt::BusyCursor);
        QCoreApplication::processEvents();
        sltInfoMessage(txt);
        setCheckERP(false);
        deleteTempGSFile();
        memoryMap->JumpToTempWorld();
        In2ImageCreator p;
        p.Convert();
        p.sltGenerateImage();
        m_workLoaded=true;
        txt=tr("Trabajo %1 cargado").arg(memoryMap->m_mapa.m_variables.NombreRutaIni);
        QApplication::restoreOverrideCursor();
        ui->m_pbExit->setEnabled(true);
        ui->m_pbLoad->setEnabled(true);
        ui->m_pbLoadScan->setEnabled(true);
        ui->m_pbLog->setEnabled(true);

        //QTimer::singleShot(0,this,SLOT(sltExit()));
    }
    //Cargamos si o si
    else if (mode==SGL_WORKLOADED_SCAN){
        QString req=manager->getValueFromERP(in_COD_BULTO_PADRE);
        ui->m_leScanBarcode->setText(req);
        QTimer::singleShot(0,this,SLOT(_sltCheckAndLoad()));
        /*if (manager->orderPending()) ui->m_pbLoadScan->setEnabled(false);
        else ui->m_pbLoadScan->setEnabled(true);
        ui->m_leScanBarcode->setEnabled(true);
        ui->m_leScanBarcode->setStyleSheet("QLineEdit { background: rgb(0, 125, 125); }");
        ui->m_leScanBarcode->setFocus();
        txt=tr("Se necesita lectura de CB");*/
    }
    else if (mode==SGL_WORKLOADED_DOC){
        txt=tr("Generando documentacion...");
    }
    else if (mode==SGL_WORKLOADED_NODOC){
        txt=tr("Error al volcar a la BBDD");
    }
    emit SendDebugInfo(LOGGER_WARNING,txt);
    sltInfoMessage(txt);
}
