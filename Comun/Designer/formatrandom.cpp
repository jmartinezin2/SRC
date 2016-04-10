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

//#include "dbformat.h"
#include "formatrandom.h"
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
#include "in2random.h"

//#include "dbgmem.h"


formatRandom::formatRandom(QWidget *parent, struct m_RandomNumberParameters &data)
    : ui(new Ui::EditRandomUI)
{
    ui->setupUi(this);

    m_Random=data;

    ui->m_limiteInferior->setText(QString("%1").arg(m_Random.limiteInferior));
    ui->m_limiteSuperior->setText(QString("%1").arg(m_Random.limiteSuperior));
    ui->m_numCifras->setText(QString("%1").arg(m_Random.numeroCifras));

    AceptarValoresCampos();

    connect(ui->m_Aceptar,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));
    connect(ui->m_Cancelar,SIGNAL(clicked()),this,SLOT(CancelarSeleccion()));
    connect(ui->m_Aplicar,SIGNAL(clicked()),this,SLOT(AceptarValoresCampos()));


}

formatRandom::~formatRandom(){

}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void formatRandom::AceptarSeleccion(){

    AceptarValoresCampos();
    struct m_RandomNumberParameters params=m_Random;
    emit this->sglRandomConfigured(params);
    this->deleteLater();


}


void formatRandom::CancelarSeleccion(){

    this->deleteLater();
}



void formatRandom::AceptarValoresCampos(){
	
    QList<int> numerosGenerados;
    numerosGenerados.clear();
    QString sLista;

    ui->m_Aceptar->setEnabled(false);
    ui->m_Cancelar->setEnabled(false);

    ui->m_Generated->clear();

    m_Random.limiteInferior=ui->m_limiteInferior->text().toInt();
    m_Random.limiteSuperior=ui->m_limiteSuperior->text().toInt();
    m_Random.numeroCifras=ui->m_numCifras->text().toInt();

    ComprobacionesCounter();

    //Generamos una lista de numeros aleatorios
    In2Random in2Ran;
    for(int i=0;i<100;i++){
        m_Random.valorActual=in2Ran.generarNumeroAleatorio(m_Random.limiteInferior,m_Random.limiteSuperior);
        numerosGenerados.append(m_Random.valorActual);
    }

    //Ordenamosla lista
    //qSort(numerosGenerados.begin(),numerosGenerados.end());

    //Los mostramos
    sLista.clear();
    for(int i=0;i<numerosGenerados.size();i++){
        m_Random.valorActual=numerosGenerados.at(i);
        sLista.append(QString("%1").arg(m_Random.valorActual).rightJustified(m_Random.numeroCifras,'0',true));
        if ((i%4)==3) sLista.append("\n");
        else sLista.append(", ");
    }

    ui->m_Generated->setText(sLista);

    //ui->m_Generated->setText(QString("%1").arg(m_Random.valorActual).rightJustified(m_Random.numeroCifras,'0',true));


    ui->m_Aceptar->setEnabled(true);
    ui->m_Cancelar->setEnabled(true);

}

void formatRandom::ComprobacionesCounter(){

    int dummyInf,dummySup;

    if (m_Random.limiteInferior>=m_Random.limiteSuperior){

        dummyInf=m_Random.limiteInferior;
        dummySup=m_Random.limiteSuperior;

        m_Random.limiteSuperior=dummyInf;
        m_Random.limiteInferior=dummySup;

        ui->m_limiteInferior->setText(QString("%1").arg(m_Random.limiteInferior));
        ui->m_limiteSuperior->setText(QString("%1").arg(m_Random.limiteSuperior));

    }

}
