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
#include "eventos.h"
#include "configuracion.h"
#include "globals.h"
#include "teclado.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextEdit>
#include "GestorDB_SQLITE.h"
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDir>



#define DEFAULT_COUNTERS_FILE "In2Counter.sqlite"



Eventos::Eventos(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::GestionEventosUI)
{	
    ui->setupUi(this);
    //GlobalSetScreenSize(this);
    m_memoryMap=MemoryMap::request(0);
    //this->SetDataCommon();
    m_Fila=0;
    m_Columna=0;
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);

    //connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    //connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));

    connect(ui->m_qtableLogs,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(sltGetLogTableItem(int,int)));
    connect(ui->m_pbExit,SIGNAL(clicked()),this,SLOT(Atras()));




    TestRoot();



    ui->m_qtableLogs->setColumnWidth(0,300);
    ui->m_qtableLogs->setColumnWidth(1,50);
    ui->m_qtableLogs->setColumnWidth(2,200);
    //ui->m_qtableLogs->horizontalHeader()->setStretchLastSection(true);
    ui->m_qtableLogs->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    LoadLogFiles();

}
/** Captura la pulsacion de la tabla de logs*/
void Eventos::sltGetLogTableItem(int x, int y){
    if (y==0){
        QTableWidgetItem *item=ui->m_qtableLogs->item(x,y);
        if (item){
            sltShowLogFile(item->text(),x);
        }

    }
}
/** Muestra el fichero de log*/
void Eventos::sltShowLogFile(const QString &file, int x){
    QFile logFile(QString("%1\\%2\\%3").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_PATH).arg(file));
    if (logFile.exists()){
        logFile.open(QIODevice::ReadOnly);
        QByteArray txt=logFile.readAll();
        logFile.close();
        //Calculamos el hash y compramos
        int posToChop=txt.lastIndexOf(HASH_ID);
        if (posToChop==-1){
            QTableWidgetItem *item=new QTableWidgetItem("NO HASH");
            ui->m_qtableLogs->setItem(x,2,item);
        }
        else{
            QByteArray fileChoped=txt.left(posToChop);

            QByteArray calculatedHash=CalculateHash(fileChoped);
            QByteArray fileToExtractHash=txt.right(txt.length()-posToChop);
            QByteArray fileHash=fileToExtractHash.right(fileToExtractHash.length()-QString("%1").arg(HASH_ID).length());
            if (fileHash==calculatedHash){
                QTableWidgetItem *item=new QTableWidgetItem("HASH OK");
                ui->m_qtableLogs->setItem(x,2,item);

            }
            else{
                QTableWidgetItem *item=new QTableWidgetItem("HASH NOK");
                ui->m_qtableLogs->setItem(x,2,item);
            }

        }
        ui->m_teShowLog->setText(QString(txt));

    }
    else{
        QMessageBox::warning(0,"ERROR","No existe el fichero",QMessageBox::Ok);
    }

    connect(this,SIGNAL(destroyed()),this,SLOT(Atras()));
}

Eventos::~Eventos()
{

}
/** Chequea el tab*/
void Eventos::sltCkeckTab(int x){
}


/** Carga los nombres de los ficheros de ese directorio*/
void Eventos::LoadLogFiles(){
    QString path=QString("%1%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_PATH);
    QDir dir(path);
    QStringList qstrLista=dir.entryList(QDir::Files);

    ui->m_qtableLogs->setRowCount(qstrLista.count());
    for (int x=0;x<qstrLista.count();x++){
        QTableWidgetItem *newItem=new QTableWidgetItem(qstrLista.at(x));
        ui->m_qtableLogs->setItem(x,0,newItem);
    }
}

/** Si no es root, bloqueo funcionalidad.*/
void Eventos::TestRoot(){
}

void Eventos::Atras(){
    ui->m_qtableLogs->clear();
    this->deleteLater();
}

void Eventos::ResetEventos(){
    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea resetear los eventos?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        //QApplication::setStyle(new InkjetStyle(THEME_NORMAL));
        flag_led_Maquina_rojo=0;
        flag_led_Lampara_rojo=0;
    }
}

//Se selecciona un campo de la tabla
void Eventos::SeleccionCampo(int x, int y){
}

//Ajusta el tamaño de la tabla
void Eventos::AjustaTabla(){
}
//Actualiza la info que le llega por teclado
void Eventos:: InputTeclado(QString Buffer){

}
//Actualiza un campo de la tabla
void Eventos::ActualizaCampo(QString item){
}

void Eventos::UpdateInfo(){
}

void Eventos::UpdateInfoFecha(QString dia, QString mes, QString ano){

}


void Eventos::AceptarValores(){

}

void Eventos::MostrarTodos(){
}

void Eventos::UpdateDiaMesAnoEvento(){

}

void Eventos::UpdateDiaEvento(){
}

void Eventos::UpdateDia(QString value){
}

void Eventos::UpdateMesEvento(){
}

void Eventos::UpdateMes(QString value){
}

void Eventos::UpdateAnoEvento(){
}

void Eventos::UpdateAno(QString value){
}

void Eventos::UpdateCalendario(void){
}



