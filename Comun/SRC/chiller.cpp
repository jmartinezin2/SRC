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
#include "chiller.h"
#include "in2gestorchiller.h"


Chiller::Chiller(QWidget *parent) :
    ui(new Ui::ChillerUI)
{
    ui->setupUi(this);
    rtnScreen=parent;
    m_lastStatus=0xFFFF;
    m_lastAlarm1=0xFFFF;
    m_lastAlarm2=0xFFFF;
    m_lastAlarm3=0xFFFF;

    In2GestorChiller *gsChiller=In2GestorChiller::request(0,1);
    connect(gsChiller,SIGNAL(sglCambioAlarm1(int)),this,SLOT(sltChangeAlarm1(int)));
    connect(gsChiller,SIGNAL(sglCambioAlarm2(int)),this,SLOT(sltChangeAlarm2(int)));
    connect(gsChiller,SIGNAL(sglCambioAlarm3(int)),this,SLOT(sltChangeAlarm3(int)));
    connect(gsChiller,SIGNAL(sglCambioStatus(int)),this,SLOT(sltChangeStatus(int)));
    connect(gsChiller,SIGNAL(sglCambioTemperatura(int)),this,SLOT(sltChangeTemperature(int)));
    connect(gsChiller,SIGNAL(sglCambioPresion(int)),this,SLOT(sltChangePresion(int)));
    connect(gsChiller,SIGNAL(sglCambioSetpoint(int)),this,SLOT(sltChangeSetPoint(int)));

    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->m_pbStart,SIGNAL(clicked()),this,SLOT(sltStartChiller()));
    connect(ui->m_pbStop,SIGNAL(clicked()),this,SLOT(sltStopChiller()));
    //GlobalSetScreenSize(this);
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);

    sltChangeStatus(0);
    sltChangeAlarm1(0);
    sltChangeAlarm2(0);
    sltChangeAlarm3(0);

    sltChangeTemperature(0);
    sltChangePresion(0);
    sltChangeSetTemperature(0);

}
/** Nos vamos*/
void Chiller::sltExit(){
    if (rtnScreen) rtnScreen->show();
    this->deleteLater();
}

Chiller::~Chiller(){

}

/** Arranca el chiller*/
void Chiller::sltStartChiller(){
    In2GestorChiller *p=In2GestorChiller::request(0,1);
    if (p) p->sltRunChiller(0);
}
/** Para el chiller*/
void Chiller::sltStopChiller(){
    In2GestorChiller *p=In2GestorChiller::request(0,1);
    if (p) p->sltStopChiller(0);
}
/** Actualiza la temperatura*/
void Chiller::sltChangeTemperature(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getDischargeTemp(0);
    ui->m_leTemperatura->setText(QString("%1.%2").arg(val/10).arg(val%10));
}

/** Actualiza el setpoint*/
void Chiller::sltChangeSetTemperature(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getDischargeSetTemp(0);
    ui->m_leSetTemperatura->setText(QString("%1.%2").arg(val/10).arg(val%10));
}


/** Actualiza la presion*/
void Chiller::sltChangePresion(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getDischargePressure(0);
    ui->m_lePresion->setText(QString("%1.%2").arg(val/10).arg(val%10));
}
/** Alarma 1*/
void Chiller::sltChangeAlarm1(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getAlarm1(x);
    if (val!=m_lastAlarm1){
        ui->m_leAlarm1->setText(QString("%1").arg(val));
        if (val&0x0001) ui->m_lblLed1->setEnabled(true);
        else ui->m_lblLed1->setEnabled(false);
        if (val&0x0002) ui->m_lblLed1_2->setEnabled(true);
        else ui->m_lblLed1_2->setEnabled(false);
        if (val&0x0004) ui->m_lblLed1_3->setEnabled(true);
        else ui->m_lblLed1_3->setEnabled(false);
        if (val&0x0008) ui->m_lblLed1_4->setEnabled(true);
        else ui->m_lblLed1_4->setEnabled(false);
        if (val&0x0010) ui->m_lblLed1_5->setEnabled(true);
        else ui->m_lblLed1_5->setEnabled(false);
        if (val&0x0020) ui->m_lblLed1_6->setEnabled(true);
        else ui->m_lblLed1_6->setEnabled(false);
        if (val&0x0040) ui->m_lblLed1_7->setEnabled(true);
        else ui->m_lblLed1_7->setEnabled(false);
        if (val&0x0080) ui->m_lblLed1_8->setEnabled(true);
        else ui->m_lblLed1_8->setEnabled(false);
        if (val&0x0100) ui->m_lblLed1_9->setEnabled(true);
        else ui->m_lblLed1_9->setEnabled(false);
        if (val&0x0200) ui->m_lblLed1_10->setEnabled(true);
        else ui->m_lblLed1_10->setEnabled(false);
        if (val&0x0400) ui->m_lblLed1_11->setEnabled(true);
        else ui->m_lblLed1_11->setEnabled(false);
        if (val&0x0800) ui->m_lblLed1_12->setEnabled(true);
        else ui->m_lblLed1_12->setEnabled(false);
        if (val&0x1000) ui->m_lblLed1_13->setEnabled(true);
        else ui->m_lblLed1_13->setEnabled(false);
        if (val&0x2000) ui->m_lblLed1_14->setEnabled(true);
        else ui->m_lblLed1_14->setEnabled(false);
        if (val&0x4000) ui->m_lblLed1_15->setEnabled(true);
        else ui->m_lblLed1_15->setEnabled(false);
        if (val&0x8000) ui->m_lblLed1_16->setEnabled(true);
        else ui->m_lblLed1_16->setEnabled(false);
        m_lastAlarm1=val;
    }

}
/** Alarma 2*/
void Chiller::sltChangeAlarm2(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getAlarm2(x);
    if (val!=m_lastAlarm2){
        ui->m_leAlarm2->setText(QString("%1").arg(val));
        if (val&0x0001) ui->m_lblLed1_17->setEnabled(true);
        else ui->m_lblLed1_17->setEnabled(false);
        if (val&0x0002) ui->m_lblLed1_18->setEnabled(true);
        else ui->m_lblLed1_18->setEnabled(false);
        if (val&0x0004) ui->m_lblLed1_19->setEnabled(true);
        else ui->m_lblLed1_19->setEnabled(false);
        if (val&0x0008) ui->m_lblLed1_20->setEnabled(true);
        else ui->m_lblLed1_20->setEnabled(false);
        if (val&0x0010) ui->m_lblLed1_21->setEnabled(true);
        else ui->m_lblLed1_21->setEnabled(false);
        if (val&0x0020) ui->m_lblLed1_22->setEnabled(true);
        else ui->m_lblLed1_22->setEnabled(false);
        if (val&0x0040) ui->m_lblLed1_23->setEnabled(true);
        else ui->m_lblLed1_23->setEnabled(false);
        if (val&0x0080) ui->m_lblLed1_24->setEnabled(true);
        else ui->m_lblLed1_24->setEnabled(false);
        if (val&0x0100) ui->m_lblLed1_25->setEnabled(true);
        else ui->m_lblLed1_25->setEnabled(false);
        if (val&0x0200) ui->m_lblLed1_26->setEnabled(true);
        else ui->m_lblLed1_26->setEnabled(false);
        if (val&0x0400) ui->m_lblLed1_27->setEnabled(true);
        else ui->m_lblLed1_27->setEnabled(false);
        if (val&0x0800) ui->m_lblLed1_28->setEnabled(true);
        else ui->m_lblLed1_28->setEnabled(false);
        if (val&0x1000) ui->m_lblLed1_29->setEnabled(true);
        else ui->m_lblLed1_29->setEnabled(false);
        if (val&0x2000) ui->m_lblLed1_30->setEnabled(true);
        else ui->m_lblLed1_30->setEnabled(false);
        if (val&0x4000) ui->m_lblLed1_31->setEnabled(true);
        else ui->m_lblLed1_31->setEnabled(false);
        if (val&0x8000) ui->m_lblLed1_32->setEnabled(true);
        else ui->m_lblLed1_32->setEnabled(false);
        m_lastAlarm2=val;
    }

}
/** Alarma 3*/
void Chiller::sltChangeAlarm3(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getAlarm3(x);
    if (val!=m_lastAlarm3){
        ui->m_leAlarm3->setText(QString("%1").arg(val));
        if (val&0x0001) ui->m_lblLed1_33->setEnabled(true);
        else ui->m_lblLed1_33->setEnabled(false);
        if (val&0x0002) ui->m_lblLed1_34->setEnabled(true);
        else ui->m_lblLed1_34->setEnabled(false);
        if (val&0x0004) ui->m_lblLed1_35->setEnabled(true);
        else ui->m_lblLed1_35->setEnabled(false);
        if (val&0x0008) ui->m_lblLed1_36->setEnabled(true);
        else ui->m_lblLed1_36->setEnabled(false);
        m_lastAlarm3=val;
    }

}


/** Ha cambiado el status del chiller*/
void Chiller::sltChangeStatus(int x){
    In2GestorChiller *chiller=In2GestorChiller::request(0,1);
    quint16 val=chiller->getStatus(x);
    if (val!=m_lastStatus){
        ui->m_leStatus->setText(QString("%1").arg(val));
        if (val&0x0001) {
            ui->m_lbStatus1_1->setEnabled(true);
            ui->m_lbStatus1_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus1_1->setEnabled(false);
            ui->m_lbStatus1_2->setEnabled(true);
        }

        if (val&0x0002) {
            ui->m_lbStatus2_1->setEnabled(true);
            ui->m_lbStatus2_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus2_1->setEnabled(false);
            ui->m_lbStatus2_2->setEnabled(true);
        }
        if (val&0x0004) {
            ui->m_lbStatus3_1->setEnabled(true);
            ui->m_lbStatus3_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus3_1->setEnabled(false);
            ui->m_lbStatus3_2->setEnabled(true);
        }
        if (val&0x0010) {
            ui->m_lbStatus4_1->setEnabled(true);
            ui->m_lbStatus4_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus4_1->setEnabled(false);
            ui->m_lbStatus4_2->setEnabled(true);
        }
        if (val&0x0020) {
            ui->m_lbStatus5_1->setEnabled(true);
            ui->m_lbStatus5_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus5_1->setEnabled(false);
            ui->m_lbStatus5_2->setEnabled(true);
        }
        if (val&0x0200) {
            ui->m_lbStatus6_1->setEnabled(true);
            ui->m_lbStatus6_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus6_1->setEnabled(false);
            ui->m_lbStatus6_2->setEnabled(true);
        }
        if (val&0x0400) {
            ui->m_lbStatus7_1->setEnabled(true);
            ui->m_lbStatus7_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus7_1->setEnabled(false);
            ui->m_lbStatus7_2->setEnabled(true);
        }
        if (val&0x0800) {
            ui->m_lbStatus8_1->setEnabled(true);
            ui->m_lbStatus8_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus8_1->setEnabled(false);
            ui->m_lbStatus8_2->setEnabled(true);
        }
        if (val&0x1000) {
            ui->m_lbStatus9_1->setEnabled(true);
            ui->m_lbStatus9_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus9_1->setEnabled(false);
            ui->m_lbStatus9_2->setEnabled(true);
        }
        if (val&0x2000) {
            ui->m_lbStatus10_1->setEnabled(true);
            ui->m_lbStatus10_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus10_1->setEnabled(false);
            ui->m_lbStatus10_2->setEnabled(true);
        }
        if (val&0x4000) {
            ui->m_lbStatus11_1->setEnabled(true);
            ui->m_lbStatus11_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus11_1->setEnabled(false);
            ui->m_lbStatus11_2->setEnabled(true);
        }
        if (val&0x8000) {
            ui->m_lbStatus12_1->setEnabled(true);
            ui->m_lbStatus12_2->setEnabled(false);
        }
        else{
            ui->m_lbStatus12_1->setEnabled(false);
            ui->m_lbStatus12_2->setEnabled(true);
        }
        m_lastStatus=val;
    }
}
