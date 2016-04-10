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
#include "in2configcorte.h"
#include "in2gestorcorte.h"
#include "clienteopc.h"
#include <QTimer>


#define MIN_FUERZA 0
#define MAX_FUERZA 100

In2ConfigCorte::In2ConfigCorte(QWidget *parent) :ui(new Ui::In2ConfigCorteUI)
{
    ui->setupUi(this);

    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    connect(ui->m_pbChangeFuerza,SIGNAL(clicked(bool)),this,SLOT(sltChangeFuerza()));
    connect(ui->m_pbExit,SIGNAL(clicked(bool)),this,SLOT(sltExit()));
    connect(ui->m_pbActivate,SIGNAL(clicked(bool)),this,SLOT(sltActivate()));
    QTimer::singleShot(0,this,SLOT(sltUpdateData()));
}


/** Actualiza las referencias*/
void In2ConfigCorte::sltUpdateData(){
    In2GestorCorte *p=In2GestorCorte::request(0);
    if (p->isEnabled()){
        ui->m_lblEnabled->setEnabled(true);
        int f=p->getFuerza();
        ui->m_leCurrentFuerza->setText(QString("%1").arg(f));
        ui->m_lblOn->setEnabled(p->isActive());

    }
    else{
        ui->m_lblEnabled->setEnabled(false);
        ui->m_pbActivate->setEnabled(false);
        ui->m_pbChangeFuerza->setEnabled(false);
        ui->m_leSetFuerza->setEnabled(false);
        ui->m_lblOn->setEnabled(false);
    }
}

/** Activa el corte*/
void In2ConfigCorte::sltActivate(){
    In2GestorCorte *p=In2GestorCorte::request(0);
    if (p->isEnabled()){
        p->sltToogleCorte();
        //ui->m_lblOn->setEnabled(val);
        QTimer::singleShot(0,this,SLOT(sltUpdateData()));
    }
}
/** Cambia la fuerza de corte*/
void In2ConfigCorte::sltChangeFuerza(){
    In2GestorCorte *p=In2GestorCorte::request(0);
    if (p->isEnabled()){
        int f=ui->m_leSetFuerza->text().toInt();
        if ((f>=MIN_FUERZA)&&(f<=MAX_FUERZA)){
            p->setFuerza(f);
            QTimer::singleShot(0,this,SLOT(sltUpdateData()));
        }
    }
}
/** Sale de la aplicacion*/
void In2ConfigCorte::sltExit(){
    this->deleteLater();
}
