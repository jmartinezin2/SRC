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

#include <QDebug>
#include "inputdata.h"
#include "globals.h"
#include <qTimer>

InputData *InputData::mInstance  = NULL;

InputData::InputData(QWidget *parent,const QString& title)
    : ui(new Ui::InputDataUI)
{
        ui->setupUi(this);
        Qt::WindowFlags flags=Qt::Window;
        this->setWindowFlags(flags|Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint);

        ui->title->setText(title);
    //Le damos el foco al
        ui->Data->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
        this->showNormal();
        SCRTeclado=new Teclado(parent,NUMERICO);
        connect(SCRTeclado,SIGNAL(bufferTeclado(QString)),this,SLOT(ActualizaCampo(QString)));
        connect(SCRTeclado,SIGNAL(Delete()),this,SLOT(DeleteTeclado()));
        connect(SCRTeclado,SIGNAL(Accept()),this,SLOT(AceptaSeleccion()));
        connect(ui->Aceptar,SIGNAL(clicked()),this,SLOT(AceptaSeleccion()));
        connect(ui->m_pbCancelar,SIGNAL(clicked()),this,SLOT(hide()));
        connect(parent,SIGNAL(destroyed()),this,SLOT(hide()));
        //QTimer::singleShot(2000,ui->Data,SLOT(setFocus()));
        ui->Data->setFocus();
        ui->Data->activateWindow();
}

InputData::InputData(QWidget *parent,const QString& title,const QString& text)
    : ui(new Ui::InputDataUI)
{
        ui->setupUi(this);
        Qt::WindowFlags flags=Qt::Window;
        this->setWindowFlags(flags|Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint);

        ui->title->setText(title);
    //Le damos el foco al
        ui->Data->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
        this->showNormal();
        SCRTeclado=new Teclado(parent,NUMERICO);
        connect(SCRTeclado,SIGNAL(bufferTeclado(QString)),this,SLOT(ActualizaCampo(QString)));
        connect(SCRTeclado,SIGNAL(Delete()),this,SLOT(DeleteTeclado()));
        connect(SCRTeclado,SIGNAL(Accept()),this,SLOT(AceptaSeleccion()));
        connect(ui->Aceptar,SIGNAL(clicked()),this,SLOT(AceptaSeleccion()));
        connect(parent,SIGNAL(destroyed()),this,SLOT(hide()));
        ui->Data->setFocus();
        ui->Data->setText(text);
}

/** Damos la posibilidad de no usar teclado virtual*/
InputData *InputData::request(QWidget *parent, const QString& title)
{
#ifdef CON_TECLADOVIRTUAL
    if(mInstance == NULL){
        mInstance = new InputData(parent, title);
    }
    return mInstance;
#else
    return NULL;
#endif
}
/** Pedimos...en caso de existir...ojito que como sea NULL, devuelve NULL*/
InputData *InputData::requestOnce(QWidget *parent, const QString& title)
{
    return mInstance;
}

InputData *InputData::request(QWidget *parent, const QString& title, const QString& text)
{
    if(mInstance == NULL){
        mInstance = new InputData(parent, title, text);
    }
    return mInstance;
}
/** Testea si existe*/
bool InputData::exist(){
    bool rtn=false;
    if (mInstance) rtn=true;
    return rtn;
}

/** Actualizao el foco*/
void InputData::setFocus(){
    ui->Data->setFocus();
}

InputData::~InputData()
{
    mInstance=NULL;
}
//Actualiza el campo seleccionado en la tabla
void InputData::ActualizaCampo(QString item){
    ui->Data->setText(item);
}
//Selecciona el nombre
void InputData::SelectData(){
    SCRTeclado->setText(ui->Data->text());
}

//Destruye
void InputData::DeleteTeclado(){
    //hide();
    SCRTeclado->deleteLater();
    this->deleteLater();
}

//Acepta la seleccion
void InputData::AceptaSeleccion(){
    this->setWindowFlags(Qt::CustomizeWindowHint);
    hide();
    emit Update(ui->Data->text());
}

void InputData::hide()
{
    //SCRTeclado->showMinimized();
    //this->showMinimized();
    DeleteTeclado();
    //this->deleteLater();
    //this->hide();
}
