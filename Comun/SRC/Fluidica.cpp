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
#include "Fluidica.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFileDialog>
//#include "principal.h"
#include "in2gestorfluidica.h"
#include <QIcon>
#include <QPixmap>
#include "omp.h"
#include "XCL_Library.h"
#include <QTimer>

#define TIMEOUT_UPDATE 1000

#define  DEFAULT_COLWIDTH 125
#define  BUT_HEIGHT 50
//Columnas
#define COL_EQUIPO1     0
#define COL_EQUIPO2     1
#define COL_EQUIPO3     2
#define COL_EQUIPO4     3
#define COL_EQUIPO5     4
#define COL_EQUIPO6     5
#define COL_COMENTARIO  6

#define MAX_EQUIPOS 6

//Filas
enum FILAS_TABLA{
    ROW_USAR=0,
    ROW_DESCRIPCION,
    ROW_COLOR,
    ROW_TSUMINISTRO,
    ROW_TRETORNO,
    ROW_TDEPOSITO,
    ROW_PSUMINISTRO,
    ROW_PRETORNO,
    ROW_PDIFERENCIAL,
    ROW_PMENISCO,
    ROW_NDEPOSITO,
    ROW_LED,
    ROW_SWITCH,
    ROW_START,
    ROW_SHUTDOWN,
    ROW_RESET,
    ROW_PAUSE,
    ROW_ESTADO,
    ROW_LASTERROR
};




Fluidica::Fluidica(QWidget *parent)
    : ui(new Ui::FluidicaUI)
{
	
    ui->setupUi(this);
    m_memoryMap=MemoryMap::request(0);
    m_parent=parent;
    ui->m_tvInfo->setColumnWidth(COL_EQUIPO1,DEFAULT_COLWIDTH);
    ui->m_tvInfo->setColumnWidth(COL_EQUIPO2,DEFAULT_COLWIDTH);
    ui->m_tvInfo->setColumnWidth(COL_EQUIPO3,DEFAULT_COLWIDTH);
    ui->m_tvInfo->setColumnWidth(COL_EQUIPO4,DEFAULT_COLWIDTH);
    ui->m_tvInfo->setColumnWidth(COL_EQUIPO5,DEFAULT_COLWIDTH);
    ui->m_tvInfo->setColumnWidth(COL_EQUIPO6,DEFAULT_COLWIDTH);

    ui->m_tvInfo->setRowHeight(ROW_START,BUT_HEIGHT);
    ui->m_tvInfo->setRowHeight(ROW_SHUTDOWN,BUT_HEIGHT);
    ui->m_tvInfo->setRowHeight(ROW_RESET,BUT_HEIGHT);
    ui->m_tvInfo->setRowHeight(ROW_PAUSE,BUT_HEIGHT);

    ui->m_tvInfo->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_tvInfo->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    QPushButton *pbExit=new QPushButton("SALIR");
    ui->m_tvInfo->setCellWidget(ROW_LASTERROR,COL_COMENTARIO,pbExit);
    connect(pbExit,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));

    for (int x=0;x<MAX_EQUIPOS;x++){
        QPushButton *pStart=new QPushButton("START");

        ui->m_tvInfo->setCellWidget(ROW_START,x,pStart);
        QPushButton *pShutdown=new QPushButton("SHUTDOWN");
        ui->m_tvInfo->setCellWidget(ROW_SHUTDOWN,x,pShutdown);
        QPushButton *pReset=new QPushButton("RESET");
        ui->m_tvInfo->setCellWidget(ROW_RESET,x,pReset);
        QPushButton *pPause=new QPushButton("PAUSE");
        ui->m_tvInfo->setCellWidget(ROW_PAUSE,x,pPause);

        if (x==0){
            connect(pStart,SIGNAL(clicked()),this,SLOT(StartHydra_1()));
            connect(pShutdown,SIGNAL(clicked()),this,SLOT(ShutdownHydra_1()));
            connect(pReset,SIGNAL(clicked()),this,SLOT(RebootHydra_1()));
            connect(pPause,SIGNAL(clicked()),this,SLOT(PausarEquipo_1()));
        }
        if (x==1){
            connect(pStart,SIGNAL(clicked()),this,SLOT(StartHydra_2()));
            connect(pShutdown,SIGNAL(clicked()),this,SLOT(ShutdownHydra_2()));
            connect(pReset,SIGNAL(clicked()),this,SLOT(RebootHydra_2()));
            connect(pPause,SIGNAL(clicked()),this,SLOT(PausarEquipo_2()));
        }
        if (x==2){
            connect(pStart,SIGNAL(clicked()),this,SLOT(StartHydra_3()));
            connect(pShutdown,SIGNAL(clicked()),this,SLOT(ShutdownHydra_3()));
            connect(pReset,SIGNAL(clicked()),this,SLOT(RebootHydra_3()));
            connect(pPause,SIGNAL(clicked()),this,SLOT(PausarEquipo_3()));
        }
        if (x==3){
            connect(pStart,SIGNAL(clicked()),this,SLOT(StartHydra_4()));
            connect(pShutdown,SIGNAL(clicked()),this,SLOT(ShutdownHydra_4()));
            connect(pReset,SIGNAL(clicked()),this,SLOT(RebootHydra_4()));
            connect(pPause,SIGNAL(clicked()),this,SLOT(PausarEquipo_4()));
        }
        if (x==4){
            connect(pStart,SIGNAL(clicked()),this,SLOT(StartHydra_5()));
            connect(pShutdown,SIGNAL(clicked()),this,SLOT(ShutdownHydra_5()));
            connect(pReset,SIGNAL(clicked()),this,SLOT(RebootHydra_5()));
            connect(pPause,SIGNAL(clicked()),this,SLOT(PausarEquipo_5()));
        }

    }

    //connect(ui->m_tvInfo,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(sltCheckClicked(QTableWidgetItem *)));
    connect(ui->m_tvInfo,SIGNAL(cellClicked(int,int)),this,SLOT(sltCheckClicked(int,int)));
    connect(this,SIGNAL(destroyed()),SLOT(AceptarSeleccion()));

    GlobalSetScreenSize(this);

    //Qt::WindowFlags flags=this->windowFlags();
    //this->setWindowFlags(flags|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    this->setWindowFlags(Qt::WindowCloseButtonHint);


    //Llamamos al gestor para coger datos*/
    m_gestor=In2GestorFluidica::request(this);
    if (m_gestor->numEquipos()>0){ //QMessageBox::warning(this,"ERROR","ERROR DE COMUNICACION CON FLUIDICA.COMPRUEBE CONEXIONES Y REINICE APLICACION",QMessageBox::Ok);
        InitTabla();
        TestRoot();
        for (int x=0;x<m_gestor->numEquipos();x++){
            if (x==0)
                QTimer::singleShot(0,this,SLOT(sltActualizaDatos_1()));
            if (x==1)
                QTimer::singleShot(500,this,SLOT(sltActualizaDatos_2()));
            if (x==2)
                QTimer::singleShot(1000,this,SLOT(sltActualizaDatos_3()));
            if (x==3)
                QTimer::singleShot(1500,this,SLOT(sltActualizaDatos_4()));
            if (x==4)
                QTimer::singleShot(2000,this,SLOT(sltActualizaDatos_5()));

        }
    }
    else
        QMessageBox::warning(this,"ERROR",tr("No se ha podido comunicar con Fluidica"),QMessageBox::Ok);
}

void Fluidica::sltCheckClicked(int row, int col){
    if (row==ROW_USAR){
        if (col<MAX_EQUIPOS){
            QTableWidgetItem *p=ui->m_tvInfo->item(row,col);
            if (p){
                bool state=false;
                if (p->checkState()==Qt::Checked) p->setCheckState(Qt::Unchecked);
                else{
                    p->setCheckState(Qt::Checked);
                    state=true;
                }
                UpdateUsarEquipo(col,state);
            }
        }
    }
}

void Fluidica::closeEvent(QCloseEvent *event)
{
    this->AceptarSeleccion();
}

Fluidica::~Fluidica()
{
    /*if (m_parent!=NULL){
        m_parent->show();
    }
    else
        GlobalShowMainWindow();*/
}

/** Inicializa los items de la tabla*/
void Fluidica::InitTabla(){

    //Todos los Hydras
    for (int y =0;y<MAX_EQUIPOS;y++){
        for (int x=0;x<ui->m_tvInfo->rowCount();x++){
            QTableWidgetItem *p=ui->m_tvInfo->item(x,y);
            if (p){
                Qt::ItemFlags Flags;
                Flags=p->flags();
                Flags&=~(Qt::ItemIsEditable);
                if (y>=m_gestor->numEquipos()) Flags&=~(Qt::ItemIsEnabled);
                p->setFlags(Flags);
            }
        }
    }

    /*m_supplyOffset=m_gestor->getSupplyOffset(x);
    m_returnOffset=m_gestor->getReturnOffset(x);*/
    for (int x=0;x<m_gestor->numEquipos();x++){ //Veo los forzados
        if (m_memoryMap->m_mapa.m_maquina.m_fluidica.Used[x]){
            QTableWidgetItem *p=ui->m_tvInfo->item(ROW_USAR,x);
            if (p) p->setCheckState(Qt::Checked);
        }
        QString desc=m_gestor->getDescription(x);
        setDescripcion(desc,x);
        setColor(x);
    }
    ///ui->m_Color_1->setPalette(paleta);
}

/** Actualiza los datos del Hydra 0*/
void Fluidica::sltActualizaDatos_1(){


    float dummy=m_gestor->getSupplyTemp(COL_EQUIPO1).val;
    setTSuministro(QString("%1").arg(dummy).left(4),COL_EQUIPO1);

    dummy=m_gestor->getReturnTemp(COL_EQUIPO1).val;
    setTRetorno(QString("%1").arg(dummy).left(4),COL_EQUIPO1);

    dummy=m_gestor->getResTemp(COL_EQUIPO1).val;
    setTDeposito(QString("%1").arg(dummy).left(4),COL_EQUIPO1);

    dummy=m_gestor->getSupplyPre(COL_EQUIPO1).val;
    setPSuministro(QString("%1").arg(dummy).left(6),COL_EQUIPO1);

    dummy=m_gestor->getReturnPre(COL_EQUIPO1).val;
    setPRetorno(QString("%1").arg(dummy).left(6),COL_EQUIPO1);

    //Estos igual los sacamos fuera?
    dummy=m_gestor->getDiferentialPresure(COL_EQUIPO1);
    setPDiferencial(QString("%1").arg(dummy).left(6),COL_EQUIPO1);

    dummy=m_gestor->getMeniscusPresure(COL_EQUIPO1);
    setPMenisco(QString("%1").arg(dummy).left(6),COL_EQUIPO1);

    dummy=m_gestor->getLevelRes(COL_EQUIPO1).val;
    setNDeposito(QString("%1").arg(dummy).left(3),COL_EQUIPO1);

    QString desc=m_gestor->getFaultsDescription(COL_EQUIPO1);
    setHydraLastError(desc,COL_EQUIPO1);

    //desc=m_gestor->getModeDescription(COL_EQUIPO1);
    setState(COL_EQUIPO1);
    setLEDs(COL_EQUIPO1);
    setSwitches(COL_EQUIPO1);

    QTimer::singleShot(TIMEOUT_UPDATE,this,SLOT(sltActualizaDatos_1()));
}


/** Actualiza los datos del Hydra 0*/
void Fluidica::sltActualizaDatos_2(){


    float dummy=m_gestor->getSupplyTemp(COL_EQUIPO2).val;
    setTSuministro(QString("%1").arg(dummy).left(4),COL_EQUIPO2);

    dummy=m_gestor->getReturnTemp(COL_EQUIPO2).val;
    setTRetorno(QString("%1").arg(dummy).left(4),COL_EQUIPO2);

    dummy=m_gestor->getResTemp(COL_EQUIPO2).val;
    setTDeposito(QString("%1").arg(dummy).left(4),COL_EQUIPO2);

    dummy=m_gestor->getSupplyPre(COL_EQUIPO2).val;
    setPSuministro(QString("%1").arg(dummy).left(6),COL_EQUIPO2);

    dummy=m_gestor->getReturnPre(COL_EQUIPO2).val;
    setPRetorno(QString("%1").arg(dummy).left(6),COL_EQUIPO2);

    //Estos igual los sacamos fuera?
    dummy=m_gestor->getDiferentialPresure(COL_EQUIPO2);
    setPDiferencial(QString("%1").arg(dummy).left(6),COL_EQUIPO2);

    dummy=m_gestor->getMeniscusPresure(COL_EQUIPO2);
    setPMenisco(QString("%1").arg(dummy).left(6),COL_EQUIPO2);

    dummy=m_gestor->getLevelRes(COL_EQUIPO2).val;
    setNDeposito(QString("%1").arg(dummy).left(3),COL_EQUIPO2);

    QString desc=m_gestor->getFaultsDescription(COL_EQUIPO2);
    setHydraLastError(desc,COL_EQUIPO2);

    setState(COL_EQUIPO2);
    setLEDs(COL_EQUIPO2);
    setSwitches(COL_EQUIPO2);

    QTimer::singleShot(TIMEOUT_UPDATE,this,SLOT(sltActualizaDatos_2()));
}


/** Actualiza los datos del Hydra 0*/
void Fluidica::sltActualizaDatos_3(){


    float dummy=m_gestor->getSupplyTemp(COL_EQUIPO3).val;
    setTSuministro(QString("%1").arg(dummy).left(4),COL_EQUIPO3);

    dummy=m_gestor->getReturnTemp(COL_EQUIPO3).val;
    setTRetorno(QString("%1").arg(dummy).left(4),COL_EQUIPO3);

    dummy=m_gestor->getResTemp(COL_EQUIPO3).val;
    setTDeposito(QString("%1").arg(dummy).left(4),COL_EQUIPO3);

    dummy=m_gestor->getSupplyPre(COL_EQUIPO3).val;
    setPSuministro(QString("%1").arg(dummy).left(6),COL_EQUIPO3);

    dummy=m_gestor->getReturnPre(COL_EQUIPO3).val;
    setPRetorno(QString("%1").arg(dummy).left(6),COL_EQUIPO3);

    //Estos igual los sacamos fuera?
    dummy=m_gestor->getDiferentialPresure(COL_EQUIPO3);
    setPDiferencial(QString("%1").arg(dummy).left(6),COL_EQUIPO3);

    dummy=m_gestor->getMeniscusPresure(COL_EQUIPO3);
    setPMenisco(QString("%1").arg(dummy).left(6),COL_EQUIPO3);

    dummy=m_gestor->getLevelRes(COL_EQUIPO3).val;
    setNDeposito(QString("%1").arg(dummy).left(3),COL_EQUIPO3);

    QString desc=m_gestor->getFaultsDescription(COL_EQUIPO3);
    setHydraLastError(desc,COL_EQUIPO3);

    //desc=m_gestor->getModeDescription(COL_EQUIPO1);
    setState(COL_EQUIPO3);
    setLEDs(COL_EQUIPO3);
    setSwitches(COL_EQUIPO3);

    QTimer::singleShot(TIMEOUT_UPDATE,this,SLOT(sltActualizaDatos_3()));
}


/** Actualiza los datos del Hydra 0*/
void Fluidica::sltActualizaDatos_4(){


    float dummy=m_gestor->getSupplyTemp(COL_EQUIPO4).val;
    setTSuministro(QString("%1").arg(dummy).left(4),COL_EQUIPO4);

    dummy=m_gestor->getReturnTemp(COL_EQUIPO4).val;
    setTRetorno(QString("%1").arg(dummy).left(4),COL_EQUIPO4);

    dummy=m_gestor->getResTemp(COL_EQUIPO4).val;
    setTDeposito(QString("%1").arg(dummy).left(4),COL_EQUIPO4);

    dummy=m_gestor->getSupplyPre(COL_EQUIPO4).val;
    setPSuministro(QString("%1").arg(dummy).left(6),COL_EQUIPO4);

    dummy=m_gestor->getReturnPre(COL_EQUIPO4).val;
    setPRetorno(QString("%1").arg(dummy).left(6),COL_EQUIPO4);

    //Estos igual los sacamos fuera?
    dummy=m_gestor->getDiferentialPresure(COL_EQUIPO4);
    setPDiferencial(QString("%1").arg(dummy).left(6),COL_EQUIPO4);

    dummy=m_gestor->getMeniscusPresure(COL_EQUIPO4);
    setPMenisco(QString("%1").arg(dummy).left(6),COL_EQUIPO4);

    dummy=m_gestor->getLevelRes(COL_EQUIPO4).val;
    setNDeposito(QString("%1").arg(dummy).left(3),COL_EQUIPO4);

    QString desc=m_gestor->getFaultsDescription(COL_EQUIPO4);
    setHydraLastError(desc,COL_EQUIPO4);

    //desc=m_gestor->getModeDescription(COL_EQUIPO1);
    setState(COL_EQUIPO4);
    setLEDs(COL_EQUIPO4);
    setSwitches(COL_EQUIPO4);

    QTimer::singleShot(TIMEOUT_UPDATE,this,SLOT(sltActualizaDatos_4()));
}

/** Actualiza los datos del Hydra 0*/
void Fluidica::sltActualizaDatos_5(){


    float dummy=m_gestor->getSupplyTemp(COL_EQUIPO5).val;
    setTSuministro(QString("%1").arg(dummy).left(4),COL_EQUIPO5);

    dummy=m_gestor->getReturnTemp(COL_EQUIPO5).val;
    setTRetorno(QString("%1").arg(dummy).left(4),COL_EQUIPO5);

    dummy=m_gestor->getResTemp(COL_EQUIPO5).val;
    setTDeposito(QString("%1").arg(dummy).left(4),COL_EQUIPO5);

    dummy=m_gestor->getSupplyPre(COL_EQUIPO5).val;
    setPSuministro(QString("%1").arg(dummy).left(6),COL_EQUIPO5);

    dummy=m_gestor->getReturnPre(COL_EQUIPO5).val;
    setPRetorno(QString("%1").arg(dummy).left(6),COL_EQUIPO5);

    //Estos igual los sacamos fuera?
    dummy=m_gestor->getDiferentialPresure(COL_EQUIPO5);
    setPDiferencial(QString("%1").arg(dummy).left(6),COL_EQUIPO5);

    dummy=m_gestor->getMeniscusPresure(COL_EQUIPO5);
    setPMenisco(QString("%1").arg(dummy).left(6),COL_EQUIPO5);

    dummy=m_gestor->getLevelRes(COL_EQUIPO5).val;
    setNDeposito(QString("%1").arg(dummy).left(3),COL_EQUIPO5);

    QString desc=m_gestor->getFaultsDescription(COL_EQUIPO5);
    setHydraLastError(desc,COL_EQUIPO5);

    //desc=m_gestor->getModeDescription(COL_EQUIPO1);
    setState(COL_EQUIPO5);
    setLEDs(COL_EQUIPO5);
    setSwitches(COL_EQUIPO5);

    QTimer::singleShot(TIMEOUT_UPDATE,this,SLOT(sltActualizaDatos_5()));
}




//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void Fluidica::AceptarSeleccion(){
    if (m_parent!=NULL){
        m_parent->show();
    }
    else
        GlobalShowMainWindow();
	this->deleteLater();
}
/** Se usa equipo*/
void Fluidica::UpdateUsarEquipo(int equipo, bool usar){
    m_memoryMap->m_mapa.m_maquina.m_fluidica.Used[equipo]=usar;
    m_gestor->sltTestAllDevicesState();
}

/** Si no es root, bloqueo funcionalidad.*/
void Fluidica::TestRoot(){
    if (m_memoryMap->getUserRights()<ACCESS_PRINTER){
        /*ui->m_StartHydra_1->setEnabled(false);
        ui->m_ShutdownHydra_1->setEnabled(false);
        ui->m_Used_1->setEnabled(false);*/
	}
}
/** Reinicia Hydra*/
void Fluidica::RebootHydra_1(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("¿Desea resetear Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->RebootEquipo(0);
    }
}
/** Inicia Hydra*/
void Fluidica::StartHydra_1(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea iniciar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->InitEquipo(0);
    }
}
/** Para Hydra*/
void Fluidica::StopHydra_1(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea parar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
        m_gestor->StopEquipo(0);
    }
}
/** Apaga Hydra*/
void Fluidica::ShutdownHydra_1(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea apagar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
           m_gestor->ShutdownEquipo(0);
    }
}

/** Reinicia Hydra*/
void Fluidica::RebootHydra_2(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("¿Desea resetear Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->RebootEquipo(1);
    }
}
/** Inicia Hydra*/
void Fluidica::StartHydra_2(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea iniciar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->InitEquipo(1);
    }
}
/** Para Hydra*/
void Fluidica::StopHydra_2(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea parar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
        m_gestor->StopEquipo(1);
    }
}
/** Apaga Hydra*/
void Fluidica::ShutdownHydra_2(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea apagar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
           m_gestor->ShutdownEquipo(1);
    }
}


/** Reinicia Hydra*/
void Fluidica::RebootHydra_3(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("¿Desea resetear Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->RebootEquipo(2);
    }
}
/** Inicia Hydra*/
void Fluidica::StartHydra_3(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea iniciar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->InitEquipo(2);
    }
}
/** Para Hydra*/
void Fluidica::StopHydra_3(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea parar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
        m_gestor->StopEquipo(2);
    }
}
/** Apaga Hydra*/
void Fluidica::ShutdownHydra_3(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea apagar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
           m_gestor->ShutdownEquipo(2);
    }
}

/** Reinicia Hydra*/
void Fluidica::RebootHydra_4(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("¿Desea resetear Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->RebootEquipo(3);
    }
}
/** Inicia Hydra*/
void Fluidica::StartHydra_4(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea iniciar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->InitEquipo(3);
    }
}
/** Para Hydra*/
void Fluidica::StopHydra_4(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea parar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
        m_gestor->StopEquipo(3);
    }
}
/** Apaga Hydra*/
void Fluidica::ShutdownHydra_4(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea apagar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
           m_gestor->ShutdownEquipo(3);
    }
}

/** Reinicia Hydra*/
void Fluidica::RebootHydra_5(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("¿Desea resetear Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->RebootEquipo(4);
    }
}
/** Inicia Hydra*/
void Fluidica::StartHydra_5(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea iniciar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
            m_gestor->InitEquipo(4);
    }
}
/** Para Hydra*/
void Fluidica::StopHydra_5(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea parar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
        m_gestor->StopEquipo(4);
    }
}
/** Apaga Hydra*/
void Fluidica::ShutdownHydra_5(){
    int respuesta;
    respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea apagar Hydra?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
           m_gestor->ShutdownEquipo(4);
    }
}


/** Filtros que lanzan alarmas*/
/*void Fluidica::filterMeniscus(int x){
    float dummy=m_gestor->getMeniscusPresure(x);
    if (dummy>0){
        emit SendDebugInfo(LOGGER_WARNING,QString(""));
    }

}*/

/** Carga la descripcion*/
void Fluidica::setColor(int equipo){
    QColor color=m_gestor->getColor(equipo);
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_COLOR,equipo);
    if (p){
        p->setBackgroundColor(color);
    }
}

/** Carga los LED*/
void Fluidica::setLEDs(int equipo){
    QString desc=m_gestor->getBitsDescription(equipo);
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_LED,equipo);
    if (p){
        p->setText(desc);
    }
}
/** Carga los Switches*/
void Fluidica::setSwitches(int equipo){
    QString desc=m_gestor->getSwitchesDescription(equipo);
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_SWITCH,equipo);
    if (p){
        p->setText(desc);
    }
}


/** Carga la descripcion*/
void Fluidica::setDescripcion(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_DESCRIPCION,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la TSuministro*/
void Fluidica::setTSuministro(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_TSUMINISTRO,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la TDeposito*/
void Fluidica::setTDeposito(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_TDEPOSITO,equipo);
    if (p){
        p->setText(txt);
    }
}

/** Carga la TRetorno*/
void Fluidica::setTRetorno(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_TRETORNO,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la PSuministro*/
void Fluidica::setPSuministro(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_PSUMINISTRO,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la PRetorno*/
void Fluidica::setPRetorno(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_PRETORNO,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la PDiferencial*/
void Fluidica::setPDiferencial(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_PDIFERENCIAL,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la PMenisco*/
void Fluidica::setPMenisco(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_PMENISCO,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga la NDeposito*/
void Fluidica::setNDeposito(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_NDEPOSITO,equipo);
    if (p){
        p->setText(txt);
    }
}

/** Carga el ultimo error*/
void Fluidica::setHydraLastError(QString txt, int equipo){
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_LASTERROR,equipo);
    if (p){
        p->setText(txt);
    }
}
/** Carga el ultimo estado*/
void Fluidica::setState(int equipo){
    QString desc=m_gestor->getModeDescription(equipo);
    QTableWidgetItem *p=ui->m_tvInfo->item(ROW_ESTADO,equipo);
    if (p){
        p->setText(desc);
    }
}





