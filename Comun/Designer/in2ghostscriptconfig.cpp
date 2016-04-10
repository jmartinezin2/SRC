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
#include "in2ghostscriptconfig.h"
#include "ui_in2ghostscriptconfig.h"
#include "gsrenderengine.h"
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "cmykconfig.h"

In2GhostScriptConfig::In2GhostScriptConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::In2GhostScriptConfig)
{
    ui->setupUi(this);
    connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(deleteLater()));
    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltSaveData()));

    connect(ui->m_pbIccFile,SIGNAL(clicked()),this,SLOT(sltChangeICCFile()));
    connect(ui->m_pbPath,SIGNAL(clicked()),this,SLOT(sltChangePath()));

    GsRenderEngine gs;
    ui->m_lePath->setText(gs.getExecutable());
    ui->m_leDevice->setText(gs.getDevice());
    ui->m_leExtraOptions->setText(gs.getExtraOptions());
    ui->m_leResolution->setText(gs.getResolution());
    ui->m_leICCFile->setText(gs.getICCFile());

    CMYKConfig cmykConfig;
    int flag=cmykConfig.getQtConversionFlag();
    int calc=cmykConfig.getCalcFormat();
    //Completa los flags
    if (flag&Qt::AutoColor) ui->m_rbColorAuto->setChecked(true);
    else if (flag&Qt::ColorOnly) ui->m_rbColorColor->setChecked(true);
    else if (flag&Qt::MonoOnly) ui->m_rbColorMono->setChecked(true);

    if (flag&Qt::DiffuseDither) ui->m_rbRGBDithDiffuse->setChecked(true);
    else if (flag&Qt::OrderedDither) ui->m_rbRGBDithOrdered->setChecked(true);
    else if (flag&Qt::ThresholdDither) ui->m_rbRGBDithThreshold->setChecked(true);

    if (flag&Qt::DiffuseAlphaDither) ui->m_rbAlphaDithDiffuse->setChecked(true);
    else if (flag&Qt::OrderedAlphaDither) ui->m_rbAlphaDithOrdered->setChecked(true);
    else if (flag&Qt::ThresholdAlphaDither) ui->m_rbAlphaDithThreshold->setChecked(true);

    if (flag&Qt::PreferDither) ui->m_rbColorMatchPreferDith->setChecked(true);
    else if (flag&Qt::AvoidDither) ui->m_rbColorMatchAvoidDith->setChecked(true);
    else if (flag&Qt::NoOpaqueDetection) ui->m_rbColorMatchNoDetect->setChecked(true);

    if (calc==(int)QImage::Format_Indexed8) ui->m_rb8Bit->setChecked(true);
    else ui->m_rb32Bit->setChecked(true);

    (cmykConfig.getAlgMejoraBlancos())? ui->m_cbAlgMejoraBlancos->setChecked(true):ui->m_cbAlgMejoraBlancos->setChecked(false);

    int sabreAngle=cmykConfig.getSabreAngle();
    if (sabreAngle==0) ui->m_rbM0->setChecked(true);
    if (sabreAngle==1) ui->m_rbM1->setChecked(true);
    if (sabreAngle==2) ui->m_rbM2->setChecked(true);
    if (sabreAngle==3) ui->m_rbM3->setChecked(true);
    if (sabreAngle==4) ui->m_rbM4->setChecked(true);
}



In2GhostScriptConfig::~In2GhostScriptConfig()
{

    this->deleteLater();
}

/** Guarda los datos*/
void In2GhostScriptConfig::sltSaveData(){

    //Guardamos todos los datos


    QString strExecutable=ui->m_lePath->text();

    QString strDevice=ui->m_leDevice->text();

    QString strResolution=ui->m_leResolution->text();

    QString strExtraOptions=ui->m_leExtraOptions->text();

    QString strICCFile=ui->m_leICCFile->text();

    int flag=0;
    if (ui->m_rbColorAuto->isChecked()) flag|=Qt::AutoColor;
    else if  (ui->m_rbColorColor->isChecked()) flag|=Qt::ColorOnly;
    else if  (ui->m_rbColorMono->isChecked()) (flag|=Qt::MonoOnly);

    if  (ui->m_rbRGBDithDiffuse->isChecked()) (flag|=Qt::DiffuseDither);
    else if  (ui->m_rbRGBDithOrdered->isChecked()) (flag|=Qt::OrderedDither);
    else if  (ui->m_rbRGBDithThreshold->isChecked()) (flag|=Qt::ThresholdDither);

    if  (ui->m_rbAlphaDithDiffuse->isChecked()) (flag|=Qt::DiffuseAlphaDither);
    else if  (ui->m_rbAlphaDithOrdered->isChecked()) (flag|=Qt::OrderedAlphaDither);
    else if  (ui->m_rbAlphaDithThreshold->isChecked()) (flag|=Qt::ThresholdAlphaDither);

    if  (ui->m_rbColorMatchPreferDith->isChecked()) (flag|=Qt::PreferDither);
    else if  (ui->m_rbColorMatchAvoidDith->isChecked()) (flag|=Qt::AvoidDither);
    else if  (ui->m_rbColorMatchNoDetect->isChecked()) (flag|=Qt::NoOpaqueDetection);

    int calc=(int)QImage::Format_Indexed8;
    if (ui->m_rb8Bit->isChecked()) calc=QImage::Format_Indexed8;
    else calc=QImage::Format_RGB32;

    bool algMejoraBlancos=false;
    (ui->m_cbAlgMejoraBlancos->isChecked())?algMejoraBlancos=true:algMejoraBlancos=false;

    int sabreAngle=0;
    if (ui->m_rbM0->isChecked()) sabreAngle=0;
    if (ui->m_rbM1->isChecked()) sabreAngle=1;
    if (ui->m_rbM2->isChecked()) sabreAngle=2;
    if (ui->m_rbM3->isChecked()) sabreAngle=3;
    if (ui->m_rbM4->isChecked()) sabreAngle=4;

    CMYKConfig cmykConfig;
    cmykConfig.setCalcFormat(calc);
    cmykConfig.setQtConversionFlag(flag);
    cmykConfig.setAlgMejoraBlancos(algMejoraBlancos);
    cmykConfig.setSabreAngle(sabreAngle);
    cmykConfig.saveConfigFile();

    //Habria que hacer lo mismo
    GsRenderEngine gs;
    gs.setDevice(strDevice);
    gs.setResolution(strResolution);
    gs.setICCFile(strICCFile);
    gs.setExtraOptions(strExtraOptions);
    gs.setExecutable(strExecutable);
    gs.saveConfigFile();


    QMessageBox::warning(this,"INFO",tr("Cambios guardados"),QMessageBox::Ok);

    this->deleteLater();
}

/** Cambia el ICC file*/
void In2GhostScriptConfig::sltChangeICCFile(){
    QFileDialog fileName(this);
    QStringList Ficheros;

    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("ICC files (*.icc)"));
#else
    fileName.setFilter(tr("ICC files (*.icc)"));
#endif

//    fileName.setFilter(tr("ICC files (*.icc)"));
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
            ui->m_leICCFile->setText(Ficheros.at(0));
        }
    }
}
/** Cambia el path*/
void In2GhostScriptConfig::sltChangePath(){
    QFileDialog fileName(this);
    QStringList Ficheros;

    fileName.setFileMode(QFileDialog::ExistingFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("exe files (*.exe)"));
#else
    fileName.setFilter(tr("exe files (*.exe)"));
#endif

//    fileName.setFilter(tr("exe files (*.exe)"));
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();
        if (!Ficheros.isEmpty()){
            ui->m_lePath->setText(Ficheros.at(0));
        }
    }

}

