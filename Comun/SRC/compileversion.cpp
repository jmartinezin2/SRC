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
#include "compileversion.h"
#include "ui_compileversion.h"
#include "In2Framework.h"
#include <QFile>
//#include "configuracion.h"

#define FILE_VERSION "Gitlog.txt"
CompileVersion::CompileVersion(QWidget *parent) :
    ui(new Ui::CompileVersion)
{
    ui->setupUi(this);
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    this->setWindowOpacity(0.75);
    QString file=QString("%1\\config\\%2").arg(QApplication::applicationDirPath()).arg(FILE_VERSION);
    QFile dummyFile(file);
    if (dummyFile.open(QIODevice::ReadOnly)){
        QByteArray readed=dummyFile.readAll();
        ui->m_text->setText(QString(readed));
        ui->m_text->update();
        dummyFile.close();
    }
    else{
        ui->m_text->setText("NO changes log provided");
    }

    setCompileVersion(IN2_RELEASE_VERSION);
    connect(ui->m_pbExit,SIGNAL(clicked()),this,SLOT(Exit()));
}
/** cAMBIAMOS LA VERSION DE COMPILACION*/
void CompileVersion::setCompileVersion(QString txt){
    ui->m_leVersion->setText(txt);
}

/** Salimos*/
void CompileVersion::Exit(){
    /*Configuracion *p=new Configuracion(this);
    p->show();*/
    this->deleteLater();
}

CompileVersion::~CompileVersion()
{
    delete ui;
}
