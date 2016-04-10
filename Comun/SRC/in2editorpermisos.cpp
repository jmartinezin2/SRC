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
#include "in2editorpermisos.h"
#include "In2GestorDB.h"
#include "in2defines.h"
#include "MemoryMap.h"
#include <QTimer>
#include <QMessageBox>

#define STORE_PERMISOS  "In2Permisos.sqlite"

In2EditorPermisos::In2EditorPermisos(QWidget *parent) : ui(new Ui::In2PermisosUI)
{
    ui->setupUi(this);
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    connect(ui->m_cbTabla,SIGNAL(currentIndexChanged(QString)),this,SLOT(sltLoadTable(QString)));
    connect(ui->m_pbExit,SIGNAL(clicked(bool)),this,SLOT(sltExit()));
    connect(ui->m_pbSave,SIGNAL(clicked(bool)),this,SLOT(sltSave()));
    ui->m_pbGuardado->setValue(0);

#if QT_VERSION>=0x050000
     ui->m_twTabla->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );

#else
    ui->m_twTabla->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
#endif
    QString foo=ui->m_cbTabla->currentText();
    if (!foo.isEmpty()){
        sltLoadTable(foo);
    }

}
/** Salimos*/
void In2EditorPermisos::sltExit(){
    ui->m_twTabla->clear();
    this->deleteLater();
}

/** Guarda los cambios*/
void In2EditorPermisos::sltSave(){
    int rtn=QMessageBox::warning(this,"INFO",tr("Desea guardar cambios?"),QMessageBox::Ok,QMessageBox::Cancel);
    if (rtn==QMessageBox::Ok){
        In2GestorDB db;

        QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_PERMISOS);
        QString table=ui->m_cbTabla->currentText();
        bool ok=false;
        if (db.open(QString("QSQLITE"),QString("In2Permisos"),file)){
            ok=true;
            //Para cada linea hacemos un insert
            ui->m_pbGuardado->setMaximum(ui->m_twTabla->rowCount());
            for (int x=0;x<ui->m_twTabla->rowCount();x++){
                ui->m_pbGuardado->setValue(x);
                QTableWidgetItem *p=ui->m_twTabla->item(x,0);
                QString item=p->text();
                int iPermiso=(1<<(ACCESS_GOD-1));
                p=ui->m_twTabla->item(x,2);
                if (p->checkState()==Qt::Checked) iPermiso|=(1<<(ACCESS_MAINTENANCE-1));
                p=ui->m_twTabla->item(x,3);
                if (p->checkState()==Qt::Checked) iPermiso|=(1<<(ACCESS_ROOT-1));
                p=ui->m_twTabla->item(x,4);
                if (p->checkState()==Qt::Checked) iPermiso|=(1<<(ACCESS_PRINTER-1));
                QString foo=QString("update %1 set Permiso='%2' where Item='%3'").arg(table).arg(QString("%1").arg(iPermiso)).arg(item);
                if (db.exec(foo)){
                    emit SendDebugInfo(LOGGER_STORE,QString("Cambio permiso [%1].%2=%3->%4").arg(table).arg(item).arg(QString("%1").arg(m_permisos.at(x))).arg(QString("%1").arg(iPermiso)));
                }
                else{
                   QMessageBox::warning(this,"ERROR",foo,QMessageBox::Ok);
                   ok=false;
                }
            }
        }
        if (ok) QMessageBox::information(0,"INFO",tr("Cambios guardados"),QMessageBox::Ok);
        else QMessageBox::information(0,"ERROR",tr("No se pudo guardar todos los registros"),QMessageBox::Ok);
    }
    ui->m_pbGuardado->setValue(0);

}
/** Carga una nueva tabla*/
void In2EditorPermisos::sltLoadTable(QString table){
    In2GestorDB db;
    bool rtn=false;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(STORE_PERMISOS);
    if (db.open(QString("QSQLITE"),QString("In2Permisos"),file)){
        QString foo=QString("select * from %1").arg(table);
        rtn=db.exec(foo);
        ui->m_twTabla->clearContents();
        ui->m_twTabla->setRowCount(0);
        if (rtn){
            m_permisos=db.m_Tabla;
            int total=m_permisos.count();
            if ((total)&&((total%2)==0)){
                ui->m_twTabla->setRowCount(total/2);
                int x=0;
                while (x<total){
                    QTableWidgetItem *p1=new QTableWidgetItem(m_permisos.at(x));
                    ui->m_twTabla->setItem(x/2,0,p1);
                    QTableWidgetItem *p2=new QTableWidgetItem();
                    p2->setCheckState(Qt::Checked);
                    Qt::ItemFlags flags=p2->flags();
                    flags&=~(Qt::ItemIsEnabled);
                    p2->setFlags(flags);
                    ui->m_twTabla->setItem(x/2,1,p2);
                    int permiso=m_permisos.at(x+1).toInt();
                    if (permiso&(1<<(ACCESS_MAINTENANCE-1))){
                        QTableWidgetItem *access=new QTableWidgetItem();
                        access->setCheckState(Qt::Checked);
                        ui->m_twTabla->setItem(x/2,2,access);
                    }
                    else{
                        QTableWidgetItem *access=new QTableWidgetItem();
                        access->setCheckState(Qt::Unchecked);
                        ui->m_twTabla->setItem(x/2,2,access);
                    }

                    if (permiso&(1<<(ACCESS_ROOT-1))) {
                        QTableWidgetItem *access=new QTableWidgetItem();
                        access->setCheckState(Qt::Checked);
                        ui->m_twTabla->setItem(x/2,3,access);
                    }
                    else{
                        QTableWidgetItem *access=new QTableWidgetItem();
                        access->setCheckState(Qt::Unchecked);
                        ui->m_twTabla->setItem(x/2,3,access);
                    }
                    if (permiso&(1<<(ACCESS_PRINTER-1))){
                        QTableWidgetItem *access=new QTableWidgetItem();
                        access->setCheckState(Qt::Checked);
                        ui->m_twTabla->setItem(x/2,4,access);

                    }
                    else{
                        QTableWidgetItem *access=new QTableWidgetItem();
                        access->setCheckState(Qt::Unchecked);
                        ui->m_twTabla->setItem(x/2,4,access);
                    }

                    x+=2;

                }
            }
        }
    }
}
