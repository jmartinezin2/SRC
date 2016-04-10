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
#include "in2configuratorapp.h"
#include "in2configuratorprinter.h"
#include "MemoryMap.h"
#include <QSettings>
#include <QFile>
#include "in2configuratordesigner.h"

#define IN2FILE     "In2.ini"
#define IN2LAUNCHER "In2Launchers.ini"
#define IN2RENDER   "In2Render.ini"
#define IN2REPORT   "In2Report.ini"


In2ConfiguratorApp::In2ConfiguratorApp(QWidget *parent) : ui(new Ui::In2ConfigAppUI)
{
    ui->setupUi(this);
    connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(deleteLater()));
    connect(ui->m_pbOk,SIGNAL(clicked()),this,SLOT(deleteLater()));
    connect(ui->m_cbEnableGrid,SIGNAL(toggled(bool)),this,SLOT(sltEnableGrid(bool)));

    sltLoadConfigDesigner();
}

/** Activa el grid o no*/
void In2ConfiguratorApp::setGridEnabled(bool val){
    ui->m_cbEnableGrid->setChecked(val);
}

In2ConfiguratorApp::~In2ConfiguratorApp()
{

}
/** Actualiza el grid*/
void In2ConfiguratorApp::sltEnableGrid(bool enable){
    emit sgnEnableGrid(enable);
}

/** Lee y carga la configuracion del fichero IN2.ini*/
void In2ConfiguratorApp::sltLoadConfigApp(){
}
/** Lee y carga la configuracion del fichero In2Designer.ini*/
void In2ConfiguratorApp::sltLoadConfigDesigner(){
    In2ConfiguratorDesigner config;
    bool  foo=config.saveCampos();
    ui->m_cbSaveCampos->setChecked(foo);
}


/** Genera una configuracion por defecto*/
void In2ConfiguratorApp::sltLoadDefaultConfig(){

}

/** Muestra el progreso*/
void In2ConfiguratorApp::showMsg(QString txt){

}

