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
#include "in2gramarcolaimpresion.h"
#include "in2gramarerp.h"
#include "in2configuratorerp.h"
#include "in2ManagerErp.h"
#include <QTimer>
#include <QTime>
#include <QFileInfo>
#include <QMessageBox>
#include "in2store.h"
#include "MemoryMap.h"
#include "in2imagecreator.h"
#include "in2compofile.h"
#include "in2previsualizator.h"
#include <QElapsedTimer>

In2GramarColaImpresion::In2GramarColaImpresion(QWidget *parent)
    : ui(new Ui::In2GramarColaImpresion)
{
    ui->setupUi(this);
    connect(ui->m_pbExit,SIGNAL(clicked()),this,SLOT(sltExit()));
    In2ConfiguratorERP config;
    config.readConfig();
    QString heads=config.getFieldsIn();
    m_lstHeads=heads.split("@");
    ui->m_twData->setColumnCount(m_lstHeads.count());
    ui->m_twData->setHorizontalHeaderLabels(m_lstHeads);
    ui->m_twGramar->setColumnCount(m_lstHeads.count());
    ui->m_twGramar->setHorizontalHeaderLabels(m_lstHeads);

    QPalette palette = ui->m_twData->palette();
    palette.setColor(QPalette::Highlight,Qt::red);
    ui->m_twData->setPalette(palette);
    ui->m_twGramar->setPalette(palette);

    connect(ui->m_twData,SIGNAL(cellPressed(int,int)),this,SLOT(sltSelectWork(int, int)));
    connect(ui->m_twGramar,SIGNAL(cellPressed(int,int)),this,SLOT(sltSelectGramarWork(int, int)));
    connect(ui->m_pbDown,SIGNAL(clicked()),this,SLOT(sltInsertWork()));
    connect(ui->m_pbDelete,SIGNAL(clicked()),this,SLOT(sltDeleteItem()));
    connect(ui->m_pbLoad,SIGNAL(clicked()),this,SLOT(sltLoadWork()));
    connect(ui->m_pbPrevGrammar,SIGNAL(clicked()),this,SLOT(sltPrevFromGramar()));
    connect(ui->m_pbPrevIn2,SIGNAL(clicked()),this,SLOT(sltPrevFromIn2()));
    connect(ui->m_pbReloadGramar,SIGNAL(clicked()),this,SLOT(sltCheckERP()));
    connect(ui->m_pbReloadIn2,SIGNAL(clicked()),this,SLOT(sltCheckIn2ERP()));
    In2ERP *erp=In2ERP::request(0);
    connect(this,SIGNAL(sgnERPNewMetrosRef(int)),erp,SIGNAL(sgnERPNewMetrosRef(int)));
    In2ManagerERP *manager=In2ManagerERP::request(0);
    connect(manager,SIGNAL(SendDebugInfo(unsigned char,QString)),this,SLOT(sltShowMessage(unsigned char,QString)));
    iprevGrammarItem=-1;
    iprevIn2Item=-1;

    //QTimer::singleShot(0,this,SLOT(sltCheckERP()));
    sltCheckERP();
    QTimer::singleShot(0,this,SLOT(sltCheckIn2ERP()));

}
/** Inserta un trabajo en la cola de impresion*/
void In2GramarColaImpresion::sltInsertWork(){
    if (m_lstHeads.count()!=9) return;
    QStringList lst;
    int row=ui->m_twGramar->currentRow();
    int cols=m_lstHeads.count();
    if (row<0) return;
    for (int x=0;x<cols;x++){
        QTableWidgetItem *p=ui->m_twGramar->item(row,x);
        QString txt;
        if (p) txt=p->text();
        lst.append(txt);
    }
    QString foo=QString("insert into ColaImpresion ('%1','%2','%3','%4','%5','%6','%7','%8','%9') Values ('%10','%11','%12','%13','%14','%15','%16','%17','%18')")
            .arg(m_lstHeads.at(0))
            .arg(m_lstHeads.at(1))
            .arg(m_lstHeads.at(2))
            .arg(m_lstHeads.at(3))
            .arg(m_lstHeads.at(4))
            .arg(m_lstHeads.at(5))
            .arg(m_lstHeads.at(6))
            .arg(m_lstHeads.at(7))
            .arg(m_lstHeads.at(8))
            .arg(lst.at(0))
            .arg(lst.at(1))
            .arg(lst.at(2))
            .arg(lst.at(3))
            .arg(lst.at(4))
            .arg(lst.at(5))
            .arg(lst.at(6))
            .arg(lst.at(7))
            .arg(lst.at(8));


    In2ManagerERP *erp=In2ManagerERP::request(0);
    erp->execSQL(foo,true);
    sltShowMessage(LOGGER_STORE,foo);
    QTimer::singleShot(0,this,SLOT(sltCheckIn2ERP()));

}
/** Elimina un item de la cola de impresion*/
void In2GramarColaImpresion::sltDeleteItem(){
    int rtn=QMessageBox::question(this,"INFO",tr("Desea eliminar el trabajo seleccionado?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Cancel) return;

    int row=ui->m_twData->currentRow();
    QTableWidgetItem *p=ui->m_twData->item(row,0);
    if (p){
        QString foo=p->text();
        QString sql=QString("delete from ColaImpresion where %1='%2'").arg(m_lstHeads.at(0)).arg(foo);
        In2ManagerERP *erp=In2ManagerERP::request(0);
        erp->execSQL(sql,true);
        sltShowMessage(LOGGER_STORE,sql);
        QTimer::singleShot(0,this,SLOT(sltCheckIn2ERP()));

    }


}


/** Selecciona un trabajo*/
void In2GramarColaImpresion::sltSelectWork(int x, int y){
    if (x==iprevIn2Item) return;
    iprevIn2Item=x;
    ui->m_twData->selectRow(x);
    ui->m_teIn2Resume->clear();
    //Mostramos un resumen
    int count=m_lstHeads.count();
    for (int col=0;col<count;col++){
        QTableWidgetItem *p=ui->m_twData->item(x,col);
        if (p){
            QString foo=QString("<font color=\"black\">%1:</font><font color=\"blue\">%2</font>").arg(m_lstHeads.at(col)).arg(p->text());
            ui->m_teIn2Resume->append(foo);
        }
    }

    ui->m_teIn2Resume->append("\n");
    ui->m_teIn2Resume->append("<font color=\"black\"><b>HISTORICO:</font></b>");
    In2ConfiguratorERP config;
    config.readConfig();
    int index=config.getCompoFieldIndex();

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
                ui->m_teIn2Resume->append(foo);
            }
            ui->m_teIn2Resume->append("\n\n");
        }
    }



//    sltShowImage();
}
/** Selecciona un trabajo*/
void In2GramarColaImpresion::sltSelectGramarWork(int x, int y){
    /*int curr=ui->m_twGramar->currentRow();
    if (curr==x) return;*/
    if (x==iprevGrammarItem) return;
    iprevIn2Item=x;

    ui->m_twGramar->selectRow(x);
    ui->m_teGramarResume->clear();
    //Mostramos un resumen
    int count=m_lstHeads.count();
    for (int col=0;col<count;col++){
        QTableWidgetItem *p=ui->m_twGramar->item(x,col);
        if (p){
            QString foo=QString("<font color=\"black\">%1:</font><font color=\"blue\">%2</font>").arg(m_lstHeads.at(col)).arg(p->text());
            ui->m_teGramarResume->append(foo);
        }
    }

    ui->m_teGramarResume->append("\n");
    ui->m_teGramarResume->append("<font color=\"black\"><b>HISTORICO:</font></b>");
    In2ConfiguratorERP config;
    config.readConfig();
    int index=config.getCompoFieldIndex();

    QTableWidgetItem *p=ui->m_twGramar->item(x,index);
    if (p){
        In2Store db;
        QString foo=p->text();
        QStringList lst=db.getAllDataFromCode(foo,QString("%1").arg(LOGGER_PARADA));
        for (int x=0;x<lst.count();x++){
            QString _foo=lst.at(x);
            QStringList lista=_foo.split("<@>");
            for (int y=0;y<lista.count();y++){
                QString foo=QString("<font color=\"green\">%1</font>").arg(lista.at(y));
                ui->m_teGramarResume->append(foo);
            }
            ui->m_teGramarResume->append("\n\n");
        }
    }

//    sltShowImage();
}


/** Chequea el ERP*/
void In2GramarColaImpresion::sltCheckERP(){
    QApplication::setOverrideCursor(Qt::BusyCursor);
    In2ConfiguratorERP config;
    config.readConfig();
    QString foo=config.getInterFile();
    this->setEnabled(false);
    sltShowMessage(LOGGER_WARNING,tr("Leyendo %1").arg(foo));
    QCoreApplication::processEvents();
    if (!QFile::exists(foo)){
        sltShowMessage(LOGGER_ERROR,tr("NO EXISTE FICHERO %1").arg(foo));
        QApplication::restoreOverrideCursor();
        this->setEnabled(true);
        return;
    }
    In2GramarErp erp;
    ui->m_twGramar->clearContents();
    QStringList data=erp.checkERP();
    if (data.count()){
        int total=data.count();
        ui->m_twGramar->setRowCount(total);
        for (int x=0;x<total;x++){
            QStringList columns=data.at(x).split('\t');
            int totalCols=columns.count();
            for (int y=0;y<totalCols;y++){
                QFont font;
                font.setPointSize(8);
                QTableWidgetItem *p=new QTableWidgetItem(columns.at(y));
                Qt::ItemFlags flags=p->flags();
                flags&=~Qt::ItemIsEditable;
                p->setFont(font);
                p->setFlags(flags);
                ui->m_twGramar->setItem(x,y,p);
            }
        }
    }
    ui->m_twGramar->resizeColumnsToContents();
    ui->m_twGramar->resizeRowsToContents();
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);

    //QTimer::singleShot(0,this,SLOT(sltCheckIn2ERP()));

}
/** Chequea el ERP-In2*/
void In2GramarColaImpresion::sltCheckIn2ERP(){
    QApplication::setOverrideCursor(Qt::BusyCursor);
    In2ConfiguratorERP config;
    config.readConfig();
    QString foo=config.getBBDD();
    this->setEnabled(false);
    sltShowMessage(LOGGER_WARNING,tr("Leyendo %1").arg(foo));
    QCoreApplication::processEvents();
    if (!QFile::exists(foo)){
        sltShowMessage(LOGGER_ERROR,tr("NO EXISTE BBDD %1").arg(foo));
        QApplication::restoreOverrideCursor();
        this->setEnabled(true);
        return;
    }
    In2ManagerERP *erp=In2ManagerERP::request(0);
    ui->m_twData->clearContents();
    QStringList data=erp->checkERP();
    if (data.count()){
        int totalCols=ui->m_twData->columnCount();
        int totalRows=data.count()/totalCols;
        MemoryMap *memoryMap=MemoryMap::request(0);
        //Para los impresores solo muestro el ultimo!
        if (memoryMap->getUserRights()!=ACCESS_GOD){
            totalRows=1;
        }
        ui->m_twData->setRowCount(totalRows);
        for (int x=0;x<totalRows;x++){
            for (int y=0;y<totalCols;y++){
                QFont font;
                font.setPointSize(8);
                QTableWidgetItem *p=new QTableWidgetItem(data.at(x*totalCols+y));
                Qt::ItemFlags flags=p->flags();
                flags&=~Qt::ItemIsEditable;
                p->setFlags(flags);
                p->setFont(font);
                ui->m_twData->setItem(x,y,p);

            }
        }
    }
    ui->m_twData->resizeColumnsToContents();
    ui->m_twData->resizeRowsToContents();

    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
}

/** Salimos*/
void In2GramarColaImpresion::sltExit(){
    ui->m_twData->clear();
    ui->m_twGramar->clear();
    this->deleteLater();
}
/** Vuelca la info*/
void In2GramarColaImpresion::sltShowMessage(unsigned char value,QString txt){
    QString time=QTime::currentTime().toString("hh:mm:ss");
    ui->m_teLog->append(QString("[%1] %2").arg(time).arg(txt));
    emit SendDebugInfo(value,txt);
}

/** Carga el trabajo*/
void In2GramarColaImpresion::sltLoadWork(){
    if (ui->m_twData->rowCount()==0){
            QMessageBox::question(this,"INFO",tr("No hay trabajos pendientes"),QMessageBox::Ok);
            return;
    }
    //Siempre siempre seleccionamos el primero de la lista
    ui->m_twData->selectRow(0);
    sltSelectWork(0,0);
    int rtn=QMessageBox::question(this,"INFO",tr("Desea cargar el trabajo seleccionado?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Cancel) return;
    m_load=true;
    if (sltGenerateFromIn2()){
        int row=ui->m_twData->currentRow();
        ui->m_teCurrentWork->clear();
        //Mostramos un resumen
        int count=m_lstHeads.count();
        for (int col=0;col<count;col++){
            QTableWidgetItem *p=ui->m_twData->item(row,col);
            if (p){
                QString foo=QString("<font color=\"black\">%1:</font><font color=\"blue\">%2</font>").arg(m_lstHeads.at(col)).arg(p->text());
                ui->m_teCurrentWork->append(foo);
            }
        }
        //Enviamos los metros a hacer
        In2ConfiguratorERP config;
        config.readConfig();
        int index=config.getMetrosFieldIndex();
        QTableWidgetItem *p=ui->m_twData->item(row,index);
        if (p){
            int metros=(int)p->text().toFloat();
            emit sgnERPNewMetrosRef(metros);
        }
        emit sgnERPTrabajoNuevo();
        QTimer::singleShot(0,this,SLOT(sltExit()));
    }

}

/** Genera un trabajo de las lista In2*/
bool In2GramarColaImpresion::sltGenerateFromIn2(){
    QElapsedTimer fooTimer;
    fooTimer.start();

    bool rtn=false;
    this->setEnabled(false);
    MemoryMap *memoryMap=MemoryMap::request(0);
    In2ConfiguratorERP config;
    config.readConfig();
    int index=config.getCompoFieldIndex();
    int row=ui->m_twData->currentRow();
    QTableWidgetItem *p=ui->m_twData->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return false;
    }

    QString iniFile=QString("%1\\%2.ini").arg(memoryMap->m_mapa.m_variables.defaultImagePath).arg(p->text());
    if (!QFile::exists(iniFile)){
        sltShowMessage(LOGGER_ERROR,tr("No existe fichero %1").arg(iniFile));
        this->setEnabled(true);
        return false;
    }

    memoryMap->m_mapa.m_variables.Proceso=p->text();
    memoryMap->m_mapa.m_variables.NombreRutaIni=iniFile;

    QFileInfo info(memoryMap->m_mapa.m_variables.NombreRutaIni);
    memoryMap->m_mapa.m_variables.NombreIni=info.fileName();
    memoryMap->m_mapa.m_variables.RutaIni=info.path();
    memoryMap->m_FilePath=memoryMap->m_mapa.m_variables.NombreRutaIni;
    index=config.getOTFieldIndex();
    p=ui->m_twData->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento OT %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return false;
    }
    memoryMap->m_Imagen=QPixmap();
    memoryMap->m_mapa.m_variables.OrdenFabricacion=p->text();
    memoryMap->m_FilePath=memoryMap->m_FilePath.replace(QString(".ini"),QString("_OF%1_C1_comp.bmp").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));
    memoryMap->m_mapa.m_variables.RutaBmp=memoryMap->m_mapa.m_variables.RutaIni;
    memoryMap->m_mapa.m_variables.NombreRutaBmp=memoryMap->m_mapa.m_variables.NombreRutaIni;
    memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");

    index=config.getLoteFieldIndex();
    p=ui->m_twData->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento Lote %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return false;
    }
    memoryMap->m_mapa.m_campos[0].ValorActualCampo=p->text();
    index=config.getFchCadFieldIndex();
    p=ui->m_twData->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento Fch Caducidad %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return false;
    }
    memoryMap->m_mapa.m_campos[1].ValorActualCampo=p->text();
    sltShowMessage(LOGGER_WARNING,tr("Generando %1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QCoreApplication::processEvents();

    deleteTempGSFile();
    memoryMap->JumpToTempWorld();
    In2CompoFile compoFile;
    compoFile.AbrirFicheroIni();    
    In2ImageCreator img;
    img.Convert();    
    img.sltGenerateImage();
    QApplication::restoreOverrideCursor();
    if (!memoryMap->m_Imagen.isNull()){       
        QApplication::restoreOverrideCursor();
        this->setEnabled(true);
        rtn=true;
        if (m_load){
            emit sgnWorkLoaded(true);
            QString txt=tr("Trabajo %1 cargado").arg(memoryMap->m_mapa.m_variables.NombreRutaIni);
            sltShowMessage(LOGGER_STORE,txt);
            In2Store p;
            p.createTable(memoryMap->m_mapa.m_variables.Proceso);
        //Meto la orden Vs el INI
            QString foo=QString("insert into OrdenVsIni (Orden,Ini,OT) Values ('%1','%2','%3')").arg(memoryMap->m_mapa.m_variables.Proceso).arg(memoryMap->m_mapa.m_variables.NombreRutaIni).arg(memoryMap->m_mapa.m_variables.OrdenFabricacion);
            p.execSql(foo);
        }
    }
    else{
        QString txt=tr("No se pudo generar %1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni);
        sltShowMessage(LOGGER_STORE,txt);
    }
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
    sltShowMessage(LOGGER_WARNING,tr("Elapse time %1").arg(fooTimer.elapsed()));
    return rtn;
    //this->setEnabled(true);
}

/** Previsualiza desde In2*/
void In2GramarColaImpresion::sltPrevFromIn2(){
    this->setEnabled(false);
    m_load=false;
    MemoryMap *memoryMap=MemoryMap::request(0);
    sltShowMessage(LOGGER_WARNING,tr("Generando %1").arg(memoryMap->m_FilePath));
    QCoreApplication::processEvents();
    sltGenerateFromIn2();
    In2Previsualizator *prev=new In2Previsualizator(this,memoryMap->m_FilePath);
    connect(this,SIGNAL(destroyed()),prev,SLOT(deleteLater()));

    prev->show();
    memoryMap->m_mapa.m_variables.OrdenFabricacion.clear();
    memoryMap->m_FilePath.clear();
    memoryMap->m_mapa.m_variables.RutaBmp.clear();
    memoryMap->m_mapa.m_variables.NombreRutaBmp.clear();
    memoryMap->m_mapa.m_variables.Proceso.clear();
    memoryMap->m_mapa.m_variables.NombreRutaIni.clear();
    memoryMap->m_mapa.m_variables.NombreIni.clear();
    memoryMap->m_mapa.m_variables.RutaIni.clear();
    this->setEnabled(true);

}



/** Genera un trabajo de la lista de Gramar*/
void In2GramarColaImpresion::sltPrevFromGramar(){
    this->setEnabled(false);
    MemoryMap *memoryMap=MemoryMap::request(0);
    In2ConfiguratorERP config;
    config.readConfig();
    int index=config.getCompoFieldIndex();
    int row=ui->m_twGramar->currentRow();
    QTableWidgetItem *p=ui->m_twGramar->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return;
    }

    QString iniFile=QString("%1\\%2.ini").arg(memoryMap->m_mapa.m_variables.defaultImagePath).arg(p->text());
    if (!QFile::exists(iniFile)){
        sltShowMessage(LOGGER_ERROR,tr("No existe fichero %1").arg(iniFile));
        this->setEnabled(true);
        return;
    }

    memoryMap->m_mapa.m_variables.Proceso=p->text();
    memoryMap->m_mapa.m_variables.NombreRutaIni=iniFile;

    QFileInfo info(memoryMap->m_mapa.m_variables.NombreRutaIni);
    memoryMap->m_mapa.m_variables.NombreIni=info.fileName();
    memoryMap->m_mapa.m_variables.RutaIni=info.path();
    memoryMap->m_FilePath=memoryMap->m_mapa.m_variables.NombreRutaIni;
    index=config.getOTFieldIndex();
    p=ui->m_twGramar->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento OT %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return;
    }


    memoryMap->m_mapa.m_variables.OrdenFabricacion=p->text();
    memoryMap->m_FilePath=memoryMap->m_FilePath.replace(QString(".ini"),QString("_OF%1_C1_comp.bmp").arg(memoryMap->m_mapa.m_variables.OrdenFabricacion));
    memoryMap->m_mapa.m_variables.RutaBmp=memoryMap->m_mapa.m_variables.RutaIni;
    memoryMap->m_mapa.m_variables.NombreRutaBmp=memoryMap->m_mapa.m_variables.NombreRutaIni;
    memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");


    index=config.getLoteFieldIndex();
    p=ui->m_twGramar->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento Lote %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return;
    }
    memoryMap->m_mapa.m_campos[0].ValorActualCampo=p->text();    
    index=config.getFchCadFieldIndex();
    p=ui->m_twGramar->item(row,index);
    if (!p){
        sltShowMessage(LOGGER_ERROR,tr("No existe elemento Fch Caducidad %1-%2").arg(row).arg(index));
        this->setEnabled(true);
        return;
    }

    memoryMap->m_mapa.m_campos[1].ValorActualCampo=p->text();
    sltShowMessage(LOGGER_WARNING,tr("Generando %1").arg(memoryMap->m_mapa.m_variables.NombreRutaIni));


    QApplication::setOverrideCursor(Qt::BusyCursor);
    QCoreApplication::processEvents();

    deleteTempGSFile();
    memoryMap->JumpToTempWorld();
    In2CompoFile compoFile;
    compoFile.AbrirFicheroIni();

    In2ImageCreator img;
    img.Convert();
    img.sltGenerateImage();
    QString txt=tr("Trabajo %1 cargado").arg(memoryMap->m_mapa.m_variables.NombreRutaIni);
    sltShowMessage(LOGGER_STORE,txt);
    QApplication::restoreOverrideCursor();
    In2Previsualizator *prev=new In2Previsualizator(this,memoryMap->m_FilePath);
    connect(this,SIGNAL(destroyed()),prev,SLOT(deleteLater()));

    prev->show();
    memoryMap->m_mapa.m_variables.OrdenFabricacion.clear();
    memoryMap->m_FilePath.clear();
    memoryMap->m_mapa.m_variables.RutaBmp.clear();
    memoryMap->m_mapa.m_variables.NombreRutaBmp.clear();
    memoryMap->m_mapa.m_variables.Proceso.clear();
    memoryMap->m_mapa.m_variables.NombreRutaIni.clear();
    memoryMap->m_mapa.m_variables.NombreIni.clear();
    memoryMap->m_mapa.m_variables.RutaIni.clear();
    this->setEnabled(true);

}
