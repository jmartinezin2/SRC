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

#include "principal.h"
#include "ui_mainwindow.h"
#include "Composicion.h"
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include "in2configuratorprinter.h"
#include "in2debugcleaner.h"
#include "inkjetdebug.h"
#include "in2gsmonitor.h"
#include <QMessageBox>



/*Principal::Principal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{*/
Principal::Principal(QObject *parent)
{
    //ui->setupUi(this);
    m_memoryMap=MemoryMap::request(this);
    m_memoryMap->Init();
    //AbrirFicheroIniImpresora();
    In2ConfiguratorPrinter configPrinter;
    configPrinter.readConfig();
    QString path;
    path.append(QApplication::applicationDirPath());
    path.append(QString("\\Database\\log.sqlite"));

    m_memoryMap->setUser("Admin");
    m_memoryMap->setUserRights(ACCESS_GOD);
    //GlobalSetMainWindow(this);
    //this->setVisible(false);    

    Composicion *ScrComposicion;
    ScrComposicion=new Composicion(0);
    ScrComposicion->show();
    connect(ScrComposicion,SIGNAL(finishDesign()),this,SLOT(deleteLater()));
}

/** Recibe la info*/
void Principal::ReceiveDebugInfo(unsigned char codigo, QString mensaje){

   // m_InfoDebug->ProcessMsg(codigo,mensaje);

}


Principal::~Principal()
{
    //miDataBaseLog.close("ConexionLog");
}
