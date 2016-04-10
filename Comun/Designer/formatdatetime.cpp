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
/** @addtogroup formatDateTime
 *  @{*/
/** @file formatDateTime.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: formatDateTime.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#include "formatdatetime.h"
#include "globals.h"
#include <QMessageBox>
#include <QFileDialog>

#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QApplication>
#include <Windows.h>
#include <QFontComboBox>
#include <assert.h>
#include <QPixmap>
#include <QDebug>


formatDateTime::formatDateTime(QWidget *parent, struct m_DateTimeParameters &data)
    : ui(new Ui::EditDateTimeUI)
{
    ui->setupUi(this);

    escalado=IMAGEN_SINESCALAR;
    offset_x=0;
    offset_y=0;
    m_DateTime=data;

    //if (!m_DateTime.formato.compare("")) m_DateTime.formato=QString("d/M/yy H");

    ui->m_formato->setText(QString("%1").arg(m_DateTime.formato));
    formatoDateTime=m_DateTime.formato;

    AceptarValoresCampos();

    connect(ui->m_Aceptar,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_Cancelar,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
    connect(ui->m_VistaPrevia,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));

}

formatDateTime::~formatDateTime(){

}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void formatDateTime::AceptarSeleccion(){

    AceptarValoresCampos();

    m_DateTime.formato=formatoDateTime;

    emit sglDateTimeConfigured(m_DateTime);

    this->deleteLater();

}


void formatDateTime::CancelarSeleccion(){

    this->deleteLater();
}



void formatDateTime::AceptarValoresCampos(){

    QString fechahora;

    ui->m_Aceptar->setEnabled(false);
    ui->m_Cancelar->setEnabled(false);


    formatoDateTime=ui->m_formato->toPlainText();

    fechahora=datetime.currentDateTime().toString(formatoDateTime);

    ui->lbl_formato->setText(QString("%1").arg(fechahora));


    ui->m_Aceptar->setEnabled(true);
    ui->m_Cancelar->setEnabled(true);

}

