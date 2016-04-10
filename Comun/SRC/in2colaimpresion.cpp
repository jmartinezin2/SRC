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
#include "in2colaimpresion.h"
#include "in2gestorerp.h"
#include "in2managererp.h"
#include "MemoryMap.h"
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include "in2compofile.h"
#include <QFileInfo>
#include "inkjetdebug.h"
#include <QMessageBox>
#include "in2store.h"


In2ColaImpresion::In2ColaImpresion(QWidget *parent)
    : ui(new Ui::In2ColaImpresionUI)
{
    ui->setupUi(this);
    m_parent=parent;
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
    connect(ui->m_pbSelect,SIGNAL(clicked()),this,SLOT(sltLoadWork()));
    connect(ui->m_twData,SIGNAL(cellPressed(int,int)),this,SLOT(sltSelectWork(int, int)));
    connect(ui->m_leScan,SIGNAL(textChanged(QString)),this,SLOT(sltSelectText(QString)));
    connect(ui->m_pbDelete,SIGNAL(clicked()),this,SLOT(sltDeleteItem()));

    QPalette palette = ui->m_twData->palette();
    palette.setColor(QPalette::Highlight,Qt::red);
    ui->m_twData->setPalette(palette);

    /*InkjetDebug *dbg=InkjetDebug::request(0);
    connect(this,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));*/

    QTimer::singleShot(0,this,SLOT(sltCheckERP()));

    //this->show();

}
/** Selecciona un trabajo*/
void In2ColaImpresion::sltSelectWork(int x, int y){
    ui->m_twData->selectRow(x);
    ui->m_teHistorico->clear();
    ui->m_teHistorico->append("\n");
    ui->m_teHistorico->append("<font color=\"black\"><b>HISTORICO:</font></b>");
    In2ConfiguratorERP config;
    config.readConfig();
    int index=config.getOTFieldIndex();

    QTableWidgetItem *p=ui->m_twData->item(x,index);
    if (p){
        In2Store db;
        QString foo=p->text();
        QStringList lst=db.getAllDataFromCode(foo,QString("%1").arg(LOGGER_PARADA));
        for (int x=0;x<lst.count();x++){
            QString _foo=lst.at(x);
            QStringList lista=_foo.split("<@>");
            for (int y=0;y<lista.count();y++){
                QString foo=QString("<font color=\"green\">%1</font>").arg(lista.at(y));
                ui->m_teHistorico->append(foo);
            }
            ui->m_teHistorico->append("\n\n");
        }
    }

    sltShowImage();
}

/** Elimina un trabajo*/
void In2ColaImpresion::sltDeleteItem(){
    int ret=QMessageBox::warning(this,"ERROR",tr("Desea eliminar el trabajo seleccionado?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (ret==QMessageBox::Ok){
        In2ManagerERP *erp=In2ManagerERP::request(0);
        int foo=erp->getItemIndex(ERP_FIELD_OT);
        int row=ui->m_twData->currentRow();
        QString strOrden=ui->m_twData->item(row,foo)->text();
        foo=erp->getItemIndex(ERP_FIELD_COMPO);
        QString strFile=ui->m_twData->item(row,foo)->text();
        QString strHeaderFile=erp->getItem(ERP_FIELD_COMPO);
        QString strHeaderOT=erp->getItem(ERP_FIELD_OT);

        QString fooStr=QString("delete from ColaImpresion where %1='%2' and %3='%4'")
                .arg(strHeaderFile)
                .arg(strFile)
                .arg(strHeaderOT)
                .arg(strOrden);
        erp->execSQL(fooStr,false);

        QTimer::singleShot(0,this,SLOT(sltCheckERP()));

        sltSelectWork(0,0);

}

}

/** Carga el trabajo seleccionado*/
void In2ColaImpresion::sltLoadWork(){
    int iFila=ui->m_twData->currentRow();
    if (ui->m_twData->rowCount()==0) return;
    if (iFila<0) return;
    int ret = QMessageBox::warning(this, tr("INFO"),tr("Desea cargar el trabajo seleccionado?"),
    QMessageBox::Ok | QMessageBox::Cancel);
    if (ret==QMessageBox::Ok){
        In2ManagerERP *gestor=In2ManagerERP::request(0);
        MemoryMap *memoryMap=MemoryMap::request(0);
        if (!gestor->getClient().compare(ERP_IN2)){
            int iFieldCompo=gestor->getItemIndex(ERP_FIELD_COMPO);
            QTableWidgetItem *pCompo=ui->m_twData->item(iFila,iFieldCompo);
            QTableWidgetItem *pPath=ui->m_twData->item(iFila,iFieldCompo+1);
            if (pCompo&&pPath){
                QString nombre=pCompo->text();
                QString ruta=pPath->text();
                QString nombreruta=QString("%1\\%2").arg(ruta).arg(nombre);
                int iFieldOrden=gestor->getItemIndex(ERP_FIELD_OT);
                memoryMap->m_mapa.m_variables.OrdenFabricacion=ui->m_twData->item(ui->m_twData->currentRow(),iFieldOrden)->text();
                memoryMap->m_mapa.m_variables.Proceso=memoryMap->m_mapa.m_variables.OrdenFabricacion;
                if (QFile::exists(nombreruta)){
                    memoryMap->m_FilePath=nombreruta;

                    QString foo;
                    int indice=nombreruta.lastIndexOf("_OF");
                    if (indice==-1){
                        //No hay orden de fabricación en el nombre
                        foo=nombreruta;
                        foo.replace("_C1_comp.bmp",".ini");
                    }
                    else{
                        foo=nombreruta.left(indice);
                        foo.append(".ini");
                    }
                    memoryMap->m_mapa.m_variables.NombreRutaIni=foo;
                    if(QFile::exists(memoryMap->m_mapa.m_variables.NombreRutaIni)){
                        In2CompoFile ficheroIni;
                        ficheroIni.AbrirFicheroIni();
                        bool rtn=true;
                        for(int i=1;i<memoryMap->m_mapa.m_variables.NumColores;i++){
                            QString nombreruta_CX=nombreruta;
                            nombreruta_CX.replace("_C1_comp.bmp",QString("_C%1_comp.bmp").arg(i+1));
                            if (!QFile::exists(nombreruta_CX)) rtn=false;
                        }

                        if(rtn==false){
                            QMessageBox::warning(0,tr("ERROR"),tr("No existe algún fichero de la composición"));

                        }
                        else{
                            memoryMap->m_UltimaAccion=UA_TRABAJO_SELECCIONADO;
                            QFileInfo info(memoryMap->m_mapa.m_variables.NombreRutaIni);
                            memoryMap->m_mapa.m_variables.NombreIni=info.fileName();
                            memoryMap->m_mapa.m_variables.RutaIni=info.path();
                            memoryMap->m_mapa.m_variables.NombreRutaBmp=memoryMap->m_mapa.m_variables.NombreRutaIni;
                            memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");
                            In2Store p;
                            p.createTable(memoryMap->m_mapa.m_variables.Proceso);
                        //Meto la orden Vs el INI
                            QString foo=QString("insert into OrdenVsIni (Orden,Ini,OT) Values ('%1','%2','%3')").arg(memoryMap->m_mapa.m_variables.Proceso).arg(memoryMap->m_mapa.m_variables.NombreRutaIni).arg(memoryMap->m_mapa.m_variables.OrdenFabricacion);
                            p.execSql(foo);

                            emit sgnWorkLoaded();
                            sltExit();
                        }
                    }
                    else{
                        QMessageBox::warning(0,"ERROR",tr("No existe el fichero\n%1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni),QMessageBox::Ok);


                    }
                }
                else{
                    QMessageBox::warning(0,"ERROR",tr("No existe el fichero\n%1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni),QMessageBox::Ok);

                }
            }
        }
    }
}
/** Previsualiza la imagen si la hay*/
void In2ColaImpresion::sltShowImage(){
    int iFila=ui->m_twData->currentRow();
    if (iFila!=-1){
        In2ManagerERP *gestor=In2ManagerERP::request(0);
        if (!gestor->getClient().compare(ERP_IN2)){
            int iFieldCompo=gestor->getItemIndex(ERP_FIELD_COMPO);
            QTableWidgetItem *pCompo=ui->m_twData->item(iFila,iFieldCompo);
            QTableWidgetItem *pPath=ui->m_twData->item(iFila,iFieldCompo+1);
            if ((pCompo)&&(pPath)){
                QString foo=QString("%1/%2").arg(pPath->text()).arg(pCompo->text());
                if ((foo.length())&&(foo.at(0)=='.')){
                    QString tmp=foo.remove(0,1);
                    foo=QString("%1/%2").arg(QApplication::applicationDirPath()).arg(tmp);
                }

                QPixmap pixmap=QPixmap(foo);
                if ((m_scene)){
                    QList<QGraphicsItem*> items=m_scene->items();
                        if (items.count()){
                        In2GraphicsPixmapItem *item=static_cast<In2GraphicsPixmapItem*>(items.at(0));
                        item->setPixmap(pixmap);
                        m_scene->setSceneRect(0,0,pixmap.width(),pixmap.height());
                        ui->m_gv->fitInView(item,Qt::KeepAspectRatio);
                    }
                }
            }
        }
    }
}

/** Swelecciona un texto de manera automatica*/
void In2ColaImpresion::sltSelectText(QString txt){
    static int prev=-1;
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    if (!gestor->getClient().compare(ERP_IN2)){
        int iFieldCompo=gestor->getItemIndex(ERP_FIELD_COMPO);
        bool  rtn=false;
        int x=0;
        for (x=0;x<ui->m_twData->rowCount();x++){
            QTableWidgetItem *p=ui->m_twData->item(x,iFieldCompo);
            if (p){
                if (p->text().contains(txt,Qt::CaseSensitive)){
                    rtn=true;
                    break;
                }
            }
        }
        if (rtn){
            ui->m_twData->selectRow(x);
            if (x!=prev){
                prev=x;
                sltShowImage();
            }
        }
    }

}


In2ColaImpresion::~In2ColaImpresion(){


}

void In2ColaImpresion::sltExit(){

    this->deleteLater();
}

/** Testea el ERP*/
void In2ColaImpresion::sltCheckERP(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    QStringList data;
    /*In2GestorERP *p=In2GestorERP::request(0);
    data=p->checkERP();*/
    In2ManagerERP *p=In2ManagerERP::request(0);
    data=p->checkERP();
    sltDataFromERP(data);
    QApplication::restoreOverrideCursor();
}

/** Vienen datos del ERP*/
void In2ColaImpresion::sltDataFromERP(const QStringList & p){
    In2ManagerERP *gestor=In2ManagerERP::request(0);
    QStringList data=p;
    if (data.count()){
        ui->m_twData->clearContents();
        QStringList strlstHeaders=gestor->getItem(ERP_FIELDS_BBDD_IN).split("@");
        int nHeaders=strlstHeaders.count();
        //if (nHeaders==p.count()){
            int rowCount=data.count()/nHeaders;
            ui->m_twData->setRowCount(rowCount);
            ui->m_twData->setColumnCount(nHeaders);
            for (int x=0;x<nHeaders;x++){
                ui->m_twData->setColumnWidth(x,ui->m_twData->width()/nHeaders);
            }
            ui->m_twData->setHorizontalHeaderLabels(strlstHeaders);

#if QT_VERSION>=0x050000
     ui->m_twData->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
     //->m_InfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->m_twData->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
    //->m_InfoTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

#endif
            for (int y=0;y<rowCount;y++){
                for (int x=0;x<nHeaders;x++){
                    QTableWidgetItem *item=new QTableWidgetItem(p.at(y*nHeaders+x));
                    Qt::ItemFlags flags=item->flags();
                    flags&=~Qt::ItemIsEditable;
                    item->setFlags(flags);
                    ui->m_twData->setItem(y,x,item);
                }
            }
        //QTimer::singleShot(0,this,SLOT(sltShowImage()));
        this->setEnabled(true);
    }
}
