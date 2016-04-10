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
#include "in2configlampara.h"
#include "in2defines.h"
#include <QTimer>
#include "In2GestorDB.h"
#include "in2configuratorlampara.h"
#include <QMessageBox>
#include "in2gestorlampara.h"
#include <QButtonGroup>


In2ConfigLampara::In2ConfigLampara(QWidget *parent) : ui(new Ui::In2ConfigLamparaUI)
{
    ui->setupUi(this);

    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    In2ConfiguratorLampara config;
    ui->m_leVendor->setText(config.getStrVendor());


    connect(ui->m_cbIntensidad,SIGNAL(currentIndexChanged(QString)),this,SLOT(sltLoadTablaIntensidad(QString)));
    connect(ui->m_cbPosicion,SIGNAL(currentIndexChanged(QString)),this,SLOT(sltLoadTablaPosicion(QString)));
    connect(ui->m_pbSaveIntesidad,SIGNAL(clicked(bool)),this,SLOT(sltSaveTablaIntensidad()));
    connect(ui->m_pbSavePosicion,SIGNAL(clicked(bool)),this,SLOT(sltSaveTablaPosicion()));
    connect(ui->m_pbAddRowIntensidad,SIGNAL(clicked(bool)),this,SLOT(sltAddRowIntensidad()));
    connect(ui->m_pbAddRowPos,SIGNAL(clicked(bool)),this,SLOT(sltAddRowPos()));
    connect(ui->m_pbDeleteTableIntensidad,SIGNAL(clicked(bool)),this,SLOT(sltDeleteTableIntensidad()));
    connect(ui->m_pbDeleteTablePosicion,SIGNAL(clicked(bool)),this,SLOT(sltDeleteTablePosicion()));
    //Desplazador
    connect(ui->m_pbJogMenos,SIGNAL(pressed()),this,SLOT(sltJogMenosOn()));
    connect(ui->m_pbJogMas,SIGNAL(pressed()),this,SLOT(sltJogMasOn()));
    connect(ui->m_pbJogMenos,SIGNAL(released()),this,SLOT(sltJogMenosOff()));
    connect(ui->m_pbJogMas,SIGNAL(released()),this,SLOT(sltJogMasOff()));
    connect(ui->m_pbGoto,SIGNAL(clicked(bool)),this,SLOT(sltGoto()));
    connect(ui->m_pbHome,SIGNAL(clicked(bool)),this,SLOT(sltResetPos()));
    connect(ui->m_pbReset,SIGNAL(clicked(bool)),this,SLOT(sltResetLampara()));
    connect(ui->m_pbExit,SIGNAL(clicked(bool)),this,SLOT(sltExit()));
    connect(ui->m_pbSetUpLampara,SIGNAL(clicked(bool)),this,SLOT(sltSetUpLampara()));

    ui->m_twIntensidad->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_twPosicion->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QButtonGroup *group=new QButtonGroup(0);
    connect(this,SIGNAL(destroyed(QObject*)),group,SLOT(deleteLater()));
    group->addButton(ui->m_pbGoto0);
    group->addButton(ui->m_pbGoto1);
    group->addButton(ui->m_pbGoto2);
    group->addButton(ui->m_pbGoto3);
    group->addButton(ui->m_pbGoto4);
    group->addButton(ui->m_pbGoto5);
    group->addButton(ui->m_pbGoto6);
    group->addButton(ui->m_pbGoto7);
    group->addButton(ui->m_pbGoto8);
    group->addButton(ui->m_pbGoto9);
    group->addButton(ui->m_pbGoto10);
    group->addButton(ui->m_pbGoto11);
    group->addButton(ui->m_pbGoto12);
    group->addButton(ui->m_pbGoto13);
    group->addButton(ui->m_pbGoto14);
    group->addButton(ui->m_pbGoto15);

    connect(group,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(sltGotoButton(QAbstractButton*)));




    QTimer::singleShot(0,this,SLOT(sltUpdatePos()));
    QTimer::singleShot(0,this,SLOT(sltLoadDataIntensidad()));
    QTimer::singleShot(0,this,SLOT(sltLoadDataPosicion()));
}

/** Hace el setup de la l�mpara*/
void In2ConfigLampara::sltSetUpLampara(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    p->init();
}

/** Movemos en funcion del boton pulsado*/
void In2ConfigLampara::sltGotoButton(QAbstractButton *p) {
    if (p){
        bool ok;
        int val=p->text().toInt(&ok,10);
        In2GestorLampara *p=In2GestorLampara::request(0);
        if (ok){
            if ((val>=LAMPARA_MINPOS)&&(val<LAMPARA_MAXPOS)){
                p->setPos(val);
            }
        }
    }
}

/** Salimos*/
void In2ConfigLampara::sltExit(){
    ui->m_twIntensidad->clear();
    ui->m_twPosicion->clear();
    this->deleteLater();
}

/** Hace Jog-Mas de la lampara*/
void In2ConfigLampara::sltJogMasOn(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    p->JogMasOn();
}
/** Hace Jog-Menos de la lampara*/
void In2ConfigLampara::sltJogMenosOn(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    p->JogMenosOn();
}
/** Hace Jog-Mas-Off de la lampara*/
void In2ConfigLampara::sltJogMasOff(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    p->JogMasOff();
}
/** Hace Jog-Menos-Off de la lampara*/
void In2ConfigLampara::sltJogMenosOff(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    p->JogMenosOff();
}

/** Actualiza la posicion*/
void In2ConfigLampara::sltUpdatePos(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    int val=p->getPos();
    ui->m_leCurrentPos->setText(QString("%1").arg(val));
}
/** Manda la lampara a una nueva posicion*/
void In2ConfigLampara::sltGoto(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    bool ok;
    int val=ui->m_leSetPoint->text().toInt(&ok,10);
    if (ok){
        if ((val>=LAMPARA_MINPOS)&&(val<LAMPARA_MAXPOS)){
            p->setPos(val);
        }
    }
}
/** Resetea la posicion de la lampara*/
void In2ConfigLampara::sltResetPos(){
    In2GestorLampara *p=In2GestorLampara::request(0);
    p->init();
}



/** Elimina una tabla de intensidad*/
void In2ConfigLampara::sltDeleteTableIntensidad(){
    QString table=ui->m_cbIntensidad->currentText();
    if (!table.compare(DEFAULT_INTENSIDAD_TABLE,Qt::CaseInsensitive)){
        QMessageBox::warning(0,"Error","No se permite borrar la tabla por defecto",QMessageBox::Ok);
        return;
    }
    if (!table.isEmpty()){
        int rtn=QMessageBox::question(this,"INFO",tr("Desea eliminar la tabla %1?").arg(table),QMessageBox::Ok,QMessageBox::Cancel);
        if (rtn==QMessageBox::Ok){
            In2GestorDB db;
            QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
            if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
                QString message=QString("drop table if exists '%1'").arg(table);
                if (!db.exec(message)){
                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                    db.close();
                    return;
                }
                else{
                    message=QString("delete from Referencias where Tabla='%1'").arg(table);
                    if (!db.exec(message)){
                        sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                        db.close();
                        return;
                    }

                    sltInfo(LOGGER_STORE,tr("Eliminada tabla %1").arg(table));
                    sltLoadDataIntensidad();
                }
                db.close();
            }
            else{
                sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
            }
        }
    }
}
/** Elimina una tabla de posicion*/
void In2ConfigLampara::sltDeleteTablePosicion(){
    QString table=ui->m_cbPosicion->currentText();
    if (!table.compare(DEFAULT_POSICION_TABLE,Qt::CaseInsensitive)){
        QMessageBox::warning(0,"Error","No se permite borrar la tabla por defecto",QMessageBox::Ok);
        return;
    }
    if (!table.isEmpty()){
        int rtn=QMessageBox::question(this,"INFO",tr("Desea eliminar la tabla %1?").arg(table),QMessageBox::Ok,QMessageBox::Cancel);
        if (rtn==QMessageBox::Ok){
            In2GestorDB db;
            QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
            if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
                QString message=QString("drop table if exists '%1'").arg(table);
                if (!db.exec(message)){
                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                    db.close();
                    return;
                }
                else{
                    message=QString("delete from Referencias where Tabla='%1'").arg(table);
                    if (!db.exec(message)){
                        sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                        db.close();
                        return;
                    }

                    sltInfo(LOGGER_STORE,tr("Eliminada tabla %1").arg(table));
                    sltLoadDataPosicion();
                }
                db.close();
            }
            else{
                sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
            }
        }
    }
}


/** Añade una row a la intensidad*/
void In2ConfigLampara::sltAddRowIntensidad(){
    int count=ui->m_twIntensidad->rowCount();
    ui->m_twIntensidad->insertRow(count);
    ui->m_twIntensidad->setItem(count,0,new QTableWidgetItem);
    ui->m_twIntensidad->setItem(count,1,new QTableWidgetItem);
}
/** Añade una row a la posicion*/
void In2ConfigLampara::sltAddRowPos(){
    int count=ui->m_twPosicion->rowCount();
    ui->m_twPosicion->insertRow(count);
    ui->m_twPosicion->setItem(count,0,new QTableWidgetItem);
    ui->m_twPosicion->setItem(count,1,new QTableWidgetItem);
}

/** Guardar tabla posicion*/
void In2ConfigLampara::sltSaveTablaPosicion(){
    QString table=ui->m_lePosicion->text();
    if (!table.isEmpty()){
        int rtn=QMessageBox::question(this,"INFO",tr("Desea guardar la tabla %1?").arg(table),QMessageBox::Ok,QMessageBox::Cancel);
        if (rtn==QMessageBox::Ok){
            In2GestorDB db;
            QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
            if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
                QString message=QString("select * from Referencias where Tabla='%1'").arg(table);
                if (!db.exec(message)){
                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                    db.close();
                    return;
                }
                if (!db.m_Tabla.count()){ //Si no existe la meto en Referencias
                    message=QString("insert into 'Referencias' (Tabla,Tipo) values ('%1','POSICION')").arg(table);
                    if (!db.exec(message)){
                        sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                        db.close();
                        return;
                    }
                }
                else{ // Si existe, elimino la tabla pero mantengo la de referencias
                    message=QString("drop table if exists '%1'").arg(table);
                    if (!db.exec(message)){
                        sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                        db.close();
                        return;
                    }
                }
                message=QString("create table '%1' ('Velocidad','Posicion')").arg(table);
                if (!db.exec(message)){
                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                }

                else{
                    for (int x=0;x<ui->m_twPosicion->rowCount();x++){
                        QTableWidgetItem *vel=ui->m_twPosicion->item(x,0);
                        QTableWidgetItem *ints=ui->m_twPosicion->item(x,1);
                        if ((vel)&&(ints)){
                            if ((!vel->text().isEmpty())&&(!ints->text().isEmpty())){
                                message=QString("insert into '%1' (Velocidad,Posicion) values ('%2','%3')").arg(table).arg(vel->text()).arg(ints->text());
                                if (!db.exec(message)){
                                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                                }
                            }
                        }
                    }
                }
                sltLoadDataPosicion();
                sltLoadTablaPosicion(table);
                db.close();
                sltInfo(LOGGER_STORE,tr("Tabla %1 guardada").arg(table));
            }
            else{
                sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
            }
        }
    }
}
/** Guardar tabla intensidad*/
void In2ConfigLampara::sltSaveTablaIntensidad(){
    QString table=ui->m_leIntensidad->text();
    if (!table.isEmpty()){
        int rtn=QMessageBox::question(this,"INFO",tr("Desea guardar la tabla %1?").arg(table),QMessageBox::Ok,QMessageBox::Cancel);
        if (rtn==QMessageBox::Ok){
            In2GestorDB db;
            QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
            if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
                QString message=QString("select * from Referencias where Tabla='%1'").arg(table);
                if (!db.exec(message)){
                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                    db.close();
                    return;
                }
                if (!db.m_Tabla.count()){ //Si no existe la meto en Referencias
                    message=QString("insert into 'Referencias' (Tabla,Tipo) values ('%1','INTENSIDAD')").arg(table);
                    if (!db.exec(message)){
                        sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                        db.close();
                        return;
                    }
                }
                else{ // Si existe, elimino la tabla pero mantengo la de referencias
                    message=QString("drop table if exists '%1'").arg(table);
                    if (!db.exec(message)){
                        sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                        db.close();
                        return;
                    }
                }
                message=QString("create table '%1' ('Velocidad','Intensidad')").arg(table);
                if (!db.exec(message)){
                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                }

                else{
                    for (int x=0;x<ui->m_twIntensidad->rowCount();x++){
                        QTableWidgetItem *vel=ui->m_twIntensidad->item(x,0);
                        QTableWidgetItem *ints=ui->m_twIntensidad->item(x,1);
                        if ((vel)&&(ints)){
                            if ((!vel->text().isEmpty())&&(!ints->text().isEmpty())){
                                message=QString("insert into '%1' (Velocidad,Intensidad) values ('%2','%3')").arg(table).arg(vel->text()).arg(ints->text());
                                if (!db.exec(message)){
                                    sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
                                }
                            }
                        }
                    }
                }
                sltLoadDataIntensidad();
                sltLoadTablaIntensidad(table);
                db.close();
                sltInfo(LOGGER_STORE,tr("Tabla %1 guardada").arg(table));
            }
            else{
                sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
            }
        }
    }
}
/** Publica la info*/
void In2ConfigLampara::sltInfo(unsigned char value, QString txt){
    //Tenemos que publicarla!!!
    emit SendDebugInfo(value,txt);
    QMessageBox::information(this,"INFO",txt,QMessageBox::Ok);
}

/** Carga datos de intensidad**/
void In2ConfigLampara::sltLoadDataIntensidad(){
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
    if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
        QString message=QString("select Tabla from Referencias where Tipo='INTENSIDAD'");
        if (db.exec(message)){
            ui->m_cbIntensidad->blockSignals(true);
            ui->m_cbIntensidad->clear();
            ui->m_cbIntensidad->blockSignals(false);
            QStringList foo=db.m_Tabla;
            //sltInfo(LOGGER_STORE,QString("Num Items=%1").arg(foo.count()));
            for (int x=0;x<foo.count();x++){
                ui->m_cbIntensidad->addItem(foo.at(x));
            }        
        }
        else{
            sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
        }
        db.close();
    }
    else{
        sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
    }
}

/** Carga datos de posicion*/
void In2ConfigLampara::sltLoadDataPosicion(){
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
    if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
        QString message=QString("select Tabla from Referencias where Tipo='POSICION'");
        if (db.exec(message)){
            QStringList foo=db.m_Tabla;
            //sltInfo(LOGGER_STORE,QString("Num Items=%1").arg(foo.count()));
            for (int x=0;x<foo.count();x++){
                ui->m_cbPosicion->addItem(foo.at(x));
            }
        }
        else{
            sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
        }
        db.close();
    }
    else{
        sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
    }
}

/** Carga los datos de una tabla  de intensidad*/
void In2ConfigLampara::sltLoadTablaIntensidad(QString table){    
    ui->m_leIntensidad->setText(table);
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
    if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
        QString message=QString("select * from %1").arg(table);
        if (db.exec(message)){
            QStringList foo=db.m_Tabla;
            //sltInfo(LOGGER_STORE,QString("Num Items=%1").arg(foo.count()));
            if ((foo.count()%2)==0){
                int x=0;
                ui->m_twIntensidad->clearContents();
                ui->m_twIntensidad->setRowCount(foo.count()/2);
                while (x<foo.count()){
                    int row=x/2;
                    QTableWidgetItem *pRef= new QTableWidgetItem(foo.at(x));
                    ui->m_twIntensidad->setItem(row,0,pRef);
                    QTableWidgetItem *pVel= new QTableWidgetItem(foo.at(x+1));
                    ui->m_twIntensidad->setItem(row,1,pVel);
                    x+=2;
                }
            }
            else{
                sltInfo(LOGGER_ERROR,QString("Datos impares, no se carga tabla"));
            }
        }
        else{
            sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
        }
        db.close();
    }
    else{
        sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
    }
}

/** Carga los datos de una tabla  de posicion*/
void In2ConfigLampara::sltLoadTablaPosicion(QString table){
    ui->m_lePosicion->setText(table);
    In2GestorDB db;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
    if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
        QString message=QString("select * from %1").arg(table);
        if (db.exec(message)){
            QStringList foo=db.m_Tabla;
            //sltInfo(LOGGER_STORE,QString("Num Items=%1").arg(foo.count()));
            if ((foo.count()%2)==0){
                int x=0;
                ui->m_twPosicion->clearContents();
                ui->m_twPosicion->setRowCount(foo.count()/2);
                while (x<foo.count()){
                    int row=x/2;
                    QTableWidgetItem *pRef= new QTableWidgetItem(foo.at(x));
                    ui->m_twPosicion->setItem(row,0,pRef);
                    QTableWidgetItem *pPos= new QTableWidgetItem(foo.at(x+1));
                    ui->m_twPosicion->setItem(row,1,pPos);
                    x+=2;
                }
            }
            else{
                sltInfo(LOGGER_ERROR,QString("Datos impares, no se carga tabla"));
            }
        }
        else{
            sltInfo(LOGGER_ERROR,tr("Error exec %1").arg(message));
        }
        db.close();
    }
    else{
        sltInfo(LOGGER_ERROR,tr("Error open %1").arg(file));
    }
}

/** Resetea la lampara*/
void In2ConfigLampara::sltResetLampara(){
   In2GestorLampara *p=In2GestorLampara::request(0);
   p->resetLampara();
}
