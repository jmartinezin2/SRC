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

#include "inkjetcommon.h"
#include "MemoryMap.h"
#include <QPainter>
#include <QMessageBox>
#include "ui_InkjetDebugUI.h"
#include "showHelp.h"

#define MAX_LABEL_LEN 32

InkjetCommon::InkjetCommon(QObject *parent) :
    QObject(parent)
{
}
/** Inicializa estructura de datos*/
InkjetCommon::InkjetCommon(QWidget *parent, stCommon &dataCommon){
    m_parent=parent;
    memcpy(&stDataCommon,&dataCommon,sizeof (struct stCommon ));
    QObject::connect(stDataCommon.m_MostrarInicio,SIGNAL(clicked()),this,SLOT(MostrarInicio()));
    QObject::connect(stDataCommon.m_MostrarAyuda,SIGNAL(clicked()),this,SLOT(MostrarAyuda()));
    QObject::connect(stDataCommon.m_MostrarMantenimiento,SIGNAL(clicked()),this,SLOT(MostrarMantenimiento()));
}

/** refresca el modo de funcionamiento*/
void InkjetCommon::RefreshUserMode(){
    if (m_memoryMap->getUserRights()==ACCESS_STRANGE) stDataCommon.m_User->setText("Modo: STRANGE");
    else if (m_memoryMap->getUserRights()==ACCESS_PRINTER) stDataCommon.m_User->setText("Modo: PRINTER");
    else if (m_memoryMap->getUserRights()==ACCESS_ROOT) stDataCommon.m_User->setText("Modo: ROOT");
    else if (m_memoryMap->getUserRights()==ACCESS_GOD) stDataCommon.m_User->setText("Modo: SUPERUSER");
}


//Muestra informacion en la ventana superior
void InkjetCommon::ShowInfo(){

    //if (m_memoryMap->m_ConfiguracionCargada==true) {
    if (m_memoryMap->m_UltimaAccion!=UA_NONE) {
        QString ruta,nombre;
        bool bret=false;

        if (m_memoryMap->m_UltimaAccion==UA_CONFIGURACION_GENERADA){
            nombre=m_memoryMap->m_mapa.m_variables.NombreIni;
            ruta=m_memoryMap->m_mapa.m_variables.RutaIni;
            bret=true;
        }
        else if (m_memoryMap->m_UltimaAccion==UA_IMAGEN_SELECCIONADA){
            nombre=m_memoryMap->m_mapa.m_variables.NombreBmpEnviado;
            ruta=m_memoryMap->m_mapa.m_variables.RutaBmpEnviado;
            bret=true;
        }
        else if (m_memoryMap->m_UltimaAccion==UA_TRABAJO_SELECCIONADO){
            nombre=m_memoryMap->m_mapa.m_variables.NombreIni;
            ruta=m_memoryMap->m_mapa.m_variables.RutaIni;
            bret=true;
        }
        else if (m_memoryMap->m_UltimaAccion==UA_CONFIGURACION_CARGADA){
            nombre=m_memoryMap->m_mapa.m_variables.NombreIni;
            ruta=m_memoryMap->m_mapa.m_variables.RutaIni;
            bret=true;
        }
        else if (m_memoryMap->m_UltimaAccion==UA_IMAGEN_ENVIADA){
            nombre=m_memoryMap->m_mapa.m_variables.NombreIni;
            ruta=m_memoryMap->m_mapa.m_variables.RutaIni;
            bret=true;
        }

        if (bret){
            if (nombre.size()>MAX_LABEL_LEN){
                nombre.chop(nombre.size()-MAX_LABEL_LEN);
                nombre.append("...");
            }
            if (ruta.size()>MAX_LABEL_LEN){
                ruta.chop(ruta.size()-MAX_LABEL_LEN);
                ruta.append("...");
            }
            stDataCommon.m_NombreFicheroConfig->setText("Fichero:  " + nombre);
            stDataCommon.m_RutaFicheroConfig->setText("Ruta:  " + ruta);
        }
        else{
            stDataCommon.m_NombreFicheroConfig->setText("Fichero:  ----");
            stDataCommon.m_RutaFicheroConfig->setText("Ruta:  ----");
        }
    }
    else{
        stDataCommon.m_NombreFicheroConfig->setText("Fichero:  ----");
        stDataCommon.m_RutaFicheroConfig->setText("Ruta:  ----");
    }

    stDataCommon.m_NombreFicheroConfig->show();
    stDataCommon.m_RutaFicheroConfig->show();

    QString dummy_usuario=QString("Usuario: %1").arg(m_memoryMap->getUser());
    stDataCommon.usuario_actual->setText(dummy_usuario);
    stDataCommon.usuario_actual->resize(24*DELTA,2*DELTA);
    stDataCommon.usuario_actual->show();

    this->RefreshUserMode();

}

//Muestra informacion en la ventana inferior
void InkjetCommon::ShowStatus(int iEstado){
    QPixmap Logo;

    Logo.load(":/res/images/folder_logo.png");


    QPixmap temp(Logo.size());
    temp.fill(Qt::transparent);
    QPainter p(&temp);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(0, 0,Logo);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(temp.rect(), QColor(0, 0, 0, 75));

    //p.end(); Creo que habria que ponerlo

    Logo = temp;
    QPixmap dummy=Logo.scaledToHeight(LOGO_SIZE);
    stDataCommon.m_TextoStatus->setPixmap((QPixmap)(dummy));
    stDataCommon.m_TextoStatus->show();
    stDataCommon.m_TextoStatus->setEnabled(false);

}

void InkjetCommon::ShowEstadoImpresora(){

    QPixmap tmp;

    if (flag_led_Maquina_rojo==1) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    stDataCommon.led_EstadoMaquina->setPixmap((QPixmap)tmp);

    if (flag_Hydra_ok==0) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    stDataCommon.led_EstadoTemperatura->setPixmap((QPixmap)tmp);

    if (flag_led_Lampara_rojo==1) tmp.load(":res/images/Ledrojo.png");
    else tmp.load(":res/images/Ledverde.png");
    stDataCommon.led_EstadoLampara->setPixmap((QPixmap)tmp);

}


//Muestra el estado de las comunicaciones
void InkjetCommon::ShowComms(){
    QDate dummyD=QDate::currentDate();
    QString dummy_date=dummyD.toString("dd'/'MM'/'yyyy");
    QTime dummyT=QTime::currentTime();
    QString dummy_time=dummyT.toString("hh:mm");
    stDataCommon.fecha->setText(QString("%1  %2").arg(dummy_date).arg(dummy_time));
    //stDataCommon.hora->setText(dummy_time);
    ShowInfo();
    if (MantenimientoPendiente!=0){
        static bool on_off=false;
            if (on_off){
                QIcon icono(":res/images/light_on.png");
                stDataCommon.m_MostrarMantenimiento->setIcon(icono);
                on_off=false;
            }
            else{
                QIcon icono(":res/images/light_off.png");
                stDataCommon.m_MostrarMantenimiento->setIcon(icono);
                on_off=true;
            }
    }
}


void InkjetCommon::MostrarInicio(void){
    //Minimizar...hay que emitir señal
     m_parent->showMinimized();
}

void InkjetCommon::MostrarAyuda(void){

        //QMessageBox::information(0,"Contacto","In2 Printing Solutions\nCarlos Corchón\nTfno 636648974\nwww.in2ps.com",QMessageBox::Ok);


}

void InkjetCommon::MostrarMantenimiento(void){

    QString mensaje;
    mensaje=QString("Alguna de las siguientes operaciones de operación quedan pendientes:");
    if (MantenimientoPendiente!=0){
        if ((MantenimientoPendiente&MASCARA_DIA_LAMPARA)==MASCARA_DIA_LAMPARA) mensaje.append("\nLimpieza de la lámpara de curado UV");
        if ((MantenimientoPendiente&MASCARA_HORAS_LAMPARA)==MASCARA_HORAS_LAMPARA) mensaje.append("\nCambio de la lámpara de curado UV");
        if ((MantenimientoPendiente&MASCARA_FILTROS)==MASCARA_FILTROS) mensaje.append("\nCambio de los filtros de tinta");
        //if ((MantenimientoPendiente&MASCARA_LIMPIEZA_LAMPARA)==MASCARA_LIMPIEZA_LAMPARA) mensaje.append("\nLimpieza de la lámpara de curado");
        QMessageBox::information(0,"Atención",mensaje,QMessageBox::Ok);
    }

}
