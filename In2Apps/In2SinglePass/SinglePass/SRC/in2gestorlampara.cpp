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
#include "in2gestorlampara.h"
#include "In2GestorDB.h"
#include "in2defines.h"
#include "in2configuratorlampara.h"
#include <QApplication>
#include "clienteopc.h"
#include <QMessageBox>
#include <QTimer>
#include "MemoryMap.h"



In2GestorLampara *In2GestorLampara::mInstance  = NULL;

/** Singleton*/
In2GestorLampara *In2GestorLampara::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new In2GestorLampara(parent);
    }
    return mInstance;
}


In2GestorLampara::In2GestorLampara(QObject *parent) : QObject(parent)
{
    m_enabled=false;
    m_bPosDinamica=false;
    m_bIntDinamica=true;
    m_Posicion.clear();
    m_Intensidad.clear();
    m_prevSpeed=0;
    m_pendingPos=-1;
    m_prevPos=-1;
    m_bMoving=false;
    m_vendor=PHOSEON;
    In2ConfiguratorLampara config;
    m_bPosDinamica=config.getPosDinamico();
    m_bIntDinamica=config.getIntDinamico();
    m_enabled=config.getEnabled();
    m_vendor=config.getVendor();
}
/** Devuelve el fabricxante de lampara*/
int In2GestorLampara::vendor(){
    return m_vendor;
}

/** Resetea el moving*/
void In2GestorLampara::setMoving(bool val){
    m_bMoving=val;
}

/** Nueva velocidad*/
void In2GestorLampara::newSpeed(int value){
    if (m_prevSpeed!=value){        
        m_prevSpeed=value;
        if (m_bIntDinamica){
            sltLoadIntensidad(value);
        }
        if (m_bPosDinamica){
            sltLoadPosicion(value);
        }
    }
}

/** Devuelve si esta activada la inetsidad dinamica*/
bool In2GestorLampara::intDinamica(){
    return m_bIntDinamica;
}
/** Devuelve si esta activada la posicion dinamica*/
bool In2GestorLampara::posDinamica(){
    return m_bPosDinamica;
}
/** Devuelve si esta habilitado el gestor*/
bool In2GestorLampara::enabled(){
    return m_enabled;
}


/** Carga una tabla de posicion*/
bool In2GestorLampara::loadPosTable(QString table){
    In2GestorDB db;
    bool  rtn=false;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
    if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
        QString message=QString("select * from '%1'").arg(table);
        if (db.exec(message)){
            QStringList foo=db.m_Tabla;
            int count=foo.count();
            if ((count>0)&&((count%2)==0)){ //Si hay elementos y son pares vacio la tabla actual
                m_Posicion.clear();
                int x=0;
                while (x<foo.count()){
                    int vel=foo.at(x).toInt();
                    int pos=foo.at(x+1).toInt();
                    m_Posicion.insert(vel,pos);
                    x+=2;
                }
                emit SendDebugInfo(LOGGER_STORE,tr("loadPosTable OK->%1 elementos").arg(count));
                rtn=true;
            }
            else{
                emit SendDebugInfo(LOGGER_ERROR,tr("Error loadPosTable->%1 elementos").arg(count));
            }
        }
        else
           emit SendDebugInfo(LOGGER_ERROR,tr("Error ejecutando %1").arg(message));
        db.close();

    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,tr("Error abriendo %1").arg(file));
    }
    return rtn;

}

/** Carga una tabla de posicion*/
bool In2GestorLampara::loadIntTable(QString table){
    In2GestorDB db;
    bool  rtn=false;
    QString file=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(LAMPARA_DB);
    if (db.open(QString("QSQLITE"),QString("In2Lampara"),file)){
        QString message=QString("select * from '%1'").arg(table);
        if (db.exec(message)){
            QStringList foo=db.m_Tabla;
            int count=foo.count();
            if ((count>0)&&((count%2)==0)){ //Si hay elementos y son pares vacio la tabla actual
                m_Intensidad.clear();
                int x=0;
                while (x<foo.count()){
                    int vel=foo.at(x).toInt();
                    int pos=foo.at(x+1).toInt();
                    m_Intensidad.insert(vel,pos);
                    x+=2;
                }
                emit SendDebugInfo(LOGGER_STORE,tr("loadIntTable OK->%1 elementos").arg(count));
                rtn=true;
            }
            else{
                emit SendDebugInfo(LOGGER_ERROR,tr("Error loadIntTable->%1 elementos").arg(count));
            }
        }
        else
           emit SendDebugInfo(LOGGER_ERROR,tr("Error ejecutando %1").arg(message));
        db.close();

    }
    else{
        emit SendDebugInfo(LOGGER_ERROR,tr("Error abriendo %1").arg(file));
    }
    return rtn;
}

/** Carga la nueva posicion*/
void In2GestorLampara::sltLoadPosicion(int speed){    
    int Pos=m_Posicion.lowerBound(speed).value();
    if ((Pos>=LAMPARA_MINPOS)&&(Pos<=LAMPARA_MAXPOS)){

        sltSendNewPosicion(Pos);
    }
}

/** Testea si puede enviar una nueva posicion*/
void In2GestorLampara::sltSendNewPosicion(int pos){    
    if (isReadyToMove()){
        if (m_prevPos!=pos)
        {
            emit sgnNewDataLampara(ITEM_LAMPARA_POS,pos);
        }

    }
}


/** Carga la nueva intensidad*/
void In2GestorLampara::sltLoadIntensidad(int speed){
    static int prevInt=-1;
    int Int=m_Intensidad.lowerBound(speed).value();
    if (Int!=prevInt){
        prevInt=Int;
        sendSignal(ITEM_INTENSIDAD_LAMPARA,QVariant(prevInt));
        MemoryMap *memoryMap=MemoryMap::request(0);
        memoryMap->m_mapa.m_maquina.intensidad_lampara=prevInt;
        //Hay que emitir una seÃ±al
    }
}

/** Resetea la posicion*/
void In2GestorLampara::resetPos(){
    //sltLoadPosicion(0);
}
/** Resetea la posicion*/
void In2GestorLampara::resetLampara(){
    sendSignal(ITEM_LAMPARA_RESET,1);
    QTimer::singleShot(1000,this,SLOT(_resetLampara1()));
}
/** Resetea la posicion*/
void In2GestorLampara::_resetLampara1(){
    sendSignal(ITEM_LAMPARA_RESET,0);
}


/** Jog Mas on*/
void In2GestorLampara::JogMasOn(){
    sendSignal(ITEM_LAMPARA_JOGF,1);
}
/** Jog menos on*/
void In2GestorLampara::JogMenosOn(){
    sendSignal(ITEM_LAMPARA_JOGR,1);
}
/** Jog Mas off*/
void In2GestorLampara::JogMasOff(){
    sendSignal(ITEM_LAMPARA_JOGF,0);
}
/** Jog menos off*/
void In2GestorLampara::JogMenosOff(){
    sendSignal(ITEM_LAMPARA_JOGR,0);
}

/** Carga una posicion de lampara*/
void In2GestorLampara::setPos(int Pos){
    //emit sgnNewDataLampara(ITEM_LAMPARA_POS,Pos);
    sltSendNewPosicion(Pos);
}
/** Carga una posicion de lampara*/
int In2GestorLampara::getPos(){
    ClienteOPC *opc=ClienteOPC::request(0);
    int rtn=opc->LeerDato(ITEM_LAMPARA_POS).toInt();
    return rtn;
}
/** Inicializa la lampara*/
void In2GestorLampara::init(){
    ClienteOPC *opc=ClienteOPC::request(0);
    bool val=opc->LeerDato(ITEM_DESPLAZ_SET).toBool();
    if (!val){
        emit SendDebugInfo(LOGGER_STORE,tr("InitGestorLampara item_Desplz=0"));
        sendSignal(ITEM_LAMPARA_SETUP,1);
        QTimer::singleShot(1000,this,SLOT(_init1()));
    }
    else{
        emit SendDebugInfo(LOGGER_STORE,tr("InitGestorLampara item_Desplz=1"));
        setMoving(false); //Forzamos el primer movimiento
        moveToInit();
    }
}
/** Inicializa la lampara*/
void In2GestorLampara::_init1(){
    if ((m_enabled)&&(m_bPosDinamica)){
        emit SendDebugInfo(LOGGER_STORE,tr("SetUpLampara 1"));
        sendSignal(ITEM_LAMPARA_SETUP,0);
    }
}
/** Devuelve la tabla actual de posicion*/
QMap<int,int> In2GestorLampara::getCurrentPosTable(){
    return m_Posicion;
}
/** Devuelve la tabla actual de intensidad*/
QMap<int,int> In2GestorLampara::getCurrentIntTable(){
    return m_Posicion;
}
/** Devuelve si esta disponible para imprimir*/
bool In2GestorLampara::isReadyToPrint(){
    bool rtn=true;
    if (m_bIntDinamica){
        if (!m_Intensidad.count()){
            rtn=false;
            QMessageBox::warning(0,"Error","No se ha cargado una tabla de intensidad de lampara",QMessageBox::Ok);
        }
    }
    if (m_bPosDinamica){
        if (!m_Posicion.count()){
            rtn=false;
            QMessageBox::warning(0,"Error","No se ha cargado una tabla de posicion de lampara",QMessageBox::Ok);
        }
    }
    return rtn;
}

/** Mueve la lamapara al inicio de la impresion*/
void In2GestorLampara::moveToInit(){
    m_prevPos=-1; //Fuerzo siempre este movimiento
    sltSendNewPosicion(LAMPARA_MINPOS);
}
/** Mueve la lamapara al inicio de la impresion*/
void In2GestorLampara::moveToReposo(){
    emit SendDebugInfo(LOGGER_STORE,tr("Moviendo lampara a posicion de reposo"));
    sltSendNewPosicion(LAMPARA_REPOSO);
}
/** Recibe el ACK de que se ha escrito la variable correspondiente*/
void In2GestorLampara::sltAckFromOPC(int item,QVariant value){
    if (item==ITEM_LAMPARA_POS){        
        m_prevPos=value.toInt();
        setMoving(true);
        emit SendDebugInfo(LOGGER_STORE,tr("Nueva posicion lampara %1").arg(m_prevPos));
    }
    else if (item==ITEM_LAMPARA_JOGF){
        setMoving(false);
    }
    else if (item==ITEM_LAMPARA_JOGR){
        setMoving(false);
    }
    else if (item==ITEM_LAMPARA_SETUP){
        setMoving(false);
        //Ojo, testeamos si hay emergencia al hacer el flanco
        int foo=value.toInt();
        if (foo==0){ //QTimer::singleShot(500,this,SLOT(sltTestSetUp())); //Cuando ha terminado el setup, lo testeo
            moveToInit();
        }
    }

}

/** Testea si se ha hecho el Setup*/
void In2GestorLampara::sltTestSetUp(){
    ClienteOPC *p=ClienteOPC::request(0);
    bool rtn=p->LeerDato(ITEM_DESPLAZ_SET).toBool();
    if (!rtn){
        QTimer::singleShot(1000,this,SLOT(init()));//No se ha hecho el setup...lo reintento
    }
    else{
        //Me muevo al cerapio
        moveToInit();
    }

}

/** Determina si esta preparado para mover*/
bool In2GestorLampara::isReadyToMove(){
    bool rtn=true;
    if (m_bMoving){
        ClienteOPC *p=ClienteOPC::request(0);
        rtn=p->LeerDato(ITEM_DESPLAZ_INP).toBool();
    }
    return rtn;
}

/** Manda una señal solo si está habilitado*/
void In2GestorLampara::sendSignal(int item,QVariant value){
    if (m_enabled)
        emit sgnNewDataLampara(item,value);
}
