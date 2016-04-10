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
#include "mantenimiento.h"
//#include "principal.h"
#include "usuarios.h"
#include "globals.h"
#include "inputdata.h"
#include "in2historico.h"
#include <Qprocess>
#include "in2editorpermisos.h"
#include <QMessageBox>
#include "compileversion.h"
#include "In2Framework.h"
#include "confusers.h"
#include "inkjetdebug.h"

//Pantalla de datos
//static InputData *tmpSCR;

//unsigned char paso_a_principal=0;

//Constructor

//static QWidget *m_parent=NULL;

Configuracion::Configuracion(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::ConfiguracionUI)
{

    ui->setupUi(this);
    m_parent=NULL;
    GlobalSetScreenSize(this);




    QObject::connect(ui->m_PrincipalButton, SIGNAL(clicked()),this, SLOT(LaunchPrincipal()));
    QObject::connect(ui->m_UserButton, SIGNAL(clicked()),this, SLOT(LaunchConfUser()));
    QObject::connect(ui->m_InfoButton, SIGNAL(clicked()),this, SLOT(LaunchEventos()));
    QObject::connect(ui->m_Mantenimiento, SIGNAL(clicked()),this, SLOT(LaunchMantenimiento()));
    QObject::connect(ui->m_pbHistorico, SIGNAL(clicked()),this, SLOT(LaunchHistorico()));
    QObject::connect(ui->m_InkCalculator, SIGNAL(clicked()),this, SLOT(LaunchCalculadoraTinta()));
    QObject::connect(ui->m_pbVersion, SIGNAL(clicked()),this, SLOT(LaunchVersion()));
    QObject::connect(ui->m_pbPermisos, SIGNAL(clicked()),this, SLOT(LaunchPermisos()));

    ui->m_UserButton->setEnabled(true);

    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->m_mapa.m_config.m_LDAPActive) ui->m_UserButton->setEnabled(false);
    //this->ShowInfo();

    TestRoot();

}
/** Establece el parent */
void Configuracion::setIn2Parent(QWidget *p){
    m_parent=p;
}

//Destructor
Configuracion::~Configuracion()
{
}

void Configuracion::TestRoot(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()==ACCESS_MAINTENANCE){
        //ui->m_InfoButton->setEnabled(false);
        //ui->m_Mantenimiento->setEnabled(false);
        ui->m_pbHistorico->setEnabled(true);
        ui->m_UserButton->setEnabled(false);
        ui->m_InfoButton->setEnabled(false);
        ui->m_Mantenimiento->setEnabled(false);
    }
}



//Lanza la pantalla principal
void Configuracion::LaunchPrincipal(){
    //paso_a_principal=1;
    //GlobalShowMainWindow();
    if (m_parent) m_parent->show();
    this->deleteLater();
    //this->ShowInfo();
}

//Lanza la pantalla de configuracion de usuario
void Configuracion::LaunchConfUser(){	
    ConfUsers *p=new ConfUsers(this);
    p->show();
}

//Lanza la pantalla de informacion de eventos
void Configuracion::LaunchEventos(){
    Eventos *ScrEventos;
    ScrEventos=new Eventos(0);
    ScrEventos->show();
}

//Lanza la pantalla de mantenimiento
void Configuracion::LaunchMantenimiento(){
    mantenimiento *Scrmantenimiento;
    Scrmantenimiento=new mantenimiento(0);
    Scrmantenimiento->show();
    //this->deleteLater();
}
// Muestra la version
void Configuracion::LaunchVersion(){
    CompileVersion *p=new CompileVersion(this);    
    p->show();
}
// Muestra los permisos
void Configuracion::LaunchPermisos(){
    In2EditorPermisos *p=new In2EditorPermisos(this);
    InkjetDebug *dbg=InkjetDebug::request(0);
    connect(p,SIGNAL(SendDebugInfo(unsigned char,QString)),dbg,SLOT(ProcessMsg(unsigned char,QString)));
    p->show();
}


//Lanza la calculadora de tinta
void Configuracion::LaunchCalculadoraTinta(){
    QProcess inkCal;
    QString strPath=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(INKCALCULATOR_FILE);
    QString command=QString("%1").arg(strPath);
    inkCal.startDetached(command);
    //inkCal.execute(command);
}

//Lanza la pantalla de Historico
void Configuracion::LaunchHistorico(){
    In2Historico *foo=new In2Historico(this);
    foo->show();
}





