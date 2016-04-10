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
#include "mantenimiento.h"
#include "configuracion.h"
#include "globals.h"
#include "teclado.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>
#include "in2gestormantenimiento.h"


#define NUM_COLS 4

mantenimiento::mantenimiento(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::mantenimientoUI)
{	
    ui->setupUi(this);

    ui->m_Tabla->setWindowTitle("Mantenimiento");

    ui->m_Tabla->setShowGrid(true);

    ui->m_Tabla->horizontalHeader()->setResizeMode(QHeaderView::Stretch );

    connect(ui->m_BackButton,SIGNAL(clicked()),this,SLOT(Atras()));
    connect(ui->m_RegistroNuevo,SIGNAL(clicked()),this,SLOT(RegistroNuevo()));

    connect(ui->m_AceptarValores, SIGNAL(clicked()),this, SLOT(AceptarValores()));
    connect(ui->m_CancelarValores, SIGNAL(clicked()),this, SLOT(CancelarValores()));
	
    ui->m_Equipo->setEnabled(false);    
    ui->m_leFecha->setEnabled(false);
    ui->m_leTiempo->setEnabled(false);
    ui->m_leUser->setEnabled(false);
    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);

    sltLeerBBDD();
}

mantenimiento::~mantenimiento()
{
    ui->m_Tabla->clear();
}

/** Si no es root, bloqueo funcionalidad.*/
void mantenimiento::TestRoot(){
}
/*void mantenimiento::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}*/

void mantenimiento::Atras(){
    this->deleteLater();
}

//Elimina la usuario seleccionado
void mantenimiento::RegistroEliminar(){


}


//Elimina el usuario seleccionado
void mantenimiento::RegistroEliminar_SinPreguntar(){

}

//Elimina la usuario seleccionado
void mantenimiento::RegistroEliminarTodo(){
}

//Modifica la usuario seleccionado
void mantenimiento::RegistroModificar(){

}


//Carga un nuevo usuario
void mantenimiento::RegistroNuevo(){
    QString tmp=QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    ui->m_leFecha->setText(tmp);
    MemoryMap *memoryMap=MemoryMap::request(0);
    tmp=memoryMap->getUser();
    ui->m_leUser->setText(tmp);
    ui->m_leTiempo->setText("1");
}

//Se selecciona un campo de la tabla
void mantenimiento::SeleccionCampo(int x, int y){
}

//Actualiza la info que le llega por teclado
void mantenimiento:: InputTeclado(QString Buffer){

}

//Actualiza un campo de la tabla
void mantenimiento::ActualizaCampo(QString item){
}

void mantenimiento::UpdateTiempoRegistro(){

}
void mantenimiento::UpdateTiempo(QString value){
}

void mantenimiento::UpdateFechaRegistro(){
}
void mantenimiento::UpdateFecha(QString value){
}



void mantenimiento::CancelarValores(){

}

//Acepta la seleccion
void mantenimiento::InsertarValores(){


}


//Acepta la seleccion
void mantenimiento::ActualizarDiaLimpiezaLampara(){



}



void mantenimiento::AceptarValores(){
}


void mantenimiento::CargarValoresTipo(){
	

}



void mantenimiento::sltLeerBBDD(void){
    In2GestorMantenimiento p;
    QStringList rslt=p.getAllRegisters();
    if (rslt.count()){
        int numRows=rslt.count()/NUM_COLS;
        ui->m_Tabla->setRowCount(numRows);
        for (int x=0;x<numRows;x++){
            for (int y=0;y<NUM_COLS;y++){
                if (x*y<rslt.count()){
                    QTableWidgetItem *p=new QTableWidgetItem(rslt.at(x*NUM_COLS+y));
                    ui->m_Tabla->setItem(x,y,p);
                }
            }
        }
    }
}

