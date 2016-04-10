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
#include "inkjetcommonext.h"
#include <QNetworkInterface>
#include <QHostAddress>
#include "MemoryMap.h"
InkjetCommonExt::InkjetCommonExt(QObject *parent) :
    QObject(parent)
{
}

/** Inicializa estructura de datos*/
InkjetCommonExt::InkjetCommonExt(QWidget *parent, stCommonExt &dataCommon){
    m_parent=parent;
    memcpy(&stDataCommon,&dataCommon,sizeof (struct stCommonExt));
    debug=InkjetDebug::request(0);
    LoadErrorList();
    LoadIPInfo();
    QObject::connect(stDataCommon.m_MostrarAyuda,SIGNAL(clicked()),this,SLOT(MostrarAyuda()));
    QObject::connect(debug,SIGNAL(sgnInfoError(QString)),this,SLOT(sltAddInfo(QString)));

    //Hasta no recibir informacion de ellos, deshabilitamos los Leds
    //stDataCommon.lblLamp->setEnabled(false);
    stDataCommon.led_EstadoLampara->setEnabled(false);
    if (stDataCommon.info){
        stDataCommon.info->ensureCursorVisible();
    }

}
/** Carga la lista de errores previa*/
void InkjetCommonExt::LoadErrorList(){
    QStringList lst=debug->getErrorList();
    for (int i=0;i<lst.count();i++){
        if (stDataCommon.info) stDataCommon.info->append(lst.at(i));
    }
}
/** Carga info relativa a IP*/
void InkjetCommonExt::LoadIPInfo(){
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)){
             if (stDataCommon.lbl_IP) stDataCommon.lbl_IP->setText(address.toString());
             //break;
        }
    }
}

/** Añade informacion a la pantalla*/
void InkjetCommonExt::sltAddInfo(const QString &p){
    if (stDataCommon.info){
        stDataCommon.info->append(p);

    }
}

/** refresca el modo de funcionamiento*/
void InkjetCommonExt::RefreshUserMode(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (memoryMap->getUserRights()==ACCESS_STRANGE) stDataCommon.m_User->setText("STRANGE");
    else if (memoryMap->getUserRights()==ACCESS_PRINTER) stDataCommon.m_User->setText("PRINTER");
    else if (memoryMap->getUserRights()==ACCESS_ROOT) stDataCommon.m_User->setText("ROOT");
    else if (memoryMap->getUserRights()==ACCESS_GOD) stDataCommon.m_User->setText("SUPERUSER");
}

//Muestra informacion en la ventana superior
void InkjetCommonExt::ShowInfo(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString dummy_usuario=QString("%1").arg(memoryMap->getUser());
    stDataCommon.usuario_actual->setText(dummy_usuario);
    stDataCommon.usuario_actual->show();
    this->RefreshUserMode();

}

void InkjetCommonExt::ShowEstadoImpresora(){

    QPixmap tmp;

    if (flag_led_Maquina_rojo==1) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    stDataCommon.led_EstadoMaquina->setPixmap((QPixmap)tmp);

    /*if (flag_Hydra_ok==0) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    stDataCommon.led_EstadoTemperatura->setPixmap((QPixmap)tmp);*/

    /*if (flag_led_Lampara_rojo==1) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    stDataCommon.led_EstadoLampara->setPixmap((QPixmap)tmp);*/
}
/** Modifica el estado de la fluidica*/
void InkjetCommonExt::sltEstadoFluidica(char state){
    QPixmap tmp;
    if (state) tmp.load(":res/images/Ledverde.png");
    else tmp.load(":res/images/Ledrojo.png");
    stDataCommon.led_EstadoTemperatura->setPixmap((QPixmap)tmp);
}

/** Modifica el estado de la lampara*/
void InkjetCommonExt::sltEstadoLampara(char state){
    QPixmap tmp;
    if (state) tmp.load(":res/images/Ledverde.png");
    else tmp.load(":res/images/Ledrojo.png");
    stDataCommon.led_EstadoLampara->setEnabled(true);
    stDataCommon.led_EstadoLampara->setPixmap((QPixmap)tmp);
}


//Muestra el estado de las comunicaciones
void InkjetCommonExt::ShowComms(){
    QDate dummyD=QDate::currentDate();
    QString dummy_date=dummyD.toString("dd'/'MM'/'yyyy");
    QTime dummyT=QTime::currentTime();
    QString dummy_time=dummyT.toString("hh:mm");
    stDataCommon.fecha->setText(QString("%1  %2").arg(dummy_date).arg(dummy_time));
    //stDataCommon.hora->setText(dummy_time);
    ShowInfo();
}

void InkjetCommonExt::MostrarAyuda(void){
}

void InkjetCommonExt::MostrarMantenimiento(void){

    /*QString mensaje;
    mensaje=QString("Alguna de las siguientes operaciones de operación quedan pendientes:");
    if (MantenimientoPendiente!=0){
        if ((MantenimientoPendiente&MASCARA_DIA_LAMPARA)==MASCARA_DIA_LAMPARA) mensaje.append("\nLimpieza de la lámpara de curado UV");
        if ((MantenimientoPendiente&MASCARA_HORAS_LAMPARA)==MASCARA_HORAS_LAMPARA) mensaje.append("\nCambio de la lámpara de curado UV");
        if ((MantenimientoPendiente&MASCARA_FILTROS)==MASCARA_FILTROS) mensaje.append("\nCambio de los filtros de tinta");
        if ((MantenimientoPendiente&MASCARA_LIMPIEZA_LAMPARA)==MASCARA_LIMPIEZA_LAMPARA) mensaje.append("\nLimpieza de la lámpara de curado");
        QMessageBox::information(0,"Atención",mensaje,QMessageBox::Ok);
    }*/

}
