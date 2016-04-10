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
#include "clienteopc.h"
#include "maquinaAdv.h"
#include "globals.h"
#include "memoryMap.h"
#include "In2Framework.h"
#include "InputData.h"
#include <QMessageBox>
#include <QFileDialog>
//#include "principal.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QSessionManager>
#include <QApplication>
#include <Windows.h>
#include <QFontComboBox>
#include <assert.h>
#include <QHash>
#include <QPixmap>
#include <QFontComboBox>
//#include "webcam.h"

#include <QDebug>

#include <QAxObject>
#include <QMetaMethod>
#include <QMetaProperty>

//Fichero de cabecera para Xaar
#include "ScorpionDLL-ext.h"
#include "compileversion.h"
#include "curvas.h"
#include "gestormaquina.h"
#include "monitordiametro.h"
#include "in2managerdiametros.h"
#include "gestorimpresion.h"
#include "inkjetdebug.h"
#include "in2gestormaquina.h"
#include "in2configuratordiameter.h"



MaquinaAdv::MaquinaAdv(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::MaquinaAdvUI)
{

    ui->setupUi(this);

    In2ConfiguratorDiameter configDiamIn(IN2MANAGER_DIN);
    bool monitor=configDiamIn.getEnabled();
    if (!monitor) ui->m_ledSensorIN->setEnabled(configDiamIn.getPLCEnabled());

    In2ConfiguratorDiameter configDiamOut(IN2MANAGER_DOUT);
    monitor=configDiamOut.getEnabled();
    if (!monitor) ui->m_ledSensorOut->setEnabled(configDiamOut.getPLCEnabled());

    m_memoryMap=MemoryMap::request(0);
    GlobalSetScreenSize(this);

    QPalette dummyPalette=GetPaletaTextEditEnabled();
    ui->m_VelocidadAjuste->setPalette(dummyPalette);
    ui->m_VelocidadBusqueda->setPalette(dummyPalette);
    ui->m_VelocidadJogMas->setPalette(dummyPalette);
    ui->m_VelocidadJogMenos->setPalette(dummyPalette);
    ui->m_TiempoBusqueda->setPalette(dummyPalette);
    ui->m_FuerzaFreno->setPalette(dummyPalette);
    ui->m_IntensidadLampara->setPalette(dummyPalette);
    ui->m_TensionBobina->setPalette(dummyPalette);
    ui->m_RampaAceleracion->setPalette(dummyPalette);
    ui->m_RampaDeceleracion->setPalette(dummyPalette);
    ui->m_DistanciaEmpalme->setPalette(dummyPalette);

    connect(ui->m_AceptarButton,SIGNAL(clicked()),this,SLOT(AceptarSeleccion()));


    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_VelocidadAjuste,SIGNAL(selectionChanged()),this,SLOT(AceptarValorVelocidadAjuste()));
    connect(ui->m_VelocidadBusqueda,SIGNAL(selectionChanged()),this,SLOT(AceptarValorVelocidadBusqueda()));
    connect(ui->m_VelocidadJogMas,SIGNAL(selectionChanged()),this,SLOT(AceptarValorVelocidadJogMas()));
    connect(ui->m_VelocidadJogMenos,SIGNAL(selectionChanged()),this,SLOT(AceptarValorVelocidadJogMenos()));
    connect(ui->m_TiempoBusqueda,SIGNAL(selectionChanged()),this,SLOT(AceptarValorTiempoBusqueda()));
    connect(ui->m_FuerzaFreno,SIGNAL(selectionChanged()),this,SLOT(AceptarValorFreno()));
    connect(ui->m_IntensidadLampara,SIGNAL(selectionChanged()),this,SLOT(AceptarValorIntensidadLampara()));
    connect(ui->m_TensionBobina,SIGNAL(selectionChanged()),this,SLOT(AceptarValorTensionBobina()));
    connect(ui->m_RampaAceleracion,SIGNAL(selectionChanged()),this,SLOT(AceptarValorRampaAceleracion()));
    connect(ui->m_RampaDeceleracion,SIGNAL(selectionChanged()),this,SLOT(AceptarValorRampaDeceleracion()));
    connect(ui->m_DistanciaEmpalme,SIGNAL(selectionChanged()),this,SLOT(AceptarValorDistanciaEmpalme()));
    #endif


    connect(ui->m_CambiarVelocidadAjuste,SIGNAL(clicked()),this,SLOT(CambiarVelocidadAjuste()));
    connect(ui->m_CambiarVelocidadBusqueda,SIGNAL(clicked()),this,SLOT(CambiarVelocidadBusqueda()));
    connect(ui->m_CambiarVelocidadJogMas,SIGNAL(clicked()),this,SLOT(CambiarVelocidadJogMas()));
    connect(ui->m_CambiarVelocidadJogMenos,SIGNAL(clicked()),this,SLOT(CambiarVelocidadJogMenos()));
    connect(ui->m_CambiarTiempoBusqueda,SIGNAL(clicked()),this,SLOT(CambiarTiempoBusqueda()));
    connect(ui->m_CambiarFuerzaFreno,SIGNAL(clicked()),this,SLOT(CambiarFuerzaFreno()));
    connect(ui->m_CambiarIntensidadLampara,SIGNAL(clicked()),this,SLOT(CambiarIntensidadLampara()));
    connect(ui->m_CambiarTensionBobina,SIGNAL(clicked()),this,SLOT(CambiarTensionBobina()));
    connect(ui->m_CambiarRampaAceleracion,SIGNAL(clicked()),this,SLOT(CambiarRampaAceleracion()));
    connect(ui->m_CambiarRampaDeceleracion,SIGNAL(clicked()),this,SLOT(CambiarRampaDeceleracion()));
    connect(ui->m_CambiarDistanciaEmpalme,SIGNAL(clicked()),this,SLOT(CambiarDistanciaEmpalme()));
    /*connect(ui->m_ReassignXUSB,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ChangeXUSBOrderAuto()));
    connect(ui->m_ReinicioManual,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ChangeXUSBOrder()));*/
    connect(ui->m_TrabajoNuevo,SIGNAL(clicked()),this,SLOT(CambiarTrabajoAdv()));
    connect(ui->m_JogMas,SIGNAL(pressed()),this,SLOT(EjecutarJogMasOn()));
    connect(ui->m_JogMenos,SIGNAL(pressed()),this,SLOT(EjecutarJogMenosOn()));
    connect(ui->m_JogMas,SIGNAL(released()),this,SLOT(EjecutarJogMasOff()));
    connect(ui->m_JogMenos,SIGNAL(released()),this,SLOT(EjecutarJogMenosOff()));
    connect(ui->m_BuscarMacula,SIGNAL(clicked()),this,SLOT(EjecutarBuscarMacula()));
    connect(ui->m_Empalme,SIGNAL(clicked()),this,SLOT(EjecutarEmpalme()));
    connect(ui->m_BobinaNueva,SIGNAL(clicked()),this,SLOT(EjecutarBobinaNueva()));
    connect(ui->m_LampOn,SIGNAL(clicked()),this,SLOT(EjecutarEncenderLampara()));
    connect(ui->m_LampOff,SIGNAL(clicked()),this,SLOT(EjecutarApagarLampara()));
    connect(ui->m_FrenoOn,SIGNAL(clicked()),this,SLOT(EjecutarFrenoOn()));
    connect(ui->m_FrenoOff,SIGNAL(clicked()),this,SLOT(EjecutarFrenoOff()));
    connect(ui->m_LampOnJogMas,SIGNAL(clicked()),this,SLOT(EjecutarLampOnJogMas()));
    connect(ui->m_LampOnJogMenos,SIGNAL(clicked()),this,SLOT(EjecutarLampOnJogMenos()));
    InkjetDebug *inkjetDebg=InkjetDebug::request(0);
    connect(ui->m_pbLog,SIGNAL(clicked()),inkjetDebg,SLOT(showLog()));
    //connect(ui->m_pbLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    connect(ui->m_pbInfo,SIGNAL(clicked()),this,SLOT(showCompileVersion()));
    connect(ui->m_pbReinitSystem,SIGNAL(clicked()),this,SLOT(sltReiniciarSistema()));

    QTimer::singleShot(0,this,SLOT(ActualizaDataSlotOPC()));

    ui->m_leXUSB1Expected->setText(m_memoryMap->m_mapa.m_maquina.m_xusbs[0].SN);
    ui->m_leXUSB2Expected->setText(m_memoryMap->m_mapa.m_maquina.m_xusbs[1].SN);
    sltCheckSerials();
    GestorImpresion *gestor=GestorImpresion::request(0);
    connect(gestor,SIGNAL(sgnReinicioSistema(int)),this,SLOT(sltUpdateReinicio(int)));

}
/** Muestra la info de la version de compilacion*/
void MaquinaAdv::showCompileVersion(){
    CompileVersion *p=new CompileVersion(this);
    p->show();
}
/** Actualñiza el reincio*/
void MaquinaAdv::sltUpdateReinicio(int x){
    ui->m_prbReinitSystem->setValue(x);
    if (x==100) QTimer::singleShot(1000,this,SLOT(sltCheckSerials()));
    else ui->label_9->setText("Reiniciando");
}
/** Reinicia sistema*/
void MaquinaAdv::sltReiniciarSistema(){
        GestorImpresion *gestor=GestorImpresion::request(0);
        gestor->sltReiniciarSistema();
}

/** Chequea los serial numbers*/
void MaquinaAdv::sltCheckSerials(){
    ui->label_9->setText("Pulse para reiniciar");
    GestorImpresion *gestor=GestorImpresion::request(0);
    char buffer[25];
    memset(&buffer,0x00,sizeof(buffer));
    gestor->getSerial(0,buffer);
    ui->m_leXUSB1Detected->setText(QString(buffer));
    memset(&buffer,0x00,sizeof(buffer));
    gestor->getSerial(1,buffer);
    ui->m_leXUSB2Detected->setText(QString(buffer));
    ui->m_prbReinitSystem->setValue(0);
}

MaquinaAdv::~MaquinaAdv(){
}


//Acepta la selección realizada y vuelve a la pantalla de Configuracion
void MaquinaAdv::AceptarSeleccion(){
    //LaunchPrincipal();
    this->deleteLater();
}


void MaquinaAdv::LaunchMaquina(){
    /*Maquina *ScrMaquina;
    ScrMaquina=new Maquina(0);
    ScrMaquina->show();*/
    this->deleteLater();
}

void MaquinaAdv::CancelarSeleccion(){
}




void MaquinaAdv::CambiarTiempoBusqueda(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar el tiempo de búsqueda de mácula?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_TiempoBusqueda->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_TIEMPO_BUSQUEDA_MACULA,valor);
        }
    }
}

void MaquinaAdv::CambiarVelocidadBusqueda(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la velocidad de búsqueda de mácula?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_VelocidadBusqueda->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_VELOCIDAD_BUSQUEDA_MACULA,valor);
        }
    }
}

void MaquinaAdv::CambiarVelocidadAjuste(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la velocidad de ajuste de búsqueda de mácula?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_VelocidadAjuste->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_VELOCIDAD_AJUSTE_MACULA,valor);
        }
    }
}

void MaquinaAdv::CambiarVelocidadJogMas(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la velocidad de Jog+?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_VelocidadJogMas->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_VELOCIDAD_JOG_MAS,valor);
            //Hacer con gestorMaquina
            //((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_JOGMAS);
        }
    }
}

void MaquinaAdv::CambiarVelocidadJogMenos(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la velocidad de Jog-?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_VelocidadJogMenos->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_VELOCIDAD_JOG_MENOS,valor);
            //Hacer con gestormaquina
            //((Principal*)dummy)->AjustarIntensidadLampara(MOVIMIENTO_JOGMENOS);
        }
    }
}


void MaquinaAdv::CambiarFuerzaFreno(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la fuerza del freno","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_FuerzaFreno->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_FUERZA_FRENO,valor);
        }
    }
}

void MaquinaAdv::CambiarIntensidadLampara(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la intensidad de la lámpara de curado","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_IntensidadLampara->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_INTENSIDAD_LAMPARA,valor);
        }
    }
}

void MaquinaAdv::CambiarTensionBobina(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la tensión de bobina","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_TensionBobina->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_TENSION_BOBINA,valor);
        }
    }
}

void MaquinaAdv::CambiarRampaAceleracion(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la rampa de aceleración","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_RampaAceleracion->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_RAMPA_ACELERACION,valor);
        }
    }
}

void MaquinaAdv::CambiarRampaDeceleracion(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la rampa de deceleración","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_RampaDeceleracion->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_RAMPA_DECELERACION,valor);
        }
    }
}

void MaquinaAdv::CambiarTrabajoAdv(){
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar de trabajo?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            GestorMaquina *m=GestorMaquina::request(this);
            m->sltTrabajoNuevo();
        }
    }
}

void MaquinaAdv::CambiarDistanciaEmpalme(){
    int dummy_int;
    if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea cambiar la distancia de empalme?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //JORGE QWidget *dummy;
            //JORGE dummy=GlobalGetMainWindow();
            dummy_int=ui->m_DistanciaEmpalme->text().toInt();
            QVariant valor=QVariant(dummy_int);
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_DISTANCIA_EMPALME,valor);
        }
    }
}
/** Actualiza leds de Bobinador*/
void MaquinaAdv::sltActualizaLedsBobinador(int value){
    if (value&(1<<0)) ui->m_ledBob1->setEnabled(true);
    else ui->m_ledBob1->setEnabled(false);
    if (value&(1<<1)) ui->m_ledBob1_2->setEnabled(true);
    else ui->m_ledBob1_2->setEnabled(false);
    if (value&(1<<2)) ui->m_ledBob1_3->setEnabled(true);
    else ui->m_ledBob1_3->setEnabled(false);
    if (value&(1<<3)) ui->m_ledBob1_4->setEnabled(true);
    else ui->m_ledBob1_4->setEnabled(false);
    if (value&(1<<4)) ui->m_ledBob1_5->setEnabled(true);
    else ui->m_ledBob1_5->setEnabled(false);
    if (value&(1<<5)) ui->m_ledBob1_6->setEnabled(true);
    else ui->m_ledBob1_6->setEnabled(false);
    if (value&(1<<6)) ui->m_ledBob1_7->setEnabled(true);
    else ui->m_ledBob1_7->setEnabled(false);
    if (value&(1<<7)) ui->m_ledBob1_8->setEnabled(true);
    else ui->m_ledBob1_8->setEnabled(false);
    if (value&(1<<8)) ui->m_ledBob1_9->setEnabled(true);
    else ui->m_ledBob1_9->setEnabled(false);
    if (value&(1<<9)) ui->m_ledBob1_10->setEnabled(true);
    else ui->m_ledBob1_10->setEnabled(false);
    if (value&(1<<10)) ui->m_ledBob1_11->setEnabled(true);
    else ui->m_ledBob1_11->setEnabled(false);
    if (value&(1<<11)) ui->m_ledBob1_12->setEnabled(true);
    else ui->m_ledBob1_12->setEnabled(false);
    if (value&(1<<12)) ui->m_ledBob1_13->setEnabled(true);
    else ui->m_ledBob1_13->setEnabled(false);
    if (value&(1<<13)) ui->m_ledBob1_14->setEnabled(true);
    else ui->m_ledBob1_14->setEnabled(false);
    if (value&(1<<14)) ui->m_ledBob1_15->setEnabled(true);
    else ui->m_ledBob1_15->setEnabled(false);
    if (value&(1<<15)) ui->m_ledBob1_16->setEnabled(true);
    else ui->m_ledBob1_16->setEnabled(false);
    if (value&(1<<16)) ui->m_ledBob1_17->setEnabled(true);
    else ui->m_ledBob1_17->setEnabled(false);
    if (value&(1<<17)) ui->m_ledBob1_18->setEnabled(true);
    else ui->m_ledBob1_18->setEnabled(false);
    if (value&(1<<18)) ui->m_ledBob1_19->setEnabled(true);
    else ui->m_ledBob1_19->setEnabled(false);
    if (value&(1<<19)) ui->m_ledBob1_20->setEnabled(true);
    else ui->m_ledBob1_20->setEnabled(false);
    if (value&(1<<20)) ui->m_ledBob1_21->setEnabled(true);
    else ui->m_ledBob1_21->setEnabled(false);
    if (value&(1<<21)) ui->m_ledBob1_22->setEnabled(true);
    else ui->m_ledBob1_22->setEnabled(false);
    if (value&(1<<22)) ui->m_ledBob1_23->setEnabled(true);
    else ui->m_ledBob1_23->setEnabled(false);
    if (value&(1<<23)) ui->m_ledBob1_24->setEnabled(true);
    else ui->m_ledBob1_24->setEnabled(false);
}

/** Actualiza leds de Bobinador*/
void MaquinaAdv::sltActualizaLedsDesbobinador(int value){
    if (value&(1<<0)) ui->m_ledDesb1_10->setEnabled(true);
    else ui->m_ledDesb1->setEnabled(false);
    if (value&(1<<1)) ui->m_ledDesb1_2->setEnabled(true);
    else ui->m_ledDesb1_2->setEnabled(false);
    if (value&(1<<2)) ui->m_ledDesb1_3->setEnabled(true);
    else ui->m_ledDesb1_3->setEnabled(false);
    if (value&(1<<3)) ui->m_ledDesb1_4->setEnabled(true);
    else ui->m_ledDesb1_4->setEnabled(false);
    if (value&(1<<4)) ui->m_ledDesb1_5->setEnabled(true);
    else ui->m_ledDesb1_5->setEnabled(false);
    if (value&(1<<5)) ui->m_ledDesb1_6->setEnabled(true);
    else ui->m_ledDesb1_6->setEnabled(false);
    if (value&(1<<6)) ui->m_ledDesb1_7->setEnabled(true);
    else ui->m_ledDesb1_7->setEnabled(false);
    if (value&(1<<7)) ui->m_ledDesb1_8->setEnabled(true);
    else ui->m_ledDesb1_8->setEnabled(false);
    if (value&(1<<8)) ui->m_ledDesb1_9->setEnabled(true);
    else ui->m_ledDesb1_9->setEnabled(false);
    if (value&(1<<9)) ui->m_ledDesb1_10->setEnabled(true);
    else ui->m_ledDesb1_10->setEnabled(false);
    if (value&(1<<10)) ui->m_ledDesb1_11->setEnabled(true);
    else ui->m_ledDesb1_11->setEnabled(false);
    if (value&(1<<11)) ui->m_ledDesb1_12->setEnabled(true);
    else ui->m_ledDesb1_12->setEnabled(false);
    if (value&(1<<12)) ui->m_ledDesb1_13->setEnabled(true);
    else ui->m_ledDesb1_13->setEnabled(false);
    if (value&(1<<13)) ui->m_ledDesb1_14->setEnabled(true);
    else ui->m_ledDesb1_14->setEnabled(false);
    if (value&(1<<14)) ui->m_ledDesb1_15->setEnabled(true);
    else ui->m_ledDesb1_15->setEnabled(false);
    if (value&(1<<15)) ui->m_ledDesb1_16->setEnabled(true);
    else ui->m_ledDesb1_16->setEnabled(false);
    if (value&(1<<16)) ui->m_ledDesb1_17->setEnabled(true);
    else ui->m_ledDesb1_17->setEnabled(false);
    if (value&(1<<17)) ui->m_ledDesb1_18->setEnabled(true);
    else ui->m_ledDesb1_18->setEnabled(false);
    if (value&(1<<18)) ui->m_ledDesb1_19->setEnabled(true);
    else ui->m_ledDesb1_19->setEnabled(false);
    if (value&(1<<19)) ui->m_ledDesb1_20->setEnabled(true);
    else ui->m_ledDesb1_20->setEnabled(false);
    if (value&(1<<20)) ui->m_ledDesb1_21->setEnabled(true);
    else ui->m_ledDesb1_21->setEnabled(false);
    if (value&(1<<21)) ui->m_ledDesb1_22->setEnabled(true);
    else ui->m_ledDesb1_22->setEnabled(false);
    if (value&(1<<22)) ui->m_ledDesb1_23->setEnabled(true);
    else ui->m_ledDesb1_23->setEnabled(false);
    if (value&(1<<23)) ui->m_ledDesb1_24->setEnabled(true);
    else ui->m_ledDesb1_24->setEnabled(false);
}


// Actualiza la info
void MaquinaAdv::ActualizaDataSlotOPC(){
    if (ui->m_updateData->isChecked()){
        if (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1){
            ClienteOPC *miCliente=ClienteOPC::request(this);
            int valor_int=miCliente->LeerDato(ITEM_TIEMPO_BUSQUEDA_MACULA).toInt();
            ui->m_TiempoBusqueda->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_VELOCIDAD_BUSQUEDA_MACULA).toInt();
            ui->m_VelocidadBusqueda->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_VELOCIDAD_AJUSTE_MACULA).toInt();
            ui->m_VelocidadAjuste->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MENOS).toInt();
            ui->m_VelocidadJogMenos->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_VELOCIDAD_JOG_MAS).toInt();
            ui->m_VelocidadJogMas->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_INTENSIDAD_LAMPARA).toInt();
            ui->m_IntensidadLampara->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_FUERZA_FRENO).toInt();
            ui->m_FuerzaFreno->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_DISTANCIA_EMPALME).toInt();
            ui->m_DistanciaEmpalme->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_TENSION_BOBINA).toInt();
            ui->m_TensionBobina->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_RAMPA_ACELERACION).toInt();
            ui->m_RampaAceleracion->setText(QString("%1").arg(valor_int));
            valor_int=miCliente->LeerDato(ITEM_RAMPA_DECELERACION).toInt();
            ui->m_RampaDeceleracion->setText(QString("%1").arg(valor_int));
            lamp_on=miCliente->LeerDato(ITEM_LAMPARA_ON).toBool();
            freno_on=miCliente->LeerDato(ITEM_FRENO_ONOFF).toBool();
            unsigned foo=miCliente->LeerDato(ITEM_ERROR_BOBINADOR).toInt();
            unsigned int dummy=ui->m_leBobinador->text().toInt();
            unsigned int val1=(foo&(0xFF))<<24;
            unsigned int val2=(foo&(0xFF00))<<8;
            unsigned int val3=(foo&(0xFF0000))>>8;
            unsigned int val4=(foo&(0xFF000000))>>24;
            foo=val1+val2+val3+val4;
            if (dummy!=foo){
                ui->m_leBobinador->setText(QString("%1").arg(foo));
                sltActualizaLedsDesbobinador(foo);
            }
            foo=miCliente->LeerDato(ITEM_ERROR_DESBOBINADOR).toInt();
            dummy=ui->m_leBobinador->text().toInt();
            val1=(foo&(0xFF))<<24;
            val2=(foo&(0xFF00))<<8;
            val3=(foo&(0xFF0000))>>8;
            val4=(foo&(0xFF000000))>>24;
            foo=val1+val2+val3+val4;
            if (dummy!=foo){
                ui->m_leDesbobinador->setText(QString("%1").arg(foo));
                sltActualizaLedsDesbobinador(foo);
            }
            if (m_memoryMap->m_mapa.m_maquina.jogmas_conlampara)
                ui->m_LampOnJogMas->setChecked(true);
            else
                ui->m_LampOnJogMas->setChecked(false);
            if (m_memoryMap->m_mapa.m_maquina.jogmenos_conlampara)
                ui->m_LampOnJogMenos->setChecked(true);
            else
                ui->m_LampOnJogMenos->setChecked(false);
            #ifdef HONLE
                ui->m_TemperaturaLampara->setText(QString("-"));
            #else
                ui->m_TemperaturaLampara->setText(QString("%1").arg(m_memoryMap->m_mapa.m_maquina.temperatura_lampara));
            #endif
        }
        In2ManagerDiametros *manager=In2ManagerDiametros::request(0);
        MonitorDiametro *diamIN=manager->getMonitor(IN2MANAGER_DIN);
        if (diamIN->isEnabled()){
            double val=diamIN->getCurrentDiameter(0);
            ui->m_leSensorFilt->setText(QString("%1").arg(val));
            ui->m_leSensorRaw->setText(diamIN->getRawDiameter());
        }

        MonitorDiametro *diamOUT=manager->getMonitor(IN2MANAGER_DOUT);
        if (diamOUT->isEnabled()){
            double val=diamOUT->getCurrentDiameter(0);
            ui->m_leSensorFilt2->setText(QString("%1").arg(val));
            ui->m_leSensorRaw2->setText(diamOUT->getRawDiameter());
        }

        signed short val=m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada_inst;
        ui->m_lePLCDiamRaw->setText(QString("%1").arg(val));
        val=m_memoryMap->m_mapa.m_maquina.diametro_bobina_entrada;
        ui->m_lePLCDiamFilt->setText(QString("%1").arg(val));
    }

    QTimer::singleShot(2500,this,SLOT(ActualizaDataSlotOPC()));


}


void MaquinaAdv::AceptarValorTiempoBusqueda(){
    InputData *input = InputData::request(this,"Tiempo Búsqueda de Mácula [ms]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarTiempoBusqueda(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarTiempoBusqueda(QString value){
    int dummy_int;
    ui->m_TiempoBusqueda->setText(value);
    ui->m_TiempoBusqueda->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_TiempoBusqueda->text().toInt();
    if (dummy_int<REF_TIEMPO_BUSQUEDA_MACULA_MIN) dummy_int=REF_TIEMPO_BUSQUEDA_MACULA_MIN;
    if (dummy_int>REF_TIEMPO_BUSQUEDA_MACULA_MAX) dummy_int=REF_TIEMPO_BUSQUEDA_MACULA_MAX;
    ui->m_TiempoBusqueda->setText(QString("%1").arg(dummy_int));
}

void MaquinaAdv::AceptarValorVelocidadBusqueda(){
    InputData *input = InputData::request(this,"Velocidad Búsqueda de Mácula [mm/min]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarVelocidadBusqueda(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarVelocidadBusqueda(QString value){
    int dummy_int;
    ui->m_VelocidadBusqueda->setText(value);
    ui->m_VelocidadBusqueda->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_VelocidadBusqueda->text().toInt();
    if (dummy_int<REF_VELOCIDAD_BUSQUEDA_MACULA_MIN) dummy_int=REF_VELOCIDAD_BUSQUEDA_MACULA_MIN;
    if (dummy_int>REF_VELOCIDAD_BUSQUEDA_MACULA_MAX) dummy_int=REF_VELOCIDAD_BUSQUEDA_MACULA_MAX;
    ui->m_VelocidadBusqueda->setText(QString("%1").arg(dummy_int));
}

void MaquinaAdv::AceptarValorVelocidadAjuste(){
    InputData *input = InputData::request(this,"Velocidad Ajuste de Mácula [mm/min]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarVelocidadAjuste(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarVelocidadAjuste(QString value){
    int dummy_int;
    ui->m_VelocidadAjuste->setText(value);
    ui->m_VelocidadAjuste->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_VelocidadAjuste->text().toInt();
    if (dummy_int<REF_VELOCIDAD_AJUSTE_MACULA_MIN) dummy_int=REF_VELOCIDAD_AJUSTE_MACULA_MIN;
    if (dummy_int>REF_VELOCIDAD_AJUSTE_MACULA_MAX) dummy_int=REF_VELOCIDAD_AJUSTE_MACULA_MAX;
    ui->m_VelocidadAjuste->setText(QString("%1").arg(dummy_int));
}

void MaquinaAdv::AceptarValorVelocidadJogMas(){
    InputData *input = InputData::request(this,"Velocidad Jog + [mm/min]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarVelocidadJogMas(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarVelocidadJogMas(QString value){
    int dummy_int;
    ui->m_VelocidadJogMas->setText(value);
    ui->m_VelocidadJogMas->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_VelocidadJogMas->text().toInt();
    if (dummy_int<REF_VELOCIDAD_JOG_MAS_MIN) dummy_int=REF_VELOCIDAD_JOG_MAS_MIN;
    if (dummy_int>REF_VELOCIDAD_JOG_MAS_MAX) dummy_int=REF_VELOCIDAD_JOG_MAS_MAX;
    ui->m_VelocidadJogMas->setText(QString("%1").arg(dummy_int));
}

void MaquinaAdv::AceptarValorVelocidadJogMenos(){
    InputData *input = InputData::request(this,"Velocidad Jog - [mm/min]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarVelocidadJogMenos(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarVelocidadJogMenos(QString value){
    int dummy_int;
    ui->m_VelocidadJogMenos->setText(value);
    ui->m_VelocidadJogMenos->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_VelocidadJogMenos->text().toInt();
    if (dummy_int<REF_VELOCIDAD_JOG_MENOS_MIN) dummy_int=REF_VELOCIDAD_JOG_MENOS_MIN;
    if (dummy_int>REF_VELOCIDAD_JOG_MENOS_MAX) dummy_int=REF_VELOCIDAD_JOG_MENOS_MAX;
    ui->m_VelocidadJogMenos->setText(QString("%1").arg(dummy_int));
}

void MaquinaAdv::AceptarValorFreno(){
    InputData *input = InputData::request(this,"Fuerza Freno Electromagnético [%]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarFreno(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarFreno(QString value){
    int dummy_int;
    ui->m_FuerzaFreno->setText(value);
    ui->m_FuerzaFreno->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_FuerzaFreno->text().toInt();
    if (dummy_int<REF_FUERZA_FRENO_MIN) dummy_int=REF_FUERZA_FRENO_MIN;
    if (dummy_int>REF_FUERZA_FRENO_MAX) dummy_int=REF_FUERZA_FRENO_MAX;
    ui->m_FuerzaFreno->setText(QString("%1").arg(dummy_int));
}


void MaquinaAdv::AceptarValorIntensidadLampara(){
    InputData *input = InputData::request(this,"Intensidad Lámpara de Curado [%]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarIntensidadLampara(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarIntensidadLampara(QString value){
    int dummy_int;
    ui->m_IntensidadLampara->setText(value);
    ui->m_IntensidadLampara->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_IntensidadLampara->text().toInt();
    if (dummy_int<REF_INTENSIDAD_LAMPARA_MIN) dummy_int=REF_INTENSIDAD_LAMPARA_MIN;
    if (dummy_int>REF_INTENSIDAD_LAMPARA_MAX) dummy_int=REF_INTENSIDAD_LAMPARA_MAX;
    ui->m_IntensidadLampara->setText(QString("%1").arg(dummy_int));
}


void MaquinaAdv::AceptarValorDistanciaEmpalme(){
    InputData *input = InputData::request(this,"Distancia empalme [mm]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarDistanciaEmpalme(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarDistanciaEmpalme(QString value){
    int dummy_int;
    ui->m_DistanciaEmpalme->setText(value);
    ui->m_DistanciaEmpalme->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_DistanciaEmpalme->text().toInt();
    if (dummy_int<REF_DISTANCIA_EMPALME_MIN) dummy_int=REF_DISTANCIA_EMPALME_MIN;
    if (dummy_int>REF_DISTANCIA_EMPALME_MAX) dummy_int=REF_DISTANCIA_EMPALME_MAX;
    ui->m_DistanciaEmpalme->setText(QString("%1").arg(dummy_int));
}


void MaquinaAdv::AceptarValorTensionBobina(){
    InputData *input = InputData::request(this,"Tensión bobina [%]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarTensionBobina(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarTensionBobina(QString value){
    int dummy_int;
    ui->m_TensionBobina->setText(value);
    ui->m_TensionBobina->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_TensionBobina->text().toInt();
    if (dummy_int<REF_TENSION_BOBINA_MIN) dummy_int=REF_TENSION_BOBINA_MIN;
    if (dummy_int>REF_TENSION_BOBINA_MAX) dummy_int=REF_TENSION_BOBINA_MAX;
    ui->m_TensionBobina->setText(QString("%1").arg(dummy_int));
}

void MaquinaAdv::AceptarValorRampaAceleracion(){
    InputData *input = InputData::request(this,"Rampa aceleración [cm/min/s]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarRampaAceleracion(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarRampaAceleracion(QString value){
    int dummy_int;
    ui->m_RampaAceleracion->setText(value);
    ui->m_RampaAceleracion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_RampaAceleracion->text().toInt();
    if (dummy_int<REF_RAMPA_ACELERACION_MIN) dummy_int=REF_RAMPA_ACELERACION_MIN;
    if (dummy_int>REF_RAMPA_ACELERACION_MAX) dummy_int=REF_RAMPA_ACELERACION_MAX;
    ui->m_RampaAceleracion->setText(QString("%1").arg(dummy_int));
}


void MaquinaAdv::AceptarValorRampaDeceleracion(){
    InputData *input = InputData::request(this,"Rampa deceleración [cm/min/s]");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(AceptarRampaDeceleracion(QString)));
        input->setFocus();
    }
}

void MaquinaAdv::AceptarRampaDeceleracion(QString value){
    int dummy_int;
    ui->m_RampaDeceleracion->setText(value);
    ui->m_RampaDeceleracion->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    dummy_int=ui->m_RampaDeceleracion->text().toInt();
    if (dummy_int<REF_RAMPA_DECELERACION_MIN) dummy_int=REF_RAMPA_DECELERACION_MIN;
    if (dummy_int>REF_RAMPA_DECELERACION_MAX) dummy_int=REF_RAMPA_DECELERACION_MAX;
    ui->m_RampaDeceleracion->setText(QString("%1").arg(dummy_int));
}



void MaquinaAdv::LaunchPrincipal(){
    //GlobalShowMainWindow();
    this->deleteLater();
    //this->showMinimized();
}

void MaquinaAdv::ShowCam(){
   /* Webcam *MiWebcam;
    MiWebcam= new Webcam(0);
    MiWebcam->show();*/
}


void MaquinaAdv::EjecutarJogMasOn(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            GestorMaquina *m=GestorMaquina::request(this);
            m->sltJogMasOn(m_memoryMap->m_mapa.m_maquina.jogmas_conlampara);
        }
    }
}

void MaquinaAdv::EjecutarJogMenosOn(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            GestorMaquina *m=GestorMaquina::request(this);
            m->sltJogMenosOn(m_memoryMap->m_mapa.m_maquina.jogmenos_conlampara);
        }
    }
}


void MaquinaAdv::EjecutarJogMasOff(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            //Si estamos en Modo Manual
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_LAMPARA_ON,0x00);
            miCliente->EscribirDato(ITEM_JOG_MAS,0x00);
        }
    }
}

void MaquinaAdv::EjecutarJogMenosOff(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            //Si estamos en Modo Manual
            ClienteOPC *miCliente=ClienteOPC::request(this);
            miCliente->EscribirDato(ITEM_LAMPARA_ON,0x00);
            miCliente->EscribirDato(ITEM_JOG_MENOS,0x00);
         }
    }
}


void MaquinaAdv::EjecutarFrenoOn(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
       //JORGE  QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea activar freno?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //if (m_memoryMap->m_mapa.m_maquina.impresion==false){
                //Si estamos en Modo Manual
                ClienteOPC *miCliente=ClienteOPC::request(this);
                miCliente->EscribirDato(ITEM_FRENO_ONOFF,0x01);
                freno_on=true;
            //}
        }
    }
}


void MaquinaAdv::EjecutarFrenoOff(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea desactivar freno?","Si","No",QString::null, 0, 1 );
        if (respuesta==0){
            //if (m_memoryMap->m_mapa.m_maquina.impresion==false){
                //Si estamos en Modo Manual
                ClienteOPC *miCliente=ClienteOPC::request(this);
                miCliente->EscribirDato(ITEM_FRENO_ONOFF,QVariant(0));
                freno_on=false;
            //}
        }
    }
}


void MaquinaAdv::EjecutarBuscarMacula(){

    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea buscar mácula?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            GestorMaquina *m=GestorMaquina::request(this);
            m->sltBuscarMacula();
        }
    }

}

void MaquinaAdv::EjecutarEmpalme(){

    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea iniciar un empalme de bobina?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            m_memoryMap->m_mapa.m_maquina.InicioSecuenciaEmpalme=0x01;
            m_memoryMap->m_mapa.m_maquina.EstadoSecuenciaEmpalme=0x01;
            In2GestorMaquina *m=In2GestorMaquina::request(this);
            m->InitEmpalme();
        }
    }
}


void MaquinaAdv::EjecutarBobinaNueva(){

    int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea poner una bobina nueva?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        if (m_memoryMap->m_mapa.m_maquina.impresion==false){
            m_memoryMap->m_mapa.m_maquina.InicioSecuenciaBobinaNueva=0x01;
            m_memoryMap->m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=0x01;
            In2GestorMaquina *m=In2GestorMaquina::request(this);
            m->InitEmpalme();
        }
    }
}

void MaquinaAdv::EjecutarEncenderLampara(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        //int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea encender la lámpara?","Si","No",QString::null, 0, 1 );
        //if (respuesta==0){
            if (m_memoryMap->m_mapa.m_maquina.impresion==false){
                //Si estamos en Modo Manual
                emit sglLamparaOn();
                //JORGE ((Principal*)dummy)->miCliente->EscribirDato(ITEM_LAMPARA_ON,0x01);
                lamp_on=true;
            }
        //}
    }
}

void MaquinaAdv::EjecutarApagarLampara(){
    if ((m_memoryMap->m_mapa.m_maquina.VersionMaquina==2) && (m_memoryMap->m_mapa.m_maquina.m_opc.estado==1)){
        //JORGE QWidget *dummy;
        //JORGE dummy=GlobalGetMainWindow();
        //int respuesta=QMessageBox::question(this,"ATENCION","¿Está seguro que desea apagar la lámpara?","Si","No",QString::null, 0, 1 );
        //if (respuesta==0){
            if (m_memoryMap->m_mapa.m_maquina.impresion==false){
                //Si estamos en Modo Manual
                ClienteOPC *miCliente=ClienteOPC::request(this);
                miCliente->EscribirDato(ITEM_LAMPARA_ON,0x00);
                lamp_on=false;
            }
        //}
    }
}

void MaquinaAdv::EjecutarLampOnJogMas(){

    if (ui->m_LampOnJogMas->isChecked())
        m_memoryMap->m_mapa.m_maquina.jogmas_conlampara=true;
    else
        m_memoryMap->m_mapa.m_maquina.jogmas_conlampara=false;

}

void MaquinaAdv::EjecutarLampOnJogMenos(){

    if (ui->m_LampOnJogMenos->isChecked())
        m_memoryMap->m_mapa.m_maquina.jogmenos_conlampara=true;
    else
        m_memoryMap->m_mapa.m_maquina.jogmenos_conlampara=false;
}



