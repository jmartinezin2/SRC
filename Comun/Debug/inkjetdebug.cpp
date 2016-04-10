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
#include "inkjetdebug.h"
#include "in2defines.h"
#include <QDesktopWidget>
#include <QPainter>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QScrollBar>
#include "in2hash.h"
#include "ui_InkjetDebugUI.h"
#include "in2store.h"
#include "MemoryMap.h"
//#include "GestorDB_SQLITE.h"
#include "In2GestorDB.h"
#include "in2debugcleaner.h"
#include <windows.h>

//Por defecto 10 sg de delay
#define DEFAULT_INKJETDEBUG_SAVE_DELAY 2500

#define DEFAULT_COUNTERS_FILE "In2Counter.sqlite"

InkjetDebug *InkjetDebug::mInstance  = NULL;

InkjetDebug::InkjetDebug(QWidget *parent)
    : /*QWidget(parent)*/ui(new Ui::InkjetDebugUI)
{
    ui->setupUi(this);
    //this->setWindowModality(Qt::WindowModal);
    contador_signal=0;
    m_pendingToSave=0;
    Parent=parent;            
    ui->m_Info->clear();
    connect(ui->m_SaveToFile,SIGNAL(clicked()),this,SLOT(SaveLogToFile()));
    connect(ui->m_Hide,SIGNAL(clicked()),this,SLOT(MinimizarVentanaDebug()));
    connect(ui->m_Clean,SIGNAL(clicked()),this,SLOT(LimpiarVentanaDebug()));
    //Vemos si es necesario comprimir
    In2DebugCleaner foo;
    foo.start();
}

/** Singleton*/
InkjetDebug *InkjetDebug::request(QWidget *parent)
{
    if(mInstance == NULL){
        mInstance = new InkjetDebug(parent);
    }
    return mInstance;
}


InkjetDebug::~InkjetDebug()
{
    AutomatedSave();
    ClosePreviousFile();
}
/** Firma el fichero*/
void InkjetDebug::SignFile(){
    QString dummyString=QString("%1%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_FILE);
    QFile File(dummyString);
    if (!File.open(QIODevice::ReadOnly)) return;
    QByteArray qbyteFile=File.readAll();
    In2Hash in2Hash;
    QByteArray hash=in2Hash.CalculateHash(qbyteFile);
    QString strOut=QString("%1%2").arg(QString(HASH_ID)).arg(QString(hash));
#if QT_VERSION>=0x050000
    qbyteFile.append(strOut.toLatin1());
#else
    qbyteFile.append(strOut.toAscii());
#endif
    File.close();
    if (!File.open(QIODevice::WriteOnly)) return;
    File.write(qbyteFile);
    File.close();
}

//Cierra el fichero previo
void InkjetDebug::ClosePreviousFile(){
    SignFile();
    QString dummy_string=QString("%1%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_FILE);
    QFile tmp(dummy_string);
    if (tmp.exists()){
        QDateTime dummyDate;
        QString newName=QString("%1%2").arg(dummy_string).arg(dummyDate.currentDateTime().toString("yyyyMMddhhmmss"));
        tmp.rename(newName);
        sltUpdateDB(newName);
    }
    //ui->m_Info->clear();
}


/** Procesa mensaje */
void InkjetDebug::ProcessMsg(unsigned char codigo,const QString  rcv){
    QString Message;
    bool bToBBDD=false;
    QString date=QDate::currentDate().toString("dd/MM/yy");
    QString time=QTime::currentTime().toString("hh:mm:ss");
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString user=memoryMap->getUser();
    QString code=QString("%1").arg(codigo);

    QString mensaje=QString("[%1]%2-%3 %4").arg(user).arg(date).arg(time).arg(rcv);
    m_stInfo.TablaDebug[contador_signal]=mensaje;    
    if (contador_signal>=N_ITEMS_TABLA_DEBUG) {
        contador_signal=0;
        ui->m_Info->append(QString("%1 Tamaño maximo de log->savefile").arg(mensaje));
    }

    switch (codigo){
        case LOGGER_DEBUG:
            Message = QString("<font color=\"blue\">%1</font>\r\n").arg(mensaje);
            //bToBBDD=false;
            bToBBDD=true;
        break;
        case LOGGER_WARNING:
            Message = QString("<font color=\"blue\">%1</font>\r\n").arg(mensaje);
            bToBBDD=false;
        break;
        case LOGGER_ERROR:
            Message = QString("<font color=\"red\">%1</font>\r\n").arg(mensaje);
            bToBBDD=false;
        break;
        case LOGGER_STORE:
            Message = QString("<font color=\"black\">%1</font>\r\n").arg(mensaje);
            bToBBDD=false;
        break;
        case LOGGER_PARADA: //Resumen de parada
            Message = QString("<font color=\"green\">%1</font>\r\n").arg(mensaje);
            bToBBDD=false;
        break;

    }
    if (!Message.isEmpty()){
        ui->m_Info->append(Message);
        emit InkjetDbgMsg(Message);
        if (!m_pendingToSave){
            m_pendingToSave=1;
            QTimer::singleShot(DEFAULT_INKJETDEBUG_SAVE_DELAY,this,SLOT(AutomatedSave()));
        }
    }
    if (bToBBDD){
        In2Store gestorBBBDD;
        strctIn2Store data;
        data.codigo=QString("%1").arg(codigo);
        data.evento=rcv;
        data.fecha=date;
        data.hora=time;
        data.tarea=memoryMap->m_mapa.m_variables.OrdenFabricacion;
        data.user=user;
        gestorBBBDD.saveMessage(data,memoryMap->m_mapa.m_variables.Proceso);
    }
    contador_signal++;
}

/** Guarda automaticamente*/
void InkjetDebug::AutomatedSave(){
    QString dummyString=QString("%1%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_FILE);
    QFile File(dummyString);
    if (!File.open(QIODevice::WriteOnly)) return;
#if QT_VERSION>=0x050000
    QByteArray contenido=ui->m_Info->toPlainText().toLatin1();
#else
    QByteArray contenido=ui->m_Info->toPlainText().toAscii();
#endif
    File.write(contenido);
    File.close();
    //sltUpdateDB(dummyString);
}
/** Actualiza la DB*/
void InkjetDebug::sltUpdateDB(QString txt){
    In2GestorDB db;
    QString strPath=QString("%1\\Database\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_COUNTERS_FILE);
    if (db.open(QString("QSQLITE"),QString("In2Counter"),strPath)){
        QString select=QString("insert into Files ('Files') Values ('%1')").arg(txt);
#if QT_VERSION>=0x050000
        db.exec(select.toLatin1().data());
#else
        db.exec(select.toAscii().data());
#endif
    }
}

/** Fuerza la escritura*/
void InkjetDebug::ForceFlush(){
    this->AutomatedSave();
}

/** Salimos*/
void InkjetDebug::Salir(){
    this->deleteLater();
}

void InkjetDebug::MinimizarVentanaDebug(){
    this->showMinimized();
}

/** Enseñamos la pantalla*/
void InkjetDebug::showLog(){
    //Movemos a esquina superior derecha
    QDesktopWidget p;
    if (ui->m_cbAvisa->isChecked()){
        //this->move(p.width()-this->width(),0);
        this->move(1024-this->width(),0);
        this->showNormal();
        this->activateWindow();
        //ui->m_Info->ensureCursorVisible();
        sltEnsureVisible();
    }
    //else this->activateWindow();
}

/** Enseñamos la pantalla*/
void InkjetDebug::forcedShowLog(){
        QDesktopWidget p;
        //this->move(p.width()-this->width(),0);
        this->move(1024-this->width(),0);
        this->setVisible(true);
        this->showNormal();
        this->show();
        this->activateWindow();

        //ui->m_Info->ensureCursorVisible();
        sltEnsureVisible();
}


void InkjetDebug::LimpiarVentanaDebug(){
    int respuesta=QMessageBox::question(this,tr("ATENCION"),tr("Desea eliminar log?"),tr("Si"),tr("No"),QString::null, 0, 1 );
    if (respuesta==0){
        for(int i=0;i<N_ITEMS_TABLA_DEBUG;i++){
            m_stInfo.TablaDebug[i].clear();
        }
        contador_signal=0;
        ui->m_Info->clear();
    }

}

void InkjetDebug::sltEnsureVisible(){
    QScrollBar *sb=ui->m_Info->verticalScrollBar();
    if (sb){
        sb->setValue(sb->maximum());
    }
}

void InkjetDebug::UpdateShowLevel(){

    /*QString Message;
    ui->m_Info->clear();

    NivelDebug=ui->m_ShowLevel->currentIndex();
    for(unsigned int i=0;i<contador_signal;i++){
        if (m_stInfo.codigo[i]>=NivelDebug){
            switch (m_stInfo.codigo[i]){
               case LOGGER_DEBUG:
                Message = QString("<font color=\"green\">%1</font>").arg(m_stInfo.TablaDebug[i]);
                   break;
               case LOGGER_WARNING:
                   Message = QString("<font color=\"blue\">%1</font>").arg(m_stInfo.TablaDebug[i]);
                   break;
               case LOGGER_ERROR:
                   Message = QString("<font color=\"red\">%1</font>").arg(m_stInfo.TablaDebug[i]);
                   break;
            }
            ui->m_Info->append(Message);
            if (ui->m_Info->isVisible()) sltEnsureVisible();//ui->m_Info->ensureCursorVisible();
        }
    }*/

}

/** Devuelve lista completa de errores acumulados*/
QStringList InkjetDebug::getErrorList(){
    /*QStringList lst;
    for(unsigned int i=0;i<contador_signal;i++){
        if (m_stInfo.codigo[i]==LOGGER_ERROR){
            lst.append(m_stInfo.TablaDebug[i]);
        }
    }
    return lst;*/
    return QStringList();
}

void InkjetDebug::SaveLogToFile(){

    int res;

    QFileDialog fileName(this);

    QByteArray contenido;
    QString tmp;
    QStringList Ficheros;
    fileName.setAcceptMode(QFileDialog::AcceptSave);
    fileName.setFileMode(QFileDialog::AnyFile);
    fileName.setViewMode(QFileDialog::List);
#if QT_VERSION>=0x050000
    fileName.setNameFilter(tr("Archivo de texto (*.txt)"));
#else
    fileName.setFilter(tr("Archivo de texto (*.txt)"));
#endif
    fileName.setDefaultSuffix("ini");


    /*QList<QPushButton *> widgets = qFindChildren<QPushButton *>(&fileName);
    foreach (QPushButton *w, widgets){
            w->setAutoDefault(false);
     }*/
    if (fileName.exec()){
        Ficheros=fileName.selectedFiles();

        if (!Ficheros.isEmpty()){
            tmp=Ficheros.at(0);
            QFile File(tmp);
            if (!File.open(QIODevice::WriteOnly)) res=-1;
#if QT_VERSION>=0x050000
            contenido=ui->m_Info->toPlainText().toLatin1();
#else
            contenido=ui->m_Info->toPlainText().toAscii();
#endif
            File.write(contenido);
            res=0;
        }
        else res=-1;//JORGE
    }
    else res=-1;//JORGE

    if (res==0) QMessageBox::information(0,"INFO",tr("Log guardado correctamente"),QMessageBox::Ok);
    else QMessageBox::warning(0,"ERROR",tr("Error al guardar log"),QMessageBox::Ok);

}

