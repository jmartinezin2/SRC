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
#include "curvas.h"
#include "configuracion.h"
#include "globals.h"
#include "teclado.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextEdit>
#include "in2log.h"
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>


#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

//Celda seleccionada
static int iFila=0;
static int iColumna=0;
QStringList NombreColumnasTablaCurvas=(QStringList()<<"x"<<"y");
static unsigned char NumeroColumnasTablaCurvas;

QStringList TipoCurvas=(QStringList()<<"Lineal");
static unsigned char NumeroTipoCurvas;

unsigned char flag_modificando_punto=0;
static QString tmpEquipo;
static QString tmpFecha;
static QString tmpTiempoSesion;
static QString tmpUsuario;

//Tamaños
#define TABLE_FIXED_WIDTH 16*FACTOR

Curvas::Curvas(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::curvasUI)
{	
    ui->setupUi(this);
    GlobalSetScreenSize(this);
    m_memoryMap=MemoryMap::request(0);

    this->setDataCommon();

    NumeroTipoCurvas=TipoCurvas.count();
    NumeroColumnasTablaCurvas=NombreColumnasTablaCurvas.count();
    ui->m_Tabla->setWindowTitle("Gestión de Curvas");
    ui->m_Tabla->setColumnCount(NumeroColumnasTablaCurvas);
    ui->m_Tabla->setShowGrid(true);
    ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasTablaCurvas);
    for (int index=0;index<NumeroColumnasTablaCurvas;index++)
            ui->m_Tabla->setColumnWidth(index,(ui->groupBoxCentral->width()-50)/NumeroColumnasTablaCurvas);
	

    connect(ui->m_BackButton,SIGNAL(clicked()),this,SLOT(Atras()));
    connect(ui->m_RegistroNuevo,SIGNAL(clicked()),this,SLOT(RegistroNuevo()));
    connect(ui->m_RegistroEliminar,SIGNAL(clicked()),this,SLOT(RegistroEliminar()));
    connect(ui->m_RegistroModificar,SIGNAL(clicked()),this,SLOT(RegistroModificar()));
    connect(ui->m_Tabla,SIGNAL(cellPressed(int,int)),this,SLOT(SeleccionCampo(int, int)));
    connect(ui->m_Tabla,SIGNAL(cellClicked(int,int)),this,SLOT(SeleccionCampo(int, int)));
    connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));

    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_TiempoSesion, SIGNAL(selectionChanged()),this, SLOT(UpdateTiempoRegistro()));
    #endif

    connect(ui->m_AceptarValores, SIGNAL(clicked()),this, SLOT(AceptarValores()));
    connect(ui->m_CancelarValores, SIGNAL(clicked()),this, SLOT(CancelarValores()));
	
	TestRoot();

    ui->m_TiempoSesion->setEnabled(false);
    ui->m_Usuario->setEnabled(false);
    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);


    QwtPlot *plot = new QwtPlot(ui->groupBoxCentral);

    plot->setGeometry(0,0,600,400);

    plot->setTitle( "Plot Demo" );
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, 0.0, 10.0 );
    plot->insertLegend( new QwtLegend() );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Some Points" );
    curve->setPen( QPen( Qt::blue, 4 ) ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve->setSymbol( symbol );

    QPolygonF points;
    points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
        << QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
        << QPointF( 4.0, 7.9 ) << QPointF( 5.0, 7.1 );
    curve->setSamples( points );

    curve->attach( plot );

    plot->resize( 600, 400 );
    plot->show();

}

Curvas::~Curvas()
{
}

/** Si no es root, bloqueo funcionalidad.*/
void Curvas::TestRoot(){
    if (m_memoryMap->getUserRights()<ACCESS_GOD){
        ui->m_RegistroNuevo->setEnabled(false);
        ui->m_RegistroModificar->setEnabled(false);
        ui->m_RegistroEliminar->setEnabled(false);
    }
}
void Curvas::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}

void Curvas::Atras(){
    this->deleteLater();
}

//Elimina la usuario seleccionado
void Curvas::RegistroEliminar(){

    QString tmp;
    QTableWidgetItem *aux0,*aux1,*aux2;
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    int FilaSeleccionada=ui->m_Tabla->currentRow();
    if (FilaSeleccionada!=-1){
        int ret = QMessageBox::warning(this, tr("ATENCIÓN"),tr("¿Desea eliminar el registro seleccionado?"),
        QMessageBox::Ok | QMessageBox::Cancel);
        if (ret==QMessageBox::Ok){
            aux0=new QTableWidgetItem();
            aux0=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
            aux1=new QTableWidgetItem();
            aux1=ui->m_Tabla->item(ui->m_Tabla->currentRow(),1);
            aux2=new QTableWidgetItem();
            aux2=ui->m_Tabla->item(ui->m_Tabla->currentRow(),2);
            tmp=QString("delete from Curvas where Equipo='%1' and Fecha='%2' and TiempoSesion='%3' ")
                        .arg(aux0->text().toAscii().data())
                        .arg(aux1->text().toAscii().data())
                        .arg(aux2->text().toAscii().data());
            In2Log db;
            if (!db.execSQLNoSave(tmp)){
                QMessageBox::warning(0,tr("ERROR Curvas (RegistroEliminar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
            }
            else{
               ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
               UpdateInfo();
               ActualizarTiemposFuncionamiento();
            }
        }
    }

    ui->m_RegistroEliminar->setEnabled(true);
    ui->m_RegistroModificar->setEnabled(true);
    ui->m_RegistroNuevo->setEnabled(true);
}


//Elimina el usuario seleccionado
void Curvas::RegistroEliminar_SinPreguntar(){

    QString tmp;
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    tmp=QString("delete from Curvas where Equipo='%1' and Fecha='%2' and TiempoSesion='%3' ")
                .arg(tmpEquipo)
                .arg(tmpFecha)
                .arg(tmpTiempoSesion);
    ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
    In2Log db;
    if (!db.execSQLNoSave(tmp))
        QMessageBox::warning(0,tr("ERROR Curvas (RegistroEliminar_sinPreguntar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
}

//Elimina la usuario seleccionado
void Curvas::RegistroEliminarTodo(){

    QString tmp;
    QString equipo;
    QTableWidgetItem *aux0;
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    aux0=new QTableWidgetItem();
    aux0=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
    equipo=aux0->text();
    int FilaSeleccionada=ui->m_Tabla->currentRow();
    if (FilaSeleccionada!=-1){
        int ret = QMessageBox::warning(this, tr("ATENCIÓN"),tr("¿Desea eliminar todos los registros de %1?").arg(equipo),
        QMessageBox::Ok | QMessageBox::Cancel);
        if (ret==QMessageBox::Ok){
            tmp=QString("delete from Curvas where Equipo='%1'")
                        .arg(aux0->text().toAscii().data());
            In2Log db;
            if (!db.execSQLNoSave(tmp)){
                QMessageBox::warning(0,tr("ERROR Curvas (RegistroEliminarTodo)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
            }
            else{               
               ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
               UpdateInfo();
               ActualizarTiemposFuncionamiento();
            }
        }
    }
    ui->m_RegistroEliminar->setEnabled(true);
    ui->m_RegistroModificar->setEnabled(true);
    ui->m_RegistroNuevo->setEnabled(true);
}

//Modifica la usuario seleccionado
void Curvas::RegistroModificar(){

    CargarValoresTipo();

    ui->m_BackButton->setEnabled(false);
    ui->m_RegistroNuevo->setEnabled(false);
    ui->m_RegistroEliminar->setEnabled(false);
    ui->m_RegistroModificar->setEnabled(false);
    ui->m_TiempoSesion->setEnabled(true);
    if (m_memoryMap->getUserRights()==ACCESS_GOD) ui->m_Usuario->setEnabled(true);
    ui->m_AceptarValores->setEnabled(true);
    ui->m_CancelarValores->setEnabled(true);

    QTableWidgetItem *aux;
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());

    aux=new QTableWidgetItem();

    aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
    tmpEquipo.sprintf(aux->text().toAscii().data());
    for (int i=0;i<NumeroTipoCurvas;i++){
        //if (tmpEquipo==TipoCurvas.at(i).toAscii().data()) ui->m_Equipo->setCurrentIndex(i);
    }

    aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),2);
    tmpTiempoSesion.sprintf(aux->text().toAscii().data());
    ui->m_TiempoSesion->setText(tmpTiempoSesion);

    aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),3);
    tmpUsuario.sprintf(aux->text().toAscii().data());
    ui->m_Usuario->setText(tmpUsuario);

    flag_modificando_punto=1;

    ui->m_RegistroModificar->setEnabled(false);
    ui->m_RegistroEliminar->setEnabled(false);
}


//Carga un nuevo usuario
void Curvas::RegistroNuevo(){
    CargarValoresTipo();
    //ui->m_OpcionesUsuarios->setEnabled(true);

    ui->m_TiempoSesion->setEnabled(true);
    ui->m_Usuario->setText(QString("%1").arg(m_memoryMap->getUser()));
    if (m_memoryMap->getUserRights()==ACCESS_GOD) ui->m_Usuario->setEnabled(true);

    ui->m_AceptarValores->setEnabled(true);
    ui->m_CancelarValores->setEnabled(true);

    ui->m_BackButton->setEnabled(false);
    ui->m_RegistroNuevo->setEnabled(false);
    ui->m_RegistroEliminar->setEnabled(false);
    ui->m_RegistroModificar->setEnabled(false);

}

//Se selecciona un campo de la tabla
void Curvas::SeleccionCampo(int x, int y){
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow()); //Se selecciona toda la fila
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_RegistroEliminar->setEnabled(true);
        ui->m_RegistroModificar->setEnabled(true);
    }
    iFila=x;
    iColumna=y;
}

//Ajusta el tamaño de la tabla
void Curvas::AjustaTabla(){
	int newWidth=0;
	int newHeight=0;
	int i;

    for(i=0;i<NumeroColumnasTablaCurvas;i++){
        newWidth+= ui->m_Tabla->columnWidth(i);
    }
    newWidth+=ui->m_Tabla->verticalHeader()->width() + 2 * ui->m_Tabla->frameWidth();
	
    newHeight=ui->m_Tabla->horizontalHeader()->height()+(ui->m_Tabla->rowCount())*ui->m_Tabla->rowHeight(0)+2*ui->m_Tabla->frameWidth();
	
	if (newHeight>HTABLA) {
		newHeight=HTABLA;
        ui->m_Tabla->verticalScrollBar()->setFixedWidth(TABLE_FIXED_WIDTH);
        for (int index=0;index<NumeroColumnasTablaCurvas;index++) ui->m_Tabla->setColumnWidth(index,(WTABLA)/NumeroColumnasTablaCurvas-1);
		newWidth=0;
        for(i=0;i<NumeroColumnasTablaCurvas;i++){
            newWidth+= ui->m_Tabla->columnWidth(i);
		}
        newWidth+=ui->m_Tabla->verticalHeader()->width() + 2 * ui->m_Tabla->frameWidth();
        newWidth+=ui->m_Tabla->verticalScrollBar()->width();	//Lo pongo a mano porque el verticalScrollBar()->width me devuelve 100??????
	}
    ui->m_Tabla->setMaximumWidth(newWidth);
    ui->m_Tabla->setMinimumWidth(newWidth);
}

//Actualiza la info que le llega por teclado
void Curvas:: InputTeclado(QString Buffer){
    QString tmp;
}

//Actualiza un campo de la tabla
void Curvas::ActualizaCampo(QString item){
    QTableWidgetItem *tmp = new QTableWidgetItem(item);
    Qt::ItemFlags Flags;
    Flags=tmp->flags();
    Flags&=~(Qt::ItemIsEditable);
    tmp->setFlags(Flags);
    ui->m_Tabla->setItem(iFila,iColumna,tmp);
}

void Curvas::UpdateInfo(){

    int index=0;
    QString sql;

    iFila=0;
    iColumna=0;   
    ui->m_Tabla->clear();
    ui->m_Tabla->setSortingEnabled(false);

    sql=QString("select * from Curvas");
    In2Log db;
    QStringList rslt=db.execSQL(sql);
    if (!rslt.count()) {
        QMessageBox::warning(0,tr("ERROR Curvas (UpdateInfo)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
    }
    else{
        ui->m_Tabla->setRowCount(rslt.count()/NumeroColumnasTablaCurvas);
        for (index=0;index<rslt.count();index++){
                iFila=index/NumeroColumnasTablaCurvas;
                iColumna=index%NumeroColumnasTablaCurvas;
                ActualizaCampo(rslt.at(index));
        }
    }
    ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasTablaCurvas);
    ui->m_Tabla->setSortingEnabled(true);
    ui->m_Tabla->sortItems(1,Qt::DescendingOrder);
}


void Curvas::UpdateTiempoRegistro(){
    //InputData *input = InputData::request(this,"Tiempo sesión");
    QString text=ui->m_TiempoSesion->toPlainText();
    InputData *input = InputData::request(this,"Tiempo sesión",text);
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateTiempo(QString)));
        input->setFocus();
    }
}
void Curvas::UpdateTiempo(QString value){
    ui->m_TiempoSesion->setText(value);
    ui->m_TiempoSesion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void Curvas::UpdateFechaRegistro(){

}

void Curvas::UpdateFecha(QString value){

}



void Curvas::CancelarValores(){

    flag_modificando_punto=0;
    ui->m_TiempoSesion->setText("");
    ui->m_Usuario->setText("");
    ui->m_BackButton->setEnabled(true);
    ui->m_RegistroNuevo->setEnabled(true);
    ui->m_RegistroEliminar->setEnabled(true);
    ui->m_RegistroModificar->setEnabled(true);
    ui->m_TiempoSesion->setEnabled(false);
    ui->m_Usuario->setEnabled(false);
    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);
}

//Acepta la seleccion
void Curvas::InsertarValores(){

    QString tmp;


    tmp=QString("insert into Curvas ('TiempoSesion','Usuario') Values ('%1','%2')")
            .arg(ui->m_TiempoSesion->toPlainText().toAscii().data())
            .arg(m_memoryMap->getUser());

    In2Log db;
    if (!db.execSQLNoSave(tmp)){
        //Emitir error!!!
    }


}


//Acepta la seleccion
void Curvas::ActualizarDiaLimpiezaLampara(){

    QString tmp;
    QString fecha,hora,fechahora;

    QDate dummyD=QDate::currentDate();
    fecha=dummyD.toString("yyyy-MM-dd");

    QTime dummyT=QTime::currentTime();
    hora=dummyT.toString();

    fechahora=QString("%1 %2").arg(fecha).arg(hora);

    int ret = QMessageBox::warning(this, tr("ATENCIÓN"),tr("¿Ha realizado una operación de limpieza de lámpara?"),
    QMessageBox::Ok | QMessageBox::Cancel);
    if (ret==QMessageBox::Ok){
        tmp=QString("update Curvas set Fecha='%1',Usuario='%2' where Equipo='LimpiezaLampara' ")
                .arg(fechahora)
                .arg(m_memoryMap->getUser());

        In2Log db;
        if (!db.execSQLNoSave(tmp)){
            //Emitir error!!!
        }
        else{
            UpdateInfo();
            ActualizarTiemposFuncionamiento();
        }
    }


}



void Curvas::AceptarValores(){

    if (flag_modificando_punto==0){
        InsertarValores();
    }
    else{
        RegistroEliminar_SinPreguntar();
        InsertarValores();
    }

    UpdateInfo();
    ActualizarTiemposFuncionamiento();
    flag_modificando_punto=0;
    ui->m_TiempoSesion->setText("");
    ui->m_Usuario->setText("");
    ui->m_BackButton->setEnabled(true);
    ui->m_RegistroNuevo->setEnabled(true);
    ui->m_RegistroEliminar->setEnabled(true);
    ui->m_RegistroModificar->setEnabled(true);
    ui->m_TiempoSesion->setEnabled(false);
    ui->m_Usuario->setEnabled(false);
    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);

}


void Curvas::CargarValoresTipo(){
	
    QString Vtemp;
    int num_equipos=TipoCurvas.count();

    for (int i=0;i<num_equipos;i++){
        Vtemp=TipoCurvas.at(i).toAscii().data();
    }

}

/** Cargamos datos comunes*/
void Curvas::setDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=NULL;
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


void Curvas::ActualizarTiemposFuncionamiento(void){


}

