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
#include "in2historico.h"
#include "in2counters.h"
#include "configuracion.h"
#include "globals.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTimer>
#include <QThread>
#include "in2managerreport.h"
#include "in2store.h"

#define DEFAULT_COL_WIDTH 150
#define SMALL_COL_WIDTH 100

#define NUM_FIELDS 6

#define COL_FECHA   0
#define COL_HORA    1
#define COL_USUARIO 2
#define COL_EVENTO  3
#define COL_CODIGO  4
#define COL_TAREA   5

In2Historico::In2Historico(QWidget *parent) :
    ui(new Ui::HistoricoUI)
{
    ui->setupUi(this);

    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    In2Store db;
    QStringList lst=db.getTables();

#if QT_VERSION>=0x050000
     ui->m_twTrabajos->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
     ui->m_twData->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
     //ui->m_twColores->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
     ui->m_twTrabajos->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
     ui->m_twData->horizontalHeader()->setResizeMode( COL_EVENTO, QHeaderView::Stretch );
#endif
     //ui->m_twData->setColumnCount(5);
    ui->m_twData->setColumnWidth(COL_FECHA,SMALL_COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_HORA,SMALL_COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_USUARIO,SMALL_COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_EVENTO,DEFAULT_COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_CODIGO,SMALL_COL_WIDTH);
    ui->m_twData->setColumnWidth(COL_TAREA,SMALL_COL_WIDTH);

    ui->m_twTrabajos->setRowCount(lst.count());
    for (int x=0;x<lst.count();x++){
        QTableWidgetItem *p=new QTableWidgetItem(lst.at(x));
        if (p) ui->m_twTrabajos->setItem(x,0,p);
    }

    connect(ui->m_twTrabajos,SIGNAL(cellPressed(int,int)),this,SLOT(sltShowInfo(int,int)));
    connect(ui->m_pbCSV,SIGNAL(clicked()),this,SLOT(sltExportToCVS()));
    connect(ui->m_pbBack,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->m_pbReport,SIGNAL(clicked()),this,SLOT(sltGenerateReport()));

    ui->m_pbEstado->setValue(0);
}

/** Muestra info*/
void In2Historico::sltShowInfo(int x,int y){
    QTableWidgetItem *p=ui->m_twTrabajos->item(x,y);
    if (p){
        In2Store db;
        QStringList lst=db.getAllData(p->text());
        ui->m_twData->clearContents();
        ui->m_twData->setRowCount(lst.count()/NUM_FIELDS);
        QFont fnt;
        fnt.setPointSize(8);
        ui->m_pbEstado->setValue(0);
        ui->m_pbEstado->setMaximum(lst.count());
        for (int index=0;index<lst.count();index++){
            ui->m_pbEstado->setValue(index);
            QTableWidgetItem *item=new QTableWidgetItem(lst.at(index));
            Qt::ItemFlags flags=item->flags();
            flags&=~Qt::ItemIsEditable;
            item->setFlags(flags);

            if (item){
                item->setFont(fnt);
                ui->m_twData->setItem(index/NUM_FIELDS,index%NUM_FIELDS,item);
            }

        }
        ui->m_twData->resizeRowsToContents();
        ui->m_twData->resizeColumnsToContents();
    }
    ui->m_pbEstado->setValue(0);
}

/** Salimos*/
void In2Historico::sltExit(){
    this->deleteLater();
}

/** Vuelca a un archivo CVS*/
void In2Historico::sltExportToCVS(){
    int rtn=QMessageBox::warning(0,"INFO",tr("Desea exportar esta tabla a formato CSV?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        QFileDialog fileName(this);
        QString Fichero;

        Fichero=fileName.getSaveFileName(this,"Selecciona un fichero",tr("txt,csv Files (*.txt *.csv)"));
        if (!Fichero.isEmpty()){
            QString foo;
            int rows=ui->m_twData->rowCount();
            int cols=ui->m_twData->columnCount();
            for (int x=0;x<rows;x++){
                for (int y=0;y<cols;y++){
                    QTableWidgetItem *p=ui->m_twData->item(x,y);
                    if (p) foo.append(QString("%1\t").arg(p->text()));
                }
                foo.chop(1);
                foo.append("\n");
            }
           foo.chop(1);
           QFile caFile(Fichero);
           caFile.open(QIODevice::WriteOnly | QIODevice::Text);
           QTextStream stream(&caFile);
           stream<<foo;
           caFile.close();
           QMessageBox::information(0,"INFO",tr("Generado %1").arg(Fichero),QMessageBox::Ok);
        }

    }
}
/** Genera un report*/
void In2Historico::sltGenerateReport(){
    QTableWidgetItem *item=ui->m_twTrabajos->currentItem();
    QString name;
    if (item){
        name=item->text();
    }
    else{
        QMessageBox::information(0,"INFO",tr("No hay trabajo seleccionado"),QMessageBox::Ok);
    }

    In2ManagerReport *report=new In2ManagerReport(0);
    In2Trabajo p;
    p.bActive=true;
    p.strCompoFile=name;
    MemoryMap *memoryMap=MemoryMap::request(0);
    memoryMap->m_mapa.m_variables.Proceso=name;
    report->setTrabajo(p);
    QString foo=QString("select Ini from OrdenVsIni where Orden='%1'").arg(memoryMap->m_mapa.m_variables.Proceso);
    In2Store db;
    QStringList rslt=db.execSql(foo);
    if (rslt.count()) {
        QFileInfo info(rslt.at(0));
        memoryMap->m_mapa.m_variables.NombreRutaIni=rslt.at(0);
        memoryMap->m_mapa.m_variables.NombreIni=info.fileName();

    }

    if (report->getAuto()){
        QThread *t=new QThread;
        t->start();
        connect(report,SIGNAL(In2ReportFinished()),report,SLOT(deleteLater()));
        connect(report,SIGNAL(In2ReportFinished()),t,SLOT(deleteLater()));
        connect(report,SIGNAL(In2NewReportGenerated(QString)),this,SLOT(sltReportGenerated()));
        report->moveToThread(t);
        QTimer::singleShot(0,report,SLOT(sltGenerateReport()));
    }
    else{
        delete (report);
    }
}

/** Ha terminado el report*/
void In2Historico::sltReportGenerated(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString foo=memoryMap->m_mapa.m_variables.NombreIni;
    foo.replace(".ini,",".pdf");
    QMessageBox::information(0,"INFO",tr("Se ha generado report %1").arg(foo),QMessageBox::Ok);
}
