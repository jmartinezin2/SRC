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
#include "in2message.h"
#include <QDesktopWidget>
#include <QTimer>

#define TTL  60000

In2Message::In2Message(QWidget *parent,QString text,int buttons) :
    ui(new Ui::In2Message)
{
    ui->setupUi(this);

    ui->m_lbMessage->setText(text);
    ui->m_Pb->setVisible(false);
    if (buttons==0){
        ui->m_pbOK->setEnabled(false);
        ui->m_pbCancel->setEnabled(false);
        ui->m_pbCancel->setVisible(false);
        ui->m_pbOK->setVisible(false);
    }
    else{
        connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltOK()));
        connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(sltCancel()));
    }
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    this->move((QApplication::desktop()->rect().width() - this->rect().width())/2,(QApplication::desktop()->rect().height() - this->rect().height())/2);
    this->show();
    QCoreApplication::processEvents();
    //if (!buttons) QTimer::singleShot(TTL,this,SLOT(deleteLater()));
}

In2Message::~In2Message(){

}

/** Actualiza la barra de progreso*/
void In2Message::sltUpdateProgressBar(int x){
    ui->m_Pb->setVisible(true);
    ui->m_Pb->setValue(x);
    QCoreApplication::processEvents();
}

/** Actualiza el mensaje*/
void In2Message::sltUpdateMessage(QString txt){
    ui->m_lbMessage->setText(txt);
    QCoreApplication::processEvents();
}
/** Pulsamos OK*/
void In2Message::sltOK(){
    sltDelete();
}
/** Pulsamos Cancel*/
void In2Message::sltCancel(){
    sltDelete();
}

/** Destruye el mensaje*/
void In2Message::sltDelete(){
    this->deleteLater();
}

/** Habilita una barra de progreso automatica en msg*/
void In2Message::sltEnableAutomaticPB(int time){
    m_autoPBTime=time/50;
    m_autoPBStep=(100-ui->m_Pb->value())/50;
    ui->m_Pb->setValue(0);
    //ui->m_Pb->show();
     sltAutoUpdatePB();
    //QTimer::singleShot(0,this,SLOT(sltAutoUpdatePB()));
}

/** Autoupdate de la PB*/
void In2Message::sltAutoUpdatePB(){
    QPalette pal=ui->m_lbMessage->palette();
    if (pal.color(QPalette::WindowText)==QColor(Qt::black)) pal.setColor(QPalette::WindowText,QColor(Qt::white));
    else pal.setColor(QPalette::WindowText,QColor(Qt::black));
    ui->m_lbMessage->setPalette(pal);

    int next=ui->m_Pb->value()+m_autoPBStep;
    if (next<100)
        QTimer::singleShot(m_autoPBTime,this,SLOT(sltAutoUpdatePB()));
    else
        next=100;
    sltUpdateProgressBar(next);
}

/** Muere en un rato*/
void In2Message::sltDeleteLater(int msg){
    QTimer::singleShot(msg,this,SLOT(deleteLater()));
}
