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
#include "in2ditheralg.h"
#include <QMessageBox>

In2DitherAlg::In2DitherAlg(QWidget *parent,struct m_Colores & color) : ui(new Ui::In2DitherAlg)
{
    ui->setupUi(this);
    m_color=color;
    int cut=m_color.m_diherCut;
    //ui->m_rbIn2Optimum->setC
    if (cut==CUT_LINEAL) ui->m_rbLineal->setChecked(true);
    else if (cut==CUT_MEDIAN) ui->m_rbMedianCut->setChecked(true);
    else ui->m_rbIn2Optimum->setChecked(true);

    ui->m_cbCustom->setChecked(color.m_customPal);    
    ui->m_cbDinamicCorr->setChecked(m_color.bCorrDinamica);

    sltUpdatePalette();

    connect(ui->m_pbOk,SIGNAL(clicked(bool)),this,SLOT(sltSaveExit()));
    connect(ui->m_pbCancel,SIGNAL(clicked(bool)),this,SLOT(sltCancelExit()));
    connect(ui->m_pbReload,SIGNAL(clicked(bool)),this,SLOT(sltReload()));
}

/** Recarga la info de color*/
void In2DitherAlg::sltUpdatePalette(){
    QStringList colors=m_color.m_ditherPal.split("@");
    if (colors.count()==4){
        unsigned int c1=colors.at(0).toUInt();
        unsigned int c2=colors.at(1).toUInt();
        unsigned int c3=colors.at(2).toUInt();
        unsigned int c4=colors.at(3).toUInt();
        ui->m_lePal1->setText(QString::number(c1,16));
        ui->m_lePal2->setText(QString::number(c2,16));
        ui->m_lePal3->setText(QString::number(c3,16));
        ui->m_lePal4->setText(QString::number(c4,16));
    }
}

/** Reload la palette*/
void In2DitherAlg::sltReloadPalette(){
    int index=m_color.index;
    MemoryMap *memoryMap=MemoryMap::request(0);
    m_color=memoryMap->m_mapa.m_colores[index];
    sltUpdatePalette();
    this->setEnabled(true);
}

/** Recarga la imagen*/
void In2DitherAlg::sltReload(){
    struct m_Colores color=m_color;
    int cut=CUT_IN2;
    if (ui->m_rbLineal->isChecked()) cut=CUT_LINEAL;
    else if (ui->m_rbMedianCut->isChecked()) cut=CUT_MEDIAN;
    color.m_diherCut=cut;
    color.bCorrDinamica=ui->m_cbDinamicCorr->isChecked();
    //Falta la paleta
    if (ui->m_cbCustom->isChecked()){
        bool ok;
        unsigned int c1=ui->m_lePal1->text().toUInt(&ok,16);
        if (!ok){
            QMessageBox::warning(this,"Error",tr("Color 1 incorrecto"),QMessageBox::Ok);
            return;
        }
        unsigned int c2=ui->m_lePal2->text().toUInt(&ok,16);
        if (!ok){
            QMessageBox::warning(this,"Error",tr("Color 2 incorrecto"),QMessageBox::Ok);
            return;
        }
        unsigned int c3=ui->m_lePal3->text().toUInt(&ok,16);
        if (!ok){
            QMessageBox::warning(this,"Error",tr("Color 3 incorrecto"),QMessageBox::Ok);
            return;
        }
        unsigned int c4=ui->m_lePal4->text().toUInt(&ok,16);
        if (!ok){
            QMessageBox::warning(this,"Error",tr("Color 4 incorrecto"),QMessageBox::Ok);
            return;
        }
        QString palette=QString("%1@%2@%3@%4").arg(QString::number(c1)).arg(QString::number(c2)).arg(QString::number(c3)).arg(QString::number(c4));
        color.m_ditherPal=palette;
        color.m_customPal=true;
    }
    else color.m_customPal=false;
    this->setEnabled(false);
    emit sgnReload(color);
}

/** Guarda cambios y sale*/
void In2DitherAlg::sltSaveExit(){
    sltReload();
    this->deleteLater();
}

/** Guarda cambios y sale*/
void In2DitherAlg::sltCancelExit(){
    emit sgnReload(m_color);
    this->setEnabled(false);
    this->deleteLater();
}

