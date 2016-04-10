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
#include "formatcounter.h"
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

//#include "dbgmem.h"


formatCounter::formatCounter(QWidget *parent, struct m_CounterParameters &data)
    : ui(new Ui::EditCounterUI)
{
    ui->setupUi(this);

    escalado=IMAGEN_SINESCALAR;
    offset_x=0;
    offset_y=0;
    m_Counter=data;

    ui->m_Configuracion->clear();
    ui->m_Configuracion->addItem(TXT_CONTADOR_CONFIGURACION_FILAS);
    ui->m_Configuracion->addItem(TXT_CONTADOR_CONFIGURACION_COLUMNAS);

    ui->m_valorInicial->setText(QString("%1").arg(m_Counter.valorInicial));
    ui->m_Incremento->setText(QString("%1").arg(m_Counter.incremento));
    ui->m_limiteInferior->setText(QString("%1").arg(m_Counter.limiteInferior));
    ui->m_limiteSuperior->setText(QString("%1").arg(m_Counter.limiteSuperior));
    ui->m_numCifras->setText(QString("%1").arg(m_Counter.numeroCifras));

    unsigned char configuracion=m_Counter.indexConfiguracion;
    ui->m_Configuracion->setCurrentIndex(configuracion);

    int offset=m_Counter.offset;
    ui->m_Offset->setText(QString("%1").arg(offset));

    if (m_Counter.m_format.zeroPadding) ui->m_zeroPadding->setChecked(true);
    else ui->m_zeroPadding->setChecked(false);

    /*ui->m_decimalSeparator->setText(QString("%1").arg(m_Counter.decimalSeparator));
    ui->m_thounsandSeparator->setText(QString("%1").arg(m_Counter.thousandsSeparator));*/
    if(!m_Counter.m_format.bFormatEnabled) ui->m_cbFormatoSeparador->setCurrentIndex(0);
    else{
        if(m_Counter.m_format.FormatSeparador==QLocale::Spanish) ui->m_cbFormatoSeparador->setCurrentIndex(1);
        else ui->m_cbFormatoSeparador->setCurrentIndex(2);
    }


    connect(ui->m_Aceptar,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_Cancelar,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));

}

formatCounter::~formatCounter(){

}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void formatCounter::AceptarSeleccion(){

    bool counterOK;

    AceptarValoresCampos();

    counterOK=ComprobacionesCounter();

    if (counterOK){
        /*struct m_CounterParameters data;
        data.valorInicial=m_Counter.valorInicial;
        data.limiteInferior=m_Counter.limiteInferior;
        data.limiteSuperior=m_Counter.limiteSuperior;
        data.incremento=m_Counter.incremento;
        data.configuracion=m_Counter.configuracion;
        data.offset=m_Counter.offset;*/

        emit sglCounterConfigured(m_Counter);

        this->deleteLater();
    }

}


void formatCounter::CancelarSeleccion(){

    this->deleteLater();
}



void formatCounter::AceptarValoresCampos(){
	
    ui->m_Aceptar->setEnabled(false);
    ui->m_Cancelar->setEnabled(false);

    int indice;

    indice=ui->m_Configuracion->currentIndex();
    if (indice==-1){
        ui->m_Configuracion->setCurrentIndex(0);
    }

    m_Counter.valorInicial=ui->m_valorInicial->text().toInt();
    m_Counter.limiteInferior=ui->m_limiteInferior->text().toLongLong();
    m_Counter.limiteSuperior=ui->m_limiteSuperior->text().toLongLong();
    m_Counter.incremento=ui->m_Incremento->text().toLongLong();
    m_Counter.configuracion=ui->m_Configuracion->currentText();
    m_Counter.indexConfiguracion=ui->m_Configuracion->currentIndex();
    m_Counter.offset=ui->m_Offset->text().toLongLong();
    m_Counter.valorInicial=ui->m_valorInicial->text().toLongLong();
    m_Counter.numeroCifras=ui->m_numCifras->text().toInt();
    m_Counter.m_format.numeroCifras=ui->m_numCifras->text().toInt();

    if (ui->m_zeroPadding->isChecked()) m_Counter.m_format.zeroPadding=true;
    else m_Counter.m_format.zeroPadding=false;

    /*m_Counter.decimalSeparator=ui->m_decimalSeparator->text();
    m_Counter.thousandsSeparator=ui->m_thounsandSeparator->text();*/
    if (ui->m_cbFormatoSeparador->currentIndex()==0) m_Counter.m_format.bFormatEnabled=false;
    else {
        m_Counter.m_format.bFormatEnabled=true;
        if (ui->m_cbFormatoSeparador->currentIndex()==1) m_Counter.m_format.FormatSeparador=QLocale::Spanish;
        else m_Counter.m_format.FormatSeparador=QLocale::English;
    }
    ui->m_Aceptar->setEnabled(true);
    ui->m_Cancelar->setEnabled(true);

}

bool formatCounter::ComprobacionesCounter(){

    bool bRet=true;
    QString error;

    error.clear();

    if (m_Counter.limiteInferior>=m_Counter.limiteSuperior){
        error.append("ERROR: Límite inferior mayor que límitesuperior\n");
        bRet=false;
    }
    if (m_Counter.valorInicial>m_Counter.limiteSuperior){
        error.append("ERROR: Valor inicial mayor que límite superior\n");
        bRet=false;
    }
    if (m_Counter.valorInicial<m_Counter.limiteInferior){
        error.append("ERROR: Valor inicial menor que límite inferior\n");
        bRet=false;
    }

    if (!bRet) ui->lbl_error->setText(QString("%1").arg(error));
    else ui->lbl_error->setText("--");

    return bRet;

}
